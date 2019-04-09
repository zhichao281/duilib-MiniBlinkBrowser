#pragma once




#include"../../3rd/ThunderOpenSDK/xldl.h"
//////////////////////////////////////////////////////////////////////////
///

class CDownloadWnd : public WindowImplBase
{
public:
	CDownloadWnd(void);
	~CDownloadWnd(void);

public:
	void  AddDownloadItem(HANDLE hTask, DownTaskInfo  stTaskInfo);



	void  UpdateDownloadItem(HANDLE hTask, DownTaskInfo stTaskInfo);




	std::string SelectResourcePath();


	std::wstring GetDownloadPath();

	virtual void OnFinalMessage(HWND);

	virtual CDuiString GetSkinFile();

	virtual LPCTSTR GetWindowClassName(void) const;

	virtual void Notify(TNotifyUI &msg);

	virtual void InitWindow();

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemSelect(TNotifyUI &msg);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
	{
		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE:
				return ResponseDefaultKeyEvent(wParam);
			default:
				break;
			}
		}
		return FALSE;
	}
private:
	bool m_bRegist;
private:
	std::wstring m_strPath;
	CEditUI* m_pPathEdit;

	std::map<int, CDuiString>    m_mapStates;
};
