#include "skel_defines.h"

#include "set_addres_func.c"
#include "tcp_functions.c"
#include "error_func.c"
#include "read_n_func.c"

SERVER_FUNC_FORWARD_DECL

#define BYTE uchar

int main(int argc, char** argv) {

	INIT();

	SOCKET listenSocket;
	SOCKET connectSocket;

	char* host = NULL;
	char* port = DEFAULT_PORT; // 9000

	if (argc == 2) {
		port = argv[1];
	}

	struct sockaddr_in local;
	struct sockaddr_in from;

	printf("host defined as %s\n", host);
	printf("port defined as %s\n", port);

	listenSocket = tcp_server(host, port, &local);

	int peerLen = 0;

	do {
		socklen_t peerLen = sizeof(from);
		connectSocket = accept(listenSocket, SA &from, &peerLen);
		if (!IS_VALID_SOCKET( connectSocket )) {
			error( 1, errno, "SOCKET FUNC MISTAKE");
		}

		printf("connection accepted\n");

		char buffer[512];
		int bytesRecieved = readvrec(connectSocket, buffer, sizeof(buffer));

		if (bytesRecieved == 0 ) {
			CLOSE(connectSocket);
			printf("Client is Offline \n");
			break;
		}

	} while (TRUE);

	EXIT(0);
}


