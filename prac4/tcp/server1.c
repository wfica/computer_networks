/*
 * Serwer echa TCP
 * 
 * Oczekuje na wiadomosc i odsyla komunikat kontrolny
 */

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "sockwrap.h"

#define BUFFSIZE 10000000	// 10 MB
char buffer[BUFFSIZE+1];

int main()
{
	// TCP: STREAM zamiast DGRAM 
	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);	

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(12345);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind (sockfd, &server_address, sizeof(server_address));

	// TCP: rozmiar kolejki na trzymanie zadan polaczenia 
	Listen (sockfd, 64);	

	while (1) {

		// Funkcja accept() blokuje do momentu kiedy jest juz jakies połączenie
		// do obslugi, zwraca gniazdo połączone (do komunikacji). Nie
		// potrzebujemy informacji na temat IP i portu klienta, bo mamy juz z
		// nim polaczenie przez gniazdo conn_sockfd.

		int conn_sockfd = Accept (sockfd, NULL, NULL);

		// Od tego momentu mamy dwa gniazda: sockfd i conn_sockfd, 
		// W sensownych programach powinnismy je obslugiwac w miare rownoczesnie
	
		// PROBLEM: Mamy strumien danych i nie wiemy, kiedy sie on konczy
		// Tutaj po prostu czekamy na do BUFFSIZE bajtow od klienta 
		// recv(_,_,_,0) = read(_,_,_). Przy UDP bylo recvfrom, przy TCP jest recv()

		int n = Recv(conn_sockfd, buffer, BUFFSIZE, 0);
		int msglen = n < 20 ? n : 20;
		buffer[msglen] = 0;

		char reply_msg[100];
		sprintf (reply_msg, "%d bytes read, first 20 bytes: ->%s<-", n, buffer);
		printf ("%s\n", reply_msg);

		// Wysylanie odpowiedzi
		// send(_,_,_,0) = write(_,_,_). Przy UDP bylo sendto, przy TCP jest send().
		Send (conn_sockfd, reply_msg, strlen(reply_msg), 0);

		// Konczymy komunikacje z tym gniazdem, wracamy do obslugi gniazda
		// nasluchujacego.
		Close (conn_sockfd);
	}
}

