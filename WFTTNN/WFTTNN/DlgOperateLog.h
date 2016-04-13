#if !defined(AFX_DLGOPERATELOG_H__402FB881_F006_4669_9672_09A995740C5E__INCLUDED_)
#define AFX_DLGOPERATELOG_H__402FB881_F006_4669_9672_09A995740C5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOperateLog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgOperateLog dialog

class CDlgOperateLog : public CDialog
{
// Construction
public:
	CDlgOperateLog(CWnd* pParent = NULL);   // standard constructor
	DECLARE_DYNCREATE(CDlgOperateLog)

// Dialog Data
	//{{AFX_DATA(CDlgOperateLog)
	enum { IDD = IDD_DIALOG_OPERATELOG };
	CListCtrl	m_ShowOperateInfoList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOperateLog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	LRESULT OnOperateInfoEventNotify(WPARAM wParam,LPARAM lParam);

	void AddOperateInfoText(CString strText);
	
	virtual void OnOK()	{}
	virtual void OnCancel()	{}

	//{{AFX_MSG(CDlgOperateLog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPERATELOG_H__402FB881_F006_4669_9672_09A995740C5E__INCLUDED_)
