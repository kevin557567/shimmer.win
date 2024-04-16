#include "rpsmgr.h"
#include <time.h>

rpsmgr::rpsmgr()
{
}

rpsmgr::~rpsmgr()
{
}


int rpsmgr::initLocalServer(uv_loop_t* loop)
{
	if (loop == NULL)
	{
		printf("%s", "mllop is null   ");
	}
	mLoop = loop;

	//若sock文件PIPENAME存在则删除
	uv_fs_t req;
	uv_fs_unlink(mLoop, &req, PIPENAME, NULL);
	uv_pipe_init(mLoop, (uv_pipe_t*)&pipeServer, 0);
	//程序终止时删除sock文件PIPENAME
	signal(SIGINT, remove_sock);

	pipeServer.ts = this;

	int ret = uv_pipe_bind((uv_pipe_t*)&pipeServer, PIPENAME);
	if (ret)
	{
		printf("%s", "Bind error ");
		return -1;
	}

	//typedef void (*fun)(uv_stream_t*,int);
	//fun on_pipe_connection=(fun)&rpsmgr::onPipeConnection;
	//ret=uv_listen((uv_stream_t*)&pipeServer,128,on_pipe_connection);
	ret = uv_listen((uv_stream_t*)&pipeServer, 128, onPipeConnectionCb);
	if (ret)
	{
		printf("%s", "Listen error ");
		return -1;
	}
	else
	{
		printf("%s", "-------22222  Listen success -----");
	}

	return 0;
}

void rpsmgr::remove_sock(int sig)
{
	uv_fs_t req;
	uv_loop_t* Loop = uv_default_loop();
	uv_fs_unlink(Loop, &req, PIPENAME, NULL);
	exit(0);
}

void rpsmgr::onPipeConnection(uv_stream_t* server, int status)
{
	if (status < 0)
	{
		printf("%s", "New pipe connection error...");
		return;
	}
	uv_pipe_t* pipeClient = (uv_pipe_t*)malloc(sizeof(uv_pipe_t));
	if (mLoop == NULL || pipeClient == NULL)
	{
		if (mLoop == NULL)
		{
			printf("%s", "1111111111111");
		}
		else if (pipeClient == NULL)
		{
			printf("%s", "22222222222222");
		}
		return;
	}
	uv_pipe_init(mLoop, pipeClient, 0);
	int ret = uv_accept(server, (uv_stream_t*)pipeClient);
	if (ret == 0)
	{
		printf("%s", "Accept success...");
		printf("%s", "-------22222  Accept success... -----");
		uv_read_start((uv_stream_t*)pipeClient, allocPipeBufferCb, onPipeReadCb);
	}
}

void rpsmgr::onPipeRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	if (nread < 0)
	{
		if (nread != UV_EOF)
		{
			printf("%s", "Read error...");
		}
		uv_close((uv_handle_t*)client, NULL);
	}
	else if (nread > 0)
	{
		struct timeval tv;
		//gettimeofday(&tv, NULL);
		//int32_t time = (uint32_t)(tv.tv_sec * 1000000 + tv.tv_usec);

		printf("%s", "---------------recieve-%ld------------time = %ld ", nread,1232);
		free(buf->base);
	}
}

void rpsmgr::allocPipeBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

void rpsmgr::onPipeConnectionCb(uv_stream_t* server, int status)
{
	UvPipe *pipe = (UvPipe*)server;
	rpsmgr *ts = pipe->ts;
	ts->onPipeConnection(server, status);
}

void rpsmgr::onPipeReadCb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	UvPipe *handle = (UvPipe*)client;
	rpsmgr *ts = handle->ts;
	ts->onPipeRead(client, nread, buf);

}

void rpsmgr::allocPipeBufferCb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	UvPipe *pipe = (UvPipe*)handle;
	rpsmgr *ts = pipe->ts;
	ts->allocPipeBuffer(handle, suggested_size, buf);
}
