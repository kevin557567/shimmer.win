#include "stdafx.h"
#include "MainWinFrame.h"
#include "resource.h"
#include <iostream>
#include <json/json.h>
#include "Machine.h"

DUI_BEGIN_MESSAGE_MAP(CMainWndFrame, WindowImplBase)
DUI_ON_CLICK_CTRNAME(_T("closebtn"), OnExitBtn)
DUI_ON_CLICK_CTRNAME(_T("login_btn"), OnLoginBtn)
DUI_ON_CLICK_CTRNAME(_T("connect_btn"), OnConnectBtn)
DUI_ON_CLICK_CTRNAME(_T("option_btn"), OnOptionBtn)
DUI_END_MESSAGE_MAP()


typedef struct asnyc_req_a_t {
	CMainWndFrame * mac;
	void (CMainWndFrame::*callback)(int);
	int arg;
}asnyc_req_a;


CDuiString CMainWndFrame::GetSkinFolder() {
	// GetInstancePath 接口返回默认的皮肤文件位置
	// 在 main 函数中我们可以通过 SetResourcePath 来设置路径
	return m_PaintManager.GetInstancePath();
}

CDuiString CMainWndFrame::GetSkinFile() {
	return _T("mainFrame.xml");;
}

LPCTSTR CMainWndFrame::GetWindowClassName(void) const {
	return _T("CMainWndFrame");;
}

//theme压缩包资源
UILIB_RESOURCETYPE CMainWndFrame::GetResourceType() const{
	return UILIB_ZIPRESOURCE;
}

LPCTSTR CMainWndFrame::GetResourceID() const{
	return MAKEINTRESOURCE(IDR_ZIPRES1);
}

void CMainWndFrame::InitWindow(){
	DuiLib::CGifAnimUI *pGifCtl = (DuiLib::CGifAnimUI*)m_PaintManager.FindControl("connectbar");
	if (pGifCtl) {
		pGifCtl->StopGif();
	}
	m_pPorxyList = (CVerticalLayoutUI*)m_PaintManager.FindControl("proxy_list");
	if (m_pPorxyList) {
		m_pPorxyList->OnEvent += MakeDelegate(this, &CMainWndFrame::OnListEvent);
		//CMachine::ansyc_execute(&CMainWndFrame::ansyc_task, this, 9);
		CMainWndFrame * frm = this;
		loadServerlist();
	}
}

void CMainWndFrame::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	PostQuitMessage(0);
}

bool CMainWndFrame::OnListEvent(void* wparam) {
	TEventUI* pEvent = (TEventUI*)wparam;
	if (pEvent->Type == UIEVENT_BUTTONDOWN) {
		if (m_pPorxyList && !m_pPorxyList->IsMouseChildEnabled()) {
			for (int i = 0; i < m_pPorxyList->GetCount(); ++i) {
				CControlUI *pItem = m_pPorxyList->GetItemAt(i);
				RECT rc = pItem->GetPos();
				if (!PtInRect(&rc, pEvent->ptMouse))
					continue;
				//ConnectFailedDlg::ShowCanChange(m_hWnd);
			}
		}
	}
	return true;
}

void CMainWndFrame::AddPorxyItem(int state, std::string ip, std::string country, std::string city ) {
	CDialogBuilder builder;
	CContainerUI* pItem = (CContainerUI*)builder.Create("proxy_item.xml", (UINT)0, this, &m_PaintManager);
	if (pItem == NULL)
		return;
	pItem->SetUserData( std::to_string(1).c_str() );
	CLabelUI *label = (CLabelUI*)pItem->FindSubControl("title");
	if (label) {
		label->SetText( ip.c_str() );
	}
	CLabelUI *cityLbl = (CLabelUI*)pItem->FindSubControl("city");
	if (cityLbl) {
		cityLbl->SetText( city.c_str() );
	}
	m_pPorxyList->AddAt(pItem, state);
	COptionUI *pOption = (COptionUI*)pItem->FindSubControl("proxy_item_btn");
	if (pOption) {
		pOption->OnNotify += MakeDelegate(this, &CMainWndFrame::OnItemClick);
		pOption->SetTag(m_pPorxyList->GetCount());
		pOption->SetUserData( ip.c_str() );
	}
	//COptionUI *pCollectbtn = (COptionUI*)pItem->FindSubControl(L"collect_btn");
	//if (pCollectbtn) {
	//	pCollectbtn->OnNotify += MakeDelegate(this, &MainFrame::OnGlobalItemCollectClick);
	//	pCollectbtn->SetTag(m_pPorxyList->GetCount());
	//	pCollectbtn->SetUserData(item.country_city_type.c_str());
	//	pCollectbtn->Selected(item.is_collected);
	//}
	CControlUI *img = (CControlUI*)pItem->FindSubControl("icon");
	if (img) {
		std::string image;
		image = "guoqi/" + country;
		image +=   ".png";
		if (m_PaintManager.GetImageEx(image.c_str(), NULL, 0, false)) {
			img->SetBkImage(image.c_str());
		}
	}
}

