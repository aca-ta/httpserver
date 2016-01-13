#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <getopt.h>
#include <grp.h>
#include <pwd.h>
#include <sys/syslog.h>
#include "sigL.h"
#include "httpL.h"
#include "daemonL.h"

#define SERVER_NAME "LittleHTTP"
#define SERVER_VERSION "1.0"
#define HTTP_MINOR_VERSION 0
#define BLOCK_BUF_SIZE 1024
#define LINE_BUF_SIZE 4096
#define MAX_REQUEST_BODY_LENGTH (1024 * 1024)
#define MAX_BACKLOG 5
#define DEFAULT_PORT "80"
#define USAGE "Usage: %s [--port=n] [chroot --user=u --group=g] [--debug] <docroot>\n"

static int debug_mode = 0;

/* option取得用構造体 */
static struct option longopts[] =
{
	{"debug", no_argument,		&debug_mode,1},
	{"chroot", no_argument, 	NULL, 'c'},
	{"user", required_argument, NULL, 'u'},
	{"group", required_argument, NULL, 'g'},
	{"port", required_argument, NULL, 'p'},
	{"help", no_argument, 		NULL, 'h'},
	{0, 0, 0, 0}
};

int main(
	int argc,
	char *argv[]
)
{
	int server = 0; //listen用socket
	char *port = NULL;
	char *docroot;
	char *user = NULL;
	char *group = NULL;
	int opt;

	while ((opt = getopt_long(argc, argv, "", longopts, NULL)) != -1)
	{
		switch(opt)
		{
			case 0:
				break;

			case 'c':
				break;

			case 'u':
				user = optarg;
				break;
			case 'g':
				group = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case 'h':
				fprintf(stdout, USAGE, argv[0]);
				exit(0);

			case '?':
				fprintf(stderr, USAGE, argv[0]);
				exit(1);
		}
	}

	if(optind != argc - 1)
	{
		fprintf(stderr, USAGE, argv[0]);
		exit(1);
	}

	/* ドキュメントのrootディレクトリを設定 */
	docroot = argv[optind];

	/* シグナル捕捉の設定 */
	install_signal_handlers();

	/* listenソケットの作成 */
	server = listen_socket(port);

	if(!debug_mode)
	{
		openlog(SERVER_NAME, LOG_PID|LOG_NDELAY, LOG_DAEMON);
		/* デーモンプロセスになる */
		become_daemon();
	}

	server_main(server, docroot);
	return 0;
}
