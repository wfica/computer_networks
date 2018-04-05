# Wojciech Fica
# 280180

CC = g++
CFLAGS  =  -std=gnu++11 -Wall -Wextra

dafault: main

main: main.cpp utils.o receive.o send.o
	$(CC) $(CFLAGS) utils.o receive.o send.o main.cpp -o router

utils.o: utils.cpp utils.h
	$(CC) $(CFLAGS) -c utils.cpp

receive.o: utils.h receive.h
	$(CC) $(CFLAGS) -c receive.cpp

send.o: utils.h send.h
	$(CC) $(CFLAGS) -c send.cpp

clean:	
	$(RM) -f *.o *~

distclean:	
	$(RM) -f router *.o *~