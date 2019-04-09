

#include "stdafx.h"


#define DUI_MSGTYPE_TABINDEXCHANGED			(_T("tabindexchanged"))
#define DUI_MSGTYPE_TABCLOSED				(_T("tabclosed"))
#define DUI_MSGTYPE_PAGERCHANGED			(_T("pagerchanged"))



class  CBrowserOptionUI : public COptionUI
{
public:
	CBrowserOptionUI();

	LPCTSTR GetClass() const
	{
		return _T("BrowserOptionUI");
	}

	LPVOID GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, _T("BrowserOption")) == 0)
			return static_cast<CBrowserOptionUI*>(this);

		return COptionUI::GetInterface(pstrName);
	}
	   
	virtual void DoEvent(TEventUI& event)
	{

	/*	if (event.Type == UIEVENT_BUTTONDOWN)
		{			
			m_pManager->SendNotify(this, DUI_MSGTYPE_TABINDEXCHANGED, event.wParam, event.lParam);
		}*/
		COptionUI::DoEvent(event);
	}
};


class CBrowserTabUI : public CContainerUI
{
public:
	CBrowserTabUI();

	LPCTSTR GetClass() const
	{
		return _T("BrowserTabUI");
	}

	LPVOID GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, _T("BrowserTab")) == 0)
			return static_cast<CBrowserTabUI*>(this);

		return __super::GetInterface(pstrName);
	}

	
public:

	void SetTabImage(LPCTSTR pStrImage);

	void UpdateOldRect() { m_rcOldRect = m_rcItem; }
	void SetOldRect(RECT rect) { m_rcOldRect = rect; }
	RECT GetOldRect() { return m_rcOldRect; }
	void SetTargetRect(RECT rect);

	RECT GetTargetRect();

	virtual void SetText(LPCTSTR pstrText);

	virtual void SetToolTip(LPCTSTR pstrText);	
	virtual void OnClick(TNotifyUI& msg);


protected:

	//当控件被添加到容器后，由容器调用的函数。
//在这里，整个Duilib程序框架已经完成，当需要做一些界面的初始操作时可以重写此函数，
//常见的用法就是在此建立Win32子窗体并且封装它，
	virtual void DoInit();

	virtual void DoEvent(TEventUI& event);


	virtual void Notify(TNotifyUI& msg);
	
	virtual bool AddAt(CControlUI* pControl, int iIndex);

	virtual void SetFocus();



protected:
	enum { ID_MOVE_TIMER = 15 };	//用于移动的定时器ID

	bool m_bIsLButtonDowning;	//是否按下左键
	int	m_nLButtonDownCX;		//鼠标在按钮中的位置CX
	int m_nLButtonDownCY;		//鼠标在按钮中的位置CY

	RECT m_rcTargetRect;		//目标位置
	RECT m_rcCurrentRect;		//当前位置
	RECT m_rcOldRect;			//移动前的位置
	int m_nStepLen;				//移动步长
	bool m_bIsMoving;			//是否正在移动

	bool m_isDragged;			//是否是被拖动的Item
	int m_nEndPos;				//被拖动Item最后的Pos;


	CBrowserOptionUI* m_pOption;

	CButtonUI* m_pCloseBtn;
	CButtonUI *m_pIcon;
private:
	int m_nStartTab;
	int m_nSelectedTab;
protected:
	int m_iSepWidth;
	UINT m_uButtonState;
	POINT ptLastMouse;
	RECT m_rcNewPos;
	bool m_bImmMode;

};

class CBrowserTabBarUI :  public CHorizontalLayoutUI
{
public:
	CBrowserTabBarUI();

	LPCTSTR GetClass() const
	{
		return _T("BrowserTabBarUI");
	}

	LPVOID GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, _T("BrowserTabBar")) == 0)
			return static_cast<CBrowserTabBarUI*>(this);

		return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	bool SetItemIndexNoRedraw(CControlUI* pControl, int index)
	{
		int nCount = m_items.GetSize();

		for (int it = 0; it < nCount; it++)
		{
			if (static_cast<CControlUI*>(m_items[it]) == pControl)
			{
				m_items.Remove(it);
				m_items.InsertAt(index, pControl);
				return true;
			}
		}

		return false;
	}

	bool IntItemsOldRect(CControlUI* pControl)
	{
		bool isAddSucc = false;
		int nCount = m_items.GetSize();

		for (int it = 0; it < nCount; )
		{
			if (static_cast<CControlUI*>(m_items[it]) == pControl)
			{
				nCount = nCount - 1;
				m_items.Remove(it);
				m_items.InsertAt(nCount, pControl);

				CBrowserTabUI* pTagMoveUI = (CBrowserTabUI*)(pControl->GetInterface(_T("TagMove")));
				if (pTagMoveUI) pTagMoveUI->UpdateOldRect();

				isAddSucc = true;
			}
			else
			{
				CControlUI* pControlItem = static_cast<CControlUI*>(m_items[it]);
				CBrowserTabUI* pTagMoveUI = (CBrowserTabUI*)(pControlItem->GetInterface(_T("TagMove")));
				if (pTagMoveUI) pTagMoveUI->UpdateOldRect();
				it++;
			}
		}

		return isAddSucc;
	}

	void SetEmptyPlaceRect(RECT rect) { m_rtEmptyPlace = rect; }
	RECT GetEmptyPlaceRect() { return m_rtEmptyPlace; }

public:
	//bool Add(CControlUI* pControl);
	//bool AddAt(CControlUI* pControl, int iIndex);
	//void Invalidate();

	//void SetStartTab(int nStart);
	//void SelectTab(CBrowserTabUI *pTab);
	//void SelectTab(int nIndex);
	//void CloseTab(CBrowserTabUI *pTab, BOOL bPrevSelected = TRUE);
	//int GetTabCount();
	//int GetVisibleTabCount();
	//CBrowserTabUI* GetTab(int nIndex);
	//int GetTabIndex(CBrowserTabUI *pTab);
	//CBrowserTabUI* GetPrevTab(CBrowserTabUI *pTab);
	//CBrowserTabUI* GetNextTab(CBrowserTabUI *pTab);



protected:
	RECT m_rtEmptyPlace;

};