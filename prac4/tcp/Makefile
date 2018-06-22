CC = gcc 
CFLAGS = -O2 -Wall -W -std=gnu99 -Wshadow
TARGETS = server1 client1 client2 server2
 
all: $(TARGETS)

server1: server1.o sockwrap.o

server2: server2.o sockwrap.o

client1: client1.o sockwrap.o

client2: client2.o sockwrap.o

clean:
	rm -f *.o

distclean: clean
	rm -f $(TARGETS)
