// TreeListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "TreeList.h"
#include "TreeListCtrl.h"
#include "TreeListHeader.h"
#include "TreeListProp.h"


// CTreeListCtrl

IMPLEMENT_DYNAMIC(CTreeList, CXTTreeCtrl)
CTreeList::CTreeList()
{
	m_pHeader = NULL;
	m_pCurProp = NULL;
}

CTreeList::~CTreeList()
{
}

BEGIN_MESSAGE_MAP(CTreeList, CXTTreeCtrl)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CTreeList::SetHeader(CTreeListHeader *pHeader)
{
	m_pHeader = pHeader;
}


void CTreeList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect treeRect;
	GetClientRect(&treeRect);
	CRect headerRect;
	m_pHeader->GetWindowRect(&headerRect);

	int nCurPos = GetScrollPos(SB_HORZ);
	int nPrevPos = nCurPos;
	switch(nSBCode)
	{
	case SB_LEFT:			nCurPos = 0;
		break;
	case SB_RIGHT:			nCurPos = GetScrollLimit(SB_HORZ)-1;
		break;
	case SB_LINELEFT:		nCurPos = max(nCurPos-6, 0);
		break;
	case SB_LINERIGHT:		nCurPos = min(nCurPos+6,GetScrollLimit(SB_HORZ)-1);
		break;
	case SB_PAGELEFT:		nCurPos = max(nCurPos-treeRect.Width(), 0);
		break;
	case SB_PAGERIGHT:		nCurPos = min(nCurPos+treeRect.Width(),GetScrollLimit(SB_HORZ)-1);
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:  
		{
			if(nPos==0)
				nCurPos = 0;
			else
				nCurPos = min(StretchWidth(nPos,6),GetScrollLimit(SB_HORZ)-1);
		}
		break;
	}

	HideProp();

	SetScrollPos(SB_HORZ,nCurPos);
	
	m_pHeader->SetWindowPos(&wndTop, -nCurPos, 0, 
		max(StretchWidth(m_pHeader->GetColumnsWidth(),treeRect.Width()),treeRect.Width()), headerRect.Height(), SWP_SHOWWINDOW);
	
	RedrawWindow();			

	CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTreeList::OnPaint()
{
	if (m_pHeader == NULL)
		return ;

	int nWidth = 0;

	int nColumnsWidth = m_pHeader->GetColumnsWidth();
	int nColumnsCount = m_pHeader->GetItemCount();
	
	//得到treelist控件窗口范围，包括滑动条以外的
	CRect rcClient,rcWnd;
	GetClientRect(&rcClient);
	rcWnd.left = 0;
	rcWnd.top = 0;
	rcWnd.right = max(nColumnsWidth,rcClient.Width());
	rcWnd.bottom = rcClient.bottom;

	CPaintDC paintDC(this); // device context for painting//	paintDC
	CRect rcClip;
	paintDC.GetClipBox( &rcClip );
	CMemDC dc(&paintDC,&rcWnd);

	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	dc.SelectClipRgn(&rgn);
	rgn.DeleteObject();

	//设置画刷起点
	COLORREF wndColor = RGB(255,255,255);
	int nOffset = GetScrollPos(SB_HORZ);
	paintDC.SetViewportOrg(-nOffset, 0);
	dc.SetViewportOrg(-nOffset, 0);
	
	dc.SetTextColor(wndColor);
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );

	HTREEITEM hItem = GetFirstVisibleItem();

	int nVisibleCount = GetVisibleCount();

	dc.FillSolidRect(m_pHeader->GetColumnWidth(0),0,rcClient.Width(),rcClient.Height(),wndColor);

	
	CRect r;
	GetItemRect( hItem, &r, FALSE );
	int nBottom = r.top;
	COLORREF GridColor = RGB(210,210,210);

	// create the font
	CFont *pFontDC;
	CFont fontDC, boldFontDC;
	LOGFONT logfont;

	CFont *pFont = GetFont();
	pFont->GetLogFont( &logfont );

	fontDC.CreateFontIndirect( &logfont );
	pFontDC = dc.SelectObject( &fontDC );

	logfont.lfWeight = 700;
	boldFontDC.CreateFontIndirect( &logfont );

	// the most urgent thing is to erase the labels that were drawn by the tree
	while(hItem!=NULL && nVisibleCount>=0)
	{		
		CRect rect;

		UINT selflag =  TVIS_SELECTED;

		CRect labelRect;
		GetItemRect( hItem, &labelRect, TRUE );
		GetItemRect( hItem, &rect, FALSE );
		if( nColumnsCount>1)
			rect.left = min(labelRect.left, m_pHeader->GetColumnWidth(0));
		else
			rect.left = labelRect.left;
		rect.right = nColumnsWidth;
		CTreeListLine *pLine = (CTreeListLine *)GetItemData(hItem);
		if (pLine != NULL)
			dc.FillSolidRect(rect.left,rect.top,rect.Width(),rect.Height(),pLine->GetBKColor());
		if (!(GetItemState( hItem, selflag ) & selflag ))
		{
			dc.SetBkMode(TRANSPARENT);
			if(pLine)
			{
				CString sItem = pLine->GetItemText(0);

				GetItemRect( hItem, &labelRect, TRUE );
				GetItemRect( hItem, &rect, FALSE );

				rect.top = labelRect.top;
				rect.bottom = labelRect.bottom;

				if(nColumnsCount>1)
					rect.left = min(labelRect.left, m_pHeader->GetColumnWidth(0));
				else
					rect.left = labelRect.left;
				rect.right = nColumnsWidth;

				dc.SetBkColor( pLine->GetBKColor() );
				dc.SetTextColor( pLine->GetTextColor());

				DrawItemText(&dc, sItem, 
					CRect(rect.left+2, rect.top, m_pHeader->GetColumnWidth(0), rect.bottom), m_pHeader->GetColumnWidth(0)-rect.left-2, GetColumnAlign(0));
				
				CRect tR1;
				nWidth = 0;
				for(int i = 1; i < nColumnsCount;i++)
				{
					if(i == 0)
						nWidth = 0;
					else
						nWidth += m_pHeader->GetColumnWidth(i-1);
					m_pHeader->GetItemRect(i, &tR1);	
					rect.left = tR1.left;
					rect.right = tR1.right;

					CBrush brush(pLine->GetBKColor());	

					LOGBRUSH logBrush;
					logBrush.lbStyle = BS_SOLID;
					logBrush.lbColor = wndColor;
					logBrush.lbHatch = HS_CROSS;

					CBrush Gridbrush;
					Gridbrush.CreateBrushIndirect(&logBrush);	

					dc.SetBkColor( pLine->GetBKColor() );

					Gridbrush.DeleteObject();
					brush.DeleteObject();

					dc.SetTextColor(pLine->GetTextColor());
					
					DrawItemText(&dc, pLine->GetItemText(i), 
						CRect(nWidth+2, rect.top, nWidth+m_pHeader->GetColumnWidth(i)-5, rect.bottom), m_pHeader->GetColumnWidth(i), GetColumnAlign(i));
				}

				dc.SetTextColor(::GetSysColor (COLOR_WINDOWTEXT ));
			}

			LOGBRUSH logBrush;
			logBrush.lbStyle = BS_SOLID;
			logBrush.lbColor = GridColor;
			logBrush.lbHatch = HS_CROSS;
			CBrush Gridbrush;
			Gridbrush.CreateBrushIndirect(&logBrush);			
			nWidth = 0;
			for(int i=1;i<nColumnsCount;i++)
			{
				nWidth += m_pHeader->GetColumnWidth(i-1);				
				CRect rect(nWidth-1,rect.top,nWidth,rect.bottom);								
				dc.FillRect (rect, &Gridbrush);
			}
			Gridbrush.DeleteObject();
		}
		else
		{
			dc.SetBkMode(TRANSPARENT);
			GetItemRect( hItem, &labelRect, TRUE );
			GetItemRect( hItem, &rect, FALSE );
			if(nColumnsCount>1)
			{
				rect.left = min(labelRect.left, m_pHeader->GetColumnWidth(0));
			}
			else
				rect.left = labelRect.left;
			rect.right = nColumnsWidth;

			COLORREF highlightColor = RGB(209, 248, 128);//GetSysColor(COLOR_INFOBK);//::GetSysColor (COLOR_HIGHLIGHT);

			CBrush brush(highlightColor);//m_wndColor
			dc.FillRect (rect, &brush);
			dc.DrawFocusRect (rect);

			if(pLine)
			{
				CString sItem = pLine->GetItemText(0);

				DrawItemText(&dc, sItem, 
					CRect(rect.left+2, rect.top, m_pHeader->GetColumnWidth(0), rect.bottom),
					m_pHeader->GetColumnWidth(0)-rect.left-2, GetColumnAlign(0));

				CRect tR1;
				nWidth = 0;
				for(int i = 1; i < nColumnsCount;i++)
				{
					if(i == 0)
						nWidth = 0;
					else
						nWidth += m_pHeader->GetColumnWidth(i-1);
					m_pHeader->GetItemRect(i, &tR1);	
					rect.left = tR1.left;
					rect.right = tR1.right;
					CBrush brush(wndColor);	
					LOGBRUSH logBrush;
					logBrush.lbStyle = BS_SOLID;
					logBrush.lbColor = wndColor;
					logBrush.lbHatch = HS_CROSS;
					CBrush Gridbrush;
					Gridbrush.CreateBrushIndirect(&logBrush);	
					dc.SetBkColor( wndColor );

					Gridbrush.DeleteObject();
					brush.DeleteObject();

					dc.SetTextColor(pLine->GetTextColor());

					DrawItemText(&dc, pLine->GetItemText(i), 
						CRect(nWidth+2, rect.top, nWidth+m_pHeader->GetColumnWidth(i)+5, rect.bottom), m_pHeader->GetColumnWidth(i), GetColumnAlign(i));
				}

				dc.SetTextColor(::GetSysColor (COLOR_WINDOWTEXT ));
			}

			LOGBRUSH logBrush;
			logBrush.lbStyle = BS_SOLID;
			logBrush.lbColor = GridColor;
			logBrush.lbHatch = HS_CROSS;
			CBrush Gridbrush;
			Gridbrush.CreateBrushIndirect(&logBrush);			
			nWidth = 0;
			for(int i=1;i<nColumnsCount;i++)
			{
				nWidth += m_pHeader->GetColumnWidth(i-1);				
				CRect rect(nWidth-1,rect.top,nWidth,rect.bottom);								
				dc.FillRect (rect, &Gridbrush);
			}

			//should be deleted
			Gridbrush.DeleteObject();
		}
		////////////////////////////////////////////////////////////
		nBottom += rect.bottom - rect.top ;	

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = GridColor;
		logBrush.lbHatch = HS_CROSS;
		CBrush Gridbrush;
		Gridbrush.CreateBrushIndirect(&logBrush);					

		CRect rect1(0,rect.top-1,rect.right,rect.top);								
		dc.FillRect (rect1, &Gridbrush);		

		//should be deleted
		Gridbrush.DeleteObject();

		hItem = GetNextVisibleItem( hItem );
		nVisibleCount--;
	}

	CPen GridPen(BS_SOLID,0,GridColor);		
	dc.SelectObject(&GridPen);

	dc.MoveTo(r.left,nBottom);		
	dc.LineTo(nColumnsWidth,nBottom);

	dc.SelectObject( pFontDC );
	paintDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), 
		&dc, 0, 0, SRCCOPY) ;

	//free object
	fontDC.DeleteObject();
	boldFontDC.DeleteObject();
}

