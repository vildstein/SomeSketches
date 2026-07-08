#include "skel_defines.h"

#include <termio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdarg.h>
#include <syslog.h>
#include <strings.h>
#include <string.h>

ERROR_FORWARD_DECL
UDP_CLIENT_FORWARD
UDP_SERVER_FORWARD
CLIENT_FUNC_FORWARD_DECL
SET_ADDRESS_FORWARD_DECL

#define INIT_DATA const size_t messageSize = 7; \
	char message[messageSize]; \
	char firstByte = FIRST_BYTE_HEADER; \
	message[0] = firstByte; \
	char deviceIdByte = DEVICE_ID_BYTE; \
	message[1] = deviceIdByte;

#define COMMAND_1_ZERO_BYTE char Command_1_Byte = ZERO_BYTE; \
	message[2] = Command_1_Byte;

#define SEND_TO int peerlen = sizeof(*peer); \
	const int ZERO_FLAG = 0; \
	if ( sendto(sock, message, sizeof(message), ZERO_FLAG, (struct sockaddr*)peer, peerlen) < 0 ) { \
		error(1, errno, "SENT_TO FUNCTION MISTAKE"); \
	}

#define INIT_CHECK_SUMM char checkSum_Byte = deviceIdByte + Command_1_Byte + Command_2_Byte + data_1_Byte + data_2_Byte; \
	message[6] = checkSum_Byte;

#define MAXLINE 4048
int daemon_proc;

static void goLeftMessage(SOCKET sock, SIN* peer);
static void goRightMessage(SOCKET sock, SIN* peer);
static void goUpMessage(SOCKET sock, SIN* peer);
static void goDownMessage(SOCKET sock, SIN* peer);
static void stopMessage(SOCKET sock, SIN* peer);
static void setZeroPosition(SOCKET sock, SIN* peer);
static void setPanPositionMessage(SOCKET sock, SIN* peer);
static void queryDiagnosticInformationResponseMessage(SOCKET sock, SIN* peer);
static void queryPanPositionMessage(SOCKET sock, SIN* peer);

static void tryToListen(SOCKET sock, const char* const query);

static void readPanPositionAnswer(SOCKET sock);

static struct termios save_termios;
static int ttysavefd = -1;
static enum { RESET, RAW, CBREAK } ttystate = RESET;

/* перевести терминал в режим посимвольного ввода */
int tty_cbreak(int fd)
{
	int err;
	struct termios buf;
	if (ttystate != RESET) {
		errno = EINVAL;
		return(-1);
	}
	if (tcgetattr(fd, &buf) < 0) {
		return(-1);
	}
	save_termios = buf; /* копия структуры */

	/* Отключить эхо-вывод и выйти из канонического режима. */
	{
		int res = (ECHO | ICANON);
		res = ~res;
		buf.c_lflag &= res;
	}
	/* Случай Б: минимум 1 байт, время ожидания не ограничено. */

	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0) {
		return(-1);
	}
 /*
 * Убедиться, что были произведены все изменения. Функция tcsetattr может
 * вернуть 0, даже если выполнена лишь часть изменений.
 */
	if (tcgetattr(fd, &buf) < 0) {
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = err;
		return(-1);
	}
	if ((buf.c_lflag & (ECHO | ICANON)) || buf.c_cc[VMIN] != 1 ||buf.c_cc[VTIME] != 0) {
		/*
		* Были произведены лишь некоторые изменения.
		* Восстановить начальные настройки.
		*/
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = EINVAL;
		return(-1);
	}
	ttystate = CBREAK;
	ttysavefd = fd;
	return(0);
}


