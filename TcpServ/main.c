#include "skel_defines.h"

#include "set_addres_func.c"
#include "tcp_functions.c"
#include "error_func.c"

SERVER_FUNC_FORWARD_DECL

int main(int argc, char** argv) {

	INIT();

	SOCKET masterSocket;
	SOCKET slaveSocket; // listening
	char* host = NULL;
	char* port = DEFAULT_PORT; // 9000

	if (argc == 2) {
		port = argv[1];
	}

	struct sockaddr_in local;
	struct sockaddr_in from;

	printf("host defined as %s\n", host);
	printf("port defined as %s\n", port);

	masterSocket = tcp_server(host, port, &local);

	int peerLen = 0;

	do {
		peerLen = sizeof(from);
		slaveSocket = accept(masterSocket, SA &from, (socklen_t*) &peerLen);
		printf("connection accepted");

		if (!IS_VALID_SOCKET( slaveSocket )) {
			error( 1, errno, "SOCKET FUNC MISTAKE");
		}

	} while (TRUE);



	EXIT(0);
}


