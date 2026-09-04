#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include "error_func.c"

void parseArgs(int argc, char** argv) {

	// -s --send -ip -p --port -f --file

	switch (argc) {
	case 1: {
			errno = EINVAL;
			error(1, errno, "no arguments was provided\n");
		}
		break;
	case 2: {

		}
		break;
	default:
		break;
	}
}


#endif //ERROR_FUNC_H