int tty_raw(int fd)  {

	int err;
	struct termios buf;

	if (ttystate != RESET) {
		errno = EINVAL;
		return(-1);
	}

	if (tcgetattr(fd, &buf) < 0) {
		return(-1);
	}

	save_termios = buf; /* копия структуры */
	/*
	* Отключить эхо-вывод, выйти из канонического режима, отключить расширенную
	* обработку ввода, отключить обработку символов, генерирующих сигналы.
	*/

	{
		int res = (ECHO | ICANON | IEXTEN | ISIG);
		res = ~res;
		buf.c_lflag &= res;
	}
	/*
	* Не выдавать сигнал SIGINT по псевдосимволу BREAK, отключить
	* преобразование CR->NL, отключить проверку паритета ввода,
	* не сбрасывать 8-й бит, отключить управление выводом.
	*/
	{
		int res = (BRKINT | ICRNL | INPCK | ISTRIP | IXON);
		res = ~res;
		buf.c_iflag &= res;
	}
	/*
	* Сбросить маску управления размером, отключить контроль четности.
	*/
	{
		int res = (CSIZE | PARENB);
		res = ~res;
		buf.c_cflag &= res;
	}
	 /*
	* Установить размер символа 8 бит/символ.
	*/
	buf.c_cflag |= CS8;
	/*
	* Отключить обработку вывода.
	*/
	{
		int res = (OPOST);
		res = ~res;
		buf.c_oflag &= res;
	}
	/*
	* Случай Б: минимум 1 байт, время ожидания не ограничено.
	*/
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0) {
		return(-1);
	}
	/*
	* Убедиться, что были произведены все изменения. Функция tcsetattr может
	* вернуть 0, даже если выполнена лишь часть изменений.
	*/
	if (tcgetattr(fd, &buf) < 0) {
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = err;
		return(-1);
	}

	if ((buf.c_lflag & (ECHO | ICANON | IEXTEN | ISIG)) || (buf.c_iflag &
						(BRKINT | ICRNL | INPCK | ISTRIP | IXON)) || (buf.c_cflag & (CSIZE | PARENB | CS8)) != CS8 ||
						(buf.c_oflag & OPOST) || buf.c_cc[VMIN] != 1 || buf.c_cc[VTIME] != 0) {

		/* Были произведены лишь некоторые изменения.
		 * Восстановить начальные настройки.
		*/
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = EINVAL;
		return(-1);
	}

	ttystate = RAW;
	ttysavefd = fd;

	return(0);
}

/* восстановить состояние терминала */
int tty_reset(int fd)
{
	if (ttystate == RESET) {
		return(0);
	}

	if (tcsetattr(fd, TCSAFLUSH, &save_termios) < 0) {
		return(-1);
	}
	ttystate = RESET;
	return(0);
}

static void err_doit(int errnoflag, int level, const char* fmt, va_list ap) {

	int errno_save, n;
	char buf[MAXLINE + 1];

	errno_save = errno;             /* value caller might want printed */
#ifdef  HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap);       /* safe */
#else
	vsprintf(buf, fmt, ap);                 /* not safe */
#endif
	n = strlen(buf);
	if (errnoflag) {
		snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
	}

	strcat(buf, "\n");
	if (daemon_proc) {
		syslog(level, buf);\
	} else {
		fflush(stdout);         /* in case stdout and stderr are the same */
		fputs(buf, stderr);
		fflush(stderr);
	}
	return;
}

void err_sys(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);

	exit(1);
}

/* позволить вызывающему процессу */
/* узнать начальное состояние терминала */
struct termios *tty_termios(void)
{
	return(&save_termios);
}

static void sig_catch(int signo) {
	printf("перехвачен сигнал\n");
	tty_reset(STDIN_FILENO);
	exit(0);
}

static int isStoped = TRUE;

// "192.168.2.93"

static int panSpeedInitial = 0x3F;

static const char ZERO_BYTE = 0x00;
static const char FIRST_BYTE_HEADER = 0xFF;
static const char DEVICE_ID_BYTE = 0x01;

static const char SET_PAN_POSITION = 0x4b;
static const char QUERY_DIAGNOSTIC_INFORMATIOM_RESPONSE = 0x71;
static const char QUERY_PAN_POSITION = 0x51;
static const char QUERY_TILT_POSITION_BYTE = 0x53;

static const char QUERY_PAN_POSITION_RESPONSE = 0x59;

void plusPanSpeed() {
	++panSpeedInitial;
	printf("Pan speed is dec = %d, HEX = %x\n", panSpeedInitial, panSpeedInitial);
}

void minusPanSpeed() {
	--panSpeedInitial;
	printf("Pan speed is dec = %d, HEX = %x\n", panSpeedInitial, panSpeedInitial);
}

