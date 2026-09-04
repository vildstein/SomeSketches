#include "skel_defines.h"

//#include "error_func.c"

#include <pthread.h>

#define ONE_BYTE 1
#define ONE_KILO_BYTE 1024
#define ONE_MEGA_BYTE 1048576
#define forever for(;;)

void error(int status, int err, char* format, ...);
void parseArgs(int argc, char** argv);

void* countFileSize(void* filePath) {

	FILE* fd;
	long long int counted = 0;

	const char* READ_FILE_BYTES = "rb";
	if ( (fd = fopen(filePath, READ_FILE_BYTES)) == NULL ) {
		errno = ENOENT;
		error(1, errno, "unable to opne file\n");
	}

	int* buff;
	buff = (int*) malloc(500 * ONE_MEGA_BYTE);

	if (buff == NULL) {
		exit(1);
	}

	int ch = getc(fd);

	while (ch != EOF) {
		buff[counted] = ch;
		++counted;
		ch = getc(fd);
	};

	fclose(fd);

	free(buff);

	buff = NULL;

	printf("In file %s counted %lld bytes\n", (char*) filePath, counted);
	printf("thread finished\n");

	return NULL;
}



int main(int argc, char** argv) {

	parseArgs(argc, argv);

	pthread_t thread;

	const char* fileName = argv[1];

	void* threadResult;

	int pthreadStatus = pthread_create(&thread, NULL, countFileSize, (char*) fileName);
	printf("thread created with the status = %d\n", pthreadStatus);

	if (pthreadStatus != 0) {
		error(1, errno, "pthread_create FINC Mistake\n");
	}

	//pthreadStatus = pthread_join(thread, threadResult);
	pthread_detach(thread);


	if (pthreadStatus != 0) {
		error(1, errno, "pthread_join FUNC Mistake\n");
	}

	sleep(6);


	// for (size_t i = 0; i < 285000; ++i) {

	// }

	//pthread_exit(NULL);

	//char* host = "127.0.0.1";
	//char* portNumber = "7500";

	// char* fileName = argv[1];

	// FILE* fd;

	// long long int counted = 0;

	// //const char* READ_FILE = "r";
	// const char* READ_FILE_BYTES = "rb";

	// if ( (fd = fopen(fileName, READ_FILE_BYTES)) == NULL ) {
	// 	errno = ENOENT;
	// 	error(1, errno, "unable to opne file\n");
	// }

	// //fseek(fd, 0L, SEEK_END);
	// //counted = ftell(fd);

	// const int INT_SIZE_BYTE = sizeof(int); // 4 bytes
	// const int UCHAR_SIZE_BYTE = sizeof(unsigned char); // 1 byte

	// int* buff;
	// buff = (int*) malloc(500 * ONE_MEGA_BYTE);
	// //buff = (int*) malloc(109682988 + 200);

	// if (buff == NULL) {
	// 	exit(1);
	// }

	// int ch = getc(fd);

	// while (ch != EOF) {
	// 	buff[counted] = ch;
	// 	++counted;
	// 	ch = getc(fd);
	// };

	// //fseek(fd, 0L, SEEK_SET);

	// //ch = getc(fd);

	// //counted = 0;

	// //while (ch != EOF) {
	// //	buff[counted] = ch;
	// //	++counted;
	// //	ch = getc(fd);
	// //};

	// fclose(fd);

	// free(buff);

	// buff = NULL;

	// printf("In file %s counted %lld bytes\n", fileName, counted);

	return 0;
}

// char* fileName = argv[1];

// FILE* fd;

// long long int counted = 0;

// //const char* READ_FILE = "r";
// const char* READ_FILE_BYTES = "rb";

// if ( (fd = fopen(fileName, READ_FILE_BYTES)) == NULL ) {
// 	errno = ENOENT;
// 	error(1, errno, "unable to opne file\n");
// }

// //fseek(fd, 0L, SEEK_END);
// //counted = ftell(fd);

// const int INT_SIZE_BYTE = sizeof(int); // 4 bytes
// const int UCHAR_SIZE_BYTE = sizeof(unsigned char); // 1 byte

// int* buff;
// buff = (int*) malloc(500 * ONE_MEGA_BYTE);
// //buff = (int*) malloc(109682988 + 200);

// if (buff == NULL) {
// 	exit(1);
// }

// int ch = getc(fd);

// while (ch != EOF) {
// 	buff[counted] = ch;
// 	++counted;
// 	ch = getc(fd);
// };

// //fseek(fd, 0L, SEEK_SET);

// //ch = getc(fd);

// //counted = 0;

// //while (ch != EOF) {
// //	buff[counted] = ch;
// //	++counted;
// //	ch = getc(fd);
// //};

// fclose(fd);

// free(buff);

// buff = NULL;

// printf("In file %s counted %lld bytes\n", fileName, counted);
