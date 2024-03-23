#ifndef _LAUNCH_SCREEN_
#define _LAUNCH_SCREEN_

class CLaunchScreen : public WindowImplBase {
protected:
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual UILIB_RESOURCETYPE GetResourceType() const override;
	virtual LPCTSTR GetResourceID() const override;

protected:

	DUI_DECLARE_MESSAGE_MAP()
};

#endif