void CTreeList::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	HideProp();

	UINT flags;
	HTREEITEM currItem = HitTest(point, &flags);
	CTreeListLine *pLine = NULL;
	if(currItem)
	{
		SelectItem(currItem);
		pLine = (CTreeListLine *)GetItemData(currItem);

		int SelColumn = -1;
		int nOffset = GetScrollPos(SB_HORZ);
		SelColumn = m_pHeader->GetColumnIndex(nOffset + point.x);
		if(SelColumn >= 0)
		{
			CRect r;
			m_pHeader->GetItemRect(SelColumn, &r);
			r.OffsetRect(-nOffset,0);

			CRect Rect;
			GetItemRect(currItem, &Rect, FALSE);

			Rect.left = r.left;
			Rect.right = r.right;
			RedrawWindow();

			if (pLine)
			{
				CTreeListProp *pProp = pLine->GetItemProp(SelColumn);
				if (pProp)
				{
					m_pCurProp = pProp;
					pProp->OnEditStart( Rect);
				}
			}
		}

		if (flags & TVHT_ONITEMSTATEICON) 
		{
			BOOL bCheck = GetCheck(currItem); 
			//根节点被选中 
			HTREEITEM hChildItem = GetChildItem(currItem) ; 
			while (hChildItem) 
			{ 
				SetCheck(hChildItem, !bCheck) ; 
				hChildItem = GetNextItem(hChildItem, TVGN_NEXT) ; 
			} 
		}

	}

	if (GetParent())
	{
		::SendMessage(GetParent()->m_hWnd,UM_ITEM_L_CLICK,(WPARAM)GetParent(),(LPARAM)pLine);
	}

	
	CXTTreeCtrl::OnLButtonDown(nFlags, point);
}

