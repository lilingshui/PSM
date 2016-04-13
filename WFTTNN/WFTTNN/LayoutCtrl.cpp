// LayoutCtrl.cpp: implementation of the CLayoutCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "RouteTool.h"
#include "LayoutCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLayoutCtrl::CLayoutCtrl(CDialog *pDlg)
{
	m_pDlg=pDlg;
}

CLayoutCtrl::~CLayoutCtrl()
{

}

void CLayoutCtrl::SetPos(DWORD dwCtrlID,int nLeftTop,int nRightBottom)
{
	CRect rect,ctrlRect;
	m_pDlg->GetClientRect(&rect);
	m_pDlg->GetDlgItem(dwCtrlID)->GetWindowRect(ctrlRect);
	m_pDlg->ScreenToClient(&ctrlRect);
	CRect newRect=CRect(-1,-1,-1,-1);
	switch (nLeftTop)
	{
	case LT_NULL:
		break;
	case LT_LEFT:
		newRect.left=ctrlRect.left;
		break;
	case LT_TOP:
		newRect.top=ctrlRect.top;
		break;
	case LT_LEFT_TOP:
		newRect.left=ctrlRect.left;
		newRect.top=ctrlRect.top;
		break;
	}
	switch (nRightBottom)
	{
	case RB_NULL:
		break;
	case RB_RIGHT:
		newRect.right=rect.Width()-(m_oldRect.Width()-ctrlRect.right);
		break;
	case RB_BOTTOM:
		newRect.bottom=rect.Height()-(m_oldRect.Height()-ctrlRect.bottom);
		break;
	case RB_RIGHT_BOTTOM:
		newRect.right=rect.Width()-(m_oldRect.Width()-ctrlRect.right);
		newRect.bottom=rect.Height()-(m_oldRect.Height()-ctrlRect.bottom);
		break;
	}
	if (newRect.left==-1)
		newRect.left=newRect.right-ctrlRect.Width();
	if (newRect.top==-1)
		newRect.top=newRect.bottom-ctrlRect.Height();
	if (newRect.right==-1)
		newRect.right=newRect.left+ctrlRect.Width();
	if (newRect.bottom==-1)
		newRect.bottom=newRect.top+ctrlRect.Height();
	m_pDlg->GetDlgItem(dwCtrlID)->MoveWindow(&newRect);
}
