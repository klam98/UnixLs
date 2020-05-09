CC = gcc
CFLAGS = -g
PROG = UnixLs
OBJS = UnixLs.o

UnixLs: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

UnixLs.o: UnixLs.c
	$(CC) $(CFLAGS) -c UnixLs.c

clean:
	rm -f $(OBJS) $(PROG)