int main( int argc, char** argv ) {

	SIN client;
	SIN peer;
	SOCKET sock;

	//char* host = "192.168.2.93";
	char* host = "127.0.0.1";
	char* portNumber = "7500";

	if (argc == 2) {
		portNumber = argv[1];
	} else if (argc == 3) {
		host = argv[1];
		portNumber = argv[2];
	}

	printf("host defined as %s\n", host);
	printf("port defined as %s\n", portNumber);

	const char* const left = "Left";
	const char* const right = "Right";
	const char* const up = "Up";
	const char* const down = "Down";
	const char* const query_Diagnostic_Information_Response = "Query Diagnostic Information Response";
	const char* const setPanPos = "Set Pan position";
	const char* const query_Pan_Position = "Query Pan Position";
	const char* const queryRecivedPanPositionStatus = "Recived Query Pan Position\n";

	INIT();

	/* включить обработку сигналов */
	if (signal(SIGINT, sig_catch) == SIG_ERR) {
		err_sys("ошибка вызова функции signal(SIGINT)");
	}

	if (signal(SIGQUIT, sig_catch) == SIG_ERR) {
		err_sys("ошибка вызова функции signal(SIGQUIT)");
	}

	if (signal(SIGTERM, sig_catch) == SIG_ERR) {
		err_sys("ошибка вызова функции signal(SIGTERM)");
	}

	sock = udp_client(host, portNumber, &client);

	int i;
	char ch;

	// настройка терминала
	if (tty_cbreak(STDIN_FILENO) < 0) {
		err_sys("ошибка вызова функции tty_cbreak");
	}

	while ((i = read(STDIN_FILENO, &ch, 1)) == 1) {
		if (ch == 'a' || ch == 'A') {
			printf("%s\n", left);
			goLeftMessage(sock, &client);
		} else if (ch == 'd' || ch == 'D') {
			printf("%s\n", right);
			goRightMessage(sock, &client);
		} else if (ch == 'w' || ch == 'W') {
			printf("%s\n", up);
			goUpMessage(sock, &client);
		} else if (ch == 's' || ch == 'S') {
			printf("%s\n", down);
			goDownMessage(sock, &client);
		} else if (ch == 43 || ch == 61) {
			printf("PLUS Pan Speed");
			plusPanSpeed();
		} else if (ch == 45) {
			printf("MINUS Pan Speed");
			minusPanSpeed();
		} else if (ch == 'z' || ch == 'Z') {
			printf("%s\n", query_Diagnostic_Information_Response);
			//setZeroPosition(sock, &client);
		} else if (ch == 'p' || ch == 'P') {
			printf("%s\n", setPanPos);
			setPanPositionMessage(sock, &client);
		} else if (ch == 'x' || ch == 'X') {
			printf("%s\n", query_Pan_Position);
			queryPanPositionMessage(sock, &client);
			readPanPositionAnswer(sock);
		} else {
			stopMessage(sock, &client);
			ch &= 255;
			if (ch == 27) {
				EXIT(0);
			}
			printf("%o\n", ch);
		}
	}

	EXIT(0);
}

static void goLeftMessage(SOCKET sock, SIN* peer) {

	INIT_DATA

	// Command 1
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]
	COMMAND_1_ZERO_BYTE
	// Command 2
	// 0 0 0 0. 0 1 0 0
	// Focus_Far[7] Zoom_Wide[6] Zoom_Tele[5] Tilt_Down[4] Tilt_Up[3] Pan_Left[2] Pan_Right[1] Fixed_to_0[0]
	char Command_2_Byte = 0x4;
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = (char) panSpeedInitial;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = ZERO_BYTE;
	message[5] = ZERO_BYTE;

	INIT_CHECK_SUMM
	SEND_TO
}

static void goRightMessage(SOCKET sock, SIN* peer) {

	INIT_DATA
	// Command 1
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]

	char Command_1_Byte = ZERO_BYTE;
	message[2] = Command_1_Byte;

	// Command 2
	// 0 0 0 0. 0 0 1 0
	// Focus_Far[7] Zoom_Wide[6] Zoom_Tele[5] Tilt_Down[4] Tilt_Up[3] Pan_Left[2] Pan_Right[1] Fixed_to_0[0]
	char Command_2_Byte = 0x02;
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = (char) panSpeedInitial;;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = 0x00;
	message[5] = data_2_Byte;

	INIT_CHECK_SUMM

	SEND_TO
}

static void goUpMessage(SOCKET sock, SIN* peer) {

	INIT_DATA

	// Command 1
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]
	COMMAND_1_ZERO_BYTE
	// Command 2
	// Focus_Far[7] Zoom_Wide[6] Zoom_Tele[5] Tilt_Down[4] Tilt_Up[3] Pan_Left[2] Pan_Right[1] Fixed_to_0[0]
	// 0 0 0 1. 0 0 0 0
	char Command_2_Byte = 0x10;
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = ZERO_BYTE;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = 0x2d;
	message[5] = data_2_Byte;

	INIT_CHECK_SUMM

	SEND_TO
}

static void goDownMessage(SOCKET sock, SIN* peer) {

	INIT_DATA

	// Command 1
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]
	COMMAND_1_ZERO_BYTE
	// Command 2
	// Focus_Far[7] Zoom_Wide[6] Zoom_Tele[5] Tilt_Down[4] Tilt_Up[3] Pan_Left[2] Pan_Right[1] Fixed_to_0[0]
	// 0 0 0 0. 1 0 0 0
	char Command_2_Byte = 0x08;
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = 0x00;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = 0x2d;
	message[5] = data_2_Byte;

	INIT_CHECK_SUMM

	SEND_TO
}

