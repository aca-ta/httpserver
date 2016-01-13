#include "memL.h"

/***************************
xmalloc

mallocする(エラー時にログ出力)
***************************/
void* xmalloc(
	size_t sz
)
{
	void *p;

	p = malloc(sz);

	if(!p)
	{
		log_exit("failed to allocate memory");
	}
	return p;
}
