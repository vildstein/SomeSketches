#ifndef ERMINAL_FUNCTIONS_H
#define ERMINAL_FUNCTIONS_H

#include <termio.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>

#define MAXLINE 4048
//int daemon_proc;

static struct termios save_termios;
static int ttysavefd = -1;
static enum { RESET, RAW, CBREAK } ttystate = RESET;


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


/* позволить вызывающему процессу */
/* узнать начальное состояние терминала */
struct termios *tty_termios(void)
{
	return(&save_termios);
}
#endif //TERMINAL_FUNCTIONS_H
