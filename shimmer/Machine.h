#ifndef _MACHINE_
#define _MACHINE_
#include "DataMgr.h"
#include "vpnmgr.h"
#include "rpcmgr.h"
#include <uv.h>
class CMainWndFrame;

class CMachine:public CDataMgr, public vpnmgr{
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

	void rpc_dispath();

protected:
	 void CreateWindows();
	 void addWindow(CWindowWnd * wnd);



public:
	 static void ansyc_execute(void (CMainWndFrame::*callback)(int), void* frame, int a);
};

#endif // _MACHINE_
