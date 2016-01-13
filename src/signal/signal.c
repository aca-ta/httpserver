#include "sigL.h"

static void trap_signal(int sig, sighandler_t handler);
static void signal_exit(int sig);
static void wait_child(int sig);

int errno;


/***************************
install_signal_handlers

捕捉するシグナルと、捕捉した際の挙動を設定する。
***************************/
void install_signal_handlers()
{
	trap_signal(SIGPIPE, signal_exit);
	trap_signal(SIGCHLD, wait_child);
}

/***************************
trap_signal

sigに指定されたシグナルを捕捉し、handlerを実行する
***************************/

static void trap_signal(
	int sig,
	sighandler_t handler
)
{
	struct sigaction act;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	if(sigaction(sig, &act, NULL) < 0)
	{
		log_exit("sigaction() failed: %s", strerror(errno));
	}
}

/***************************
signal_exit

シグナル捕捉時にexitする
***************************/
static void signal_exit(
	int sig
)
{
	log_exit("exit by signal %d", sig );
}

/***************************
wait_child

シグナル捕捉時にwaitする
***************************/
static void wait_child(
	int sig
)
{
	wait(NULL);
}
