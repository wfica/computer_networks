# Wojciech Fica
# 280180

CC = gcc
CFLAGS  = -Wall -Wextra

dafault: tracer

tracer: traceroute.o utils.o
	$(CC) $(CFLAGS) -o traceroute traceroute.o utils.o

traceroute.o: traceroute.c utils.h
	$(CC) $(CFLAGS) -c traceroute.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:	
	$(RM) traceroute *.o *~