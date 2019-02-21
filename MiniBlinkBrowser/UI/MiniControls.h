#pragma once

#include "../stdafx.h"


#define  DUI_CTR_LISTHBOXELEMENT                 (_T("ListHBoxElement"))
#define  DUI_CTR_ICON						     (_T("Icon"))


namespace DuiLib
{

	class CIconUI : public CControlUI
	{
	public:
		CIconUI(void);
		~CIconUI(void);
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintBkImage(HDC hDC);

		void SetIcon(HICON hIcon);
		void SetInset(RECT rc);
	private:
		HICON m_hIcon;
		RECT  m_rcInset;
	};

}