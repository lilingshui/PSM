// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "MainFrm.h"
#include "WFTTNNDoc.h"
#include "WFTTNNView.h"

#include "DlgTerminalBaoWen.h"	// Added by ClassView
#include "DlgOperateLog.h"

#include "DlgConnect.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int g_nModuleControl;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_MENUITEM_TERMINALINFO, OnMenuitemTerminalinfo)
	ON_COMMAND(ID_MENUITEM_RNDREAD, OnMenuitemRndread)
	ON_COMMAND(ID_MENUITEM_DATABROWSE, OnMenuitemDatabrowse)
	ON_COMMAND(ID_MENUITEM_EXIT, OnMenuitemExit)
	ON_COMMAND(ID_MENUITEM_SETPARAM, OnMenuitemSetparam)
	ON_COMMAND(ID_MENUITEM_SETCHANNEL, OnMenuitemSetchannel)
	ON_COMMAND(ID_MENUITEM_CONNECT, OnMenuitemConnect)
	ON_COMMAND(ID_MENUITEM_STOP, OnMenuitemStop)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_BATCHINFO, OnMenuBatchInfo)
	ON_COMMAND(ID_MENUITEM_METERINFO, OnMenuMeterInfo)
	ON_COMMAND(ID_Menu_Read_RealTimeData, OnMenuReadRealtimedata)
	ON_COMMAND(ID_Menu_Read_HistoryData, OnIdMenuReadHistorydata)
	ON_COMMAND(ID_Menu_Read_AlarmData, OnMenuReadAlarmdata)
	ON_COMMAND(ID_Menu_Read_EventData, OnMenuReadEventdata)
	ON_COMMAND(ID_Menu_Read_ReadTermInfo, OnMenuReadReadterminfo)
	ON_COMMAND(ID_MENUITEM_AUTOTEST, OnMenuitemAutotest)
	ON_COMMAND(ID_MENUITEM_TASK, OnMenuitemTask)
	ON_COMMAND(ID_MENUITEM_OTHERTEST, OnMenuItemOtherTest)
	ON_COMMAND(ID_MENUITEM_VISUALMETER, OnMenuitemVisualmeter)
	ON_COMMAND(ID_MENUITEM_VIEW_TOOLBAR, &CMainFrame::OnMenuitemViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_MENUITEM_VIEW_TOOLBAR, &CMainFrame::OnUpdateMenuitemViewToolbar)
	ON_COMMAND(ID_MENUITEM_VIEW_TREE, &CMainFrame::OnMenuitemViewTree)
	ON_UPDATE_COMMAND_UI(ID_MENUITEM_VIEW_TREE, &CMainFrame::OnUpdateMenuitemViewTree)
	ON_COMMAND(ID_MENUITEM_VIEW_OUTPUT, &CMainFrame::OnMenuitemViewOutput)
	ON_UPDATE_COMMAND_UI(ID_MENUITEM_VIEW_OUTPUT, &CMainFrame::OnUpdateMenuitemViewOutput)
	ON_COMMAND(ID_MENU_TEST_REREAD, &CMainFrame::OnMenuTestReread)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) /*||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME)*/)
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	//下面的代码是设置按钮的宽度和长度
	m_wndToolBar.GetToolBarCtrl().SetButtonWidth(50,90);
	CImageList img;
	img.Create(21, 21, ILC_COLOR16|ILC_MASK, 2,2);
	img.SetBkColor(::GetSysColor(COLOR_BTNFACE));
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_TERM));//前台程序
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_CONN));//前台程序
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_STOP));//前台程序
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_CHANNELSET));//前台程序
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_SET));//前台程序
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_RNDREAD));//前台程序
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_AUTOCESHI));//前台程序
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_BROWSE));//前台程序
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_ABOUT));//前台程序
	img.Add(AfxGetApp()->LoadIcon(IDI_ICON_EXIT));//前台程序
	m_wndToolBar.GetToolBarCtrl().SetImageList(&img);
	img.Detach();
	//改变属性
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT |CBRS_TOOLTIPS | TBSTYLE_TRANSPARENT|TBBS_CHECKBOX );
	m_wndToolBar.SetButtons(NULL,16);
	m_wndToolBar.SetButtonInfo(0, ID_MENUITEM_TERMINALINFO, TBSTYLE_BUTTON, 0);
	m_wndToolBar.SetButtonText(0, "终端管理");
	m_wndToolBar.SetButtonInfo(1, ID_MENUITEM_CONNECT, TBSTYLE_BUTTON, 1);
	m_wndToolBar.SetButtonText(1, "终端连接");
	m_wndToolBar.SetButtonInfo(2, ID_MENUITEM_SETCHANNEL, TBSTYLE_BUTTON, 3);
	m_wndToolBar.SetButtonText(2, "通道设置");
	m_wndToolBar.SetButtonInfo(3, ID_MENUITEM_SETPARAM, TBSTYLE_BUTTON, 4);
	m_wndToolBar.SetButtonText(3, "参数设置");
	m_wndToolBar.SetButtonInfo(4, IDC_BUTTON1, TBBS_SEPARATOR, 0);  //split	
	m_wndToolBar.SetButtonInfo(5, ID_Menu_Read_RealTimeData, TBSTYLE_BUTTON , 5);
	m_wndToolBar.SetButtonText(5, "当前数据");
	m_wndToolBar.SetButtonInfo(6, ID_Menu_Read_HistoryData, TBSTYLE_BUTTON , 5);
	m_wndToolBar.SetButtonText(6, "历史数据");
	m_wndToolBar.SetButtonInfo(7, ID_Menu_Read_EventData, TBSTYLE_BUTTON , 5);
	m_wndToolBar.SetButtonText(7, "事件数据");
	m_wndToolBar.SetButtonInfo(8, ID_Menu_Read_AlarmData, TBSTYLE_BUTTON , 5);
	m_wndToolBar.SetButtonText(8, "报警数据");
	m_wndToolBar.SetButtonInfo(9, ID_Menu_Read_ReadTermInfo, TBSTYLE_BUTTON , 5);
	m_wndToolBar.SetButtonText(9, "终端信息");
	m_wndToolBar.SetButtonInfo(10, ID_MENUITEM_AUTOTEST, TBSTYLE_BUTTON , 6);
	m_wndToolBar.SetButtonText(10, "自动测试");
	m_wndToolBar.SetButtonInfo(11, ID_MENU_TEST_REREAD, TBSTYLE_BUTTON , 6);
	m_wndToolBar.SetButtonText(11, "补抄测试");
	m_wndToolBar.SetButtonInfo(12, ID_MENUITEM_DATABROWSE, TBSTYLE_BUTTON , 7);
	m_wndToolBar.SetButtonText(12, "数据管理");
	m_wndToolBar.SetButtonInfo(13, IDC_BUTTON1, TBBS_SEPARATOR, 0);  //split
	m_wndToolBar.SetButtonInfo(14, ID_APP_ABOUT, TBSTYLE_BUTTON , 8);
	m_wndToolBar.SetButtonText(14, "版本说明");
	m_wndToolBar.SetButtonInfo(15, ID_MENUITEM_EXIT, TBSTYLE_BUTTON , 9);
	m_wndToolBar.SetButtonText(15, "退出系统");

	CRect rectToolBar;
	//得到按钮的大小
	m_wndToolBar.GetItemRect(0, &rectToolBar);
	//设置按钮的大小
	m_wndToolBar.SetSizes(rectToolBar.Size(), CSize(20,20));
	// 
		
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
		
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);	
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	

	// TODO: Delete these three lines if you don't want the toolbar to
	if (!m_wndMyBar1.Create(_T(""), this, CSize(230,260),TRUE,123))
	{
		TRACE0("Failed to create mybar\n");
			return -1;
	}
	m_wndMyBar1.SetBarStyle(m_wndMyBar1.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndMyBar1.EnableDocking(CBRS_ALIGN_LEFT);	
	DockControlBar(&m_wndMyBar1, AFX_IDW_DOCKBAR_LEFT);
	EnableDocking(CBRS_ALIGN_ANY);
	m_wndMyBar1.ModifyStyle(WS_SYSMENU, 0, 0);
	//
	m_TerminalParentDlgBar.Create(&m_wndMyBar1, IDD_DIALOG_TERMINALPARENT,CBRS_LEFT, 111111);	
	CChannelMgr::getInstance()->Register(&m_TerminalParentDlgBar);

	if (!m_wndMyBar2.Create(_T(""), this,CSize(200,180),TRUE,124))
	{
		TRACE0("Failed to create mybar\n");
		return -1;
	}
	m_wndMyBar2.SetBarStyle(m_wndMyBar2.GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndMyBar2.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMyBar2, AFX_IDW_DOCKBAR_BOTTOM);
	//
	m_TabCtrl.Create(TCS_DOWN|WS_CHILD|WS_VISIBLE,CRect(0,0,100,100),&m_wndMyBar2,125);
	m_TabCtrl.AddPage(RUNTIME_CLASS(CDlgTerminalBaoWen),IDD_DIALOG_BAOWEN,"报文日志",NULL);
	m_TabCtrl.AddPage(RUNTIME_CLASS(CDlgOperateLog),IDD_DIALOG_OPERATELOG,"操作日志",NULL);
	m_TabCtrl.UpdateWindow();

	m_pDlgConn = new CDlgConnect();
	m_pDlgConn->Create(IDD_DIALOG_CONNECT, this);
	m_TerminalParentDlgBar.AddRecver(m_pDlgConn);

	OnMenuBatchInfo();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

// 	cs.style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX;
// //	cs.style &= ~FWS_ADDTOTITLE;
// 
// 	SetTitle(g_AppTitle);
 	cs.style |= WS_CAPTION | WS_MAXIMIZE;

 	cs.cx = ::GetSystemMetrics(SM_CXSCREEN); 
 	cs.cy = GetSystemMetrics(SM_CYSCREEN); 

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers



void CMainFrame::OnMenuitemExit() 
{
	// TODO: Add your command handler code here
	CMainFrame::OnClose();
}

void CMainFrame::OnMenuitemSetparam() 
{
	OnMenuMeterInfo();
}

void CMainFrame::OnMenuitemConnect() 
{
	m_pDlgConn->CenterWindow(this);
	m_pDlgConn->ShowWindow(SW_SHOW);	
}

void CMainFrame::OnMenuitemStop() 
{
	// TODO: Add your command handler code here
}

void CMainFrame::OnMenuitemRndread() 
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 随时召测");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgRndRead->ShowWindow(SW_SHOW);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);		
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);

		g_nModuleControl = Module_RndRead;
	}	
}


