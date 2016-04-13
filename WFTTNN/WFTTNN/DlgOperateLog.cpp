// DlgOperateLog.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgOperateLog.h"
#include "MsgOutput.h"
#include ".\dlgoperatelog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOperateLog dialog

IMPLEMENT_DYNCREATE(CDlgOperateLog, CDialog)

CDlgOperateLog::CDlgOperateLog(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOperateLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOperateLog)
		// NOTE: the ClassWizard will add member initialization here
	CMsgOutput::GetInstance()->RegisterOperateInfoWnd(this);
	//}}AFX_DATA_INIT
}


void CDlgOperateLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOperateLog)
	DDX_Control(pDX, IDC_LIST_OPERATEINFO, m_ShowOperateInfoList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOperateLog, CDialog)
	//{{AFX_MSG_MAP(CDlgOperateLog)
	ON_MESSAGE(OPERATEINFO_EVENT_NOTIFY,OnOperateInfoEventNotify)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOperateLog message handlers

BOOL CDlgOperateLog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	DWORD dwExtendStyle = m_ShowOperateInfoList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ShowOperateInfoList.SetExtendedStyle(dwExtendStyle);
	
	/**************初始化列表显示**************************************/
	m_ShowOperateInfoList.InsertColumn(0,"操作时间",LVCFMT_CENTER,100);
	m_ShowOperateInfoList.InsertColumn(1,"操作描述",LVCFMT_LEFT,400);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CDlgOperateLog::OnOperateInfoEventNotify(WPARAM wParam,LPARAM lParam)
{
	OperateInfo *pInfo=(OperateInfo*)lParam;
	AddOperateInfoText(pInfo->strInfo);
	delete pInfo;
	return 0;
}

void CDlgOperateLog::AddOperateInfoText(CString strText)
{
	CString strTime = CTime::GetCurrentTime().Format("[%H:%M:%S] ");
//	strText=strTime+strText;

	static int nIndex = 0;
	m_ShowOperateInfoList.InsertItem(nIndex, strTime);
	m_ShowOperateInfoList.SetItemText(nIndex, 1, strText);
	
	//m_ShowOperateInfoList.SetItemText(nIndex, 2, strRtuAddr);
//	m_ShowOperateInfoList
/*
	int nLen=m_edtMsg.GetWindowTextLength();
	if (nLen<8000)
	{
		m_edtMsg.SetSel(nLen,nLen);
		m_edtMsg.ReplaceSel(strText+"\r\n");
	}
	else
	{
		m_edtMsg.SetWindowText(strText+"\r\n");
	}
	m_edtMsg.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
*/
}


void CDlgOperateLog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(rect);

	if (m_ShowOperateInfoList.GetSafeHwnd())
	{
		m_ShowOperateInfoList.MoveWindow(rect);
	}
}
