// MonitorService.cpp : 定义控制台应用程序的入口点。
//

#ifdef _WIN32
#pragma warning(disable: 4995)  //for swprintf

#include <io.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>

#include "atlcomtime.h"
#pragma comment(lib, "advapi32.lib")

int MyMain(int argc, char * argv[]);		//Actual Main function, should be defined
extern TCHAR SVCNAME[128];		    //SVC Name, should be defined
extern TCHAR SVCDESC[128];

SERVICE_STATUS          gSvcStatus; 
SERVICE_STATUS_HANDLE   gSvcStatusHandle; 
HANDLE                  ghSvcStopEvent = NULL;

VOID SvcInstall(void);
VOID DoDeleteSvc(void);
VOID WINAPI SvcCtrlHandler( DWORD ); 
VOID WINAPI SvcMain( DWORD, LPTSTR * ); 

VOID ReportSvcStatus( DWORD, DWORD, DWORD );
VOID SvcInit( DWORD, LPTSTR * ); 
VOID SvcReportEvent( LPCSTR );
BOOL ShowMenu(void);
VOID CALLBACK MainThread(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
HANDLE m_HThread;
DWORD m_dwThreadID;
BOOL g_bRun = FALSE;

int __cdecl main(int argc, char *argv[]) 
{ 
	if( argc>1 &&  strcmp( argv[1], "install") == 0 ){
		SvcInstall();
		return 0;
	}
	else if( argc > 1 &&  strcmp( argv[1], "delete") == 0 ){
		DoDeleteSvc();
		return 0;
	}
	SERVICE_TABLE_ENTRY DispatchTable[] = { 
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION) SvcMain }, 
		{ NULL, NULL } 
	}; 

	g_bRun = TRUE;
	if (!StartServiceCtrlDispatcher( DispatchTable )) { 
		SvcReportEvent("StartServiceCtrlDispatcher");
		MyMain(argc, argv);
		MSG msg;
		while (GetMessage(&msg, 0, 0, 0))
			DispatchMessage(&msg);
	};
	return 0;
} 

