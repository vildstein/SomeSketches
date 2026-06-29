#ifndef READ_N_FUNC_H
#define READ_N_FUNC_H

#include "skel_defines.h"

// unsigned long int ntohl, unsigned short int ntol ntohs преобразует двоичные данные из сетевого порядка следования байтов в серверный.
// https://www.opennet.ru/man.shtml?topic=ntohl&category=3&russian=0

int readn(SOCKET socDescriptor, char* bufferToRead, size_t messageLenght) {

	int count = 0;
	int recived = 0;

	count = messageLenght;

	while (count > 0) {
		recived = recv(socDescriptor, bufferToRead,  count, 0);

		if (recived < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}

		if (recived == 0) {
			return 	messageLenght - count;
		}

		bufferToRead += recived;
		count -= recived;

	}

	return messageLenght;
}

// Для чтения сообщения переменной длинны
int readvrec(SOCKET socDescriptor, char* bufferToRead, size_t messageLenght) {

	u_int32_t recLenght = 0;
	int recived = 0;

	// read record lenght
	recived = readn(socDescriptor, bufferToRead, messageLenght);

	if (recived != sizeof(u_int32_t)) {
		return recived < 0 ? -1 : 0;
	}

	recLenght = ntohl(recLenght);
	if (recLenght > messageLenght) {

		// Not enought free space in buffer
		// return errno*/
		while (recLenght > 0) {
			recived = readn(socDescriptor, bufferToRead, messageLenght);
			if (recived != messageLenght) {
				return recived < 0 ? -1 : 0;
			}
			recLenght -= messageLenght;
			if (recLenght < messageLenght) {
				messageLenght = recLenght;
			}
		}
		// set_errno(EMSGSIZE);
		return -1;
	}

	// read the messgae
	recived = readn(socDescriptor, bufferToRead, messageLenght);
	if (recived != recLenght) {
		return recived < 0 ? -1 : 0;
	}

	return recLenght;
}

#endif //ERROR_FUNC_H
