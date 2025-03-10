if(DEFINED RTOS_ID)
SET(RTOS_FLAG "-DRTOSID=${RTOS_ID}")
else()
SET(RTOS_FLAG "")
endif()

IF(NOT DEFINED DEBUG_CONSOLE_CONFIG)
    SET(DEBUG_CONSOLE_CONFIG "-DSDK_DEBUGCONSOLE=1")
ENDIF()

SET(CMAKE_ASM_FLAGS_DEBUG " \
    -DDEBUG \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -DGUEST \
    -g \
    -mcpu=cortex-a53 \
    -Wall \
    -Wno-address-of-packed-member \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -std=gnu99 \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_ASM_FLAGS_RELEASE " \
    -DNDEBUG \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -DGUEST \
    -mcpu=cortex-a53 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -std=gnu99 \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_ASM_FLAGS_DDR_DEBUG " \
    -DDEBUG \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -DGUEST \
    -g \
    -mcpu=cortex-a53 \
    -Wall \
    -Wno-address-of-packed-member \
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
    -mcpu=cortex-a53 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -std=gnu99 \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_ASM_FLAGS_RAM_RELEASE " \
    -DNDEBUG \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -DGUEST \
    -mcpu=cortex-a53 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -std=gnu99 \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_ASM_FLAGS_FLASH_DEBUG " \
    -DDEBUG \
    -DFLASH_TARGET \
    -D__STARTUP_CLEAR_BSS \
    -D__STARTUP_INITIALIZE_NONCACHEDATA \
    -DGUEST \
    -g \
    -mcpu=cortex-a53 \
    -Wall \
    -Wno-address-of-packed-member \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -std=gnu99 \
    ${RTOS_FLAG} \
    ${DEBUG_CONSOLE_CONFIG} \
")
SET(CMAKE_C_FLAGS_DEBUG " \
    -DDEBUG \
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=0 \
    -DFSL_RTOS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -g \
    -O0 \
    -mcpu=cortex-a53 \
    -Wall \
    -Wno-address-of-packed-member \
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
SET(CMAKE_C_FLAGS_RELEASE " \
    -DNDEBUG \
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=0 \
    -DFSL_RTOS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -O3 \
    -mcpu=cortex-a53 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
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
SET(CMAKE_C_FLAGS_DDR_DEBUG " \
    -DDEBUG \
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
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
    -mcpu=cortex-a53 \
    -Wall \
    -Wno-address-of-packed-member \
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
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
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
    -mcpu=cortex-a53 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
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
SET(CMAKE_C_FLAGS_FLASH_DEBUG " \
    -DDEBUG \
    -DFLASH_TARGET \
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=0 \
    -DFSL_RTOS_FREE_RTOS \
    -DMCUXPRESSO_SDK \
    -g \
    -O0 \
    -mcpu=cortex-a53 \
    -Wall \
    -Wno-address-of-packed-member \
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
SET(CMAKE_CXX_FLAGS_DEBUG " \
    -DDEBUG \
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
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
    -mcpu=cortex-a53 \
    -Wall \
    -Wno-address-of-packed-member \
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
SET(CMAKE_CXX_FLAGS_RELEASE " \
    -DNDEBUG \
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=0 \
    -O3 \
    -mcpu=cortex-a53 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
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
SET(CMAKE_CXX_FLAGS_DDR_DEBUG " \
    -DDEBUG \
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
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
    -mcpu=cortex-a53 \
    -Wall \
    -Wno-address-of-packed-member \
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
SET(CMAKE_CXX_FLAGS_RAM_RELEASE " \
    -DNDEBUG \
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=0 \
    -O3 \
    -mcpu=cortex-a53 \
    -Wall \
    -Werror \
    -Wno-address-of-packed-member \
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
    -DCPU_MIMX8MM6DVTLZ_ca53 \
    -DGUEST \
    -DFSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL \
    -DSERIAL_PORT_TYPE_UART=1 \
    -DPRINTF_FLOAT_ENABLE=1 \
    -DPRINTF_ADVANCED_ENABLE=1 \
    -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING \
    -DDEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    -DDEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    -DDEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=256 \
    -DDEBUG_CONSOLE_RX_ENABLE=0 \
    -O3 \
    -mcpu=cortex-a53 \
    -Wall \
    -Wno-address-of-packed-member \
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
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG " \
    -g \
    -mcpu=cortex-a53 \
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
    -T${BoardPath}/armgcc_aarch64/MIMX8MM6xxxxx_ca53_ram.ld -static \
")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE " \
    -mcpu=cortex-a53 \
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
    -T${BoardPath}/armgcc_aarch64/MIMX8MM6xxxxx_ca53_ram.ld -static \
")

if(NOT DEFINED DDR_LD_FILE)
SET(DDR_LD_FILE	"\
    ${BoardPath}/armgcc_aarch64/MIMX8MM6xxxxx_ca53_ddr_ram.ld \
")
endif()

SET(CMAKE_EXE_LINKER_FLAGS_DDR_DEBUG " \
    -g \
    -mcpu=cortex-a53 \
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
    -mcpu=cortex-a53 \
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
SET(CMAKE_EXE_LINKER_FLAGS_RAM_RELEASE " \
    -mcpu=cortex-a53 \
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
    -T${BoardPath}/armgcc_aarch64/MIMX8MM6xxxxx_ca53_ram.ld -static \
")
