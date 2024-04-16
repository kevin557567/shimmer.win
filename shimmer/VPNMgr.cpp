#include "stdafx.h"
#include "vpnmgr.h"
#include <uv.h>

uv_process_options_t options;

void on_exit(uv_process_t* process, int64_t exit_status, int term_signal) {
	printf("Process exited with status %lld, signal %d\n", exit_status, term_signal);
	process->pid = 0;
	uv_close((uv_handle_t*)process, NULL);
}

void read_apipe(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
	printf("read %li bytes in a %lu byte buffer\n", nread, buf.len);
	if (nread + 1 > (size_t)buf.len) return;
	buf.base[nread] = '\0'; // turn it into a cstring
	printf("read: |%s|", buf.base);
}

std::string ExeDir() {
	TCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer, pos+1);
}

vpnmgr::vpnmgr() {
	memset(&child_req, 0x00, sizeof(child_req));
}

vpnmgr::~vpnmgr() {
	if (child_req.pid) {
		disconnect();
	}
}

int vpnmgr::connect() {
	if (child_req.pid) {
		return -1;
	}
	uv_loop_t* loop = uv_default_loop();
	uv_pipe_init(loop, &stdin_pipe, 0);
	char* args[3];
	args[0] = "";
	args[1] = NULL;
	args[2] = NULL;
	uv_stdio_container_t child_stdio[3];
	child_stdio[0].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_READABLE_PIPE | UV_WRITABLE_PIPE);
	child_stdio[0].data.stream = (uv_stream_t*)&stdin_pipe;
	child_stdio[0].data.stream->flags = 0;
	child_stdio[1].flags = UV_IGNORE;
	child_stdio[2].flags = UV_INHERIT_FD;
	child_stdio[2].data.fd = 2;
	options.flags = UV_PROCESS_DETACHED;
	options.stdio = child_stdio;
	options.stdio_count = 3;
	options.exit_cb = on_exit;
	options.file = "vpntest.exe";
	options.args = args;

	int r;
	if ((r = uv_spawn(loop, &child_req, &options))) {
		fprintf(stderr, "%s\n", uv_strerror(r));
		return -1;
	}
	child_req.data = (void*)&options;
	uv_read_start((uv_stream_t*)&stdin_pipe, [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
		buf->base = (char*)malloc(suggested_size);
		buf->len = suggested_size;
	}, [](uv_stream_t* pipe, ssize_t nread, const uv_buf_t* buf) {
		if (nread < 0) {
			if (nread == UV_EOF) {
			}
			else {
			}
		}
		else if (nread > 0) {
		}
		if (buf->base) {
			free(buf->base);
		}
	});
	return 0;
}

int vpnmgr::disconnect() {
	if (!child_req.pid) {
		return -1;
	}
	uv_write_t* write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_buf_t buf = uv_buf_init("exit\n", 5);
	int rw = uv_write(write_req, (uv_stream_t*)&stdin_pipe, &buf, 1, [](uv_write_t* req, int status) {
		if (status < 0) {
		}
		free(req);
	});
	WaitForSingleObject(child_req.process_handle,(rw==SEC_E_OK)?INFINITE:200);
	::TerminateProcess(child_req.process_handle, 0);
	return 0;
}

