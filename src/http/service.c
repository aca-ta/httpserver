#include <stdio.h>
#include "httpL.h"


static void free_request(HttpRequest *req);

/***************************
service

リクエストのread,レスポンスのwrite
***************************/
void service(
    FILE *in,
    FILE *out,
    char *docroot
)
{
    HttpRequest *req;

    req = read_request(in);
    respond_to(req, out, docroot);
    free_request(req);


}

/***************************
free_request

リクエストの解放
***************************/
static void free_request(
	HttpRequest *req
)
{
	HttpHeaderField *h = NULL;
	HttpHeaderField *head = NULL;

	head = req->header;
	while(head)
	{
		h = head;
		head = head->next;
		free(h->name);
		free(h->value);
		free(h);
	}

	free(req->method);
	free(req->path);
	free(req->body);
	free(req);
}
