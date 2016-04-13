#if !defined(AFX_DLGTERMINALPARENT_H__40894481_AB23_4B8C_BE0A_78911421D34A__INCLUDED_)
#define AFX_DLGTERMINALPARENT_H__40894481_AB23_4B8C_BE0A_78911421D34A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTerminalParent.h : header file
//

#include "EventCenter.h"
#include "TerminalTreeCtrl.h"
#include "Channel/ChannelReceiver.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTerminalParent dialog

class CDlgTerminalParent : public CDialogBar, public CEventCenter, public CChannelReceiver
{
// Construction
public:
	CDlgTerminalParent();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTerminalParent)
	enum { IDD = IDD_DIALOG_TERMINALPARENT };
	
	//}}AFX_DATA
	BOOL Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID);

	//void RefreshTerminalTree(CString strBatchNo, CString strTermNo, CString strTermAddr,int nType = 0);//MODIFY BY WM 20131126
	void RefreshTerminalTree(CString strBatchNo, CString strTermNo, int nType = 0);
	void RefreshBatchInfoTree(CString strNodeNo, int nType = 0);

	int GetTreeNodeValue();

	int GetTreeRootNodeValue();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTerminalParent)
	public:
	//BOOL Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;
	CTerminalTreeCtrl  m_TerminalTreeCtrl;
	HTREEITEM m_hItem;

	// Generated message map functions
	//{{AFX_MSG(CDlgTerminalParent)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnDblclkTreeTerminal(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTreeTerminal(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnReceive(int state, char *buf, int len, u_int64 strTermAddr);
	void RrefreshState(const char *addr, int state);

public:
	afx_msg void OnTvnSelchangedTreeTerminal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTERMINALPARENT_H__40894481_AB23_4B8C_BE0A_78911421D34A__INCLUDED_)
