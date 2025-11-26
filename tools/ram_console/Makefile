OBJS = ram_console_dump.o
MCUX_SDK_PATH=../../../mcuxsdk/mcuxsdk
TARGET=ram_console_dump

${TARGET} : $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

%.o : %.c
	$(CC) -c $< -o $@ -I${MCUX_SDK_PATH}/components/debug_console

clean :
	rm -rf *.o ram_console_dump
