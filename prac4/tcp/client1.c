/* 
 * Klient TCP, wersja 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "sockwrap.h"

#define BUFFSIZE 4096

int main (int argc, char* argv[])
{
	if (argc != 2) { printf ("Usage: client <nbytes>\n"); exit(1); }

	// TCP: STREAM zamiast DGRAM
	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);	

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family	= AF_INET;
	server_address.sin_port		= htons(12345);
	inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

	// TCP: trzeba nawiazac polaczenie, ale potem juz dane wysylamy 
	// bez podawania kazdorazowo adresu IP i portu.
	Connect (sockfd, &server_address, sizeof(server_address));	

	int n = atoi(argv[1]); // brak kontroli wartosci n!
	char sending_buffer[n+1];
	for (int i=0; i<n; i++) { sending_buffer[i] = '0' + i%10; }
	Send (sockfd, sending_buffer, n, 0);

	// Czytamy tylko BUFFSIZE bajtow z tego co wysyla nam serwer
	char receiving_buffer[BUFFSIZE+1];
	Recv (sockfd, receiving_buffer, BUFFSIZE, 0);
	printf ("server reply: %s\n", receiving_buffer);
	
	Close (sockfd);
}
	
