#include <sys/stat.h>
#include <ctype.h>
#include "httpL.h"
#include <netdb.h>

/***************************
upcase

大文字に変換する
***************************/
void upcase(
	char *str
)
{
	char *p;

	for(p = str; *p ; p++)
	{
		*p = (char)toupper((int)*p);
	}

}

/***************************
listen_socket

listen用socketの生成
***************************/
int listen_socket(
	char *port
)
{
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *ai;
	int err;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((err = getaddrinfo(NULL, port, &hints, &res)) != 0)
	{
		log_exit(gai_strerror(err));
	}

	for(ai = res; ai; ai->ai_next)
	{
		int sock;

		sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

		if(sock < 0)
		{
			continue;
		}

		if(bind(sock, ai->ai_addr, ai->ai_addrlen) < 0)
		{
			close(sock);
			continue;
		}

		if(listen(sock, MAX_BACKLOG) < 0)
		{
			close(sock);
		}

		freeaddrinfo(res);
		return sock;
	}
	log_exit("failed to listen sock");
	return -1;


}

/***************************
server_main

server処理main関数
***************************/
void server_main(
	int server,
	char *docroot
)
{

	/* リクエスト待ち受けLOOP */
	for(;;)
	{
		struct sockaddr_storage addr;
		socklen_t addrlen = sizeof(addr);
		int sock;
		int pid;
		FILE *inf = NULL;
		FILE *outf = NULL;
		
		sock = accept(server, (struct sockaddr*)&addr, &addrlen);
		if(sock < 0)
		{
			log_exit("accept() failed: %s, strerror(errno)");
		}

		/* accept()後の処理は子プロセスに実行させる */
		pid = fork();

		if(pid < 0)
		{
			exit(3);
		}

		if(pid == 0)
		{
			inf = fdopen(sock, "r");
			outf = fdopen(sock, "w");

			service(inf, outf, docroot);
			exit(0);
		}
		close(sock);
	}
}
