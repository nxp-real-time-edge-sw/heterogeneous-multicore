if(DEFINED RTOS_ID)
SET(RTOS_FLAG "-DRTOSID=${RTOS_ID}")
else()
SET(RTOS_FLAG "")
endif()

IF(NOT DEFINED DEBUG_CONSOLE_CONFIG)
    SET(DEBUG_CONSOLE_CONFIG "-DSDK_DEBUGCONSOLE=1")
ENDIF()

SET(CMAKE_ASM_FLAGS_DDR_DEBUG " \
    -DDEBUG \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -DGUEST \
    -g \
    -mcpu=cortex-a55 \
    -Wall \
    -Wno-address-of-packed-member \
    -Wno-misleading-indentation \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -std=gnu99 \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_ASM_FLAGS_DDR_RELEASE " \
    -DNDEBUG \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -DGUEST \
    -mcpu=cortex-a55 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
    -Wno-misleading-indentation \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -std=gnu99 \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")

SET(CMAKE_C_FLAGS_DDR_DEBUG " \
    -DDEBUG \
    -DCPU_MIMX9111CVXXJ \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DFSL_ETH_ENABLE_CACHE_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=1 \
    -DFSL_RTOS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -g \
    -O0 \
    -mcpu=cortex-a55 \
    -Wall \
    -Wno-address-of-packed-member \
    -Wno-misleading-indentation \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -std=gnu99 \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_C_FLAGS_DDR_RELEASE " \
    -DNDEBUG \
    -DCPU_MIMX9111CVXXJ \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DFSL_ETH_ENABLE_CACHE_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=1 \
    -DFSL_RTOS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -O3 \
    -mcpu=cortex-a55 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
    -Wno-misleading-indentation \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -std=gnu99 \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")

SET(CMAKE_CXX_FLAGS_DDR_DEBUG " \
    -DDEBUG \
    -DCPU_MIMX9111CVXXJ \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DFSL_ETH_ENABLE_CACHE_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=0 \
    -g \
    -O0 \
    -mcpu=cortex-a55 \
    -Wall \
    -Wno-address-of-packed-member \
    -Wno-misleading-indentation \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -fno-rtti \
    -fno-exceptions \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_CXX_FLAGS_DDR_RELEASE " \
    -DNDEBUG \
    -DCPU_MIMX9111CVXXJ \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DFSL_ETH_ENABLE_CACHE_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=0 \
    -O3 \
    -mcpu=cortex-a55 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
    -Wno-misleading-indentation \
    -MMD \
    -MP \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -fno-rtti \
    -fno-exceptions \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")

if(NOT DEFINED DDR_LD_FILE)
SET(DDR_LD_FILE	"\
    ${BoardPath}/armgcc_aarch64/MIMX9111xxxxx_ca55_ddr_ram.ld \
")
endif()

SET(CMAKE_EXE_LINKER_FLAGS_DDR_DEBUG " \
    -g \
    -mcpu=cortex-a55 \
    -Wall \
    --specs=nosys.specs \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -Xlinker \
    --gc-sections \
    -Xlinker \
    -static \
    -Xlinker \
    -z \
    -Xlinker \
    muldefs \
    -Xlinker \
    -Map=output.map \
    -T${DDR_LD_FILE} -static \
")
SET(CMAKE_EXE_LINKER_FLAGS_DDR_RELEASE " \
    -mcpu=cortex-a55 \
    -Wall \
    --specs=nosys.specs \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -Xlinker \
    --gc-sections \
    -Xlinker \
    -static \
    -Xlinker \
    -z \
    -Xlinker \
    muldefs \
    -Xlinker \
    -Map=output.map \
    -T${DDR_LD_FILE} -static \
")
