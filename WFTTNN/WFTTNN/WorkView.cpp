// WorkView.cpp : implementation file
//

#include "stdafx.h"
#include "WorkView.h"

// CWorkView

IMPLEMENT_DYNCREATE(CWorkView, CScrollView)

CWorkView::CWorkView()
: m_pos(0)
{

}

CWorkView::~CWorkView()
{
}


BEGIN_MESSAGE_MAP(CWorkView, CScrollView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CWorkView drawing

void CWorkView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CWorkView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CWorkView diagnostics

#ifdef _DEBUG
void CWorkView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CWorkView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWorkView message handlers


void CWorkView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CScrollView::OnPaint() for painting messages
	CRect rect;
	GetClientRect(&rect);
	GetPanelImage(dc, rect, m_pos);
}

void CWorkView::GetPanelImage(CDC &dc, CRect rect, int pos)
{
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	memDC.SelectObject(&bmp);
	CBrush brush(RGB(255,255,255));
	memDC.FillRect(CRect(0,0,rect.Width(),rect.Height()),&brush);

	m_shapeMgr.get_image(&memDC, rect, pos);
	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.DeleteDC();
	bmp.DeleteObject();
}


void CWorkView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (nSBCode ==5)
	{
		m_pos = nPos;
		//Invalidate(FALSE);
	}
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CWorkView::Print()
{
	CPrintDialog printDlg(FALSE,PD_ALLPAGES|PD_ALLPAGES|PD_NOPAGENUMS,NULL);
	if(printDlg.DoModal() == IDOK)
	{
		CDC dcPrint;
		dcPrint.Attach(printDlg.GetPrinterDC());

		dcPrint.SetMapMode(MM_ANISOTROPIC); //转换坐标映射方式
		CSize size = CSize(900,2000);
	//	CRect rect;
		//GetClientRect(&rect);
		//CSize size(rect.Width(), rect.Height());
		dcPrint.SetWindowExt(size);//确定窗口的大小
		//得到实际设备每逻辑英寸的像素数量
		int xLogPixelPerInch = dcPrint.GetDeviceCaps(LOGPIXELSX);
		int yLogPixelPerInch = dcPrint.GetDeviceCaps(LOGPIXELSY);

		//得到设备坐标和逻辑坐标的比例
		long xExt = (long)size.cx * xLogPixelPerInch / 108;
		long yExt = (long)size.cy * yLogPixelPerInch / 108;
		dcPrint.SetViewportExt((int)xExt,(int)yExt); //设置视口大小



		DOCINFO   docInfo;
		memset(&docInfo, 0, sizeof(docInfo));
		docInfo.cbSize = sizeof(docInfo);
		docInfo.lpszDocName = (LPCTSTR)"Print Report";
		dcPrint.StartDoc(&docInfo);

		dcPrint.StartPage();
		GetPanelImage(dcPrint, CRect(0, 0, xExt, yExt), 0);
		dcPrint.EndPage();

		dcPrint.EndDoc();
		dcPrint.DeleteDC();
	}
}