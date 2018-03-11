# Wojciech Fica
# 280180

CC = gcc
CFLAGS  =  -std=gnu99 -Wall -Wextra

dafault: tracer

tracer: utils.o traceroute.o  sending.o receiving.o
	$(CC) $(CFLAGS) -o traceroute utils.o   sending.o receiving.o traceroute.o

traceroute.o: traceroute.c utils.h
	$(CC) $(CFLAGS) -c traceroute.c

utils.o: utils.c 
	$(CC) $(CFLAGS) -c utils.c

sending.o: sending.c utils.h
	$(CC) $(CFLAGS) -c sending.c

receiving.o: receiving.c utils.h
	$(CC) $(CFLAGS) -c receiving.c

clean:	
	$(RM) *.o *~

distclean:	
	$(RM) traceroute *.o *~