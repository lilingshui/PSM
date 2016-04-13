// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__4B3B3732_3A0C_4100_8F0A_B96068AEBDB0__INCLUDED_)
#define AFX_MAINFRM_H__4B3B3732_3A0C_4100_8F0A_B96068AEBDB0__INCLUDED_

#include "DlgTerminalParent.h"	// Added by ClassView
//#include "DlgTerminalBaoWen.h"	// Added by ClassView
#include "DlgConnect.h"

#include "AcceptChannel.h"

#include "CoolTabCtrl.h"	// Added by ClassView


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CDlgTerminalParent m_TerminalParentDlgBar;
	CDlgConnect *m_pDlgConn;
	//CDlgTerminalBaoWen m_TerminalBaoWenBar;

	CCoolTabCtrl	m_TabCtrl;
	CCoolBar		m_wndMyBar1;
	CCoolBar		m_wndMyBar2;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	afx_msg void OnMenuitemRndread();
	afx_msg void OnMenuitemDatabrowse();
	afx_msg void OnMenuitemExit();
	afx_msg void OnMenuitemSetparam();
	afx_msg void OnMenuitemSetchannel();
	afx_msg void OnMenuitemConnect();
	afx_msg void OnMenuitemStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMenuBatchInfo();
	afx_msg void OnMenuitemTerminalinfo();
	afx_msg void OnMenuMeterInfo();
	afx_msg void OnMenuReadRealtimedata();
	afx_msg void OnIdMenuReadHistorydata();
	afx_msg void OnMenuReadAlarmdata();
	afx_msg void OnMenuReadEventdata();
	afx_msg void OnMenuReadReadterminfo();
	afx_msg void OnMenuitemAutotest();
	afx_msg void OnMenuitemTask();
	afx_msg void OnMenuItemOtherTest();
	afx_msg void OnMenuitemVisualmeter();
	afx_msg void OnMenuitemViewToolbar();
	afx_msg void OnUpdateMenuitemViewToolbar(CCmdUI *pCmdUI);
	afx_msg void OnMenuitemViewTree();
	afx_msg void OnUpdateMenuitemViewTree(CCmdUI *pCmdUI);
	afx_msg void OnMenuitemViewOutput();
	afx_msg void OnUpdateMenuitemViewOutput(CCmdUI *pCmdUI);
	void RefreshTaskArch();
	void RefreshChannelArch();
	void RefreshConnChannel();
	void RefreshTermArch();
	void RefreshBatchArch();
	void RefreshMeterArch();
	afx_msg void OnMenuTestReread();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__4B3B3732_3A0C_4100_8F0A_B96068AEBDB0__INCLUDED_)
