#ifndef ERROR_FUNC_H
#define ERROR_FUNC_H

#include <syslog.h>

#include "skel_defines.h"

#define MAXLINE 4048
int daemon_proc;

void error(int status, int err, char* format, ...) {

	va_list ap;

	va_start(ap, format);
	fprintf(stderr, "%s: ", program_name);
	vfprintf(stderr, format, ap);

	va_end(ap);

	if (err) {
		fprintf(stderr, ": %s (%d)\n", strerror( err ), err);
	}
	if (status) {
		EXIT(status);
	}
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


#endif //ERROR_FUNC_H
