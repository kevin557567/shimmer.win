#pragma once
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <uv.h>


using namespace std;
#define PIPENAME "\\\\.\\pipe\\pipeTest"

class rpcmgr;

typedef struct {
	uv_pipe_t server_client;
	rpcmgr *ts;
}UvPipe;

class rpcmgr
{
public:
	rpcmgr();
	~rpcmgr();

	void setStr(const char *str);
	int initPipeClient();

	static void on_connect_cb(uv_connect_t* req, int status);
	void on_connect(uv_connect_t* req, int status);

	static void write_server_cb(uv_write_t* req, int status);
	void write_server(uv_write_t* req, int status);
	void send_data(char *data, int length);

private:
	uv_loop_t *loop;
	UvPipe pipeClient;
	uv_connect_t *pipeConnect;
	uv_connect_t* reqConnect;
	char cmd[20];
};