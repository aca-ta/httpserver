#include "logL.h"

/***************************
log_exit

エラーログ出力し、exitする
***************************/
void log_exit(
	char *fmt,
	...
)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}
