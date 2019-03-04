#include "../StdAfx.h"
#include "DownloadWnd.h"

#include "../../Common/String/NSString.h"

//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CDownloadWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnItemSelect)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

CDownloadWnd::CDownloadWnd(void)
{
	m_bRegist = false;
	m_strPath = _T("");
	m_mapStates[NOITEM] = _T("");
	m_mapStates[TSC_ERROR] = _T("下载异常");
	m_mapStates[TSC_PAUSE] = _T("暂停下载");
	m_mapStates[TSC_DOWNLOAD] = _T("下载中");
	m_mapStates[TSC_COMPLETE] = _T("下载完成");
	m_mapStates[TSC_STARTPENDING] = _T("开始任务") ;
	m_mapStates[TSC_STOPPENDING] = _T("结束任务");

}

CDownloadWnd::~CDownloadWnd(void)
{
}

std::wstring CDownloadWnd::GetDownloadPath()
{
	return m_strPath;
}

void CDownloadWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CDownloadWnd::InitWindow()
{
	m_pPathEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("downPath")));
	m_strPath = m_pPathEdit->GetText();
}


DuiLib::CDuiString CDownloadWnd::GetSkinFile()
{
	return _T("DownloadFrame.xml");
}

LPCTSTR CDownloadWnd::GetWindowClassName(void) const
{
	return _T("DownloadFrame");
}

std::string CDownloadWnd::SelectResourcePath()
{
	BROWSEINFO broweInfo;
	LPITEMIDLIST pil = NULL;
	INITCOMMONCONTROLSEX InitCtrls;
	std::wstring strPath;
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, MAX_PATH);
	ZeroMemory(&InitCtrls, sizeof(INITCOMMONCONTROLSEX));
	ZeroMemory(&broweInfo, sizeof(BROWSEINFO));
	broweInfo.hwndOwner = GetHWND();
	broweInfo.lpszTitle = _T("选择需要保存到到文件路径");
	broweInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_STATUSTEXT;
	InitCommonControlsEx(&InitCtrls);
	pil = SHBrowseForFolder(&broweInfo);
	if (pil)
		SHGetPathFromIDList(pil, szPath);
	CoTaskMemFree(pil);
	strPath = szPath;
	if (strPath.empty())
	{
		strPath = m_pPathEdit->GetText() + _T("\\");
	}
	else
	{
		m_pPathEdit->SetText(strPath.c_str());
	}
	m_strPath = strPath;
	return NStr::Unicode2ANSI(strPath.c_str());
}

void CDownloadWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (sName.CompareNoCase(_T("closebtn")) == 0)
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return;
	}
	else if (sName.CompareNoCase(_T("minbtn")) == 0)
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return;
	}
	else if (sName.CompareNoCase(_T("clearbtn")) == 0)
	{
		// List控件添加元素
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("listViewEx")));
		pList->RemoveAll();

	}
	else if (sName.CompareNoCase(_T("newbtn")) == 0)
	{

	}
	else if (sName.CompareNoCase(_T("settingbtn")) == 0)
	{
		CTabLayoutUI * download_tab = static_cast <CTabLayoutUI*>(m_pm.FindControl(_T("down_Tablayout")));
		if (download_tab)
		{
			if (download_tab->GetCurSel() == 0)
			{
				download_tab->SelectItem(1);
			}
			else
			{
				download_tab->SelectItem(0);
			}

		}
	}
	else if (!_tcsicmp(sName, _T("savePath")))
	{

		std::string strFilePath = SelectResourcePath();
	}

}

void CDownloadWnd::OnSelectChanged(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	//账号登录
	if (sName.CompareNoCase(_T("btnAccount")) == 0)
	{
	}
	else if (sName.CompareNoCase(_T("btnSMSLogin")) == 0)
	{
	}
}

void CDownloadWnd::OnItemSelect(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
}

LRESULT CDownloadWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

void CDownloadWnd::Notify(TNotifyUI &msg)
{
	return WindowImplBase::Notify(msg);
}

LRESULT CDownloadWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
	if (wParam == SC_CLOSE) {
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(*this) != bZoomed) {
		if (!bZoomed) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if (pControl) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if (pControl) pControl->SetVisible(true);
		}
		else {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if (pControl) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if (pControl) pControl->SetVisible(false);
		}
	}
	return lRes;
}


