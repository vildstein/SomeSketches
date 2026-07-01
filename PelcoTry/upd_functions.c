#ifndef UDP_FUNCTIONS_H
#define UDP_FUNCTIONS_H

#include "skel_defines.h"

ERROR_FORWARD_DECL
SET_ADDRESS_FORWARD_DECL

SOCKET udp_client(char* hostName, char* portName, struct sockaddr_in* sp) {

	SOCKET sock;

	set_address(hostName, portName, sp, "udp");
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if ( !IS_VALID_SOCKET(sock) ) {
		error( 1, errno, "SOCKET FUNC MISTAKE");
	}

	return sock;
}

SOCKET udp_server(char* hostName, char* portName, struct sockaddr_in* local) {

	SOCKET sock;

	set_address( hostName, portName, local, "udp" );
	sock = socket( AF_INET, SOCK_DGRAM, 0 );

	if (!IS_VALID_SOCKET( sock )) {
		error( 1, errno, "SOCKET FUNC MISTAKE");
	}

	if ( bind ( sock, (struct sockaddr*) local, sizeof(*local) ) ) {
		error( 1, errno, "BIND FUNC MISTAKE");
	}
	return sock;
}

#endif //UDP_FUNCTIONS_H
