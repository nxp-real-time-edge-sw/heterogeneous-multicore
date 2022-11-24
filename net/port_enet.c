/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <os/stdio.h>
#include <os/irq.h>
#include <os/stdlib.h>
#include <os/unistd.h>
#include <net/data_pkt.h>
#include <net/net_switch.h>

#include "app_board.h"
#include "app_virtio_config.h"
#include "enet_os.h"
#include "port_enet_priv.h"

#define ENET_PHY_ADDRESS		BOARD_PHY0_ADDRESS
#define ENET_PHY_MIIMODE		BOARD_NET_PORT0_MII_MODE
/* MDIO operations. */
#define ENET_MDIO_OPS			BOARD_PHY0_MDIO_OPS
/* PHY operations. */
#define ENET_PHY_OPS			BOARD_PHY0_OPS
#ifndef PHY_AUTONEGO_TIMEOUT_COUNT
#define PHY_AUTONEGO_TIMEOUT_COUNT	(10000)
#endif

#define ENET_RXBD_NUM			(256)
#define ENET_TXBD_NUM			(512)
#define ENET_RXBUFF_SIZE		(ENET_FRAME_MAX_FRAMELEN)
#define ENET_TXBUFF_SIZE		(ENET_FRAME_MAX_FRAMELEN)

/* Rx interrupt coalescing frame count threshold */
#define ENET_RX_ICS_COUNT		(200)
/* Rx interrupt coalescing timer threshold (us) */
#define ENET_RX_ICS_TIMEOUT_US		(1000)

#ifndef APP_ENET_BUFF_ALIGNMENT
#define APP_ENET_BUFF_ALIGNMENT		ENET_BUFF_ALIGNMENT
#endif

#define NONCACHEABLE(var, alignbytes)	AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes)

/* Buffer descriptors should be in non-cacheable region and should be align to "ENET_BUFF_ALIGNMENT". */
NONCACHEABLE(static enet_rx_bd_struct_t enet_rx_buffer_desc[ENET_RXBD_NUM], ENET_BUFF_ALIGNMENT);
NONCACHEABLE(static enet_tx_bd_struct_t enet_tx_buffer_desc[ENET_TXBD_NUM], ENET_BUFF_ALIGNMENT);

#define STATIC_BUFF_COUNT	(ENET_RXBD_NUM * 3)