//////////////////////////////////////////////////////////////////////////
// 功能表

void CMainFrame::OnMenuBatchInfo()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 批次管理");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{		
		pView->pDlgBatchInfo->ShowWindow(SW_SHOW);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);
	}	
}


void CMainFrame::OnMenuitemTerminalinfo() 
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 终端管理");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgTerminalInfo->ShowWindow(SW_SHOW);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);
	}
}

void CMainFrame::OnMenuMeterInfo()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 表计管理");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgSetParam->ShowWindow(SW_SHOW);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);	
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);

		g_nModuleControl = Module_SetParam;
	}
}

void CMainFrame::OnMenuReadRealtimedata()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 随时召测（当前数据）");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgReadCurrentData->ShowWindow(SW_SHOW);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);	
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);

		g_nModuleControl = Module_RndRead_CurrentData;
	}
}

void CMainFrame::OnIdMenuReadHistorydata()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 随时召测（历史数据）");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgReadHistoryData->ShowWindow(SW_SHOW);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);	
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);

		g_nModuleControl = Moudle_RndRead_HistoryData;
	}
}

void CMainFrame::OnMenuReadEventdata()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 随时召测（事件数据）");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgReadEventData->ShowWindow(SW_SHOW);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);	
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);

		g_nModuleControl = Moudle_RndRead_EventData;
	}
}

