#pragma once
#include <uv.h>

class vpnmgr{
public:
	vpnmgr();
	~vpnmgr();
public:
	int connect();
	int disconnect();
private:
	PROCESS_INFORMATION process_info;
	uv_pipe_t stdin_pipe;
	uv_pipe_t stdout_pipe;
	uv_process_t child_req;
};

