// DlgTaskManage.cpp : 实现文件
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgTaskManage.h"
#include ".\dlgtaskmanage.h"
#include "MainFrm.h"


#define IDD_TABCTRL_DATATYPE	10001
static void *s_hPage[7];

// CDlgTaskManage 对话框

IMPLEMENT_DYNAMIC(CDlgTaskManage, CDialog)
CDlgTaskManage::CDlgTaskManage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTaskManage::IDD, pParent)
	, m_strTaskName(_T(""))
{
}

CDlgTaskManage::~CDlgTaskManage()
{
}

void CDlgTaskManage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TASK, m_ctrlTaskList);
	DDX_Text(pDX, IDC_EDTASKNAME, m_strTaskName);
}

BEGIN_MESSAGE_MAP(CDlgTaskManage, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDD_TABCTRL_DATATYPE, OnTcnSelchangeTabDataType)
	ON_BN_CLICKED(IDC_DEL, OnBnClickedDel)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TASK, &CDlgTaskManage::OnNMDblclkListTask)
END_MESSAGE_MAP()


// CDlgTaskManage 消息处理程序
BOOL CDlgTaskManage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD dwExtendStyle = 0;

	dwExtendStyle = m_ctrlTaskList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ctrlTaskList.SetExtendedStyle(dwExtendStyle);
	m_ctrlTaskList.InsertColumn(0, "任务号", LVCFMT_CENTER, 100);
	m_ctrlTaskList.InsertColumn(1, "任务名称", LVCFMT_LEFT, 500);
	InitTaskList();

	// tab控件, 数据类型
	CRect rect;
	GetDlgItem(IDC_RECT)->GetWindowRect(&rect);
	ScreenToClient(rect);
	m_tabDataType.Create(0, CRect(0, 0, 0, 0), this, IDD_TABCTRL_DATATYPE);	
	m_tabDataType.InsertItem(0, _T("事件数据"));	
	m_tabDataType.InsertItem(1, _T("告警数据"));
	m_tabDataType.InsertItem(2, _T("当前数据"));
	m_tabDataType.InsertItem(3, _T("曲线数据"));
	m_tabDataType.InsertItem(4, _T("日冻结数据"));
	m_tabDataType.InsertItem(5, _T("月冻结数据"));
	m_tabDataType.InsertItem(6, _T("其它"));
 	m_tabDataType.MoveWindow(rect);
	m_tabDataType.SetFont(GetFont());
	m_tabDataType.SetCurSel(0);
	
	m_tabDataType.ShowWindow(SW_SHOW);
	
		
	//	Tab客户区域
	CRect rs;
	m_tabDataType.GetClientRect(rs);
	rs.top += 30;
	rs.bottom -= 10;
	rs.left += 10;
	rs.right -= 10;	

	// 事件数据
	m_pPageEvent = new CPageEvent(5);
	m_pPageEvent->Create(IDD_TABPAGE_EVENT, &m_tabDataType);
	m_pPageEvent->MoveWindow(rs);
	m_pPageEvent->ShowWindow(SW_SHOW);
	s_hPage[0] = m_pPageEvent;

	// 告警数据
	m_pPageAlarm = new CPageEvent(8);
	m_pPageAlarm->Create(IDD_TABPAGE_EVENT, &m_tabDataType);
	m_pPageAlarm->MoveWindow(rs);
	s_hPage[1] = m_pPageAlarm;

	// 当前数据页
	m_pPageNow = new CPageNow(1);
	m_pPageNow->Create(IDD_TABPAGE_NOW, &m_tabDataType);
	m_pPageNow->MoveWindow(rs);
	s_hPage[2] = m_pPageNow;

	// 曲线数据
	m_pPageQX = new CPageHistory(2);
	m_pPageQX->Create(IDD_TABPAGE_HISTORY, &m_tabDataType);
	m_pPageQX->MoveWindow(rs);
	s_hPage[3] = m_pPageQX;	

	// 日冻结数据
	m_pPageRDJ = new CPageHistory(3);
	m_pPageRDJ->Create(IDD_TABPAGE_HISTORY, &m_tabDataType);
	m_pPageRDJ->MoveWindow(rs);
	s_hPage[4] = m_pPageRDJ;

	// 月冻结数据
	m_pPageYDJ = new CPageHistory(4);
	m_pPageYDJ->Create(IDD_TABPAGE_HISTORY, &m_tabDataType);
	m_pPageYDJ->MoveWindow(rs);
	s_hPage[5] = m_pPageYDJ;

	// 其它
	m_pPageParam = new CPageNow(6);
	m_pPageParam->Create(IDD_TABPAGE_NOW, &m_tabDataType);
	m_pPageParam->MoveWindow(rs);
	s_hPage[6] = m_pPageParam;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTaskManage::InitTaskList()
{
	m_ctrlTaskList.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	int nRow = 0;
	char strSQL[300] = {0};	
	sprintf(strSQL,"SELECT * FROM READING_DESC ");
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		SYSTEMTIME startTime, endTime;
		CString strTaskid = "";
		CString strTaskName = "";
		CString strStartTime = "";
		CString strEndTime = "";		

		record.GetFieldValue("READING_ID",val);
		strTaskid.Format("%d", (long)val);

		record.GetFieldValue("READING_ID_DESC",val);
		strTaskName=val.bstrVal;
		
		m_ctrlTaskList.InsertItem(nRow, strTaskid);
		m_ctrlTaskList.SetItemText(nRow, 1, strTaskName);
		
		record.RecordMoveNext();
		nRow++;
	}
}

