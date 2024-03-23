#ifndef _MACHINE_
#define _MACHINE_
#include "VPNMgr.h"
#include "DataMgr.h"
#include <uv.h>
class CMainWndFrame;

class CMachine:public CVPNMgr, public CDataMgr{
private:
	CMachine() {};
	CMachine( const CMachine&);
	CMachine& operator=(const CMachine&);
	~CMachine(){}
	static CMachine* instance;

public:
	static CMachine* getInstance();
	static void deleteInstance();
	void run();

protected:
	 void CreateWindows();
	 void addWindow(CWindowWnd * wnd);

public:
	 static void ansyc_execute(void (CMainWndFrame::*callback)(int), void* frame, int a);
};

#endif // _MACHINE_
