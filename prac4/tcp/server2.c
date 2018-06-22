/*
 * Serwer echa TCP
 * 
 * Oczekuje na wiadomosc aż do otrzymania znaku "X"
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include "sockwrap.h"


// Zwraca -2 jeśli wystąpił timeout
// Zwraca -1 jeśli wystąpił błąd odczytu z gniazda (errno zawiera kod błędu)
// Zwraca 0 jeśli nadawca zamknął połączenie nie wysławszy "X".
// Zwraca liczbę przeczytanych bajtów zawierających "X" w przeciwnym przypadku.

// Uwaga: Jeśli klient wyśle kilka wierszy, to poniższa funkcja może zwrócić
// przykładowo półtora wiersza (albo trzy i pół). Kolejne wywołanie
// GetDataTillEOL nie zwróci już tych danych.
 
int GetDataTillX (int fd, char *buffer, int buffer_size, int timeout)
{
	struct timeval tv; tv.tv_sec = timeout; tv.tv_usec = 0;		// pozostały czas
	bool found = false;
	char *buff_ptr = buffer;
	int total_bytes_read = 0;
	while (total_bytes_read < buffer_size) {
		printf ("DEBUG: current value of tv = %.3f\n", tv.tv_sec + tv.tv_usec * 1.0 / 1000000);

		// Czekamy aż będzie coś w gnieździe
		fd_set descriptors;
		FD_ZERO (&descriptors);
		FD_SET (fd, &descriptors);
		int ready = Select(fd+1, &descriptors, NULL, NULL, &tv);
		if (!ready) return -2; 		// timeout

		// Odczytujemy dane z gniazda (pierwsze recv() nie zablokuje).
		int bytes_read = recv(fd, buff_ptr, buffer_size - total_bytes_read, 0);
		if (bytes_read <= 0) return bytes_read;
		char* new_buff_ptr = buff_ptr + bytes_read;
		for (; buff_ptr < new_buff_ptr; buff_ptr++)
			if (*buff_ptr == 'X') 
				found = true;

		total_bytes_read += bytes_read;
		if (found)
			return total_bytes_read;
	}
	return 0;
}
	
int main()
{
	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);   
	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(12345);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind (sockfd, &server_address, sizeof(server_address));
	Listen (sockfd, 64);        

	while (1) {	

		// accept() jak poprzednio, ale wypisujemy informacje na temat klienta
		struct sockaddr_in client_address;
		socklen_t len = sizeof(client_address);
		int conn_sockfd = Accept (sockfd, &client_address, &len);
		char ip_address[20];
		inet_ntop (AF_INET, &client_address.sin_addr, ip_address, sizeof(ip_address));
		printf ("New client %s:%d\n", ip_address, ntohs(client_address.sin_port));

		const int BUFFER_SIZE = 1000;
		char recv_buffer[BUFFER_SIZE+1];

		// Czekamy max. 10 sekund na ciąg bajtów zawierający znak 'X'.
		int n = GetDataTillX (conn_sockfd, recv_buffer, BUFFER_SIZE, 10);

		if (n == -2) {				
			printf ("DEBUG: Timeout\n");
		} else if (n == -1) {
			printf ("DEBUG: Read error: %s\n", strerror(errno));
		} else if (n == 0) {
			printf ("DEBUG: Client closed connection");
		} else {
			assert (n > 0);
			recv_buffer[n] = 0;
			printf ("Data received: ->%s<-\n", recv_buffer);

			// Odsylamy to co zapisalismy do bufora do klienta.  Uwaga: powinnismy
			// to robic podobnie jak w programie klienta, tj. wysylac dane do
			// skutku, a nie wywolywac pojedyncza funkcje send()
			Send (conn_sockfd, recv_buffer, n, 0);
		}
		
		Close (conn_sockfd);
		printf ("Disconnected\n");
	}
}

