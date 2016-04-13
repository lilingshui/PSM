// DlgTerminalBaoWen.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgTerminalBaoWen.h"
#include ".\dlgterminalbaowen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTerminalBaoWen dialog
IMPLEMENT_DYNCREATE(CDlgTerminalBaoWen, CDialog)

CDlgTerminalBaoWen::CDlgTerminalBaoWen(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTerminalBaoWen::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTerminalBaoWen)
	CMsgOutput::GetInstance()->RegisterPacketInfoWnd(this);
	//}}AFX_DATA_INIT
}


void CDlgTerminalBaoWen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTerminalBaoWen)
	DDX_Control(pDX, IDC_EDIT_PACKETLOG, m_edtPacketLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTerminalBaoWen, CDialog)
	//{{AFX_MSG_MAP(CDlgTerminalBaoWen)
		// NOTE: the ClassWizard will add message map macros here
	ON_MESSAGE(PACKETINFO_EVENT_NOTIFY,OnPacketInfoEventNotify)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTerminalBaoWen message handlers


LRESULT CDlgTerminalBaoWen::OnPacketInfoEventNotify(WPARAM wParam,LPARAM lParam)
{
	OperateInfo *pInfo=(OperateInfo*)lParam;
	AddPacketLogText(pInfo->strInfo);
	delete pInfo;
	return 0;
}


void CDlgTerminalBaoWen::AddPacketLogText(CString strText)
{
	
	CString strTime = CTime::GetCurrentTime().Format("[%H:%M:%S] ");
	strText=strTime+strText;
	int nLen=m_edtPacketLog.GetWindowTextLength();
	if (nLen<8000)
	{
		m_edtPacketLog.SetSel(nLen,nLen);
		m_edtPacketLog.ReplaceSel(strText+"\r\n");	
		//m_edtPacketLog.SetWindowText(strText+"\r\n");
	}
	else
	{
		m_edtPacketLog.SetWindowText(strText+"\r\n");
	}
	//m_edtPacketLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	GetDlgItem(IDC_EDIT_PACKETLOG)->ShowScrollBar(SB_VERT, TRUE);
}

void CDlgTerminalBaoWen::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(rect);

	if (m_edtPacketLog.GetSafeHwnd())
	{
		m_edtPacketLog.MoveWindow(rect);
	}
}
