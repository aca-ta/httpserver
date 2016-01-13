#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include "logL.h"
#include "memL.h"

#define SERVER_VERSION "1.0"
#define HTTP_MINOR_VERSION 0
#define BLOCK_BUF_SIZE 1024
#define LINE_BUF_SIZE 4096
#define MAX_REQUEST_BODY_LENGTH (1024 * 1024)
#define MAX_BACKLOG 5
#define DEFAULT_PORT "80"
#define METHOD_GET "GET"
#define METHOD_HEAD "HEAD"
#define METHOD_POST "POST"

#define HTTPRES_200OK "200 OK"
#define HTTP_MINOR_VERSION 0


struct HTTPHEADERFIELD
{
	char *name;
	char *value;
	struct HTTPHEADERFIELD *next;
};

typedef struct HTTPHEADERFIELD HttpHeaderField;

/* HTTPリクエスト */
typedef struct HTTPREQUEST
{
	int protocol_minor_version;
	char *method;
	char *path;
	HttpHeaderField *header;
	char *body;
	long length;
}HttpRequest;

typedef struct FILEINFO
{
	char *path;
	long size;
	int ok;		/* ファイルの存在有無*/

}FileInfo;


int listen_socket(char *port);

void respond_to(HttpRequest *req, FILE *out, char *docroot);

void upcase(char *str);

void service(FILE *in, FILE *out, char *docroot);

HttpRequest* read_request(FILE *in);

void server_main(int server, char *docroot);