void CDlgTaskManage::OnTcnSelchangeTabDataType(NMHDR *pNMHDR, LRESULT *pResult)
{
	int curSel = m_tabDataType.GetCurSel();
	
	s_hPage[curSel] && ((CWnd *)s_hPage[curSel])->ShowWindow(SW_SHOW);
	for (int i = 0; i < 7; i++)
	{
		if (curSel == i)
			continue;

		s_hPage[i] && ((CWnd *)s_hPage[i])->ShowWindow(SW_HIDE);
	}

	*pResult = 0;
}

void CDlgTaskManage::OnBnClickedDel()
{
	int nRow = m_ctrlTaskList.GetSelectionMark();
	if(nRow < 0)
	{
		MessageBox("请选择任务！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;

	UpdateData(TRUE);	
	CString strTasId = m_ctrlTaskList.GetItemText(nRow, 0);
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	char strSQL[300] = {0};
	sprintf(strSQL,"DELETE FROM READING_DESC where READING_ID = %s", strTasId);
	pConnect->ExecSQL(strSQL);
	sprintf(strSQL,"DELETE FROM AUTOTEST_READING_ARCH where READING_ID = %s", strTasId);
	pConnect->ExecSQL(strSQL);
	m_ctrlTaskList.DeleteItem(nRow);

	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	pMainFrm->RefreshTaskArch();
}


void CDlgTaskManage::OnBnClickedSave()
{
	int nRow = m_ctrlTaskList.GetSelectionMark();
	if(nRow < 0)
	{
		MessageBox("请选择任务！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;

	UpdateData(TRUE);	
	CString strTaskId = m_ctrlTaskList.GetItemText(nRow, 0);
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	char strSQL[300] = {0};
	sprintf(strSQL,"DELETE FROM AUTOTEST_READING_ARCH where READING_ID = %s", strTaskId);
	pConnect->ExecSQL(strSQL);
	
	int maxId = atoi(strTaskId.GetBuffer(0));

	// 当前数据
	for (int i= 0; i < m_pPageNow->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageNow->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageNow->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageNow->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageNow->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 1, tBegin, tEnd, cycle);
	}

	// 曲线数据
	for (int i= 0; i < m_pPageQX->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageQX->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageQX->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageQX->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageQX->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 2, tBegin, tEnd, cycle);
	}

	// 日冻结数据
	for (int i= 0; i < m_pPageRDJ->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageRDJ->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageRDJ->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageRDJ->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageRDJ->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 3, tBegin, tEnd, cycle);
	}

	// 月冻结数据
	for (int i= 0; i < m_pPageYDJ->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageYDJ->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageYDJ->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageYDJ->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageYDJ->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 4, tBegin, tEnd, cycle);
	}

	// 事件数据
	for (int i= 0; i < m_pPageEvent->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageEvent->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageEvent->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageEvent->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageEvent->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 5, tBegin, tEnd, cycle);
	}

	// 参数数据
	for (int i= 0; i < m_pPageParam->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageParam->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageParam->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageParam->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageParam->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 6, tBegin, tEnd, cycle);
	}

	// 告警数据
	for (int i= 0; i < m_pPageAlarm->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageAlarm->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageAlarm->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageAlarm->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageAlarm->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 8, tBegin, tEnd, cycle);
	}

	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	pMainFrm->RefreshTaskArch();
}


