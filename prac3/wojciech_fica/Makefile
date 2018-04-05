# Wojciech Fica
# 280180

CC = g++
CFLAGS  =  -O2 -std=gnu++11 -Wall -Wextra -Wfloat-equal -Wundef -Wunreachable-code

dafault: main

main: transport.cpp
	$(CC) $(CFLAGS) transport.cpp -o transport

clean:	
	$(RM) -f *.o *~

distclean:	
	$(RM) -f transport *.o *~