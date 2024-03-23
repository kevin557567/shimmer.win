#include "stdafx.h"
#include "FloatWindow.h"
#include "resource.h"

DUI_BEGIN_MESSAGE_MAP(CFloatWindow, WindowImplBase)
DUI_ON_CLICK_CTRNAME(_T("closebtn"), OnLoginBtn)
DUI_END_MESSAGE_MAP()

CDuiString CFloatWindow::GetSkinFolder() {
	return m_PaintManager.GetInstancePath();
}

CDuiString CFloatWindow::GetSkinFile() {
	return _T("floatWnd.xml");;
}

LPCTSTR CFloatWindow::GetWindowClassName(void) const {
	return _T("CFloatWindow");;
}

//themeÑ¹Ëõ°ü×ÊÔ´
UILIB_RESOURCETYPE CFloatWindow::GetResourceType() const
{
	return UILIB_ZIPRESOURCE;
}

LPCTSTR CFloatWindow::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_ZIPRES1);
}

void CFloatWindow::InitWindow()
{
	SetWindowPos(m_hWnd, HWND_BOTTOM, 10, 10, 0, 0, SWP_NOSIZE);
}

void CFloatWindow::OnLoginBtn(TNotifyUI& msg) {

	return;
}