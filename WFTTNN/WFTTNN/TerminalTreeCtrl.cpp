// TerminalTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "TerminalTreeCtrl.h"
#include "ItemData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerminalTreeCtrl

CTerminalTreeCtrl::CTerminalTreeCtrl()
{
}

CTerminalTreeCtrl::~CTerminalTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CTerminalTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CTerminalTreeCtrl)
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalTreeCtrl message handlers

void CTerminalTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CRect rect_Item;

	HTREEITEM hSelItem_cur = HitTest(point);
	if(hSelItem_cur != NULL)
	{
		GetItemRect(hSelItem_cur, &rect_Item, TRUE);
		if(PtInRect(rect_Item, point))
			Select(hSelItem_cur, TVGN_CARET);
		else return;
	}

	HTREEITEM hSelItem = GetSelectedItem();
	if(hSelItem != NULL)
	{
		GetItemRect(hSelItem, &rect_Item, TRUE);

		if(PtInRect(rect_Item, point))
		{
			POINT m_posScreen;
			RECT rect;
			GetWindowRect(&rect);

			m_posScreen.x = rect.left;
			m_posScreen.y = rect.top;
		
			m_posScreen.x += point.x;
			m_posScreen.y += point.y;
            
			ItemData *data = (ItemData *)GetItemData(hSelItem);
			if (data->m_type == ITEM_TYPE_TERMINAL)
			{				
				CMenu menu;
				menu.LoadMenuA(IDR_MAINFRAME);
				menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,m_posScreen.x,m_posScreen.y,this);				
			}			
		}
	}
	
	CTreeCtrl::OnRButtonDown(nFlags, point);
}



void CTerminalTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	

	
	*pResult = 0;
}

