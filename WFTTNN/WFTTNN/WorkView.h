#pragma once

#include "shape_mgr.h"

// CWorkView view

class CWorkView : public CScrollView
{
	DECLARE_DYNCREATE(CWorkView)

protected:
	CWorkView();           // protected constructor used by dynamic creation
	virtual ~CWorkView();

public:
	shape_mgr m_shapeMgr;
	void Print();


#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

	DECLARE_MESSAGE_MAP()
private:
	int m_pos;
	void GetPanelImage(CDC &dc, CRect rect, int pos);
public:
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