void CMainFrame::OnMenuReadAlarmdata()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 随时召测（报警事件）");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgReadAlarmData->ShowWindow(SW_SHOW);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);	
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);

		g_nModuleControl = Moudle_RndRead_AlarmData;
	}
}

void CMainFrame::OnMenuReadReadterminfo()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 随时召测（终端信息）");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgReadTermData->ShowWindow(SW_SHOW);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);	
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);

		g_nModuleControl = Module_RndRead_TerminalInfo;
	}
}

void CMainFrame::OnMenuitemSetchannel() 
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 通道管理");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgSetChannel->ShowWindow(SW_SHOW);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);	
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);
	}

	g_nModuleControl = Module_SetChannel;
}
void CMainFrame::OnMenuitemAutotest()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 自动检测");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgAutoTest->ShowWindow(SW_SHOW);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);
	}

	g_nModuleControl = Module_AutoCeShi;
}

void CMainFrame::OnMenuitemTask()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 方案管理");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgTaskManage->ShowWindow(SW_SHOW);
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);
	}
}

void CMainFrame::OnMenuItemOtherTest()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 检测录入");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgOtherTestInfo->ShowWindow(SW_SHOW);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);
	}
}
void CMainFrame::OnMenuitemVisualmeter()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 虚拟表检测");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgVisualMeter->ShowWindow(SW_SHOW);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);
	}
}