void CTreeList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UINT flags;
	HTREEITEM currItem = HitTest(point, &flags);
	CTreeListLine *pLine = NULL;
	if(currItem)
	{
		pLine = (CTreeListLine *)GetItemData(currItem);
	}

	if (GetParent())
	{
		::SendMessage(GetParent()->m_hWnd,UM_ITEM_LDB_CLICK,(WPARAM)GetParent(),(LPARAM)pLine);
	}

	CXTTreeCtrl::OnLButtonDblClk(nFlags, point);
}


void CTreeList::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	HideProp();
	
	UINT flags;
	HTREEITEM currItem = HitTest(point, &flags);
	if(currItem)
	{
		SelectItem(currItem);
	}

	CTreeListLine *pLine = NULL;
	if (currItem)
	{
		pLine = (CTreeListLine *)GetItemData(currItem);
	}

	if (GetParent())
	{
		::SendMessage(GetParent()->m_hWnd,UM_ITEM_R_CLICK,(WPARAM)GetParent(),(LPARAM)pLine);
	}

	CXTTreeCtrl::OnLButtonDown(nFlags, point);
}

void CTreeList::HideProp()
{
	if (m_pCurProp != NULL)
		m_pCurProp->OnEditOver();
	m_pCurProp = NULL;
}

void CTreeList::DrawItemText (CDC* pDC, CString text, CRect rect, int nWidth, int nFormat)
{
	rect.DeflateRect( 2, 0, 3, 0 );

    UINT nStyle = DT_VCENTER | DT_SINGLELINE;
	nStyle |= DT_WORD_ELLIPSIS;


    if (nFormat == LVCFMT_LEFT)
        nStyle |= DT_LEFT;
    else if (nFormat == LVCFMT_CENTER)
        nStyle |= DT_CENTER;
    else 
        nStyle |= DT_RIGHT;

	if((text.GetLength()>0) && (rect.right>rect.left))
		pDC->DrawText (text, rect, nStyle);
}


int CTreeList::GetColumnAlign(int nCol)
{
	HD_ITEM hItem;
	hItem.mask = HDI_FORMAT;
	if(!m_pHeader->GetItem(nCol, &hItem))
		return LVCFMT_LEFT;

	if(hItem.fmt & HDF_RIGHT)
		return LVCFMT_RIGHT;
	else
	if(hItem.fmt & HDF_CENTER)
		return LVCFMT_CENTER;
	else
		return LVCFMT_LEFT;
}

int CTreeList::StretchWidth(int nWidth, int nMeasure)
{
	if(nMeasure == 0)
		return 0;
	return ((nWidth/nMeasure)+1)*nMeasure;
}

