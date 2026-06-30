#include "skel_defines.h"

ERROR_FORWARD_DECL
UDP_CLIENT_FORWARD
CLIENT_FUNC_FORWARD_DECL
SET_ADDRESS_FORWARD_DECL

static void goLeftMessage(SOCKET sock, SIN* peer);
static void goRightMessage(SOCKET sock, SIN* peer);
static void stopMessage(SOCKET sock, SIN* peer);

int main( int argc, char** argv ) {

	SIN peer;
	SOCKET sock;

	char* host = "192.168.2.93";
	char* portNumber = "6000";

	INIT();

	sock = udp_client(host, portNumber, &peer);

	char ch;

	do {
		ch = getchar();

		if (ch == 'a') {
			goLeftMessage(sock, &peer);
		} else if (ch == 'd') {
			goRightMessage(sock, &peer);
		} else if (ch == 's') {
			stopMessage(sock, &peer);
		}

	} while (ch != 'z' || ch != 'x');

	EXIT(0);
}

static void goLeftMessage(SOCKET sock, SIN* peer) {

	const size_t messageSize = 7; // Bytes
	char message[messageSize];

	char firstByte = 0xff;
	message[0] = firstByte;
	char deviceIdByte = 0x01; // device Id
	message[1] = deviceIdByte;

	// Command 1
	// Sense Reserved Reserved Auto/ Manual Scan Camera On/Off Iris Close Iris Open Focus Near
	char Command_1_Byte = 0x00;
	message[2] = Command_1_Byte;

	// Command 2
	// 0 0 0 0. 0 1 0 0
	//Command 2 Focus_Far Zoom_Wide Zoom_Tele Tilt_Down Tilt_Up Pan_Left Pan_Right Fixed_to_0
	char Command_2_Byte = 0x4;
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = 0x3F;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = 0x00;
	message[5] = data_2_Byte;

	char checkSum_Byte = deviceIdByte + Command_1_Byte + Command_2_Byte + data_1_Byte + data_2_Byte;
	message[6] = checkSum_Byte;

	int peerlen = sizeof(*peer);

	const int ZERO_FLAG = 0;

	if ( sendto(sock, message, sizeof(message), ZERO_FLAG, (struct sockaddr*)peer, peerlen) < 0 ) {
		error(1, errno, "SENT_TO FUNCTION MISTAKE");
	}
}

static void goRightMessage(SOCKET sock, SIN* peer) {

	const size_t messageSize = 7; // Bytes
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
	char data_1_Byte = 0x3F;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = 0x00;
	message[5] = data_2_Byte;

	char checkSum_Byte = deviceIdByte + Command_1_Byte + Command_2_Byte + data_1_Byte + data_2_Byte;
	message[6] = checkSum_Byte;

	int peerlen = sizeof(*peer);

	const int ZERO_FLAG = 0;

	if ( sendto(sock, message, sizeof(message), ZERO_FLAG, (struct sockaddr*)peer, peerlen) < 0 ) {
		error(1, errno, "SENT_TO FUNCTION MISTAKE");
	}


}

static void stopMessage(SOCKET sock, SIN* peer) {

	const size_t messageSize = 7; // Bytes
	char message[messageSize];

	char firstByte = 0xff;
	message[0] = firstByte;
	char deviceIdByte = 0x01; // device Id
	message[1] = deviceIdByte;

	// Command 1
	char Command_1_Byte = 0x00; // Sense Reserved Reserved Auto/ Manual Scan Camera On/Off Iris Close Iris Open Focus Near
	message[2] = Command_1_Byte;

	// Command 2
	// 0 0 0 0. 0 0 0 0
	char Command_2_Byte = 0x00; //Command 2 Focus_Far Zoom_Wide Zoom_Tele Tilt_Down Tilt_Up Pan_Left Pan_Right Fixed_to_0
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = 0x00;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = 0x00;
	message[5] = data_2_Byte;

	char checkSum_Byte = deviceIdByte + Command_1_Byte + Command_2_Byte + data_1_Byte + data_2_Byte;
	message[6] = checkSum_Byte;

	int peerlen = sizeof(*peer);

	const int ZERO_FLAG = 0;

	if ( sendto(sock, message, sizeof(message), ZERO_FLAG, (struct sockaddr*)peer, peerlen) < 0 ) {
		error(1, errno, "SENT_TO FUNCTION MISTAKE");
	}

}
