#pragma once


// CTreeListCtrl
//#include "CommonUI_Global.h"
#include "TreeListLine.h"
#include <XTToolkitPro.h>   // Codejock Software Components

class CTreeListHeader;
class CTreeListProp;
class CTreeList : public CXTTreeCtrl
{
	DECLARE_DYNAMIC(CTreeList)

public:
	CTreeList();
	virtual ~CTreeList();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();


public:
	void SetHeader(CTreeListHeader *pHeader);
	void HideProp();
	
protected:
	CTreeListHeader *m_pHeader;
	CTreeListProp *m_pCurProp;
	int StretchWidth(int nWidth, int nMeasure);
	int GetColumnAlign(int nCol);
	void DrawItemText (CDC* pDC, CString text, CRect rect, int nWidth, int nFormat);
	
};