void CDlgTaskManage::OnBnClickedAdd()
{
	UpdateData(true);
	if (m_strTaskName == "")
	{
		AfxMessageBox("任务名称不能为空！");
		return;
	}

	long maxId = 0;
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;
	char strSQL[300] = {0};
	sprintf(strSQL,"SELECT MAX(READING_ID) as READING_ID FROM READING_DESC ");
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;	
	if(!record.IsEndEOF())
	{
		record.GetFieldValue("READING_ID", maxId);
		maxId += 1;
	}

	sprintf(strSQL,"INSERT INTO READING_DESC(READING_ID,READING_ID_DESC) VALUES(%d,'%s') ", maxId, m_strTaskName);
	if (pConnect->ExecSQL(strSQL))
	{
		CString strId;
		strId.Format("%d", maxId);

		int icount = m_ctrlTaskList.GetItemCount();
		m_ctrlTaskList.InsertItem(icount, strId);	
		m_ctrlTaskList.SetItemText(icount, 1, m_strTaskName);
		m_ctrlTaskList.SetSelectionMark(icount);
	}

	// 当前数据
	for (int i= 0; i < m_pPageNow->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageNow->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageNow->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageNow->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageNow->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 1, tBegin, tEnd, cycle);
	}

	// 曲线数据
	for (int i= 0; i < m_pPageQX->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageQX->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageQX->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageQX->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageQX->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 2, tBegin, tEnd, cycle);
	}

	// 日冻结数据
	for (int i= 0; i < m_pPageRDJ->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageRDJ->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageRDJ->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageRDJ->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageRDJ->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 3, tBegin, tEnd, cycle);
	}

	// 月冻结数据
	for (int i= 0; i < m_pPageYDJ->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageYDJ->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageYDJ->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageYDJ->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageYDJ->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 4, tBegin, tEnd, cycle);
	}

	// 事件数据
	for (int i= 0; i < m_pPageEvent->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageEvent->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageEvent->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageEvent->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageEvent->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 5, tBegin, tEnd, cycle);
	}

	// 参数数据
	for (int i= 0; i < m_pPageParam->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageParam->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageParam->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageParam->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageParam->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 6, tBegin, tEnd, cycle);
	}	

	// 告警数据
	for (int i= 0; i < m_pPageAlarm->m_ctrlDataItem.GetItemCount(); i++)
	{
		if (!m_pPageAlarm->m_ctrlDataItem.GetCheck(i))
			continue;
		
		CString strItemID, strItemName;
		time_t tBegin, tEnd;
		int cycle;
		strItemID = m_pPageAlarm->m_ctrlDataItem.GetItemText(i, 1);
		strItemName = m_pPageAlarm->m_ctrlDataItem.GetItemText(i, 2);
		m_pPageAlarm->GetTime(tBegin, tEnd, cycle);
		AddOneTaskItemToDB(maxId, strItemID, strItemName, 8, tBegin, tEnd, cycle);
	}

	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	pMainFrm->RefreshTaskArch();
}

//向数据库添加一个任务数据项
void CDlgTaskManage::AddOneTaskItemToDB(int nTaskId, CString strItemId, CString strItemName, int nType, time_t tBegin, time_t tEnd, int nCycle)
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[1024] = {0};	
	sprintf(strSQL,"DELETE FROM AUTOTEST_READING_ARCH where READING_ID=%d and PARAM_ID='%s' and PARAM_TYPE=%d", nTaskId, strItemId, nType);
	pConnect->ExecSQL(strSQL);
	
	sprintf(strSQL,"INSERT INTO AUTOTEST_READING_ARCH(READING_ID, PARAM_ID, CRS_DATA_TYPE, PARAM_TYPE, CRS_S_TIME, CRS_E_TIME, DATA_CYCLE_TYPE) "
			"VALUES(%d, '%s', '%s', %d, %I64d, %I64d, %d)", nTaskId, strItemId, strItemName, nType, (UINT64)tBegin, (UINT64)tEnd, nCycle);
	pConnect->ExecSQL(strSQL);
}

void CDlgTaskManage::OnNMDblclkListTask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	
	
	int nSel = m_ctrlTaskList.GetSelectionMark();
	if (nSel < 0)
		return;
	CString strID = m_ctrlTaskList.GetItemText(nSel, 0);
	m_strTaskName = m_ctrlTaskList.GetItemText(nSel, 1);
	m_pPageNow->RefreshDataItem(strID);
	m_pPageQX->RefreshDataItem(strID);
	m_pPageRDJ->RefreshDataItem(strID);
	m_pPageYDJ->RefreshDataItem(strID);
	m_pPageEvent->RefreshDataItem(strID);
	m_pPageParam->RefreshDataItem(strID);
	m_pPageAlarm->RefreshDataItem(strID);
	UpdateData(FALSE);

	*pResult = 0;
}