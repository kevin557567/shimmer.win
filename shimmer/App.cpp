// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Machine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow){
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetCurrentPath(CPaintManagerUI::GetInstancePath());
	CMachine::getInstance()->run();
	CMachine::deleteInstance();
	CPaintManagerUI::Term();
	::CoUninitialize();
	return 0;
}