void CMainFrame::OnMenuitemDatabrowse() 
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 数据管理");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgDataBrowse->ShowWindow(SW_SHOW);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);		
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
		pView->pDlgRereadTest->ShowWindow(SW_HIDE);
	}	
}

void CMainFrame::OnMenuTestReread()
{
	AfxGetMainWnd()->SetWindowText(APP_TITLE" - 补抄检测");

	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgRereadTest->ShowWindow(SW_SHOW);
		pView->pDlgAutoTest->ShowWindow(SW_HIDE);
		pView->pDlgSetParam->ShowWindow(SW_HIDE);
		pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
		pView->pDlgRndRead->ShowWindow(SW_HIDE);
		pView->pDlgDataBrowse->ShowWindow(SW_HIDE);
		pView->pDlgSetChannel->ShowWindow(SW_HIDE);
		pView->pDlgBatchInfo->ShowWindow(SW_HIDE);
		pView->pDlgReadCurrentData->ShowWindow(SW_HIDE);
		pView->pDlgReadHistoryData->ShowWindow(SW_HIDE);
		pView->pDlgReadEventData->ShowWindow(SW_HIDE);
		pView->pDlgReadTermData->ShowWindow(SW_HIDE);
		pView->pDlgTaskManage->ShowWindow(SW_HIDE);
		pView->pDlgOtherTestInfo->ShowWindow(SW_HIDE);
		pView->pDlgVisualMeter->ShowWindow(SW_HIDE);
		pView->pDlgReadAlarmData->ShowWindow(SW_HIDE);
	}

	g_nModuleControl = Moudle_Test_Reread;
}

void CMainFrame::OnMenuitemViewToolbar()
{
	ShowControlBar(&m_wndToolBar, !m_wndToolBar.IsWindowVisible(), FALSE);
}


void CMainFrame::OnUpdateMenuitemViewToolbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndToolBar.IsVisible());
}


void CMainFrame::OnMenuitemViewTree()
{
	ShowControlBar(&m_wndMyBar1, !m_wndMyBar1.IsWindowVisible(), FALSE);
}


void CMainFrame::OnUpdateMenuitemViewTree(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndMyBar1.IsVisible());
}


void CMainFrame::OnMenuitemViewOutput()
{
	ShowControlBar(&m_wndMyBar2, !m_wndMyBar2.IsWindowVisible(), FALSE);
}


void CMainFrame::OnUpdateMenuitemViewOutput(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndMyBar2.IsVisible());
}

void CMainFrame::RefreshTaskArch()
{
	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgAutoTest->InitTaskList();
	}
}

void CMainFrame::RefreshChannelArch()
{
	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgAutoTest->InitChannelList();
	}
}

void CMainFrame::RefreshConnChannel()
{
	m_pDlgConn->InitChannelType();
}

void CMainFrame::RefreshTermArch()
{
	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgSetParam->InitialTerminalInfo();		
	}
}

void CMainFrame::RefreshBatchArch()
{
	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgTerminalInfo->InitBatchInfoList();
	}
}

void CMainFrame::RefreshMeterArch()
{
	CWFTTNNView *pView=(CWFTTNNView*)GetActiveView();
	if (pView)
	{
		pView->pDlgReadCurrentData->InitMeterList();	
		pView->pDlgReadHistoryData->InitMeterList();
		pView->pDlgReadEventData->InitMeterList();
		pView->pDlgReadAlarmData->InitMeterList();
	}
}