VOID SvcInstall(){
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR szPath[MAX_PATH];
	if( !GetModuleFileName( NULL, szPath, MAX_PATH ) ){
		printf("安装服务失败 (%d)", GetLastError()); 
		return;
	}
	// Get a handle to the SCM database. 
	schSCManager = OpenSCManager( NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL == schSCManager) {
		printf("打开服务管理器失败 (%d)", GetLastError());
		return;
	}

	// Create the service
	schService = CreateService( 
		schSCManager,              // SCM database 
		SVCNAME,                   // name of service 
		SVCDESC,                   // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_WIN32_OWN_PROCESS, // service type 
		SERVICE_AUTO_START,      // start type 
		SERVICE_ERROR_NORMAL,      // error control type 
		szPath,                    // path to service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 

	if (schService == NULL) {
		MessageBox(NULL,SVCNAME, szPath, NULL );
		printf("创建服务失败 (%d)", GetLastError()); 
		CloseServiceHandle(schSCManager);
		return;
	}
	else 
		printf("\r\n 科技有限公司 \r\n www.smartx.com \r\n 电话：\r\n 服务安装成功"); 
	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
}

VOID DoDeleteSvc(){
	SC_HANDLE schSCManager;
	SC_HANDLE schService; 
	schSCManager = OpenSCManager( NULL,NULL,SC_MANAGER_ALL_ACCESS);  // full access rights 
	if (NULL == schSCManager) {
		printf("打开服务管理器失败 (%d)", GetLastError());
		return;
	}

	// Get a handle to the service
	schService = OpenService( schSCManager,SVCNAME,DELETE);            // need delete access 
	if (schService == NULL){ 
		printf("获取服务失败(%d)", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	if (! DeleteService(schService) )
		printf("删除服务失败(%d)", GetLastError());
	else 
		printf("服务成功移除");
	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
}


VOID WINAPI SvcMain( DWORD dwArgc, LPTSTR *lpszArgv ){
	// Register the handler function for the service
	gSvcStatusHandle = RegisterServiceCtrlHandler( SVCNAME, SvcCtrlHandler);
	if( !gSvcStatusHandle ){ 
		SvcReportEvent("RegisterServiceCtrlHandler"); 
		return; 
	} 
	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
	gSvcStatus.dwServiceSpecificExitCode = 0;    
	ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );
	SvcInit( dwArgc, lpszArgv );
}

VOID SvcInit( DWORD dwArgc, LPTSTR *lpszArgv){
	TCHAR szPath[MAX_PATH];
	GetModuleFileName( NULL, szPath, MAX_PATH );
	TCHAR drive[MAX_PATH],dir[MAX_PATH],fname[MAX_PATH],ext[MAX_PATH];
	_tsplitpath_s( szPath,drive,dir,fname,ext );
	_tcscpy_s( szPath, drive );
	_tcscat_s( szPath, dir );
	SetCurrentDirectory( szPath );

	ghSvcStopEvent = CreateEvent(NULL,TRUE,FALSE,NULL);   // no name
	if ( ghSvcStopEvent == NULL){
		ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
		return;
	}
	m_HThread = CreateThread( (LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)MainThread,	0, 0,   &m_dwThreadID);
	if( m_HThread != NULL ){
		g_bRun = TRUE;
	}
	// Report running status when initialization is complete.
	ReportSvcStatus( SERVICE_RUNNING, NO_ERROR, 0 );
	bool bWriteFlag = false;
	while(1){
		WaitForSingleObject(ghSvcStopEvent, INFINITE);
		if(!bWriteFlag){
			printf("服务正常停止");
			bWriteFlag = true;
		}
		g_bRun = FALSE;
		ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
	}
}


VOID ReportSvcStatus( DWORD dwCurrentState,DWORD dwWin32ExitCode,DWORD dwWaitHint){
	static DWORD dwCheckPoint = 1;
	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;
	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	if ( (dwCurrentState == SERVICE_RUNNING) ||(dwCurrentState == SERVICE_STOPPED) )
		gSvcStatus.dwCheckPoint = 0;
	else gSvcStatus.dwCheckPoint = dwCheckPoint++;
	// Report the status of the service to the SCM.
	SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
}

VOID WINAPI SvcCtrlHandler( DWORD dwCtrl ){
	switch(dwCtrl) {  
	case SERVICE_CONTROL_STOP: 
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		SetEvent(ghSvcStopEvent);
		ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
		return;
	case SERVICE_CONTROL_INTERROGATE: 
		break; 
	default: 
		break;
	} 
}

VOID SvcReportEvent(LPCSTR szFunction) { 
	HANDLE hEventSource;
	LPCTSTR lpszStrings[2];
	TCHAR Buffer[80];
	hEventSource = RegisterEventSource(NULL, SVCNAME);
	if( NULL != hEventSource ){
		StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());
		lpszStrings[0] = SVCNAME;
		lpszStrings[1] = Buffer;
		ReportEvent(hEventSource,        // event log handle
			EVENTLOG_ERROR_TYPE, // event type
			0,                   // event category
			0,           // event identifier
			NULL,                // no security identifier
			2,                   // size of lpszStrings array
			0,                   // no binary data
			lpszStrings,         // array of strings
			NULL);               // no binary data
		DeregisterEventSource(hEventSource);
	}
}

BOOL ShowMenu(void){
	HDESK   hdeskCurrent;
	HDESK   hdesk;
	HWINSTA hwinstaCurrent;
	HWINSTA hwinsta;

	hwinstaCurrent = GetProcessWindowStation();
	if (hwinstaCurrent == NULL){
		return FALSE;
	}

	hdeskCurrent = GetThreadDesktop(GetCurrentThreadId());
	if (hdeskCurrent == NULL){
		return FALSE;
	}

	//打开winsta0
	hwinsta = OpenWindowStation(_T("winsta0"), FALSE,                          
		WINSTA_ACCESSCLIPBOARD   |
		WINSTA_ACCESSGLOBALATOMS |
		WINSTA_CREATEDESKTOP     |
		WINSTA_ENUMDESKTOPS      |
		WINSTA_ENUMERATE         |
		WINSTA_EXITWINDOWS       |
		WINSTA_READATTRIBUTES    |
		WINSTA_READSCREEN        |
		WINSTA_WRITEATTRIBUTES);
	if (hwinsta == NULL){
		return FALSE;
	}

	if (!SetProcessWindowStation(hwinsta)){
		return FALSE;
	}

	//打开desktop
	hdesk = OpenDesktop(_T("default"), 0, FALSE,                
		DESKTOP_CREATEMENU |
		DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE    |
		DESKTOP_HOOKCONTROL  |
		DESKTOP_JOURNALPLAYBACK |
		DESKTOP_JOURNALRECORD |
		DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP |
		DESKTOP_WRITEOBJECTS);
	if (hdesk == NULL){
		return FALSE;
	}

	SetThreadDesktop(hdesk);

	//到这一步，我们获取了和用户交互（如显示窗口）的权利
	//m_TrayIcon.Create(NULL,	WM_ICON_NOTIFY,	_T("实时通道监听服务"), ::LoadIcon(NULL, IDI_ASTERISK), IDR_POPUP_MENU);
	//m_TrayIcon.SetMenuDefaultItem(2, TRUE);       

	if (!SetProcessWindowStation(hwinstaCurrent))
		return FALSE;

	if (!SetThreadDesktop(hdeskCurrent))
		return FALSE;

	if (!CloseWindowStation(hwinsta))
		return FALSE;

	if (!CloseDesktop(hdesk))
		return FALSE;

	return TRUE;
}

VOID CALLBACK MainThread(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime){
	int argc = 0;
	char * pArgv = NULL;
	MyMain(argc, &pArgv);
	printf("主线程退出成功");
}
#endif