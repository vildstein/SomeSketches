#include "skel_defines.h"

#include <stdio.h>

ERROR_FORWARD_DECL
UDP_CLIENT_FORWARD
UDP_SERVER_FORWARD
CLIENT_FUNC_FORWARD_DECL
SET_ADDRESS_FORWARD_DECL


int main( int argc, char** argv ) {

	SOCKET listeningSocket;
	SIN server;

	char* host = NULL;
	char* portNumber = "5999";
	const int buffSize = 7;
	char buffer[buffSize];
	int readed = 0;

	const int no_options = 0;

	listeningSocket = udp_server(host, portNumber, &server);

	size_t servLen = sizeof(server);

	while (TRUE) {
		readed = recvfrom(listeningSocket, buffer, sizeof( buffer ), no_options, (struct sockaddr*) &server, &servLen);
		if (readed < 0) {
			error(0, errno, "readvrec function mistake");
		} else if (readed == 0) {
			error(0, errno, "client gone");
		} else {
			write(STDOUT_FILENO, buffer, readed);
		}
	}

	EXIT(0);
}
