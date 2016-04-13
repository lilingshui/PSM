#if !defined(AFX_DLGTERMINALBAOWEN_H__24E81128_7177_43CE_85CD_584FF0409DC6__INCLUDED_)
#define AFX_DLGTERMINALBAOWEN_H__24E81128_7177_43CE_85CD_584FF0409DC6__INCLUDED_

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
	CDlgTerminalBaoWen();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTerminalBaoWen)
	enum { IDD = IDD_DIALOG_BAOWEN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTerminalBaoWen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTerminalBaoWen)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTERMINALBAOWEN_H__24E81128_7177_43CE_85CD_584FF0409DC6__INCLUDED_)