void CMainWndFrame::loadServerlist() {
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(CMachine::rpcjson_dispath("getConfig", "config_debug"), root) && root.isObject()) {
		if (!root.isMember("serverlist") || !root["serverlist"].isArray())
			return ;
		Json::Value::ArrayIndex ArraySize = root["serverlist"].size();
		for (Json::Value::ArrayIndex index = 0; index < ArraySize; ++index) {
			Json::Value node = root["serverlist"][index];
			if (!node.isObject())
				continue;
			if (node.isMember("country") && node["country"].isString() &&
				node.isMember("ip") && node["ip"].isString() ){
				AddPorxyItem(index, node["ip"].asString().c_str(),node["country"].asString().c_str(),"city");
			}
		}
	}
}

bool CMainWndFrame::OnItemClick(void* wparam) {
	DuiLib::TNotifyUI* pMsg = (DuiLib::TNotifyUI*)wparam;
	if (pMsg->sType == DUI_MSGTYPE_CLICK) {
		pMsg->pSender->GetTag();
		std::string country = pMsg->pSender->GetUserData();
	}
	switchPlane(connect_plane);

	DuiLib::CGifAnimUI *pGifCtl = (DuiLib::CGifAnimUI*)m_PaintManager.FindControl("connectbar");
	if (pGifCtl) {
		pGifCtl->StopGif();
	}

	CContainerUI *pOption = (CContainerUI*)m_pPorxyList->GetItemAt(0);
	if (pOption) {
		CLabelUI *label = (CLabelUI*)pOption->FindSubControl("title");
		if (label) {
			std::string country = label->GetText();
		}
		CLabelUI *cityLbl = (CLabelUI*)pOption->FindSubControl("city");
		if (cityLbl) {
			std::string country = cityLbl->GetText();
		}
	}

	return 0;
}

void CMainWndFrame::OnExitBtn(TNotifyUI& msg) {
	Close();
	return;
}

void CMainWndFrame::OnLoginBtn(TNotifyUI& msg) {
	switchPlane(login_plane);
	return;
}

void CMainWndFrame::OnConnectBtn(TNotifyUI& msg) {
	//switchPlane(connect_plane);
	CMachine::getInstance()->connect();
	return;
}

void CMainWndFrame::OnOptionBtn(TNotifyUI& msg) {
	//switchPlane(option_plane);
	//CMachine::getInstance()->disconnect();

	CMachine::getInstance()->rpc_dispath();
	return;
}

void CMainWndFrame::switchPlane(plane_t plane) {
	CVerticalLayoutUI * loginPlane = (CVerticalLayoutUI*)m_PaintManager.FindControl("login_plane");
	CVerticalLayoutUI * connectPlane = (CVerticalLayoutUI*)m_PaintManager.FindControl("connect_plane");
	CVerticalLayoutUI * optionPlane = (CVerticalLayoutUI*)m_PaintManager.FindControl("option_plane");
	switch (plane) {
	case login_plane:
		if (loginPlane) loginPlane->SetVisible(true);
		if (connectPlane) connectPlane->SetVisible(false);
		if (optionPlane)optionPlane->SetVisible(false);
		break;
	case connect_plane:
		if (loginPlane) loginPlane->SetVisible(false);
		if (connectPlane) connectPlane->SetVisible(true);
		if (optionPlane)optionPlane->SetVisible(false);
		break;
	case option_plane:
		if (loginPlane) loginPlane->SetVisible(false);
		if (connectPlane) connectPlane->SetVisible(false);
		if (optionPlane)optionPlane->SetVisible(true);
		break;
	}
	return;
}