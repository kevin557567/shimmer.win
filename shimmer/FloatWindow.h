#ifndef _FLOATWINDOW_SCREEN_
#define _FLOATWINDOW_SCREEN_

class CFloatWindow : public WindowImplBase {
protected:
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual UILIB_RESOURCETYPE GetResourceType() const override;
	virtual LPCTSTR GetResourceID() const override;

	virtual void InitWindow() override;

protected:

	void OnLoginBtn(TNotifyUI& msg);

	DUI_DECLARE_MESSAGE_MAP()
};

#endif
