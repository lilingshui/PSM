// WFTTNNView.cpp : implementation of the CWFTTNNView class
//

#include "stdafx.h"
#include "WFTTNN.h"

#include "WFTTNNDoc.h"
#include "WFTTNNView.h"
#include "MainFrm.h"
#include ".\wfttnnview.h"
#include "Channel/ChannelMgr.h"
#include "ReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWFTTNNView

IMPLEMENT_DYNCREATE(CWFTTNNView, CFormView)

BEGIN_MESSAGE_MAP(CWFTTNNView, CFormView)
	//{{AFX_MSG_MAP(CWFTTNNView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWFTTNNView construction/destruction

CWFTTNNView::CWFTTNNView()
	: CFormView(CWFTTNNView::IDD)
{
	//{{AFX_DATA_INIT(CWFTTNNView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	pDlgBatchInfo = NULL;
	pDlgTerminalInfo = NULL;
	pDlgSetParam = NULL;
	pDlgRndRead = NULL;
	pDlgDataBrowse = NULL;	
	pDlgAutoTest = NULL;
	pDlgSetChannel = NULL;
	pDlgReadCurrentData = NULL;
	pDlgReadEventData = NULL;
	pDlgReadTermData = NULL;
	pDlgReadHistoryData = NULL;
	pDlgAutoTask = NULL;
	pDlgOtherTestInfo = NULL;
	pDlgVisualMeter = NULL;
	pDlgTaskManage = NULL;
	pDlgRereadTest = NULL;
}

CWFTTNNView::~CWFTTNNView()
{
	if (pDlgBatchInfo)		delete pDlgBatchInfo;
	if (pDlgTerminalInfo)	delete pDlgTerminalInfo;
	if (pDlgSetParam)		delete pDlgSetParam;
	if (pDlgSetChannel)		delete pDlgSetChannel;
	if (pDlgReadCurrentData)delete pDlgReadCurrentData;
	if (pDlgReadEventData)	delete pDlgReadEventData;
	if (pDlgReadTermData)	delete pDlgReadTermData;
	if (pDlgReadHistoryData)delete pDlgReadHistoryData;
	if (pDlgRndRead)		delete pDlgRndRead;
	if (pDlgAutoTest)		delete pDlgAutoTest;
	if (pDlgDataBrowse)		delete pDlgDataBrowse;
	if (pDlgAutoTask)		delete pDlgAutoTask;
	if (pDlgOtherTestInfo)	delete pDlgOtherTestInfo;
	if (pDlgTaskManage)		delete pDlgTaskManage;
	if (pDlgRereadTest)		delete pDlgRereadTest;
}

void CWFTTNNView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWFTTNNView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CWFTTNNView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CWFTTNNView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	InitialDlgObject();
}

/////////////////////////////////////////////////////////////////////////////
// CWFTTNNView printing

BOOL CWFTTNNView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWFTTNNView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWFTTNNView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CWFTTNNView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CWFTTNNView diagnostics

#ifdef _DEBUG
void CWFTTNNView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWFTTNNView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CWFTTNNDoc* CWFTTNNView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWFTTNNDoc)));
	return (CWFTTNNDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWFTTNNView message handlers

void CWFTTNNView::InitialDlgObject()
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();

	pDlgTerminalInfo = new CDlgTerminalInfo();
	pDlgTerminalInfo->Create(IDD_DIALOG_TERMINALINFO,this);

	pDlgRndRead = new CDlgRndRead();
	pDlgRndRead->Create(IDD_DIALOG_RNDREAD,this);

	pDlgSetParam = new CDlgSetParam();
	pDlgSetParam->Create(IDD_DIALOG_SETPARAM,this);

	pDlgSetChannel = new CDlgSetChannel();
	pDlgSetChannel->Create(IDD_DIALOG_SETCHANNEL,this);

	pDlgAutoTest = new CDlgAutoTest();
	pDlgAutoTest->Create(IDD_FORM_AUTO_TEST,this);
	
	pDlgDataBrowse = new CDlgDataBrowse();
	pDlgDataBrowse->Create(IDD_DIALOG_DATABROWSE,this);

	pDlgBatchInfo = new CDlgBatchInfo();
	pDlgBatchInfo->Create(IDD_DIALOG_BATCHINFO,this);

	pDlgReadCurrentData = new CDlgReadCurrentData();
	pDlgReadCurrentData->Create(IDD_FORM_READ_CURRENT_DATA, this);

	pDlgReadHistoryData = new CDlgReadHistoryData();
	pDlgReadHistoryData->Create(IDD_FORM_READ_HISTORY_DATA, this);

	pDlgReadEventData = new CDlgReadEventData();
	pDlgReadEventData->Create(IDD_FORM_READ_EVENT_DATA, this);

	pDlgReadTermData = new CDlgReadTermInfo();
	pDlgReadTermData->Create(IDD_FORM_READ_TERMINAL_INFO, this);

	pDlgOtherTestInfo = new CDlgOtherTestInfo();
	pDlgOtherTestInfo->Create(IDD_FORM_OTHER_TEST, this);

	pDlgVisualMeter = new CDlgVisualMeter();
	pDlgVisualMeter->Create(IDD_FORM_VISUAL_METER, this);

	pDlgTaskManage = new CDlgTaskManage();
	pDlgTaskManage->Create(IDD_FORM_TASK_MANAGE, this);

	pDlgReadAlarmData = new CDlgReadAlarmData();
	pDlgReadAlarmData->Create(IDD_FORM_READ_ALARM_DATA, this);

	pDlgRereadTest = new CDlgRereadTest();
	pDlgRereadTest->Create(IDD_FORM_TEST_REREAD, this);

	CChannelMgr::getInstance()->Register(pDlgReadCurrentData);
	CChannelMgr::getInstance()->Register(pDlgSetParam);
	CChannelMgr::getInstance()->Register(pDlgReadTermData);
	CChannelMgr::getInstance()->Register(pDlgAutoTest);
	CChannelMgr::getInstance()->Register(pDlgReadHistoryData);
	CChannelMgr::getInstance()->Register(pDlgReadEventData);
	CChannelMgr::getInstance()->Register(pDlgReadAlarmData);
	CChannelMgr::getInstance()->Register(pDlgRereadTest);


	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgBatchInfo);
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgTerminalInfo);
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgSetParam);
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgReadCurrentData);
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgReadHistoryData);
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgReadEventData);
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgReadTermData);
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgAutoTest);
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgVisualMeter);	
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgOtherTestInfo);	
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgReadAlarmData);
	pMainWnd->m_TerminalParentDlgBar.AddRecver(pDlgRereadTest);
	
	pMainWnd->OnMenuBatchInfo();
}

void CWFTTNNView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (pDlgAutoTest->GetSafeHwnd())
	{
		pDlgAutoTest->MoveWindow(0, 0, cx, cy);		
		pDlgDataBrowse->MoveWindow(0, 0, cx, cy);	
		pDlgSetParam->MoveWindow(0, 0, cx, cy);	
		pDlgSetChannel->MoveWindow(0, 0, cx, cy);	
		pDlgTerminalInfo->MoveWindow(0, 0, cx, cy);
		pDlgBatchInfo->MoveWindow(0, 0, cx, cy);
		pDlgReadCurrentData->MoveWindow(0, 0, cx, cy);
		pDlgReadHistoryData->MoveWindow(0, 0, cx, cy);
		pDlgReadEventData->MoveWindow(0, 0, cx, cy);
		pDlgReadAlarmData->MoveWindow(0, 0, cx, cy);
		pDlgRereadTest->MoveWindow(0, 0, cx, cy);
	}	
}
