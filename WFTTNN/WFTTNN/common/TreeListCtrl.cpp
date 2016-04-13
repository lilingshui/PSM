// TreeListView.cpp : 实现文件
//

#include "stdafx.h"
#include <CommonUI/TreeListCtrl.h>

#define ID_HEADER 2399
#define ID_TREE_LIST 2398
#define ID_SCROLLBAR 2397

// CTreeListView

IMPLEMENT_DYNAMIC(CTreeListCtrl, CWnd)
CTreeListCtrl::CTreeListCtrl()
{
	m_pMessageRecver = NULL;
}

CTreeListCtrl::~CTreeListCtrl()
{
	int nCount = (int)m_vLine.size();
	for (int n=0;n<nCount;n++)
	{
		if (m_vLine[n] != NULL)
			delete m_vLine[n];
	}
}


BEGIN_MESSAGE_MAP(CTreeListCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_MESSAGE(UM_ITEM_LDB_CLICK, OnNMItemLDBClick)
	ON_MESSAGE(UM_ITEM_L_CLICK, OnNMItemLClick)
	ON_MESSAGE(UM_ITEM_R_CLICK, OnNMItemRClick)
	ON_MESSAGE(UM_ITEM_PROP_CHANGED, OnNMItemPropChanged)
END_MESSAGE_MAP()



// CTreeListView 消息处理程序
CWnd *CTreeListCtrl::GetMessageRecver()
{
	return m_pMessageRecver;
}

void CTreeListCtrl::SetMessageRecver(CWnd *pWnd)
{
	m_pMessageRecver = pWnd;
}

CTreeList & CTreeListCtrl::GetTreeList()
{
	return m_TreeList;
}

CTreeListHeader & CTreeListCtrl::GetHeader()
{
	return m_Header;
}


int CTreeListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect wndRect;
	GetWindowRect(&wndRect);

	CRect headerRect;
	headerRect.top = 0;
	headerRect.left = 0;
	headerRect.bottom = 22;
	headerRect.right = wndRect.Width();

	m_Header.Create(WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ |HDS_HOTTRACK, 
						headerRect, this, ID_HEADER);

	m_Header.InitializeHeader(true);
	m_Header.ShowSortArrow(true);

	m_headerFont.CreateFont(14,0,0,0,FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,NULL);
	
	m_Header.SetFont(&m_headerFont);

	CRect treeRect;
	GetWindowRect(&wndRect);

	treeRect.top = headerRect.bottom;
	treeRect.left = 0;
	treeRect.bottom = wndRect.Height();
	treeRect.right = wndRect.Width();

	m_TreeList.Create(	WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT 
					| TVS_HASBUTTONS | TVS_SHOWSELALWAYS | WS_VSCROLL | ES_AUTOVSCROLL | TVS_FULLROWSELECT , 
					treeRect, this, ID_TREE_LIST);

	m_TreeList.SetItemHeight(21);

	m_TreeList.SetHeader(&m_Header);

	return 0;
}

int CTreeListCtrl::InsertColumn( int nCol,CString strColumnText,
								 int nFormat,int nWidth)
{
	HD_ITEM hdi;
	hdi.mask = HDI_TEXT | HDI_FORMAT;
	if(nWidth!=-1)
	{
		hdi.mask |= HDI_WIDTH;
		hdi.cxy = nWidth;
	}
	hdi.pszText = (LPTSTR)strColumnText.GetBuffer(0);
	hdi.fmt = HDF_OWNERDRAW;
	if(nFormat == LVCFMT_RIGHT)
		hdi.fmt |= HDF_RIGHT;
	else if(nFormat == LVCFMT_CENTER)
		hdi.fmt |= HDF_CENTER;
	else
		hdi.fmt |= HDF_LEFT;

	int nReturn = m_Header.InsertItem(nCol, &hdi);

	ResetScrollBar();

	return nReturn;
}

CTreeListLine *CTreeListCtrl::InsertLine(CString strLineText,int nImage, int nSelectedImage,
								CTreeListLine *pParent,CTreeListLine *pInsertAfter,
								HTREEITEM hAfterItem)
{
	HTREEITEM hParentItem = TVI_ROOT;
	if (pParent != NULL)
		hParentItem = pParent->GetTreeItem();

	HTREEITEM hInsertAfter = TVI_LAST;
	if (pInsertAfter != NULL)
		hInsertAfter = pInsertAfter->GetTreeItem();
	else
		hInsertAfter = hAfterItem;

	CTreeListLine *pLine = new CTreeListLine(&m_TreeList);
	pLine->SetItemText(0,strLineText);
	HTREEITEM hItem = m_TreeList.InsertItem(TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE, 
		strLineText, nImage, nSelectedImage, 0, 0, (LPARAM)pLine, hParentItem, hInsertAfter);

	if (hItem == NULL)
	{
		delete pLine;
		return NULL;
	}
	else
	{
		pLine->SetTreeItem(hItem);
		m_vLine.push_back(pLine);
		return pLine;
	}
}

void CTreeListCtrl::DeleteLine(CTreeListLine *pLine)
{
	if (pLine == NULL)
		return ;

	m_TreeList.HideProp();

	HTREEITEM hItem = pLine->GetTreeItem();
	if (hItem == NULL)
		return ;

	while (1)
	{
		HTREEITEM hChild = m_TreeList.GetChildItem(hItem);
		if (hChild)
		{
			CTreeListLine *pChildLine = (CTreeListLine *)m_TreeList.GetItemData(hChild);
			DeleteLine(pChildLine);
		}
		else
			break;
	}
	m_TreeList.DeleteItem(hItem);
	vector <CTreeListLine *>::iterator it;
	for(it = m_vLine.begin(); it != m_vLine.end(); it++)
	{
		if (pLine == *it)
		{
			m_vLine.erase(it);
			delete pLine;
			break;
		}
	}
	
}


