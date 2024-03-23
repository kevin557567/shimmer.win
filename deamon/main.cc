#define _CRTDBG_MAP_ALLOC

#include <iostream>
#include "machine.h"
#include<crtdbg.h>
char SVCNAME[128] = "shimmer";
char SVCDESC[128]   = "service for shimmer project";

#ifdef _DEBUG //这个要加上，否则不会输出定义到那个文件中（及不包含存在内存泄露的该cpp文件的相关信息）  
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 

int MyMain(int argc, char * argv[]) {
	printf("shimmer deamon\n");
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
	_CrtDumpMemoryLeaks();
	CMachine::run();
	printf("exit\n");
    return 0;
}