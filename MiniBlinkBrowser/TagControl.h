

#include "stdafx.h"



class CTagMoveUI : public CHorizontalLayoutUI
	{
	public:	
		CTagMoveUI();

		LPCTSTR GetClass() const
		{
			return _T("TagMoveUI");
		}

		LPVOID GetInterface( LPCTSTR pstrName )
		{
			if( _tcscmp(pstrName, _T("TagMove")) == 0 ) 
				return static_cast<CTagMoveUI*>(this);

			return CHorizontalLayoutUI::GetInterface(pstrName);
		}

		virtual void DoEvent(TEventUI& event);

		void UpdateOldRect(){m_rcOldRect = m_rcItem;}
		void SetOldRect(RECT rect){m_rcOldRect = rect;}
		RECT GetOldRect(){return m_rcOldRect; }
		void SetTargetRect(RECT rect);
		RECT GetTargetRect();
		

	protected:
		enum{ ID_MOVE_TIMER = 15};	//用于移动的定时器ID
		
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

	};

	class CTagLayoutUI : public CHorizontalLayoutUI
	{
	public:	
		CTagLayoutUI();

		LPCTSTR GetClass() const
		{
			return _T("TagLayoutUI");
		}

		LPVOID GetInterface( LPCTSTR pstrName )
		{
			if( _tcscmp(pstrName, _T("TagLayout")) == 0 ) 
				return static_cast<CTagLayoutUI*>(this);

			return CHorizontalLayoutUI::GetInterface(pstrName);
		}

		bool SetItemIndexNoRedraw(CControlUI* pControl, int index)
		{
			int nCount =   m_items.GetSize();

			for( int it = 0; it < nCount; it++) 
			{					
				if( static_cast<CControlUI*>(m_items[it]) == pControl ) 
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
			int nCount =   m_items.GetSize();

			for( int it = 0; it < nCount; ) 
			{					
				if( static_cast<CControlUI*>(m_items[it]) == pControl ) 
				{ 
					nCount = nCount - 1;
					m_items.Remove(it);
					m_items.InsertAt(nCount, pControl);

					CTagMoveUI* pTagMoveUI = (CTagMoveUI*)(pControl->GetInterface(_T("TagMove")));
					if(pTagMoveUI) pTagMoveUI->UpdateOldRect();

					isAddSucc = true;
				}
				else
				{
					CControlUI* pControlItem =  static_cast<CControlUI*>(m_items[it]);
					CTagMoveUI* pTagMoveUI = (CTagMoveUI*)(pControlItem->GetInterface(_T("TagMove")));
					if(pTagMoveUI) pTagMoveUI->UpdateOldRect();
					it++;
				}
			}

			return isAddSucc;
		}

		void SetEmptyPlaceRect(RECT rect){ m_rtEmptyPlace = rect;}
		RECT GetEmptyPlaceRect() { return m_rtEmptyPlace;}

	protected:
		RECT m_rtEmptyPlace;

	};