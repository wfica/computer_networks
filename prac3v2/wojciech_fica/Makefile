# Wojciech Fica
# 280180

CC = g++
CFLAGS  =  -O2 -std=gnu++11 -Wall -Wextra -Wfloat-equal -Wundef -Wunreachable-code

default: transport.cpp  utils.o 
	$(CC) $(CFLAGS) utils.o transport.cpp -o transport

utils.o: utils.cpp utils.h
	$(CC) $(CFLAGS) -c utils.cpp

clean:	
	$(RM) *.o *~

distclean:	
	$(RM) transport *.o *~
