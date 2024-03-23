#pragma once

#include <uv.h>

class AsyncDispath
{
public:
	template <class T>
	void ansyc_execute(void (T::*callback)(uv_loop_t*, int), int a);

};

