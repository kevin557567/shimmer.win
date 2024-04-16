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


class rpsmgr;

typedef struct {
	uv_pipe_t server_client;
	rpsmgr *ts;
}UvPipe;

class rpsmgr
{
public:
	rpsmgr();
	~rpsmgr();

	int initLocalServer(uv_loop_t* loop);
	static void remove_sock(int sig);
	static void onPipeConnectionCb(uv_stream_t* server, int status);
	void onPipeConnection(uv_stream_t* server, int status);
	static void onPipeReadCb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
	void onPipeRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
	static void allocPipeBufferCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	void allocPipeBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);

private:
	uv_loop_t* mLoop;
	UvPipe pipeServer;
};