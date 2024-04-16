#include "stdafx.h"
#include "rpcmgr.h"
#include <time.h>

rpcmgr::rpcmgr(){
}

rpcmgr::~rpcmgr(){
}

void rpcmgr::setStr(const char *str){
	memset(cmd, 0, 20);
	strcpy(cmd, str);
}

int rpcmgr::initPipeClient(){
	reqConnect = nullptr;
	memset(&pipeClient, 0, sizeof(pipeClient));
	loop = uv_default_loop();
	pipeClient.ts = this;
	uv_pipe_init(loop, (uv_pipe_t*)&pipeClient, 0);
	pipeConnect = (uv_connect_t*)malloc(sizeof(uv_connect_t));
	uv_pipe_connect(pipeConnect, (uv_pipe_t*)&pipeClient, PIPENAME, on_connect_cb);
	return 0;
}

void rpcmgr::on_connect_cb(uv_connect_t* req, int status){
	UvPipe* pipe = (UvPipe*)req->handle;
	rpcmgr* ts = pipe->ts;
	ts->on_connect(req, status);
	printf("%s","=======================on_connect_cb ====1111====\n");
}

void rpcmgr::on_connect(uv_connect_t* req, int status){
	if (status < 0){
		printf("%s","------------------------- New conect error...");
	}else{
		printf("%s","----------------------- New conect sucess...");
		reqConnect = req;
	}
}

void rpcmgr::send_data(char *data, int length){
	if (!reqConnect){
		initPipeClient();
		return;
	}
	uv_write_t* wr = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_buf_t buf = uv_buf_init(data, length);
	int ret = uv_write(wr, (uv_stream_t*)reqConnect->handle, &buf, 1, write_server_cb);
	if (ret < 0)
	{
		printf("%s","---------------------Write error...");
	}
	else
	{
		printf("%s","---------------------Write sucess...");
	}
}

void rpcmgr::write_server_cb(uv_write_t* req, int status)
{
	UvPipe* pipe = (UvPipe*)req;
	rpcmgr* ts = pipe->ts;
	ts->write_server(req, status);
}

void rpcmgr::write_server(uv_write_t* req, int status)
{
	if (status < 0)
	{
		printf("%s","--------------------Write error...");
	}
	else
	{
		printf("%s","--------------------Write success...");
	}
}
