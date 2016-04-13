#if !defined(AFX_DLGTERMINALINFO_H__CD9DB358_364A_418E_8B94_39E02C4CB482__INCLUDED_)
#define AFX_DLGTERMINALINFO_H__CD9DB358_364A_418E_8B94_39E02C4CB482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTerminalInfo.h : header file
//

#include "EventRecver.h"
#include "AcceptChannel.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTerminalInfo dialog

class CDlgTerminalInfo : public CDialog, public CEventRecver
{
// Construction
public:
	CDlgTerminalInfo(CWnd* pParent = NULL);   // standard constructor

	void InitBatchInfoList();

// Dialog Data
	//{{AFX_DATA(CDlgTerminalInfo)
	enum { IDD = IDD_DIALOG_TERMINALINFO };
	CComboBox	m_cbTerminalType;
	CComboBox	m_cbBatchInfo;
	CListCtrl	m_ShowTerminalInfoListCtrl;
	CString	m_strTerminalNo;
	CString	m_strTerminalAddr;
	CString	m_strAssertNo;
	CString m_strTerminalModel;//MODIFY BY WM 20131126
	CString m_strCJBH;
	//}}AFX_DATA

	CString m_strBatchNo;
	HTREEITEM m_hCurrentTreeNode;

	void InitTerminalInfoList(CString strBatchNo=_T(""));
	void SetCurrentTreeNode(HTREEITEM node);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTerminalInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
//	CListenSocket m_lstnSocket;


// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgTerminalInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAddtermianl();
	afx_msg void OnButtonDelterminal();
	afx_msg void OnButtonModifyterminal();
//	afx_msg void OnClickListTerminalinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual void OnEvent(unsigned int nEventID,	void *pParam);
	afx_msg void OnCbnSelchangeComboBatchInfo();
	afx_msg void OnLvnItemchangedListTerminalinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
private:
	CLayoutCtrl m_layoutCtrl;
	CString m_strTerminalFactory;
	CComboBox m_cbFactory;
	long m_nTerminalNo;
		
	long GetNewTerminalID();
	void InitFactory();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTERMINALINFO_H__CD9DB358_364A_418E_8B94_39E02C4CB482__INCLUDED_)
