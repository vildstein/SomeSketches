#include "skel_defines.h"

#include <stdio.h>

ERROR_FORWARD_DECL
UDP_CLIENT_FORWARD
//SOCKET udp_server(char* hostName, char* portName); //UDP_SERVER_FORWARD
UDP_SERVER_FORWARD
CLIENT_FUNC_FORWARD_DECL
SET_ADDRESS_FORWARD_DECL

static void listenFrom(SOCKET udpSocket);

void echoFunc( SOCKET sock, struct sockaddr* clientAddr, socklen_t clLen );

int main( int argc, char** argv ) {

	SOCKET listeningSocket;
	SIN servAddr;
	SIN clientAddr;

	char* host = NULL;
	char* portNumber = "7500";

	if (argc == 2) {
		portNumber = argv[1];
	} else if (argc == 3) {
		host = argv[1];
		portNumber = argv[2];
	}

	printf("port defined as %s\n", portNumber);

	INIT();

	listeningSocket = udp_server(host, portNumber, &servAddr);
	echoFunc(listeningSocket, (struct sockaddr*) &clientAddr, sizeof(clientAddr));

	EXIT(0);
}

void echoFunc( SOCKET sock, struct sockaddr* clientAddr, socklen_t clLen ) {
	int n;
	socklen_t len;
	const size_t mesgLen = 7;
	char msg[mesgLen];

	const int ZERO_FLAG = 0;

	while(TRUE) {
		len = clLen;
		n = recvfrom(sock, msg, mesgLen, ZERO_FLAG, clientAddr, &len);
		if (n < 0) {
			error(0, errno, "readvrec function mistake");
		} else {
			//if ( sendto(sock, msg, mesgLen, ZERO_FLAG, clientAddr, len) < 0 ) {
			//	error(1, errno, "SENT_TO FUNCTION MISTAKE");
			//}

			//int arrInt[mesgLen];

			for (size_t index = 0; index < mesgLen; ++index) {
				printf("%x\n", msg[index]);
			}

			//printf("%.s", msg);
			//write(1, msg, mesgLen);
			printf("\n");
		}
	}


}


static void listenFrom(SOCKET udpSocket) {

	SIN peer;
	unsigned int peerLen = 0;
	const char bufLen = 7;
	const int no_options = 0;

	int bytesRaded = 0;
	char buffer[bufLen];

	while(TRUE) {
		peerLen = sizeof( peer );
		bytesRaded = recvfrom(udpSocket, buffer, sizeof( buffer ), no_options, (struct sockaddr*) &peer, &peerLen);
		if (bytesRaded < 0) {
			error(0, errno, "readvrec function mistake");
		} else {
			write(STDOUT_FILENO, buffer, bytesRaded);
		}
	}
}
