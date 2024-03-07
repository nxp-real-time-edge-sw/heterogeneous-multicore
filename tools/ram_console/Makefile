OBJS = ram_console_dump.o
MCUX_SDK_PATH=../../../mcux-sdk
TARGET=ram_console_dump

${TARGET} : $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

%.o : %.c
	$(CC) -c $< -o $@ -I${MCUX_SDK_PATH}/utilities/debug_console

clean :
	rm -rf *.o ram_console_dump
