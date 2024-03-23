#include "machine.h"
#include <uv.h>
#include <iostream>
#include <rpc/server.h>
#include <string.h>
#include <map>
//#include "firebase.h"
#include "appmgr.h"

static std::map<std::string, std::string> cfg_firebase;

void foo() {
	std::cout << "foo was called!" << std::endl;
}

void bad(int x) {
	if (x == 5) {
		throw std::runtime_error("x == 5. I really don't like 5.");
	}
}

//rpc worker
void worker_rpc(uv_work_t *req ) {
	printf("start rpc service\n");
	rpc::server srv("127.0.0.1", rpc::constants::DEFAULT_PORT);
	std::string gmsg = "GMsg";
	srv.bind("add", [](int a, int b) { return a + b; });
	srv.bind("getConfig", [gmsg](std::string msg) {
		std::map<std::string, std::string>::iterator ifnd = cfg_firebase.find(msg);
		if (ifnd != cfg_firebase.end()) {
			return ifnd->second;
		}
		return std::string("{}");
	});
	srv.bind("runAsAdmin", [](std::string path) {
		if (!createAdminProcess(path, "")) {
			OutputDebugStringA("call createAdminProcess failure ,call LaunchAppIntoDifSession");
			//LaunchAppIntoDifferentSession((LPTSTR)file_path.c_str());
		}
	});
	srv.suppress_exceptions(true);
	srv.bind("bad", &bad);
	srv.run();
}


//idle service
void wait_for_idle(uv_idle_t* handle) {
	printf("start idle service\n");
	while (cfg_firebase.size() == 0 ) {
		//get_configure(cfg_firebase);
		Sleep(1000);
	}
	uv_idle_stop(handle);
}

int CMachine::run() {
	uv_loop_t * loop = uv_default_loop();

	uv_work_t req;
	uv_queue_work(loop, &req, worker_rpc, [](uv_work_t *req, int status){});

	//idle server
	uv_idle_t  idler;
	uv_idle_init(loop, &idler);
	idler.data = (void*)123123;
	uv_idle_start(&idler, wait_for_idle);

	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);
	return 0;
}