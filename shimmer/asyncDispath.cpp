#include "stdafx.h"
#include "asyncDispath.h"

template <class T>
class asnyc_disp_t {
	T * mac;
	void (T::*callback)(uv_loop_t*, int);
	int arg;
};

//Òì²½½Ó¿Ú
template <class T>
void AsyncDispath::ansyc_execute(void (T::*callback)(uv_loop_t*, int), int a) {
	uv_async_t  * async = (uv_async_t*)malloc(sizeof(uv_async_t));;
	asnyc_disp_t * mrq = (asnyc_req*)malloc(sizeof(asnyc_disp_t));
	//mrq->mac = frame;
	mrq->callback = callback;
	mrq->arg = a;
	async->data = (void*)mrq;
	uv_async_init(uv_default_loop(), async, [](uv_async_t* handle) {
		asnyc_req * mrq = (asnyc_req*)handle->data;
		(mrq->mac->*(mrq->callback))(nullptr, mrq->arg);
		printf("ansyc_task thread_id:%d  value:%d\n", (int)uv_thread_self(), mrq->arg);
		uv_close((uv_handle_t*)handle, [](uv_handle_t* handle) {
			free(handle->data);
			free(handle);
		});
	});
	uv_async_send(async);
	return;
}
