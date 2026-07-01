#include "skel_defines.h"

ERROR_FORWARD_DECL
UDP_CLIENT_FORWARD
UDP_SERVER_FORWARD
CLIENT_FUNC_FORWARD_DECL
SET_ADDRESS_FORWARD_DECL
READ_V_REC_FORWARD

int main( int argc, char** argv ) {

	SOCKET listeningSocket;
	SIN server;

	char* host = NULL;
	char* portNumber = "6000";
	const int buffSize = 7;
	char buffer[buffSize];
	int readed = 0;

	listeningSocket = udp_server(NULL, portNumber, &server);


	while (TRUE) {
		readed = readvrec( listeningSocket, buffer, sizeof(buffer) );
		if (readed < 0) {
			error(0, errno, "readvrec function mistake");
		} else if (readed == 0) {
			error(0, errno, "client gone");
		} else {
			for (int i = 0; i < buffSize; ++i) {
				printf("%c\n", buffer[i]);
			}
		}

	}



	EXIT(0);
}