/*
 * The data buffers can be in cacheable region or in non-cacheable region.
 * If use cacheable region, the alignment size should be the maximum size of "CACHE LINE SIZE" and "ENET_BUFF_ALIGNMENT"
 * If use non-cache region, the alignment size is the "ENET_BUFF_ALIGNMENT".
 */
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && (FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
SDK_ALIGN(static uint8_t enet_rx_buffer[STATIC_BUFF_COUNT][SDK_SIZEALIGN(ENET_RXBUFF_SIZE + DATA_PKT_HEAD_ROOM, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
SDK_ALIGN(static uint8_t enet_tx_buffer[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
#else
NONCACHEABLE(static uint8_t enet_rx_buffer[STATIC_BUFF_COUNT][SDK_SIZEALIGN(ENET_RXBUFF_SIZE + DATA_PKT_HEAD_ROOM, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
NONCACHEABLE(static uint8_t enet_tx_buffer[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)], ENET_BUFF_ALIGNMENT);
#endif

/* ENET, PHY and MDIO interface handler */
static mdio_handle_t mdio_handle = {.ops = ENET_MDIO_OPS};
static phy_handle_t phy_handle = {.phyAddr = ENET_PHY_ADDRESS, .mdioHandle = &mdio_handle, .ops = ENET_PHY_OPS};
static enet_handle_t enet_handle;
static uint8_t default_mac_addr[6] = {0x00, 0x04, 0x9f, 0x06, 0x01, 0x02};

void enet_process_pkts(void *param)
{
	struct enet_device *enet_dev = param;
	enet_handle_t *handle = enet_dev->handle;
	struct switch_port *port = enet_dev->port_dev;
	status_t status;
	struct data_pkt *pkt;
	struct enet_event event;
	enet_buffer_struct_t rx_buffer;
	enet_rx_frame_struct_t rx_frame = {.rxBuffArray = &rx_buffer};
	void *addr;
	bool need_notify;

	do {
		if (!os_mq_receive(&enet_dev->mqueue, &event, 0, OS_QUEUE_EVENT_TIMEOUT_MAX)) {
			/* Read all data from ring buffer and send to up layer */
			need_notify = false;
			do {
				/* Get the Frame size */
				status = ENET_GetRxFrame(ENET, handle, &rx_frame, 0);
				if (status == kStatus_Success) {
					addr = rx_buffer.buffer;
					pkt = data_pkt_get(addr);
					data_pkt_set_len(pkt, rx_buffer.length);
					port_in_pkt_nocpy(port, pkt, false);
					need_notify = true;
				} else if (status == kStatus_ENET_RxFrameEmpty)
					break;
				else	/* Error packets */
					port->stats.in_dropped++;
			} while (1);
			if (need_notify)
				switch_notify(port);
		}
	} while (1);
}

#if FSL_FEATURE_ENET_QUEUE > 1
static void enet_callback(ENET_Type *base, enet_handle_t *handle, uint32_t ringId,
		enet_event_t event, enet_frame_info_t *frameInfo, void *param)
#else
static void enet_callback(ENET_Type *base, enet_handle_t *handle, enet_event_t event,
		enet_frame_info_t *frameInfo, void *param)
#endif
{
	struct enet_device *dev = param;
	struct enet_event mq_event;

	switch (event) {
	case kENET_RxEvent:
		mq_event.type = ENET_RCV;
		os_mq_send(&dev->mqueue, &mq_event,
				OS_MQUEUE_FLAGS_ISR_CONTEXT, 0);
		break;
	default:
		break;
	}
}

int enet_output(struct switch_port *port, void *data_paket)
{
	struct data_pkt *pkt = data_paket;
	status_t status;

	if (pkt->data_len >= ENET_FRAME_MAX_FRAMELEN)
		return -1;
	status = ENET_SendFrame(ENET, &enet_handle, pkt->data, pkt->data_len, 0, false, NULL);
	if (status != kStatus_Success)
		return -1;

	return 0;
}

static int phy_setup(uint32_t enet_ipg_freq)
{
	phy_config_t phy_config = {0};
	uint32_t count;
	bool link = false;
	bool autonego = false;
	status_t status;

	phy_config.phyAddr = ENET_PHY_ADDRESS;
	phy_config.autoNeg = true;
	mdio_handle.resource.base = ENET;
	mdio_handle.resource.csrClock_Hz = enet_ipg_freq;
	/* Initialize PHY */
	status = PHY_Init(&phy_handle, &phy_config);
	if (status != kStatus_Success) {
		os_printf("ENET: PHY init failed!\r\n");
		return -1;
	}
	os_printf("ENET: Wait for PHY link up...\r\n");
	/* Wait for auto-negotiation success and link up */
	count = PHY_AUTONEGO_TIMEOUT_COUNT;
	do {
		PHY_GetAutoNegotiationStatus(&phy_handle, &autonego);
		PHY_GetLinkStatus(&phy_handle, &link);
		if (autonego && link)
			break;
		os_msleep(10);
	} while (--count);

	if (!count) {
		os_printf("ENET: PHY Auto-negotiation (%s) and link (%s)\r\n",
				autonego ? "compeleted" : "failed", link ? "up" : "down");
		return -1;
	}

	return 0;
}

extern void ENET_DriverIRQHandler(void);

void enet_irq_handler(void *param)
{
	ENET_DriverIRQHandler();
}

static void static_buff_init(void)
{
	struct data_pkt *pkt;
	int i;

	for (i = 0; i < STATIC_BUFF_COUNT; i++) {
		pkt = data_pkt_build(enet_rx_buffer[i], sizeof(enet_rx_buffer[i]),
			DATA_PKT_FLAG_CAN_FREE | DATA_PKT_FLAG_STATIC_BUFF);
		pkt->used = false;
	}
}

static void *rx_buff_alloc(ENET_Type *base, void *user_data, uint8_t ring_id)
{
	struct data_pkt *pkt;
	int i;

	for (i = 0; i < STATIC_BUFF_COUNT; i++) {
		pkt = (struct data_pkt *)enet_rx_buffer[i];
		if (!pkt->used) {
			pkt->used = true;
			return pkt->data;
		}
	}

	return NULL;
}

static void rx_buff_free(ENET_Type *base, void *buffer,  void *user_data, uint8_t ring_id)
{
	struct data_pkt *pkt = data_pkt_get(buffer);

	pkt->used = false;
}

static int enet_setup(void *enet_dev)
{
	enet_buffer_config_t buffer_config = {0};
	enet_intcoalesce_config_t coalescing_cfg = {0};
	enet_config_t config;
	phy_duplex_t duplex;
	phy_speed_t speed;
	uint32_t enet_ipg_freq;
	uint32_t enet_rx_ic_timer;
	int ret;

	os_assert(!(DATA_PKT_HEAD_ROOM % ENET_BUFF_ALIGNMENT),
		"Data packet header room must be aligned with enet buffer\r\n" );
	static_buff_init();

	enet_ipg_freq = CLOCK_GetFreq(kCLOCK_EnetIpgClk);

	ret = phy_setup(enet_ipg_freq);
	if (ret)
		return ret;

	/* Prepare the buffer configuration. */
	buffer_config.rxBdNumber = ENET_RXBD_NUM;
	buffer_config.txBdNumber = ENET_TXBD_NUM;
	buffer_config.rxBuffSizeAlign = SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT);
	buffer_config.txBuffSizeAlign = SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT);
	buffer_config.rxBdStartAddrAlign = &enet_rx_buffer_desc[0];
	buffer_config.txBdStartAddrAlign = &enet_tx_buffer_desc[0];
	buffer_config.rxBufferAlign = NULL;
	buffer_config.txBufferAlign = &enet_tx_buffer[0][0];
	buffer_config.rxMaintainEnable = true;
	buffer_config.txMaintainEnable = true;
	buffer_config.txFrameInfo = NULL;

	ENET_GetDefaultConfig(&config);
	/* Enable promiscuous mode */
	config.macSpecialConfig |= kENET_ControlPromiscuousEnable;
	/* The miiMode should be set according to the different PHY interfaces. */
	config.miiMode = ENET_PHY_MIIMODE;

	config.rxBuffAlloc = rx_buff_alloc;
	config.rxBuffFree = rx_buff_free;

	/* Get the actual PHY link speed. */
	PHY_GetLinkSpeedDuplex(&phy_handle, &speed, &duplex);
	os_printf("ENET: PHY link speed %s %s-duplex\r\n",
			speed ? ((speed == 2) ? "1000M" : "100M") : "10MB", duplex ? "full" : "half");
	/* Change the MII speed and duplex for actual link status. */
	config.miiSpeed  = (enet_mii_speed_t)speed;
	config.miiDuplex = (enet_mii_duplex_t)duplex;
	config.interrupt = kENET_RxFrameInterrupt;
	config.callback = enet_callback;
	config.userData = enet_dev;

	enet_rx_ic_timer = ENET_RX_ICS_TIMEOUT_US * (enet_ipg_freq / 64000) / 1000;

	coalescing_cfg.rxCoalesceFrameCount[0] = ENET_RX_ICS_COUNT;
	coalescing_cfg.rxCoalesceTimeCount[0]  = enet_rx_ic_timer;
	config.intCoalesceCfg = &coalescing_cfg;

	ENET_Init(ENET, &enet_handle, &config, &buffer_config, default_mac_addr,  enet_ipg_freq);

	ret = os_irq_register(ENET_IRQn, enet_irq_handler, NULL, ENET_IRQ_PRIO);
	if (ret)
		os_printf("Register ENET IRQ failed\r\n", ENET_IRQn);

	return 0;
}

static void enet_active(void)
{
	ENET_ActiveRead(ENET);
}

/*
 * TODO:
 * Currently promiscuous mode is used, but the target is to use hash table algorithm
 * to filter specified mac addresses
 */
static int enet_setup_macaddr(uint8_t addr_ops, uint8_t *new_addr, uint8_t *old_addr)
{
	switch (addr_ops) {
	case OPS_ADDR_ADD:

		break;

	case OPS_ADDR_DEL:

		break;

	case OPS_ADDR_UPDATE:

		break;

	default:
		os_printf("Invaliad address operation: %d\r\n", addr_ops);
		break;
	}

	return 0;
}

int enet_port_init(void *switch_dev)
{
	struct enet_device *enet_dev;
	void *switch_port;
	struct port_config config;
	int ret;

	enet_dev = os_malloc(sizeof(struct enet_device));
	os_assert(enet_dev, "no memory for enet device");
	memset(enet_dev, 0, sizeof(struct enet_device));
	ret = os_mq_open(&enet_dev->mqueue, "enet_mqueue", 10, sizeof(struct enet_event));
	os_assert(!ret, "enet mqueue initialization failed");

	switch_port = switch_init_port(switch_dev);

	enet_dev->port_dev = switch_port;
	enet_dev->handle = &enet_handle;

	ret = enet_setup(enet_dev);
	if (ret) {
		os_printf("Enet setup failed");
		return -1;
	}

	enet_dev->link_up = true;
	config.dev_end_on = &enet_dev->link_up;

	config.is_local = false;
	config.port_out_cb = enet_output;
	memset(config.mac_addr, 0, 6);
	config.setup_address_cb = enet_setup_macaddr;
	strlcpy(config.name, "enet_port", sizeof(config.name));
	switch_add_port(switch_port, &config);

	create_enet_thread(enet_dev);

	enet_active();
	os_irq_enable(ENET_IRQn);

	return 0;
}
