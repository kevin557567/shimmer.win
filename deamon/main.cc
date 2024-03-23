#define _CRTDBG_MAP_ALLOC

#include <iostream>
#include "machine.h"
#include<crtdbg.h>
char SVCNAME[128] = "shimmer";
char SVCDESC[128]   = "service for shimmer project";

#ifdef _DEBUG //���Ҫ���ϣ����򲻻�������嵽�Ǹ��ļ��У��������������ڴ�й¶�ĸ�cpp�ļ��������Ϣ��  
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