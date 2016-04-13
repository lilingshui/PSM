// LayoutCtrl.h: interface for the CLayoutCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAYOUTCTRL_H__0F92AF61_165B_4041_851B_DB8A6B4310C8__INCLUDED_)
#define AFX_LAYOUTCTRL_H__0F92AF61_165B_4041_851B_DB8A6B4310C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum
{
	LT_NULL,
	LT_LEFT,
	LT_TOP,
	LT_LEFT_TOP
};

enum
{
	RB_NULL,
	RB_RIGHT,
	RB_BOTTOM,
	RB_RIGHT_BOTTOM
};

class CLayoutCtrl  
{
public:
	CLayoutCtrl(CDialog *pDlg);
	virtual ~CLayoutCtrl();
	void SetPos(DWORD dwCtrlID,int nLeftTop,int nRightBottom);
	void SetOldRect(CRect rect){m_oldRect=rect;}
private:
	CDialog *m_pDlg;
	CRect m_oldRect;
};

#endif // !defined(AFX_LAYOUTCTRL_H__0F92AF61_165B_4041_851B_DB8A6B4310C8__INCLUDED_)
