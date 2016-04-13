#if !defined(AFX_DLGSETCHANNEL_H__18A91FDD_B498_4523_B441_9B39F5694C95__INCLUDED_)
#define AFX_DLGSETCHANNEL_H__18A91FDD_B498_4523_B441_9B39F5694C95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetChannel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetChannel dialog

class CDlgSetChannel : public CDialog
{
// Construction
public:
	CDlgSetChannel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetChannel)
	enum { IDD = IDD_DIALOG_SETCHANNEL };
	CIPAddressCtrl	m_IpAddress;
	CListCtrl	m_ShowSetChannelInfo;
	CComboBox	m_cbStopBit;
	CComboBox	m_cbSerail;
	CComboBox	m_cbDataBit;
	CComboBox	m_cbCheckWay;
	CComboBox	m_cbChannelType;
	CComboBox	m_cbBaud;
	CString	m_strChannelID;
	CString	m_strChannelName;
	CString	m_strPhone;
	CString	m_strPort;
	CString	m_strChar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetChannel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK()	{}
	virtual void OnCancel()	{}

	void InitChannelInfoList();

	// Generated message map functions
	//{{AFX_MSG(CDlgSetChannel)
	afx_msg void OnButtonAddchannel();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDelchannel();
	afx_msg void OnButtonModifychannel();
	afx_msg void OnSelchangeComboChanneltype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedListSetchannelinfo(NMHDR *pNMHDR, LRESULT *pResult);
	char test;
	CLayoutCtrl m_layoutCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETCHANNEL_H__18A91FDD_B498_4523_B441_9B39F5694C95__INCLUDED_)
