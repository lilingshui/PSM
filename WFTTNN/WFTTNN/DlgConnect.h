#if !defined(AFX_DLGCONNECT_H__65FA0207_1619_4F22_BEE2_F51E83F7C9E4__INCLUDED_)
#define AFX_DLGCONNECT_H__65FA0207_1619_4F22_BEE2_F51E83F7C9E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConnect.h : header file
//

#include "EventRecver.h"
#include "afxwin.h"
#include "Channel/ChannelMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgConnect dialog

class CDlgConnect : public CDialog, public CEventRecver
{
// Construction
public:
	CDlgConnect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgConnect)
	enum { IDD = IDD_DIALOG_CONNECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
protected:
	virtual void OnEvent(unsigned int nEventID,	void *pParam);

private:
	CString m_strTerminalAddr;
	CString m_strTerminalName;		

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConnect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConnect)
	afx_msg void OnButtonCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnBnClickedBtnConnect();


private:	
	CComboBox m_cmbChlType;
	virtual BOOL OnInitDialog();	

public:
	CChannel *m_pChannel;
	afx_msg void OnBnClickedBtnDisconnect();
	void InitChannelType();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONNECT_H__65FA0207_1619_4F22_BEE2_F51E83F7C9E4__INCLUDED_)
