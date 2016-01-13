#include "httpL.h"

#define TIME_BUF_SIZE	64
#define SERVERNAME	"local"
#define SERVER_VERSION	"1.0"

static void do_file_response(HttpRequest *req, FILE *out, char *docroot);
static void output_common_header_fields(HttpRequest *req, FILE *out, char *status);
static char* guess_content_type(FileInfo *info);
static void not_implemented(HttpRequest *req, FILE *out);
static void free_fileinfo(FileInfo *info);
static void not_found(HttpRequest *req, FILE *out);
static void method_not_allowed(HttpRequest *req, FILE *out);
static FileInfo* get_fileinfo(char *docroot, char *urlpath);
static char* build_fspath(char *docroot, char *urlpath);


/***************************
respond_to

レスポンスの作成
***************************/
void respond_to(
	HttpRequest *req,
	FILE *out,
	char *docroot
)
{
	if(strcmp(req->method, "GET") == 0)
	{
		do_file_response(req, out, docroot);
	}
	else if(strcmp(req->method, "HEAD") == 0)
	{
		do_file_response(req, out, docroot);
	}
	else if(strcmp(req->method, "POST") == 0)
	{
		method_not_allowed(req, out);
	}
	else
	{
		not_implemented(req, out);
	}
}

/***************************
do_file_response

指定のファイルをレスポンスする
***************************/
static void do_file_response(
	HttpRequest *req,
	FILE 		*out,
	char 		*docroot
)
{
	FileInfo *info;
	int fd;
	char buf[BLOCK_BUF_SIZE];
	ssize_t n;

	info = get_fileinfo(docroot, req->path);

	if(!info->ok)
	{
		free_fileinfo(info);
		not_found(req, out);
		return;
	}

	output_common_header_fields(req, out, "200 OK");

	fprintf(out, "Content-Length: %ld\r\n", info->size);
	fprintf(out, "Content-Type: %s\r\n", guess_content_type(info));
	fprintf(out, "\r\n");

	if(strcmp(req->method, METHOD_HEAD) != 0)
	{
		fd = open(info->path, O_RDONLY);

		if(fd < 0)
		{
			log_exit("failed to open %s: %s", info->path, strerror(errno));
		}

		for(;;)
		{
			n = read(fd, buf, BLOCK_BUF_SIZE);

			if(n < 0)
			{
				log_exit("failed to read %s: %s", info->path, strerror(errno));
			}

			if(n == 0)
			{
				break;
			}

			if(fwrite(buf, 1, n, out) < (size_t)n)
			{
				log_exit("failed to write to socket: %s", strerror(errno));
			}
		}
		close(fd);

	}

	fflush(out);

	free_fileinfo(info);
}

/***************************
output_common_header_fields

レスポンスヘッダを出力する
***************************/
static void output_common_header_fields(
	HttpRequest *req,
	FILE		*out,
	char		*status
)
{

	time_t t;
	struct tm *tm;
	char buf[TIME_BUF_SIZE];

	t = time(NULL);
	tm = gmtime(&t);

	if(!tm)
	{
		log_exit("gmtime() failed: %s", strerror(errno));
	}

	strftime(buf, TIME_BUF_SIZE, "%a, %d %b %Y %H:%M:%s GMT", tm);


	fprintf(out, "HTTP/1.%d %s\r\n", HTTP_MINOR_VERSION, status);

	fprintf(out, "Date: %s\r\n", buf);

	fprintf(out, "Server: %s/%s\r\n", SERVERNAME, SERVER_VERSION);

	fprintf(out, "Connection: close\r\n");

}


/***************************
guess_content_type

content typeを設定する
***************************/
static char* guess_content_type(
	FileInfo *info
)
{

	char *filetype;
	char pathbuf[256];

	memset(pathbuf, '\0', sizeof(pathbuf));

	memcpy(pathbuf, info->path, strlen(info->path));

	strtok(pathbuf, ".");
	filetype = strtok(NULL, ".");

	if(strncmp(filetype, "html", 4) == 0)
	{
		return "text/html";
	}

    return "text/plain";
}

/***************************
not_implemented

想定外のメソッド
***************************/
static void not_implemented(
	HttpRequest *req,
	FILE *out
)
{
    output_common_header_fields(req, out, "501 Not Implemented");
    fprintf(out, "Content-Type: text/html\r\n");
    fprintf(out, "\r\n");
    fprintf(out, "<html>\r\n");
    fprintf(out, "<header>\r\n");
    fprintf(out, "<title>501 Not Implemented</title>\r\n");
    fprintf(out, "<header>\r\n");
    fprintf(out, "<body>\r\n");
    fprintf(out, "<p>The request method %s is not implemented</p>\r\n", req->method);
    fprintf(out, "</body>\r\n");
    fprintf(out, "</html>\r\n");
    fflush(out);
}

/***************************
free_fileinfo

fileinfoをfreeする
***************************/
static void free_fileinfo(
	FileInfo *info
)
{
    free(info->path);
    free(info);
}


static void not_found(
	HttpRequest *req,
	FILE *out
)
{
    output_common_header_fields(req, out, "404 Not Found");
    fprintf(out, "Content-Type: text/html\r\n");
    fprintf(out, "\r\n");
    if (strcmp(req->method, "HEAD") != 0) {
        fprintf(out, "<html>\r\n");
        fprintf(out, "<header><title>Not Found</title><header>\r\n");
        fprintf(out, "<body><p>File not found</p></body>\r\n");
        fprintf(out, "</html>\r\n");
    }
    fflush(out);
}

/***************************
free_fileinfo

対応していないメソッドのリクエストが送られた場合のレスポンスを返却する
***************************/
static void method_not_allowed(
	HttpRequest *req,
	FILE *out
)
{
    output_common_header_fields(req, out, "405 Method Not Allowed");
    fprintf(out, "Content-Type: text/html\r\n");
    fprintf(out, "\r\n");
    fprintf(out, "<html>\r\n");
    fprintf(out, "<header>\r\n");
    fprintf(out, "<title>405 Method Not Allowed</title>\r\n");
    fprintf(out, "<header>\r\n");
    fprintf(out, "<body>\r\n");
    fprintf(out, "<p>The request method %s is not allowed</p>\r\n", req->method);
    fprintf(out, "</body>\r\n");
    fprintf(out, "</html>\r\n");
    fflush(out);
}

/***************************
free_fileinfo

対応していないメソッドのリクエストが送られた場合のレスポンスを返却する
***************************/
static FileInfo* get_fileinfo(
	char *docroot,
	char *urlpath
)
{
    FileInfo *info;
    struct stat st;

    info = xmalloc(sizeof(FileInfo));
    info->path = build_fspath(docroot, urlpath);
    info->ok = 0;
    if (lstat(info->path, &st) < 0) return info;
    if (!S_ISREG(st.st_mode)) return info;
    info->ok = 1;
    info->size = st.st_size;
    return info;
}

/***************************
build_fspath

パスを作成する
***************************/
static char* build_fspath(
	char *docroot,
	char *urlpath
)
{
    char *path;

    path = xmalloc(strlen(docroot) + 1 + strlen(urlpath) + 1);
    sprintf(path, "%s/%s", docroot, urlpath);
    return path;
}
