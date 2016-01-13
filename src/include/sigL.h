#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "logL.h"

#ifndef SA_RESTART
	#define SA_RESTART 0x10000000
#endif


typedef void (*sighandler_t)(int);

void install_signal_handlers();

