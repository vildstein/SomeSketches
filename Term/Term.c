// Taken from the book Advanced Programming in the Unix Environment of W. Richard Stevens

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

#define MAXLINE 4048
int daemon_proc;

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

/* перевести терминал в режим прозрачного ввода (raw) */
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

/* может быть установлена вызовом atexit(tty_atexit) */
void tty_atexit(void)
{
	if (ttysavefd >= 0) {
		tty_reset(ttysavefd);
	}
}

static void err_doit(int errnoflag, int level, const char* fmt, va_list ap) {

	int errno_save, n;
	char buf[MAXLINE + 1];

	errno_save = errno;             /* value caller might want printed */
#ifdef  HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap);       /* safe */
#else
	vsprintf(buf, fmt, ap);                          /* not safe */
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

int main(void) {

	int i;
	char c;

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

	if (tty_raw(STDIN_FILENO) < 0) {
		err_sys("ошибка вызова функции tty_raw");
	}

	printf("Переход в режим raw, выход из режима по нажатию DELETE\n");

	while ((i = read(STDIN_FILENO, &c, 1)) == 1) {
		if ((c &= 255) == 0x7f/*0177*/) /* 0177 = ASCII DELETE */{
			break;
		}
		printf("%o\n", c);
	}

	if (tty_reset(STDIN_FILENO) < 0) {
		err_sys("ошибка вызова функции tty_reset");
	}

	if (i <= 0) {
		err_sys("ошибка чтения");
	}

	if (tty_cbreak(STDIN_FILENO) < 0) {
		err_sys("ошибка вызова функции tty_cbreak");
	}

	printf("\nПереход в режим cbreak, выход из режима по сигналу SIGINT\n");

	while ((i = read(STDIN_FILENO, &c, 1)) == 1) {
		c &= 255;
		printf("%o\n", c);
	}

	if (tty_reset(STDIN_FILENO) < 0) {
		err_sys("ошибка вызова функции tty_reset");
	}

	if (i <= 0) {
		err_sys("ошибка чтения");
	}

	exit(0);
}