void CTreeListCtrl::DeleteLine(HTREEITEM hItem)
{
	if (hItem == NULL)
		return ;
	if (hItem == NULL)
	{
		DeleteAll();
		return;
	}

	CTreeListLine *pLine = (CTreeListLine *)m_TreeList.GetItemData(hItem);
	DeleteLine(pLine);
}


void CTreeListCtrl::DeleteAll()
{
	m_TreeList.DeleteAllItems();

	int nCount = (int)m_vLine.size();
	for (int n=0;n<nCount;n++)
	{
		if (m_vLine[n] != NULL)
			delete m_vLine[n];
	}
	m_vLine.clear();
}

void CTreeListCtrl::UpdateCtrl()
{
	m_TreeList.HideProp();
}

CTreeListLine * CTreeListCtrl::GetLine(HTREEITEM hItem)
{
	if (hItem == NULL || hItem == TVI_ROOT)
		return NULL;

	return (CTreeListLine *)m_TreeList.GetItemData(hItem);

}

int CTreeListCtrl::GetChildCount(CTreeListLine *pLine)
{
	HTREEITEM parent = NULL;
	if (pLine == NULL)
	{
		parent = TVI_ROOT;
	}
	else
	{
		parent = pLine->GetTreeItem();
	}

	if (parent == NULL)
		return 0;

	int nCount = 0;
	HTREEITEM item = m_TreeList.GetChildItem(parent);
	while (item)
	{
		nCount++;
		item = m_TreeList.GetNextItem(item,TVGN_NEXT);
	}
	return nCount;
}

BOOL CTreeListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	HD_NOTIFY *pHDN = (HD_NOTIFY*)lParam;
	if((wParam == ID_HEADER) && (pHDN->hdr.code == HDN_ITEMCHANGED))
	{
		ResetScrollBar();
		m_TreeList.HideProp();
		m_TreeList.RedrawWindow();
		m_TreeList.Invalidate();
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}


void CTreeListCtrl::ResetScrollBar()
{
	CRect treeRect;
	m_TreeList.GetClientRect(&treeRect);

	int nPageWidth = treeRect.Width();
	int nTotalWidth = m_Header.GetColumnsWidth();

	if(nTotalWidth > nPageWidth)
	{
		SCROLLINFO si;
		m_TreeList.GetScrollInfo( SB_HORZ, &si, SIF_ALL );		
		si.cbSize	= sizeof( SCROLLINFO );
		si.nMin		= 0;
		si.nMax		= nTotalWidth;
		si.nPage	= nPageWidth;
		si.fMask	=  SIF_PAGE | SIF_RANGE | SIF_POS;
		if( si.nPos > si.nMax - (int)si.nPage )
			si.nPos = si.nMax - (int)si.nPage;
		m_TreeList.SetScrollInfo(SB_HORZ, &si, TRUE );
		m_TreeList.EnableScrollBarCtrl(SB_HORZ,TRUE);
	}
	else
	{
		m_TreeList.SetScrollRange(SB_HORZ,0,0);
		m_TreeList.ShowScrollBar(SB_HORZ,SW_HIDE);
		m_TreeList.EnableScrollBarCtrl(SB_HORZ,FALSE);
	}
}



BOOL CTreeListCtrl::VerticalScrollVisible()
{
	int sMin, sMax;
	m_TreeList.GetScrollRange(SB_VERT, &sMin, &sMax);
	return sMax!=0;
}


void CTreeListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);


	CRect wndRect;
	GetClientRect(&wndRect);

	CRect headerRect;
	m_Header.GetClientRect(&headerRect);
	m_Header.SetWindowPos(&wndTop,0, 1, wndRect.Width() , headerRect.Height() , SWP_NOMOVE);

	m_TreeList.SetWindowPos(&wndTop, 0, 0, wndRect.Width(), wndRect.Height() - headerRect.Height(), SWP_NOMOVE);
	

	ResetScrollBar();

}

LRESULT CTreeListCtrl::OnNMItemLDBClick(WPARAM wParam,LPARAM lParam)
{
	if (m_pMessageRecver)
		::SendMessage(m_pMessageRecver->m_hWnd,UM_ITEM_LDB_CLICK,wParam,lParam);
	return 0;
}

LRESULT CTreeListCtrl::OnNMItemLClick(WPARAM wParam,LPARAM lParam)
{
	if (m_pMessageRecver)
		::SendMessage(m_pMessageRecver->m_hWnd,UM_ITEM_L_CLICK,wParam,lParam);
	return 0;
}

LRESULT CTreeListCtrl::OnNMItemRClick(WPARAM wParam,LPARAM lParam)
{
	if (m_pMessageRecver)
		::SendMessage(m_pMessageRecver->m_hWnd,UM_ITEM_R_CLICK,wParam,lParam);
	return 0;
}

LRESULT CTreeListCtrl::OnNMItemPropChanged(WPARAM wParam,LPARAM lParam)
{
	if (m_pMessageRecver)
		::SendMessage(m_pMessageRecver->m_hWnd,UM_ITEM_PROP_CHANGED,wParam,lParam);
	return 0;
}
