#include "stdafx.h"
#include "LaunchScreen.h"
#include "resource.h"

DUI_BEGIN_MESSAGE_MAP(CLaunchScreen, WindowImplBase)
DUI_END_MESSAGE_MAP()

CDuiString CLaunchScreen::GetSkinFolder() {
	return m_PaintManager.GetInstancePath();
}

CDuiString CLaunchScreen::GetSkinFile() {
	return _T("skin.xml");;
}

LPCTSTR CLaunchScreen::GetWindowClassName(void) const {
	return _T("CLaunchScreen");;
}

//themeÑ¹Ëõ°ü×ÊÔ´
UILIB_RESOURCETYPE CLaunchScreen::GetResourceType() const
{
	return UILIB_ZIPRESOURCE;
}

LPCTSTR CLaunchScreen::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_ZIPRES1);
}
