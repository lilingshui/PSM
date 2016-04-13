#include "atltime.h"
#include "afxcmn.h"
#include "afxwin.h"
#if !defined(AFX_DLGDATABROWSE_H__D67827AE_A017_48A3_9758_769C0D9054C1__INCLUDED_)
#define AFX_DLGDATABROWSE_H__D67827AE_A017_48A3_9758_769C0D9054C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDataBrowse.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDataBrowse dialog

class CDlgDataBrowse : public CDialog
{
// Construction
public:
	CDlgDataBrowse(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDataBrowse)
	enum { IDD = IDD_DIALOG_DATABROWSE };
	CListCtrl	m_ShowDataBrowseInfoListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDataBrowse)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgDataBrowse)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	afx_msg void OnBnClickedBtnSearch();
	void InitFactory();
	void InitTermData(CString strBatchNo, CString strTerminalNo);
	afx_msg void OnNMDblclkListDmTerm(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
	CLayoutCtrl m_layoutCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	BOOL m_bInspectDT;
	CString m_strBatchNo;
	CString m_strInspectUnit;
	CString m_strSupplyYear;
	CString m_strCheckType;
	CTime m_dtInspectStart;
	CTime m_dtInspectEnd;
	CString m_strTermAddr;
	CString m_strFactory;
	CString m_strOutNo;
	int m_nTermType;
	int m_nResult;
	CListCtrl m_ctrlTermList;	
	CComboBox m_cbSupplyYear;
	CComboBox m_cbCheckType;
	CComboBox m_cbFactory;
	CComboBox m_cbTermType;
	CComboBox m_cbResult;
	int m_nTermId;
	int m_nBatchId;
public:
	afx_msg void OnBnClickedBtnDbReport();
	//afx_msg void OnLvnItemchangedListDmTerm(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDATABROWSE_H__D67827AE_A017_48A3_9758_769C0D9054C1__INCLUDED_)
