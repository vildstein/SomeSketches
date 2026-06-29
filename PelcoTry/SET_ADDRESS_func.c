#ifndef SET_ADDRESS_FUNC_H
#define SET_ADDRESS_FUNC_H

#include "skel_defines.h"

void error(int status, int err, char* format, ...);

void set_address(char* hostName, char* portNumber, SIN* sap, char* protocol) {

	struct servent* sp;
	struct hostent* hp;
	char* endPtr;
	short port;

	BZERO_SOCK_ADDR_IN_STRUCT(sap)

	sap->sin_family = AF_INET;

	if ( hostName != NULL ) {

		if ( !inet_aton( hostName, &sap->sin_addr ) ) {
			hp = gethostbyname(hostName);
			printf("hostName = %s \n", hostName);

			if (hp == NULL) {
				error(1, 0, "unknown host: %s\n", hostName);
			}
			sap->sin_addr = *(struct in_addr*) hp->h_addr;
		}

	} else {
		sap->sin_addr.s_addr = htonl( INADDR_ANY );
	}

	port = (short)strtol(portNumber, &endPtr, 0 );

	if (*endPtr == '\0') {
		sap->sin_port = htons( port );
	} else {
		sp = getservbyname( portNumber, protocol);
		if (sp == NULL) {
			error(1, 0, "unknown service: %s\n", portNumber);
		}
		sap->sin_port = sp->s_port;
	}
}

#endif //SET_ADDRESS_FUNC_H