void CDownloadWnd::AddDownloadItem(HANDLE hTask, DownTaskInfo  stTaskInfo)
{

	// List控件添加元素
	CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("listViewEx")));
	if (pList != NULL)
	{
		CDialogBuilder builder1;
		CListContainerElementUI* pListItem = (CListContainerElementUI*)builder1.Create(_T("downitem.xml"), NULL, this, &m_pm, NULL);

		CControlUI* pLabel = pListItem->FindSubControl(_T("NumberLaber"));
		if (pLabel != NULL) pLabel->SetText(NStr::IntToWStr((int)hTask).c_str());


		CControlUI* pFileNameLabel = pListItem->FindSubControl(_T("downFileNameLabel"));
		if (pFileNameLabel != NULL) pFileNameLabel->SetText(stTaskInfo.szFilename);

		CProgressUI* pPro1 = static_cast<CProgressUI*>(pListItem->FindSubControl(_T("downprogress")));
		pPro1->SetValue(stTaskInfo.fPercent * 100);

		TCHAR szSizeInfo[16] = { 0 };
		_stprintf_s(szSizeInfo, 16, TEXT("%I64d/%I64d"), stTaskInfo.nTotalDownload, stTaskInfo.nTotalSize);

		CControlUI* pSizeInfoLabel = pListItem->FindSubControl(_T("sizeInfoLabel"));
		if (pSizeInfoLabel != NULL) pSizeInfoLabel->SetText(szSizeInfo);
	
	
		CControlUI* pStyleBtn = pListItem->FindSubControl(_T("stateLabel"));
		if (pStyleBtn != NULL) pStyleBtn->SetText(m_mapStates[stTaskInfo.stat]);

		int sec = 0;
		if (stTaskInfo.nSpeed >0 )
		{
			sec = (stTaskInfo.nTotalSize - stTaskInfo.nTotalDownload) / stTaskInfo.nSpeed;
		}
		
		int h = sec / 3600;
		int m = sec % 3600 / 60;
		int s = sec % 60; //不足60的就是秒，够60就是分
		TCHAR szSpeedInfo[16] = { 0 };
		_stprintf_s(szSpeedInfo, 16, TEXT("%d KB/S %02d:%02d:%02d"), stTaskInfo.nSpeed / 1024,
			h, m, s);
		CControlUI* pleftTimeLabel = pListItem->FindSubControl(_T("leftTimeLabel"));
		if (pleftTimeLabel != NULL) pleftTimeLabel->SetText(szSpeedInfo);

		pList->Add(pListItem);
	}



}

void CDownloadWnd::UpdateDownloadItem(HANDLE hTask, DownTaskInfo  stTaskInfo)
{
	// List控件添加元素
	CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("listViewEx")));
	for (int i = 0; i < pList->GetCount(); i++)
	{
		CListContainerElementUI *pListElement = static_cast<CListContainerElementUI*>(pList->GetItemAt(i));
		if (pListElement)
		{
			CControlUI* pLabel = pListElement->FindSubControl(_T("NumberLaber"));
			if (pLabel != NULL && pLabel->GetText().CompareNoCase(NStr::IntToWStr((int)hTask).c_str()) == 0)
			{
				CProgressUI* pPro1 = static_cast<CProgressUI*>(pListElement->FindSubControl(_T("downprogress")));
				pPro1->SetValue(stTaskInfo.fPercent * 100);			

				CControlUI* pFileNameLabel = pListElement->FindSubControl(_T("downFileNameLabel"));
				if (pFileNameLabel != NULL) pFileNameLabel->SetText(stTaskInfo.szFilename);

				CControlUI* pStyleBtn = pListElement->FindSubControl(_T("stateLabel"));
				if (pStyleBtn != NULL) pStyleBtn->SetText(m_mapStates[stTaskInfo.stat]);

				TCHAR szSizeInfo[64] = { 0 };
				int nTotalSize = stTaskInfo.nTotalSize / 1024 / 1024;
				int nTotalDownload = stTaskInfo.nTotalDownload / 1024 / 1024;
				_stprintf_s(szSizeInfo, 64, TEXT("%d M/%d M"), nTotalDownload, nTotalSize);
				CControlUI* pSizeInfoLabel = pListElement->FindSubControl(_T("sizeInfoLabel"));
				if (pSizeInfoLabel != NULL) pSizeInfoLabel->SetText(szSizeInfo);

				int sec = 0;
				if (stTaskInfo.nSpeed > 0)
				{
					sec = (stTaskInfo.nTotalSize - stTaskInfo.nTotalDownload) / stTaskInfo.nSpeed;
				}
	
				int h = sec / 3600;
				int m = sec % 3600 / 60;
				int s = sec % 60; //不足60的就是秒，够60就是分
				TCHAR szSpeedInfo[32] = { 0 };
				_stprintf_s(szSpeedInfo, 32, TEXT("%d KB/S %02d:%02d:%02d"), stTaskInfo.nSpeed / 1024 ,
					h, m, s);
				CControlUI* pleftTimeLabel = pListElement->FindSubControl(_T("leftTimeLabel"));
				if (pleftTimeLabel != NULL) pleftTimeLabel->SetText(szSpeedInfo);

			}
		}
	}
}
