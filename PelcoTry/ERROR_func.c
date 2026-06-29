#ifndef ERROR_FUNC_H
#define ERROR_FUNC_H

#include "skel_defines.h"

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

#endif //ERROR_FUNC_H