// Motion
static void stopMessage(SOCKET sock, SIN* peer) {

	INIT_DATA

	// Command 1
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]
	COMMAND_1_ZERO_BYTE
	// Command 2	
	// Focus_Far[7] Zoom_Wide[6] Zoom_Tele[5] Tilt_Down[4] Tilt_Up[3] Pan_Left[2] Pan_Right[1] Fixed_to_0[0]
	// 0 0 0 0. 0 0 0 0
	char Command_2_Byte = 0x00;
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = 0x00;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = 0x00;
	message[5] = data_2_Byte;

	INIT_CHECK_SUMM

	SEND_TO

}


static void setPanPositionMessage(SOCKET sock, SIN* peer) {

	INIT_DATA
	COMMAND_1_ZERO_BYTE

	// Response 2
	char Command_2_Byte = SET_PAN_POSITION;
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = ZERO_BYTE;
	message[4] = data_1_Byte;

	char data_2_Byte = ZERO_BYTE;
	message[5] = data_2_Byte;

	INIT_CHECK_SUMM
	SEND_TO

}

static void setZeroPosition(SOCKET sock, SIN* peer) {

	INIT_DATA
	COMMAND_1_ZERO_BYTE

	// Command 2
	char Command_2_Byte = 0x49;
	message[3] = Command_2_Byte;
	// Pan speed
	char data_1_Byte = ZERO_BYTE;
	message[4] = data_1_Byte;

	char data_2_Byte = ZERO_BYTE;
	message[5] = data_2_Byte;

	INIT_CHECK_SUMM
	SEND_TO
}


static void queryDiagnosticInformationResponseMessage(SOCKET sock, SIN* peer) {

	INIT_DATA
	// Response 1
	COMMAND_1_ZERO_BYTE
	// Response 2
	char Command_2_Byte = QUERY_DIAGNOSTIC_INFORMATIOM_RESPONSE; //0x71
	message[3] = Command_2_Byte;
	// Pan speed
	char data_1_Byte = ZERO_BYTE;
	message[4] = data_1_Byte;

	char data_2_Byte = ZERO_BYTE;
	message[5] = data_2_Byte;

	INIT_CHECK_SUMM

	SEND_TO
}

// (ch == 'x' || ch == 'X')
static void queryPanPositionMessage(SOCKET sock, SIN* peer) {
	INIT_DATA
	// Response 1
	COMMAND_1_ZERO_BYTE

	char Command_2_Byte = QUERY_PAN_POSITION;
	message[3] = Command_2_Byte;
	// Pan speed
	char data_1_Byte = ZERO_BYTE;
	message[4] = data_1_Byte;

	char data_2_Byte = ZERO_BYTE;
	message[5] = data_2_Byte;

	INIT_CHECK_SUMM
	SEND_TO
}

static void tryToListen(SOCKET sock, const char* const query) {

	const int ZERO_FLAG = 0;
	const size_t mesgLen = 7;
	char msg[mesgLen];
	int recieved = 0;

	recieved = recvfrom(sock, msg, mesgLen, ZERO_FLAG, NULL, NULL);

	if (recieved > 0) {

		printf("%s\n", query);

		for (size_t index = 0; index < mesgLen; ++index) {
			printf("HEX = %x, DEC = %d\n", msg[index], msg[index]);
		}
	}

	printf("\n");
}
// (ch == 'x' || ch == 'X')
static void readPanPositionAnswer(SOCKET sock) {
	const int ZERO_FLAG = 0;
	const size_t mesgLen = 7;
	char msg[mesgLen];
	int recieved = 0;

	recieved = recvfrom(sock, msg, mesgLen, ZERO_FLAG, NULL, NULL);

	if (recieved > 0) {
		char ms = msg[4];
		printf("msg[4] = %d ... %x ... %b\n", ms, ms, ms);
		short angleMSb = (short)ms;
		printf("msg[4] = %d ... %x ... %b\n", angleMSb, angleMSb, angleMSb);
		angleMSb <<= 8;
		printf("angleMSb <<= 8 = %d ... %x ... %b\n", angleMSb, angleMSb , angleMSb);
		unsigned char lb = msg[5];
		printf("msg[5] = %d ... %x ...  %b\n", lb, lb, lb);
		short angleLSb = (short)lb;
		printf("angleLSb = %d ... %x ... %b\n", angleLSb, angleLSb, angleLSb);

		//angleLSb >>= 8;
		short angle = angleMSb ^ angleLSb;
		printf("%d\n", angle);
		angle /= 1000;
		printf("angle ^ = %d ... %b\n", angle, angle);
	} else {
		const char* errorMesg = "Nothing recieved";
		printf("%s\n", errorMesg);
	}
	printf("\n");
}
