#include "daemonL.h"
#include "httpL.h"


/***************************
become_daemon

デーモンプロセスになる
***************************/
void become_daemon()
{
	int n = 0;

	if(chdir("/") < 0)
	{
		log_exit("chdir(2) failed: %s", strerror(errno));
	}

	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

	n = fork();

	if(n < 0)
	{
		log_exit("fork(2) failed: %s");
	}

	if(n != 0)
	{
		/* 親プロセスは終了する */
		_exit(0);
	}

	/* 新たなセッションを作成する */
	if(setsid() < 0)
	{
		log_exit("setsid(2) failed: %s", strerror(errno));

	}

}
