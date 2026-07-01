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

#define MAXLINE 4048
int daemon_proc;

static void goLeftMessage(SOCKET sock, SIN* peer);
static void goRightMessage(SOCKET sock, SIN* peer);
static void goUpMessage(SOCKET sock, SIN* peer);
static void goDownMessage(SOCKET sock, SIN* peer);
static void stopMessage(SOCKET sock, SIN* peer);

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

int main( int argc, char** argv ) {

	SIN client;
	SOCKET sock;

	char* host = "192.168.2.93";
	char* portNumber = "6000";

	const char* const left = "Left";
	const char* const right = "Right";
	const char* const up = "Up";
	const char* const down = "Down";

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

	const size_t messageSize = 7; // Bytes
	char message[messageSize];

	char firstByte = 0xff;
	message[0] = firstByte;
	char deviceIdByte = 0x01; // device Id
	message[1] = deviceIdByte;

	// Command 1
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]
	char Command_1_Byte = 0x00;
	message[2] = Command_1_Byte;

	// Command 2
	// 0 0 0 0. 0 1 0 0
	// Focus_Far[7] Zoom_Wide[6] Zoom_Tele[5] Tilt_Down[4] Tilt_Up[3] Pan_Left[2] Pan_Right[1] Fixed_to_0[0]
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
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]
	char Command_1_Byte = 0x00;
	message[2] = Command_1_Byte;

	// Command 2
	// 0 0 0 0. 0 0 1 0
	// Focus_Far[7] Zoom_Wide[6] Zoom_Tele[5] Tilt_Down[4] Tilt_Up[3] Pan_Left[2] Pan_Right[1] Fixed_to_0[0]
	char Command_2_Byte = 0x02;
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

static void goUpMessage(SOCKET sock, SIN* peer) {

	const size_t messageSize = 7; // Bytes
	char message[messageSize];
	char firstByte = 0xff;
	message[0] = firstByte;
	char deviceIdByte = 0x01; // device Id
	message[1] = deviceIdByte;

	// Command 1
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]
	char Command_1_Byte = 0x00;
	message[2] = Command_1_Byte;

	// Command 2
	// Focus_Far[7] Zoom_Wide[6] Zoom_Tele[5] Tilt_Down[4] Tilt_Up[3] Pan_Left[2] Pan_Right[1] Fixed_to_0[0]
	// 0 0 0 1. 0 0 0 0
	char Command_2_Byte = 0x10;
	message[3] = Command_2_Byte;

	// Pan speed
	char data_1_Byte = 0x00;
	message[4] = data_1_Byte;

	// Tilt speed
	char data_2_Byte = 0x2d;
	message[5] = data_2_Byte;

	char checkSum_Byte = deviceIdByte + Command_1_Byte + Command_2_Byte + data_1_Byte + data_2_Byte;
	message[6] = checkSum_Byte;

	int peerlen = sizeof(*peer);

	const int ZERO_FLAG = 0;

	if ( sendto(sock, message, sizeof(message), ZERO_FLAG, (struct sockaddr*)peer, peerlen) < 0 ) {
		error(1, errno, "SENT_TO FUNCTION MISTAKE");
	}
}

static void goDownMessage(SOCKET sock, SIN* peer) {

	const size_t messageSize = 7; // Bytes
	char message[messageSize];
	char firstByte = 0xff;
	message[0] = firstByte;
	char deviceIdByte = 0x01; // device Id
	message[1] = deviceIdByte;

	// Command 1
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]
	char Command_1_Byte = 0x00;
	message[2] = Command_1_Byte;

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
	// Sense[7] Reserved[6] Reserved[5] Auto_Manual_Scan[4] Camera_On_Off[3] Iris_Close[2] Iris_Open[1] Focus_Near[0]
	char Command_1_Byte = 0x00;
	message[2] = Command_1_Byte;

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

	char checkSum_Byte = deviceIdByte + Command_1_Byte + Command_2_Byte + data_1_Byte + data_2_Byte;
	message[6] = checkSum_Byte;
	int peerlen = sizeof(*peer);

	const int ZERO_FLAG = 0;

	if ( sendto(sock, message, sizeof(message), ZERO_FLAG, (struct sockaddr*)peer, peerlen) < 0 ) {
		error(1, errno, "SENT_TO FUNCTION MISTAKE");
	}
}

