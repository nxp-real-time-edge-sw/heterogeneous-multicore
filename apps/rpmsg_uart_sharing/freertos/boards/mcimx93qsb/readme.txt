Overview
========
This is a FreeRTOS example to share physical UART owned by Cortex-M Core with Cortex-A Core.
The SRTM(Simplified Real Time Messaging) protocol is used to achieve this. LPUART5 on MCIMX93-QSB
is used for UART sharing.

Hardware requirements
=====================
- Type-C USB cable
- MCIMX93-QSB board
- J-Link Debug Probe
- 12V~20V power supply
- Personal Computer

Board settings
==============
Connect LPUART5 to host PC using USB To TTL serial cable.

    LPUART5 RX:  Pin 27 of J1401
    LPUART5 TX:  Pin 28 of J1401
    LPUART5 GND: Pin 30 of J1401

Prepare the Demo
================
1. Connect a Type-C USB cable between the host PC and the J1708 USB port on the target board.
   Open two serial terminals for A Core and M Core with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
    (e.g. /dev/ttyUSB0~3, /dev/ttyUSB2 for A Core, /dev/ttyUSB3 for M Core)
2. Connect 12V~20V power supply and J-Link Debug Probe to the board, switch SW301 to power on the board
3. Boot Linux BSP to u-boot, and load M core image from SD card to run.
   => load mmc 1:2 0x80000000 /examples/rpmsg-lite-uart-sharing-rtos/release/rpmsg_lite_uart_sharing_rtos.bin
   => cp.b 0x80000000 0x201e0000 0x10000
   => bootaux 0x1ffe0000 0
4. After M core running, boot the linux kernel to create the rpmsg channel between A core and M core.
   Make sure to use the correct DTB and append "clk_ignore_unused" in u-boot "mmcargs" env, before booting linux.
   => setenv fdtfile imx93-9x9-qsb-rpmsg.dtb
   => setenv jh_clk clk_ignore_unused
   => boot
5. Boot to linux.
6. After login, make sure rpmsg_tty kernel module is inserted (lsmod) or insert it (modprobe rpmsg_tty).

Running the demo
================
After the boot process succeeds, the ARM Cortex-M33 terminal (connected to LPUART2) displays the following information:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
####################  RPMSG UART SHARING DEMO  ####################

    Build Time: Oct 30 2023 11:20:34

Start SRTM communication
********************************
Wait for the Linux kernel boot up to create the link between M core and A core.
********************************
Handle Peer Core Linkup
********************************
The rpmsg channel between M core and A core created!
********************************

Task A is running now.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The user can then input an arbitrary string to the virtual RPMsg tty using the following echo command on Cortex-A terminal:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo test > /dev/ttyRPMSG<num>

Note: <num> here is the allocated ttyRPMsg channel number. Please find out the number in the file system by "ls" command.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
When /dev/ttyRPMSG0 to /dev/ttyRPMSG9 is used, the following output is shown on the terminal connected to LPUART5
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$UT,test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
When /dev/ttyRPMSG10 is used, the following output is shown on the M33 terminal (connected to LPUART2).
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SRTM_UartService_ReceiveNotify: 452, data = test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
