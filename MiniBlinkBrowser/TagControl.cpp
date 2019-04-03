


#include "TagControl.h"

CTagLayoutUI::CTagLayoutUI()
	{
		ZeroMemory(&m_rtEmptyPlace,sizeof(m_rtEmptyPlace));
	}

	CTagMoveUI::CTagMoveUI()
	:m_bIsLButtonDowning(false)
	,m_nLButtonDownCX(0)
	,m_nLButtonDownCY(0)
	,m_nStepLen(1)
	,m_bIsMoving(false)
	,m_isDragged(false)
	,m_nEndPos(-1)
	{
		ZeroMemory(&m_rcTargetRect,sizeof(m_rcTargetRect));
		ZeroMemory(&m_rcOldRect,sizeof(m_rcOldRect));
		ZeroMemory(&m_rcCurrentRect,sizeof(m_rcCurrentRect));
	}

	void CTagMoveUI::DoEvent( TEventUI& event )
	{
		
		if( event.Type == UIEVENT_BUTTONDOWN )
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) 
			{
				if(m_bIsMoving == false)
				{
					m_bIsLButtonDowning = true;
					m_nLButtonDownCX = event.ptMouse.x - m_rcItem.left;
					m_nLButtonDownCY = event.ptMouse.y - m_rcItem.top;
					CTagLayoutUI* pParentContainer = static_cast<CTagLayoutUI*>(GetParent());
					if (pParentContainer)
					{
						pParentContainer->SetEmptyPlaceRect(m_rcItem);
						pParentContainer->IntItemsOldRect(this);
					}
				}	
			}
			return ;

		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			
			if (m_bIsLButtonDowning)
			{
				CTagLayoutUI* pParentContainer = static_cast<CTagLayoutUI*>(GetParent());
				if (pParentContainer)
				{
					RECT rtParent = pParentContainer->GetPos();
					RECT rect = m_rcItem;
					rect.left = event.ptMouse.x - m_nLButtonDownCX;
					rect.right = rect.left + (m_rcItem.right - m_rcItem.left);
					//超出rtParent的范围
					if(rect.left < (rtParent.left - (m_rcItem.right - m_rcItem.left)/3)
						|| rect.right > rtParent.right + (m_rcItem.right - m_rcItem.left)/3)
						return;

					SetPos(rect);
					int nCount = pParentContainer->GetCount() - 1;
					for (int i=0; i< nCount; i++)
					{
						CTagMoveUI* pNext = static_cast<CTagMoveUI*>(
							pParentContainer->GetItemAt(i));
						if (!pNext) continue;
						RECT rt = pNext->GetOldRect();
						int mid1 = rt.left + (rt.right-rt.left)/2;
						int mid2 = m_rcItem.left + (m_rcItem.right-m_rcItem.left)/2;

						
						if((m_rcItem.right > mid1 && mid2 < mid1)
							||(m_rcItem.left < mid1 && mid2 > mid1))
						{
							RECT oldEmptyRect = pParentContainer->GetEmptyPlaceRect();
							RECT tempRect = oldEmptyRect;
							
							

							SetOldRect(rt);
							pNext->SetOldRect(oldEmptyRect);
							pNext->SetTargetRect(tempRect);
							pParentContainer->SetEmptyPlaceRect(rt);	
							Invalidate();
							break;
						}

					}

				}

			}
			else{
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
			}
			
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if (m_bIsLButtonDowning)
			{
				m_bIsLButtonDowning = false;

				CTagLayoutUI* pParentContainer = static_cast<CTagLayoutUI*>(GetParent());
				if (pParentContainer)
				{
					int nCount = pParentContainer->GetCount();
					for (int i=0; i < nCount; i++)
					{
						CTagMoveUI* pNext = static_cast<CTagMoveUI*>(
							pParentContainer->GetItemAt(i));
						if (!pNext) continue;
						RECT rt = pNext->GetOldRect();
						if (rt.right > m_rcItem.right)
						{
							m_nEndPos = i;
							break;
						}
					}
				}
				
				if(m_rcItem.left != m_rcOldRect.left)
				{
					m_isDragged = true;
					SetTargetRect(m_rcOldRect);
				}
					
			}
			return;
		}

		if( event.Type == UIEVENT_TIMER && event.wParam == ID_MOVE_TIMER )
		{

			m_rcCurrentRect.left += m_nStepLen;
			m_rcCurrentRect.right += m_nStepLen;
			if ((m_nStepLen < 0 && m_rcCurrentRect.left <= m_rcTargetRect.left)
				||(m_nStepLen > 0 && m_rcCurrentRect.left >= m_rcTargetRect.left))
			{
				m_rcCurrentRect = m_rcTargetRect;
				ZeroMemory(&m_rcTargetRect,sizeof(m_rcTargetRect));
				m_pManager->KillTimer(this, ID_MOVE_TIMER);
				m_bIsMoving = false;

				if(m_isDragged && m_nEndPos != -1)
				{
					CTagLayoutUI* pParentContainer = static_cast<CTagLayoutUI*>(GetParent());
					if (pParentContainer)
						pParentContainer->SetItemIndexNoRedraw(this, m_nEndPos);

					m_isDragged = false;
					m_nEndPos = -1;
				}
				
			}
			SetPos(m_rcCurrentRect);
			Invalidate();
			return;
		}

		CHorizontalLayoutUI::DoEvent(event);

	}

	void CTagMoveUI::SetTargetRect( RECT rect )
	{
		
		//不需要移动
		//if((m_bIsMoving == false) && (rect.left == m_rcItem.left))
		//	return;

		//正在移动
		m_bIsMoving = true;
		if(rect.left == m_rcTargetRect.left)
			return;

		m_rcTargetRect = rect;
		m_rcCurrentRect = m_rcItem;
		int nStepLen = (m_rcTargetRect.left - m_rcCurrentRect.left)/10;
		if (nStepLen == 0)
			m_nStepLen = m_rcTargetRect.left - m_rcCurrentRect.left;
		else
			m_nStepLen = nStepLen;

		m_pManager->SetTimer(this, ID_MOVE_TIMER, 30U);
	}

	RECT CTagMoveUI::GetTargetRect()
	{
		return m_rcTargetRect;
	}