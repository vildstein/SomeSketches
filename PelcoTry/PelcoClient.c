#include "skel_defines.h"

ERROR_FORWARD_DECL
UDP_CLIENT_FORWARD
CLIENT_FUNC_FORWARD_DECL
SET_ADDRESS_FORWARD_DECL
static void sendPelcoMessage(SOCKET s, SIN* peer);

// 192.168.2.93



int main( int argc, char** argv ) {

	SIN peer;
	SOCKET sock;

	char* host = "192.168.2.93";
	char* portNumber = "6000";

	// switch(argc) {
	// case 1:
	// 	puts("NO HOST ADDRESS OR PORT WAS PROVIDED. EXIT.\n");
	// EXIT(1);
	// case 2:
	// host = "127.0.0.1";
	// portNumber = argv[1];
	// 	break;
	// case 3:
	// 	host = argv[1];
	// 	portNumber = argv[2];
	// 	break;
	// default:
	// 	break;
	// };

	INIT();

	sock = udp_client(host, portNumber, &peer);
	sendPelcoMessage(sock, &peer);

	EXIT(0);
}

static void sendPelcoMessage(SOCKET sock, SIN* peer) {

	const int messageSize = 7; // Bytes
	char message[messageSize];

	char firstByte = 0xff;
	message[0] = firstByte;
	char deviceIdByte = 0x01; // device Id
	message[1] = deviceIdByte;

	// Command 1
	char Command_1_Byte = 0x00; // Sense Reserved Reserved Auto/ Manual Scan Camera On/Off Iris Close Iris Open Focus Near
	message[2] = Command_1_Byte;

	// Command 2
	// 0 0 0 0. 0 0 1 0
	char Command_2_Byte = 0x02; //Command 2 Focus_Far Zoom_Wide Zoom_Tele Tilt_Down Tilt_Up Pan_Left Pan_Right Fixed_to_0
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = 0x05;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = 0x00;
	message[5] = data_2_Byte;

	char checkSum_Byte = deviceIdByte + Command_1_Byte + Command_2_Byte + data_1_Byte + data_2_Byte;
	message[6] = checkSum_Byte;

	int peerlen = sizeof(*peer);;

	if ( sendto(sock, message, sizeof(message), 0, (struct sockaddr*)peer, peerlen) < 0 ) {
		error(1, errno, "SENT_TO FUNCTION MISTAKE");
	}
}



static void client(SOCKET s, SIN* peer) {
	int rc = 0;
	int peerlen;
	char buf[120];

	peerlen = sizeof(*peer);

	if ( sendto(s, "", 1, 0, (struct sockaddr*)peer, peerlen) < 0 ) {
		error(1, errno, "SENT_TO FUNCTION MISTAKE");
	}
	rc = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)peer, &peerlen);

	if (rc >= 0) {
		write( 1, buf, rc );
	} else {
		error(1, errno, "RECVFROM FUNCTION MISTAKE");
	}
}
