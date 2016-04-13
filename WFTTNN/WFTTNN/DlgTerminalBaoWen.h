#include "afxwin.h"
#if !defined(AFX_DLGTERMINALBAOWEN_H__D58D499D_15FA_4810_8B7B_5D6377233A82__INCLUDED_)
#define AFX_DLGTERMINALBAOWEN_H__D58D499D_15FA_4810_8B7B_5D6377233A82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTerminalBaoWen.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTerminalBaoWen dialog

class CDlgTerminalBaoWen : public CDialog
{
// Construction
public:
	CDlgTerminalBaoWen(CWnd* pParent = NULL);   // standard constructor
	DECLARE_DYNCREATE(CDlgTerminalBaoWen)

// Dialog Data
	//{{AFX_DATA(CDlgTerminalBaoWen)
	enum { IDD = IDD_DIALOG_BAOWEN };
	CEdit	m_edtPacketLog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTerminalBaoWen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	LRESULT OnPacketInfoEventNotify(WPARAM wParam,LPARAM lParam);

	void AddPacketLogText(CString strText);

	virtual void OnOK()	{}
	virtual void OnCancel()	{}


	// Generated message map functions
	//{{AFX_MSG(CDlgTerminalBaoWen)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTERMINALBAOWEN_H__D58D499D_15FA_4810_8B7B_5D6377233A82__INCLUDED_)
