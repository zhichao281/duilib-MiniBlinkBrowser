


#include "TagControl.h"



CBrowserTabBarUI::CBrowserTabBarUI()
	{
		ZeroMemory(&m_rtEmptyPlace,sizeof(m_rtEmptyPlace));
	}

	CBrowserTabUI::CBrowserTabUI()
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
		SetEnabled(false);
	}

	void CBrowserTabUI::DoInit()
	{
	
		//OptionUI
		m_pOption = new CBrowserOptionUI;
		//关闭按钮
		m_pCloseBtn = new CButtonUI;
		//图标
		m_pIcon = new CContainerUI;		
		//标签HorLayout  ==  OptionUI + 关闭按钮	
		this->ApplyAttributeList(_T("maxwidth=\"174\" name=\"browser_tabbar\" "));
		
		//在标签HorLayout上增加 OptionUI 和 关闭按钮
		this->Add(m_pOption);
		this->Add(m_pCloseBtn);
		this->Add(m_pIcon);
		m_pOption->ApplyAttributeList(_T(" float=\"true\" ")\
			_T("group=\"group1\" text=\"新标签页\" name=\"browseroption\" pos=\"0,0,174,32\" endellipsis=\"true\" textpadding=\"50,0,50,0\"  text=\"新标签页\" maxwidth=\"174\" height=\"30\" maxwidth=\"174\" normalimage=\"file=\'imgs/tabitem.png\' dest = \'0,0,174,32\' source=\'0,0,174,32\'\" hotimage=\"file=\'imgs/tabitem.png\' source=\'174,0,348,32\'\" pushedimage=\"file=\'imgs/tabitem.png\' source=\'348,0,522,32\'\" selectedimage=\"file=\'imgs/tabitem.png\' source=\'348,0,522,32\'\" align=\"center\" "));

		m_pCloseBtn->ApplyAttributeList(_T("float=\"true\" pos=\"150, 12, 172, 24\" name=\"btn_tabclose\" normalimage=\"file = \'imgs/tab_close.png\' dest = \'0,0,12,12\' source = \'0,0,12,12\'\" hotimage=\"file = \'imgs/tab_close.png\' dest = \'0,0,12,12\' source = \'12,0,24,12\'\" pushedimage=\"file =\'imgs/tab_close.png\' dest = \'0,0,12,12\' source = \'24,0,36,12\'\""));
		m_pIcon->SetAttribute(L"pos", L"0,0,50,32");
		m_pIcon->ApplyAttributeList(_T(" float=\"true\" mouse=\"false\" pos=\"0,2,50,32\" bkimage=\"file = \'imgs/logo.png\' dest = \'0,0,30,30\' source = \'0,0,30,30\'\""));

	
		this->SetAttribute(L"name", L"browsertab");



	}

	void CBrowserTabUI::DoEvent( TEventUI& event )
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
					CBrowserTabBarUI* pParentContainer = static_cast<CBrowserTabBarUI*>(GetParent());
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
				CBrowserTabBarUI* pParentContainer = static_cast<CBrowserTabBarUI*>(GetParent());
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
						CBrowserTabUI* pNext = static_cast<CBrowserTabUI*>(
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

				CBrowserTabBarUI* pParentContainer = static_cast<CBrowserTabBarUI*>(GetParent());
				if (pParentContainer)
				{
					int nCount = pParentContainer->GetCount();
					for (int i=0; i < nCount; i++)
					{
						CBrowserTabUI* pNext = static_cast<CBrowserTabUI*>(
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
					CBrowserTabBarUI* pParentContainer = static_cast<CBrowserTabBarUI*>(GetParent());
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

		__super::DoEvent(event);

	}

	void CBrowserTabUI::Notify(TNotifyUI & msg)
	{
		if (msg.sType == _T("dbclick"))
		{
			// 这里会传进来很多次双击消息，所以只获取祖先控件的消息
			if (!msg.pSender->GetParent())
			{
				//FullScreen(!m_bFullScreenMode);
			}
		}

	}

	bool CBrowserTabUI::AddAt(CControlUI * pControl, int iIndex)
	{
		m_nSelectedTab = iIndex;
		return __super::AddAt(pControl,iIndex);
	}

	void CBrowserTabUI::SetFocus()
	{
		m_pOption->Selected(true);
		__super::SetFocus();
	}


	void CBrowserTabUI::SetTargetRect( RECT rect )
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

	RECT CBrowserTabUI::GetTargetRect()
	{
		return m_rcTargetRect;
	}

	void CBrowserTabUI::SetText(LPCTSTR pstrText)
	{
		m_pOption->SetText(pstrText);

		__super::SetText(pstrText);
	}

	void CBrowserTabUI::SetToolTip(LPCTSTR pstrText)
	{
		m_pOption->SetToolTip(pstrText);
	}

	void CBrowserTabUI::OnClick(TNotifyUI & msg)
	{
	}

	CBrowserOptionUI::CBrowserOptionUI()
	{
	}
