#include "stdafx.h"
#include "Machine.h"
#include "LaunchScreen.h"
#include "MainWinFrame.h"
#include "FloatWindow.h"
#include <rpc/client.h>
#include <rpc/rpc_error.h>
#include <iostream>

typedef struct mac_req_t {
	CMachine * mac;
	void (CMachine::*func)(uv_loop_t*, int);
	int arg;
}mac_req;

typedef struct asnyc_req_t {
	CMainWndFrame * mac;
	void (CMainWndFrame::*callback)(int);
	int arg;
}asnyc_req;

rpcmgr *rpcc = NULL;

//void on_exit(uv_process_t *req, int64_t exit_status, int term_signal) {
//	//fprintf(stderr, "Process exited with status %"PRId64", signal %d\n", exit_status, term_signal);
//	uv_close((uv_handle_t*)req, NULL);
//}
// 

CMachine::CMachine(const CMachine&) {
	printf("CMachine");
}

void CMachine::addWindow(CWindowWnd * wnd) {
	char szClassName[256] = { 0 };
	RealGetWindowClass(wnd->GetHWND(), szClassName, 256);
}

void CMachine::CreateWindows() {
	//CLaunchScreen * pLaunchScreen = new CLaunchScreen;
	//pLaunchScreen->Create(nullptr, "Launch Screen", UI_WNDSTYLE_DIALOG, 0);
	//pLaunchScreen->CenterWindow();
	//pLaunchScreen->ShowWindow();

	CFloatWindow * pFloatWnd = new CFloatWindow;
	pFloatWnd->Create(NULL, "Float Window", UI_WNDSTYLE_DIALOG, 0);
	pFloatWnd->ShowWindow();

	//主窗体
	CMainWndFrame* pMainWndFrame = new CMainWndFrame;
	pMainWndFrame->Create(nullptr, "Main Frame", UI_WNDSTYLE_DIALOG, 0);
	pMainWndFrame->CenterWindow();
	pMainWndFrame->ShowWindow();

	//addWindow(pLaunchScreen);
	//addWindow(pFloatWnd);
	//addWindow(pMainWndFrame);

	CPaintManagerUI::MessageLoop();
	//delete pLaunchScreen;
	//delete pMainWndFrame;
}


void CMachine::rpc_dispath() {

	rpcc->send_data("abcdef", 6);

}

void CMachine::run() {
	uv_loop_t *loop = uv_default_loop();
	//uv_thread_t id = uv_thread_self();

	//ui dispath
	uv_work_t req_ui;
	uv_queue_work(loop, &req_ui, 
		  [](uv_work_t *req){instance->CreateWindows();}
		, [](uv_work_t *req, int status) {}
	);


	rpcc = new rpcmgr();
	rpcc->initPipeClient();

 
	////idler
	//uv_idle_t idler;
	//uv_idle_init(loop, &idler);
	////idler.data = (void*)this;
	//uv_idle_start(&idler, [](uv_idle_t* handle) {
	//	instance->update();
	//	uv_idle_stop(handle);
	//});

	 uv_run(loop, UV_RUN_DEFAULT);
	 uv_loop_close(loop);
}

//异步接口
void CMachine::ansyc_execute(void (CMainWndFrame::*callback)(int), void* frame, int a) {
	uv_async_t  * async = (uv_async_t*)malloc(sizeof(uv_async_t));;
	asnyc_req * mrq = (asnyc_req*)malloc(sizeof(asnyc_req));
	mrq->mac = (CMainWndFrame*)frame;
	mrq->callback = callback;
	mrq->arg = a;
	async->data = (void*)mrq;
	uv_async_init(uv_default_loop(), async, [](uv_async_t* handle) {
		asnyc_req * mrq = (asnyc_req*)handle->data;
		(mrq->mac->*(mrq->callback))(mrq->arg);
		printf("ansyc_task thread_id:%d  value:%d\n", (int)uv_thread_self(), mrq->arg);
		uv_close((uv_handle_t*)handle, [](uv_handle_t* handle) {
			free(handle->data);
			free(handle);
		});
	});
	uv_async_send(async);
	return;
}

CMachine* CMachine::instance = new CMachine();
CMachine* CMachine::getInstance() {
	return instance;
}
void CMachine::deleteInstance() {
	delete instance;
}