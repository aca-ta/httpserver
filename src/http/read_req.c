#include "httpL.h"

#define HTTPVER "HTTP/1."
#define CONTENT_LENGTH "Content-Length"

static void read_request_line(HttpRequest *req, FILE *in);
static HttpHeaderField *read_header_field(FILE *in);
static long content_length(HttpRequest *req);
static char *lookup_header_field_value(HttpRequest *req, char *name);


/***************************
read_request

リクエストのread
***************************/
HttpRequest* read_request(
	FILE *in
)
{
	HttpRequest 		*req;
	HttpHeaderField 	*h;
	int					ret = 0;

	req = xmalloc(sizeof(HttpRequest));
	read_request_line(req, in);
	req->header = NULL;

	while(NULL != (h = read_header_field(in)))
	{
		h->next = req->header;
		req->header = h;
	}

	req->length = content_length(req);

	if(req->length != 0)
	{

		if(req->length != MAX_REQUEST_BODY_LENGTH)
		{
			log_exit("request body too long");
		}

		req->body = xmalloc(req->length);

		ret = fread(req->body, req->length, 1, in);

		if(ret < 1)
		{
			log_exit("failed to read request body");
		}
		else
		{
			req->body = NULL;

		}
	}
	return req;
}

/***************************
read_request_line

リクエストラインのread
***************************/
static void read_request_line(
	HttpRequest *req,
	FILE 		*in
)
{

	char buf[LINE_BUF_SIZE];
	char *path;
	char *p;

	if(!fgets(buf, LINE_BUF_SIZE, in))
	{
		log_exit("no request line");
	}

	p = strchr(buf, ' ');

	if(!p)
	{
		log_exit("parse error on request line (1):%s", buf);
	}
	*p++ = '\0';

	req->method = xmalloc(p - buf);
	strcpy(req->method, buf);
	upcase(req->method);

	path = p;
	p = strchr(path, ' ');

	if(!p)
	{
		log_exit("parse error on request line(2): %s", buf);
	}

	*p++ = '\0';
	req->path = xmalloc(p - path);
	strcpy(req->path, path);

	if(strncasecmp(p, HTTPVER, strlen(HTTPVER)) != 0)
	{
		log_exit("parse error on request line(3): %s", buf);
	}

	p += strlen(HTTPVER);

	req->protocol_minor_version = atoi(p);
}

/***************************
read_header_field

リクエストヘッダのread
***************************/
static HttpHeaderField *read_header_field(
	FILE *in
)
{
	HttpHeaderField *h;
	char buf[LINE_BUF_SIZE];
	char *p;

	if(!fgets(buf, LINE_BUF_SIZE, in))
	{
		log_exit("failed to read request header field:%s", strerror(errno));
	}

	if(buf[0] == '\n' ||
	   strcmp(buf,"\r\n") == 0)
	{
		return NULL;
	}

	p = strchr(buf, ':');
	if(!p)
	{
		log_exit("parse error on request header field:%s", buf);
	}
	*p++ = '\0';

	h = xmalloc(sizeof(HttpHeaderField));
	h->name = xmalloc(p - buf);
	strcpy(h->name, buf);

	p +=strspn(p, "\t");
	h->value = xmalloc(strlen(p) + 1);
	strcpy(h->value, p);

	return h;
}

/***************************
content_length

Content-Lengthを取得
***************************/
static long content_length(
	HttpRequest *req
)
{
	char *val;
	long len;

	val = lookup_header_field_value(req, CONTENT_LENGTH);

	if(!val)
	{
		return 0;
	}

	len = atol(val);

	if(len < 0)
	{
		log_exit("negative Content-Length value");
	}

	return len;
}

/***************************
lookup_header_field_value(

nameに指定されたヘッダフィールドを探す
***************************/
static char* lookup_header_field_value(
	HttpRequest *req,
	char *name
)
{
	HttpHeaderField *h;

	for(h = req->header; h; h= h->next)
	{
		if(strcasecmp(h->name, name) ==0)
		{
			return h->value;
		}
	}

	return NULL;
}
