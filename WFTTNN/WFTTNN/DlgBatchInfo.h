#if !defined(AFX_DLGBATCHINFO_H__88095F4C_F326_4858_BECF_30F453D69F2F__INCLUDED_)
#define AFX_DLGBATCHINFO_H__88095F4C_F326_4858_BECF_30F453D69F2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBatchInfo.h : header file
//

#include "EventRecver.h"
#include "afxdtctl.h"
#include "LayoutCtrl.h"
#include "atltime.h"
#include "common/SortListCtrl.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgBatchInfo dialog

class CDlgBatchInfo : public CDialog, public CEventRecver
{
// Construction
public:
	CDlgBatchInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBatchInfo)
	enum { IDD = IDD_DIALOG_BATCHINFO };	
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBatchInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgBatchInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAddbatchinfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnEvent(unsigned int nEventID,	void *pParam);
	afx_msg void OnLvnItemchangedListBatchinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDelbatchinfo();
	afx_msg void OnBnClickedButtonModifybatchinfo();
	
	virtual void OnOK()	{}
	virtual void OnCancel()	{}

private:
	CSortListCtrl m_ShowBatchInfoListCtrl;
	CString	m_strBatchNo;
	CString	m_strInspectUnit;
	CTime m_dtCheck;
	CString m_strCheckType;
	CComboBox m_cbSupply;
	CString m_strSupply;
	CLayoutCtrl m_layout;

	void InitBatchInfoList();

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);	
	afx_msg void OnSize(UINT nType, int cx, int cy);	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBATCHINFO_H__88095F4C_F326_4858_BECF_30F453D69F2F__INCLUDED_)
