#if !defined(AFX_TERMINALTREECTRL_H__CBAB2CAC_004C_49CB_A106_0ECFFAA09DFB__INCLUDED_)
#define AFX_TERMINALTREECTRL_H__CBAB2CAC_004C_49CB_A106_0ECFFAA09DFB__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerminalTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTerminalTreeCtrl window

class CTerminalTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CTerminalTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerminalTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CTerminalTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTerminalTreeCtrl)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINALTREECTRL_H__CBAB2CAC_004C_49CB_A106_0ECFFAA09DFB__INCLUDED_)
