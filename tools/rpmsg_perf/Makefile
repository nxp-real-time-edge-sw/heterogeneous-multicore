OBJS = rpmsg_perf.o
TARGET=rpmsg_perf

${TARGET} : $(OBJS)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) -c $< -o $@

clean :
	rm -rf *.o rpmsg_perf
