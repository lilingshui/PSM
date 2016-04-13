// DlgAutoTest.cpp : 实现文件
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgAutoTest.h"
#include ".\dlgautotest.h"
#include "DlgAutoTask.h"
#include "ItemData.h"
#include "ptcl_codec\ptcl_packet_csg.h"
#include "ptcl_codec\ptcl_module_csg.h"
#include "ptcl_codec\data_format_csg.h"
#include "ptcl_codec\data_parse_csg.h"
#include "ReportDlg.h"
#include "LogMgr.h"
#include "ptcl_codec/data_parse_csg.h"
#include "ptcl_codec/ptcl_module_dlt.h"
#include "ptcl_codec/data_parse_dlt.h"

static bool s_bWait = false;

// CDlgAutoTest 对话框

IMPLEMENT_DYNAMIC(CDlgAutoTest, CDialog)
CDlgAutoTest::CDlgAutoTest(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAutoTest::IDD, pParent)
	, m_layout(this)
	, m_taskTime(0)
	, m_timeout(30)
	, m_retryCount(0)
	, m_termId(0)
	, m_batchId(0)
	, m_pAutoTask(NULL)
	, m_nCurrentTestCount(0)
	, m_nDayTestCount(0)
	, m_nMonthTestCount(0)
	, m_nEventTestCount(0)
	, m_nCurveTestCount(0)
	, m_nAlarmTestCount(0)
{
	InitMap();
}

CDlgAutoTest::~CDlgAutoTest()
{
}

void CDlgAutoTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_LIST_CHANNEL, m_ctrlChannelList);
	DDX_Control(pDX, IDC_LIST_TASKS, m_ctrlTaskList);
	DDX_Control(pDX, IDC_LIST_REPORT, m_ctrlReportList);
	DDX_Control(pDX, IDC_EDIT_TERM, m_EditTerm);
	DDX_Control(pDX, IDC_COMBO_RESULT, m_comboResult);
}


BEGIN_MESSAGE_MAP(CDlgAutoTest, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_TASK_MANAGE, OnBnClickedBtnTaskManage)
	ON_BN_CLICKED(IDC_BTN_TEST, OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_BTN_STOP, &CDlgAutoTest::OnBnClickedBtnStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_REPORT, &CDlgAutoTest::OnBnClickedBtnReport)
	ON_BN_CLICKED(IDC_BUTTON_RESULT, &CDlgAutoTest::OnBnClickedButtonResult)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_REPORT, &CDlgAutoTest::OnCustomdrawList)
END_MESSAGE_MAP()


// CDlgAutoTest 消息处理程序
BOOL CDlgAutoTest::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwExtendStyle = 0;

	// 通道列表
	dwExtendStyle = m_ctrlChannelList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlChannelList.SetExtendedStyle(dwExtendStyle);
	m_ctrlChannelList.InsertColumn(0,"#",LVCFMT_CENTER,20);
	m_ctrlChannelList.InsertColumn(1,"通道ID",LVCFMT_CENTER, 60);
	m_ctrlChannelList.InsertColumn(2,"通道名称",LVCFMT_LEFT,500);
	InitChannelList();

	// 任务列表
	dwExtendStyle = m_ctrlTaskList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlTaskList.SetExtendedStyle(dwExtendStyle);
	m_ctrlTaskList.InsertColumn(0,"#",LVCFMT_CENTER,20);
	m_ctrlTaskList.InsertColumn(1,"任务ID",LVCFMT_LEFT,60);
	m_ctrlTaskList.InsertColumn(2,"任务名称",LVCFMT_LEFT,500);
	InitTaskList();
	
	// 结果列表
	dwExtendStyle = m_ctrlReportList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ctrlReportList.SetExtendedStyle(dwExtendStyle);
	m_ctrlReportList.InsertColumn(0,"测量点",LVCFMT_LEFT,80);
	m_ctrlReportList.InsertColumn(1,"通道名称",LVCFMT_LEFT,100);
	m_ctrlReportList.InsertColumn(2,"任务名称",LVCFMT_LEFT,150);
	m_ctrlReportList.InsertColumn(3,"数据项类型",LVCFMT_LEFT,150);
	m_ctrlReportList.InsertColumn(4,"数据项名称",LVCFMT_LEFT,150);
	m_ctrlReportList.InsertColumn(5,"实测值",LVCFMT_LEFT,100);
	m_ctrlReportList.InsertColumn(6,"参考值",LVCFMT_LEFT,100);
	m_ctrlReportList.InsertColumn(7,"执行结果",LVCFMT_LEFT,100);

	m_comboResult.AddString(_T("合格"));
	m_comboResult.AddString(_T("不合格"));

	GetDlgItem(IDC_EDIT_TERM)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);

	CRect rect;
	GetClientRect(&rect);
	m_layout.SetOldRect(rect);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAutoTest::OnEvent(unsigned int nEventID, void *pParam)
{	
	if (Event_Type_Item_Double_Click != nEventID || !::IsWindowVisible(this->GetSafeHwnd()))
		return;

	if (ITEM_TYPE_TERMINAL  == ((ItemData *)pParam)->m_type)
	{
		TerminalItemData *data = (TerminalItemData *)pParam;
		
		CString strNo = "";
		strNo.Format("%d", data->m_id);
		
		CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
		if (pConnect == NULL)
			return ;

		char strSQL[300] = {0};		
		sprintf(strSQL,"SELECT * FROM RTU_ARCH WHERE RTU_ID = %d ", data->m_id);
		CADORecordSet record;
		if (!pConnect->ExecSQL(strSQL,record))
			return ;

		if (!record.IsEndEOF())
		{
			int batch_id = 0;		
			record.GetFieldValue("BatchNo",batch_id);

			m_termId = data->m_id;
			m_termAddr = data->m_addr;
			m_batchId = batch_id;			
		}

		m_EditTerm.SetWindowText(strNo);	
	}
}

void CDlgAutoTest::InitChannelList()
{
	m_ctrlChannelList.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};
	int nRow = 0;
	sprintf(strSQL,"SELECT * FROM COMM_ARCH WHERE CH_ID <> %d ORDER BY CH_ID", m_virtualDeviceId);
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nChannelID;
		CString strChannelID = _T("");
		CString strChannelName = _T("");
		
		record.GetFieldValue("CH_ID",nChannelID);
		strChannelID.Format("%d", nChannelID);
		record.GetFieldValue("CH_NAME",val);
		strChannelName = val.bstrVal;

		m_ctrlChannelList.InsertItem(nRow, "");
		m_ctrlChannelList.SetItemText(nRow, 1, strChannelID);
		m_ctrlChannelList.SetItemText(nRow, 2, strChannelName);

		record.RecordMoveNext();
		nRow++;
	}
}

void CDlgAutoTest::InitTaskList()
{
	m_ctrlTaskList.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};
	int nRow = 0;
	sprintf(strSQL,"SELECT * FROM READING_DESC ORDER BY READING_ID ");//主站采集测试方案描述

	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		int ParamID;
		CString strID = _T("");
		CString strParamName = _T("");

		record.GetFieldValue("READING_ID",ParamID);
		strID.Format("%d", ParamID);
		record.GetFieldValue("READING_ID_DESC",val);
		strParamName = val.bstrVal;

		m_ctrlTaskList.InsertItem(nRow, "");
		m_ctrlTaskList.SetItemText(nRow, 1, strID);
		m_ctrlTaskList.SetItemText(nRow, 2, strParamName);		

		record.RecordMoveNext();
		nRow++;
	}
}

void CDlgAutoTest::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (!GetDlgItem(IDC_LIST_REPORT)->GetSafeHwnd())
		return;
	m_layout.SetPos(IDC_LIST_REPORT, LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	m_layout.SetPos(IDC_LIST_TASKS, LT_LEFT_TOP, RB_RIGHT);
	
	m_layout.SetOldRect(CRect(0, 0, cx, cy));
}

void CDlgAutoTest::OnBnClickedBtnTaskManage()
{
	CDlgAutoTask dlg;
	dlg.DoModal();
}

void CDlgAutoTest::OnBnClickedBtnTest()
{
	UpdateData(TRUE);
	int rlt = AfxMessageBox("在每次进行自动测试前请确认是否已设置【终端信息】菜单中的【使用系统时间对时】和【数据区复位】？“是”为确认已设置；“否”为未设置，请设置！",MB_YESNO|MB_ICONWARNING);
	if(rlt != 6)
	{
		return;
	}
	if (m_termId == 0)
	{
		AfxMessageBox("无终端存在，请选择终端！");
		return;
	}

	InitTask();

	if (m_vChannelId.size() <= 0)
	{
		AfxMessageBox("无通道存在，请检查是否勾选通道列表！");
		return;
	}
	
	if (m_vTaskId.size() <= 0)
	{
		AfxMessageBox("无采集任务存在，请检查是否勾选采集任务列表！");
		return;
	}

	if (m_vTask.empty())
	{
		AfxMessageBox("无任务存在，请检查配置参数！");
		return;
	}
	
	GetDlgItem(IDC_BTN_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	m_ctrlReportList.DeleteAllItems();

	m_pAutoTask = &m_vTask.at(0);
	m_nCurrentTestCount = 0;
	m_nDayTestCount = 0;
	m_nMonthTestCount = 0;
	m_nEventTestCount = 0;
	m_nCurveTestCount = 0;
	m_nAlarmTestCount = 0;
	SetTimer(1, 500, NULL);

	//自动测试开始前对装置进行系统时间对时    ADD BY WM 20131206
	ptcl_packet_dlt packetDlt;
	CChannel *pMeterChannel = CChannelMgr::getInstance()->getChannel(m_virtualDeviceId);

	CTime now(time(NULL));
	packetDlt.set_meter_addr(0x999999999999);
	ptcl_module_dlt::get_instance()->format_packet_set_time(&packetDlt, now.GetTime());
	ShowOperateLog("自动测试开始!");// ADD BY WM 20140108 
	ShowOperateLog("虚拟表装置对时到系统时间");
	if (packetDlt.check_packet() > 0)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
}

void CDlgAutoTest::OnBnClickedBtnStop()
{
	KillTimer(1);
	m_vTask.clear();
	m_pAutoTask = NULL;
	s_bWait = false;
	
	for (UINT i = 0; i < m_vChannelId.size(); ++i)
	{
		CChannel *pRtuChannel = CChannelMgr::getInstance()->getChannel(m_vChannelId.at(i).nId);
		if (pRtuChannel)
		{
			unsigned __int64 id = ptcl_packet_csg::convert_device_addr(m_termAddr.GetBuffer(0));
			pRtuChannel->Disonnect(id);
		}
	}
	CChannel *pMeterChannel = CChannelMgr::getInstance()->getChannel(m_virtualDeviceId);
	if (pMeterChannel)
	{
		pMeterChannel->Disonnect(m_virtualDeviceId);
	}

	GetDlgItem(IDC_BTN_TEST)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	ShowOperateLog("自动测试结束!");// ADD BY WM 20140108 
	
}

void CDlgAutoTest::OnBnClickedBtnReport()
{
	CReportDlg dlg(m_termId, m_batchId, GetDesktopWindow());
	dlg.DoModal();
}

void CDlgAutoTest::OnBnClickedButtonResult()
{	
	CString strResult = _T("");
	m_comboResult.GetWindowText(strResult);
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[1024] = {0};	

	BOOL flag = TRUE;
	if (strResult.Find("不") >= 0)
		flag = FALSE;

	sprintf(strSQL,"INSERT INTO AUTOTEST_TOTAL_RESULT(BANTCH_ID, TERM_ID, RESULT) "
					"VALUES(%d, %d, %d)", m_batchId, m_termId, flag);
					
	if(pConnect->ExecSQL(strSQL) == FALSE)
	{
		memset(strSQL, 0, 1024);
		sprintf(strSQL,"UPDATE AUTOTEST_TOTAL_RESULT SET RESULT = %d WHERE BANTCH_ID=%d AND TERM_ID=%d ",
			flag, m_batchId, m_termId);
		pConnect->ExecSQL(strSQL);
	}
}

void CDlgAutoTest::InitTask()
{
	m_vChannelId.clear();
	for (int i = 0; i < m_ctrlChannelList.GetItemCount(); i++)
	{
		if (!m_ctrlChannelList.GetCheck(i))
			continue;
		CString strId = m_ctrlChannelList.GetItemText(i, 1);
		int nChannelId = atoi(strId);
		ChannelParam param;
		param.nId = nChannelId;
		param.sChannelDesc = m_ctrlChannelList.GetItemText(i, 2);
		m_vChannelId.push_back(param);
	}

	m_vTaskId.clear();
	for (int i = 0; i < m_ctrlTaskList.GetItemCount(); i++)
	{
		if (!m_ctrlTaskList.GetCheck(i))
			continue;
		CString strId = m_ctrlTaskList.GetItemText(i, 1);
		int nTaskId = atoi(strId);
		TaskParam param;
		param.nId = nTaskId;
		param.sTaskDesc = m_ctrlTaskList.GetItemText(i, 2);
		m_vTaskId.push_back(param);
	}

	m_vTask.clear();
	for (int i=0;i<m_vChannelId.size();i++)
	{
		for (int n=0;n<m_vTaskId.size();n++)
		{
			InsertChildTask(m_vChannelId.at(i).nId, m_vTaskId.at(n).nId, m_termId, m_termAddr);
		}
	}
}

void CDlgAutoTest::InsertChildTask(int ChannelId, int TaskId, int TermId, CString TermAddr)
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};
	sprintf(strSQL,"SELECT * FROM AUTOTEST_READING_ARCH WHERE READING_ID = %d ORDER BY PARAM_TYPE, PARAM_ID", TaskId);
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		unsigned int StartTime;
		unsigned int EndTime;
		CString strParamId;
		CString strDesc;
		int ParamType;
		int Densy;

		record.GetFieldValue("CRS_S_TIME",StartTime);
		record.GetFieldValue("CRS_E_TIME",EndTime);
		record.GetFieldValue("PARAM_ID",val);
		strParamId = val.bstrVal;
		record.GetFieldValue("PARAM_TYPE",ParamType);
		record.GetFieldValue("DATA_DENSY",Densy);
		record.GetFieldValue("CRS_DATA_TYPE",val);
		strDesc = val.bstrVal;
		//m_mTaskDesc[strParamId] = strDesc; 
		m_mTaskDesc[strDesc] = strParamId;  //MODIFY BY WM 20140114

		std::string sDataItem = strParamId.GetBuffer(0);
		std::string sStrDataItem = strDesc.GetBuffer(0);//MODIFY BY WM 20140114
		std::string sTermAddr = TermAddr.GetBuffer(0);

		if ( strParamId.Compare("E0000130") == 0 || strParamId.Compare("E2010014") == 0 )
		{
			AutoTask task(ChannelId, TaskId, ParamType, 0, 0, TermAddr.GetBuffer(0), "", strParamId.GetBuffer(0),strDesc.GetBuffer(0),StartTime, EndTime);
			m_vTask.push_back(task);
		}
		else
		{
			InsertMeterTask(ChannelId, TaskId, TermId, sTermAddr, ParamType, StartTime, EndTime, sDataItem, sStrDataItem, Densy);
		}

		record.RecordMoveNext();
	}
}

void CDlgAutoTest::InsertMeterTask(int ChannelId, int TaskId,int TermId, std::string sTermAddr, int ParamType,
									unsigned int StartTime, unsigned int EndTime, std::string sDataItem, std::string sStrDataItem,int Densy)
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;

	char strSQL[300] = {0};
	sprintf(strSQL,"SELECT * FROM MP_ARCH WHERE RTU_ID = %d ORDER BY MP_ID", TermId);
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	bool bIsFirst = true;
	while (!record.IsEndEOF())
	{
		_variant_t val;
		int Pn;
		unsigned int start = StartTime;
		record.GetFieldValue("MP_ID", Pn);

		CString strMeterAddr = "";
		record.GetFieldValue("MP_ADDR", val);
		strMeterAddr = val.bstrVal;

		int nProtocol, nPort, nBaud, nCheck, nDatabit, nStopbit;
		record.GetFieldValue("MP_PROTOCOL", nProtocol);
		record.GetFieldValue("MP_PORT", nPort);
		record.GetFieldValue("MP_BAUD", nBaud);
		record.GetFieldValue("MP_CHECKBIT", nCheck);
		record.GetFieldValue("MP_DATABIT", nDatabit);
		record.GetFieldValue("MP_STOPBIT", nStopbit);		
		nPort = (nPort > 0) ? (nPort - 1) : 0;
		nBaud = pow(2.0, nBaud);	// temp change
		nDatabit = nDatabit + 5;		

		switch (ParamType)
		{
		case 2:	// 曲线数据
			{
				int Densy = 3;
				AutoTask task(ChannelId, TaskId, ParamType, Pn, Densy, sTermAddr, strMeterAddr.GetBuffer(0), sDataItem,sStrDataItem, start, start, bIsFirst); //MODIFY BY WM 20140114
				m_vTask.push_back(task);					
				break;
			}
		case 3:	// 日冻结
			{
				int Densy = 6;
				CTime now(time(NULL));
				now = GetOnDay(now);
				AutoTask task(ChannelId, TaskId, ParamType, Pn, Densy, sTermAddr, strMeterAddr.GetBuffer(0), sDataItem, sStrDataItem,now.GetTime(), now.GetTime(), bIsFirst);//MODIFY BY WM 20140114
				m_vTask.push_back(task);				
				break;
			}
		case 4:	// 月冻结
			{
				int Densy = 7;
				CTime now(time(NULL));
				now = CTime(now.GetYear(), now.GetMonth(), 1, 0, 0, 0);
				AutoTask task(ChannelId, TaskId, ParamType, Pn, Densy, sTermAddr, strMeterAddr.GetBuffer(0), sDataItem, sStrDataItem, now.GetTime(), now.GetTime(), bIsFirst);//MODIFY BY WM 20140114
				m_vTask.push_back(task);
				break;
			}
		case 5:	// 表计事件数据
			{
				AutoTask task(ChannelId, TaskId, ParamType, Pn, Densy, sTermAddr, strMeterAddr.GetBuffer(0), sDataItem, sStrDataItem, time(NULL), time(NULL) + 24*3600, bIsFirst);//MODIFY BY WM 20140114
				if ( (sDataItem.compare("E201000F") == 0 || sDataItem.compare("E2010012") == 0) && nProtocol == 0 )
				{
					// 97规约电表不支持电量清零和开表盖事件, 默认合格
					task.status = 2;
				}
				m_vTask.push_back(task);
				break;
			}
		case 6:	// 参数数据
		case 1:	// 当前数据
			{
				AutoTask task(ChannelId, TaskId, ParamType, Pn, Densy, sTermAddr, strMeterAddr.GetBuffer(0), sDataItem, sStrDataItem, start, start, bIsFirst);//MODIFY BY WM 20140114
				task.SetMeterInfo(nProtocol, nPort, nBaud, nCheck, nDatabit, nStopbit);
				m_vTask.push_back(task);
				break;
			}
		case 8:	// 告警数据
			{
				AutoTask task(ChannelId, TaskId, ParamType, Pn, Densy, sTermAddr, strMeterAddr.GetBuffer(0), sDataItem, sStrDataItem, time(NULL), time(NULL) + 24*3600, bIsFirst);//MODIFY BY WM 20140114
				m_vTask.push_back(task);
			}	
			break;
		}

		// 数据透传测试是针对终端的功能，只要对某一个计量点进行透传测试就行。
		if (sDataItem.compare("E3020001") == 0)
			return;

		bIsFirst = false;
		record.RecordMoveNext();
	}
}

void CDlgAutoTest::WriteResultDb(int batch_id , int term_id, CString strPn, CString strChannelDesc, 
								CString strTaskDesc,CString strVirtualValue, CString strValue,
								CString strDataItemType, CString strDataItem, CString strResult, CString strTime)
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	int Pn = atoi(strPn);
	CTime now(time(NULL));
	CString strNow = now.Format("%Y-%m-%d %H:%M:%S");

	char strSQL[1024] = {0};	
	
	VARIANT_BOOL flag = -1;
	if (strResult.Find("不") >= 0)
		flag = 0;

	sprintf(strSQL,"INSERT INTO RD_AUTOTEST_RT(BANTCH_ID, SID, MP_ID, CHANNEL_NAME, TASK_NAME,VIRTUAL_VALUE, SVALUE, PARAM_TYPE, PARAM_ID, SRESULT, STIME) "
					"VALUES(%d, %d, %d, '%s', '%s', '%s','%s','%s','%s',%d,'%s')", batch_id, term_id, Pn,strChannelDesc, strTaskDesc, 
					strVirtualValue, strValue, strDataItemType ,strDataItem, flag, strTime);

	if(pConnect->ExecSQL(strSQL) == FALSE)
	{
		memset(strSQL, 0, 1024);
		sprintf(strSQL,"UPDATE RD_AUTOTEST_RT SET TASK_NAME='%s', VIRTUAL_VALUE='%s',SVALUE='%s',PARAM_TYPE='%s',SRESULT=%d,STIME='%s'\
				WHERE BANTCH_ID=%d AND SID=%d AND MP_ID=%d AND CHANNEL_NAME='%s' AND PARAM_ID='%s' "
				, strTaskDesc, strVirtualValue, strValue, strDataItemType , flag, strTime, batch_id, term_id, Pn,strChannelDesc,strDataItem);
		pConnect->ExecSQL(strSQL);
	}
}

CTime CDlgAutoTest::GetOnHour(const CTime tmSrc)
{
	return CTime(tmSrc.GetYear(),tmSrc.GetMonth(),tmSrc.GetDay(),tmSrc.GetHour(),0,0);
}

CTime CDlgAutoTest::GetOnDay(const CTime tmSrc)
{
	return CTime(tmSrc.GetYear(),tmSrc.GetMonth(),tmSrc.GetDay(),0,0,0);
}

CTime CDlgAutoTest::GetNextMonth(const CTime tmSrc)  
{  
    int newYear = tmSrc.GetMonth()==12 ? tmSrc.GetYear()+1 : tmSrc.GetYear();  
    int newMonth = tmSrc.GetMonth()==12 ? 1 : tmSrc.GetMonth()+1;  
       
    return CTime(newYear, newMonth, 1,0,0,0);  
}  

CTime CDlgAutoTest::GetLastMonth(const CTime tmSrc)  
{  
    int newYear = tmSrc.GetMonth()==1 ? tmSrc.GetYear()-1 : tmSrc.GetYear();  
    int newMonth = tmSrc.GetMonth()==1 ? 12 : tmSrc.GetMonth()-1;   
     
    return CTime(newYear, newMonth, 1,0,0,0);  
}  

CTime CDlgAutoTest::GetOnMonth(const CTime tmSrc)  
{
	return CTime(tmSrc.GetYear(), tmSrc.GetMonth(), 1,0,0,0);  
}

CString CDlgAutoTest::GetChildTaskDesc(int type)
{
	CString strDesc;
	switch (type)
	{
	case 1:
		strDesc = "当前数据";
		break;
	case 2:
		strDesc = "曲线数据";
		break;
	case 3:
		strDesc = "日冻结数据";
		break;
	case 4:
		strDesc = "月冻结数据";
		break;
	case 5:
		strDesc = "事件数据";
		break;
	case 6:
		strDesc = "其它";
		break;
	case 7:
		strDesc = "终端参数";
		break;
	case 8:
		strDesc = "告警数据";
		break;
	case 9:
		strDesc = "终端告警";
		break;
	default:
		break;
	}
	return strDesc;
}

void CDlgAutoTest::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:		
		if (s_bWait)
		{
			if (time(NULL) - m_taskTime > m_timeout)
			{
				s_bWait = false;
				m_taskTime = 0;
				m_pAutoTask->status = 3;
			}
		}
		else
		{
			if (m_pAutoTask - 1 == &m_vTask[m_vTask.size() - 1])
			{
				OnBnClickedBtnStop();
			}
			else
			{
				if (time(NULL) - m_taskTime > m_pAutoTask->waittime)
				{
					switch (m_pAutoTask->status)
					{
					case 0:
						s_bWait = ExcuteTask(m_pAutoTask);
						m_taskTime = s_bWait ? time(NULL) : 0;
						break;
					case 1:		// 任务完成
						m_pAutoTask++;
						break;
					case 2:		// 默认合格
						ProcessSucceeded(m_pAutoTask);
						m_pAutoTask++;
						break;
					case 3:		// 响应超时
					case 4:		// 未实现
					case 5:		// 连接终端不成功
					case 6:		// 连接虚拟表不成功
						ProcessFailed(m_pAutoTask);	
						m_pAutoTask++;
						break;
					default:
						m_pAutoTask++;
						break;
					}
				}
			}
		}
		break;
	}

	__super::OnTimer(nIDEvent);
}

bool CDlgAutoTest::ExcuteTask(AutoTask *pTask)
{
	CChannel *pRtuChannel = CChannelMgr::getInstance()->getChannel(pTask->channelId);
	u_int64 nTermAddr = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	if (pRtuChannel == NULL || pRtuChannel->ConnectTo(nTermAddr) != 0)
	{
		pTask->status = 5;
		return false;
	}

	CChannel *pMeterChannel = CChannelMgr::getInstance()->getChannel(m_virtualDeviceId);
	if (pMeterChannel == NULL || pMeterChannel->ConnectTo(m_virtualDeviceId) != 0)
	{
		pTask->status = 6;
		return false;
	}

	bool bWait = false;
	switch (pTask->type)
	{
	case 1:
		bWait = ExcuteTask_ReadCurrent(pTask, pRtuChannel, pMeterChannel);
		break;
	case 2:
		bWait = ExcuteTask_ReadHistory_Curve(pTask, pRtuChannel, pMeterChannel);		
		break;
	case 3:
		bWait = ExcuteTask_ReadHistory_Day(pTask, pRtuChannel, pMeterChannel);
		break;
	case 4:
		bWait = ExcuteTask_ReadHistory_Month(pTask, pRtuChannel, pMeterChannel);
		break;
	case 5:
		bWait = ExcuteTask_ReadEvent(pTask, pRtuChannel, pMeterChannel);
		break;
	case 6:
		bWait = ExcuteTask_Other(pTask, pRtuChannel, pMeterChannel);
		break;
	case 8:
		bWait = ExcuteTask_ReadAlarm(pTask, pRtuChannel, pMeterChannel);
		break;
	default:
		pTask->status = 4;
		break;
	}
	return bWait;
}

bool CDlgAutoTest::ExcuteTask_ReadEvent(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	bool bWait = false;

	UINT32 dataitem = 0;
	sscanf_s(pTask->dataitem.c_str(), "%X", &dataitem);
	switch (dataitem)
	{
	case 0xE2010004:	// A相失压记录
		bWait = ExcuteTask_E2010004(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE2010005:
		bWait = ExcuteTask_E2010005(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE2010006:
		bWait = ExcuteTask_E2010006(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE2010007:	// A相失流记录
		bWait = ExcuteTask_E2010007(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE2010008:
		bWait = ExcuteTask_E2010008(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE2010009:
		bWait = ExcuteTask_E2010009(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE201000B:	// A相断相记录
		bWait = ExcuteTask_E201000B(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE201000C:
		bWait = ExcuteTask_E201000C(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE201000D:
		bWait = ExcuteTask_E201000D(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE2010001:	// 需量手动清零、编程、开表盖、电量清零
	case 0xE2010003:		
	case 0xE201000F:
	case 0xE2010012:
		bWait = ExcuteTask_E2010001(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE2010014:
		bWait = ExcuteTask_E2010014(pTask, pRtuChannel, pMeterChannel);
		break;
	default:
		pTask->status = 4;
		break;
	}

	return bWait;
}

bool CDlgAutoTest::ExcuteTask_ReadHistory_Day(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_dlt packetDlt;
	ptcl_packet_csg packetCsg;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packetCsg.set_device_addr(id);

	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (++m_nDayTestCount > 1)  //此步骤只执行一次
				return false;

			//读取终端当前时间
			ptcl_module_csg::pnfn_data pnfn;
			pnfn.pn = 0;
			pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			ptcl_module_csg::get_instance()->format_packet_read_parameter(&packetCsg, &pnfn, 1);
			ShowOperateLog("日冻结数据测试任务开始！");
			ShowOperateLog("读取终端当前时间");
			pTask->bdataitemE0000130 = true; //MODIFY BY WM 20131116
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (m_nDayTestCount > 1)
				return false;
			
			CTime now(pTask->terminaltime);
			CTime temp(now.GetYear(), now.GetMonth(), now.GetDay(), 23, 55, 0);
			pTask->starttime = temp.GetTime();
			pTask->endtime = temp.GetTime() + 5 * 60;

			// 设置后续任务（日冻结数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->type != pTask->type)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
			pNextTask--;
			pNextTask->starttime = 123456;	// 恢复终端及设备时钟的判定标志

			// 终端校时至23:55
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(temp.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(temp.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(temp.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(temp.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(temp.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(temp.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packetCsg, (unsigned char *)"", &dataunit, 1);	
			ShowOperateLog("终端对时到终端时间当日23：55分");

			// 虚拟表装置校时至23:55
			packetDlt.set_meter_addr(0x999999999999);
			ptcl_module_dlt::get_instance()->format_packet_set_time(&packetDlt, temp.GetTime());
			ShowOperateLog("虚拟表装置对时到终端时间当日23：55分");

			pTask->waittime = 30;
		}
		break;
	case 3:
		{
			if (m_nDayTestCount > 1)
				return false;

			// 下发停止走字命令给虚拟表装置
			packetDlt.set_meter_addr(0xAAAAAAAAAAAA);
			ptcl_module_dlt::get_instance()->format_packet_start_clock_communicate(&packetDlt, 0xE852, 0x0F, 10);
			ShowOperateLog("向虚拟表装置下发日冻结测试任务命令0xE852！10分钟后抄读数据");
			pTask->waittime = 10 * 60;
		}
		break;
	case 4:
		{
			pTask->waittime = 0;
			// 抄读虚拟表装置中的日冻结数据
			map<string, string>::iterator it = m_mapSg2Dlt.find(pTask->dataitem);
			if (it != m_mapSg2Dlt.end())
			{
				unsigned int dateitem = ptcl_packet_dlt::convert_fn(it->second);
				UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
				packetDlt.set_meter_addr(addr);
				ptcl_module_dlt::get_instance()->format_packet_read_data(&packetDlt, dateitem);
			}
			else
			{
				pTask->status = 4;
				return false;
			}
		}
		break;
	case 5:
		{
			// 召测终端日冻结数据
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_history(&packetCsg, pnfn_data, 1, pTask->endtime, pTask->endtime, pTask->desity);
			pTask->waittime = 0;
		}
		break;
	/*case 6:  MODFY BY WM 20131115
		{
			if (pTask->starttime != 123456)
				return false;
			
			// 终端校时至正常时钟
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packetCsg, (unsigned char *)"", &dataunit, 1);

			// 虚拟表装置校时至正常时钟
			packetDlt.set_meter_addr(0x999999999999);
			ptcl_module_dlt::get_instance()->format_packet_set_time(&packetDlt, time(NULL));

			pTask->waittime = 30;
		}
		break;*/
	case 6:
		pTask->status = 1;
		return false;
		break;
	}

	if (packetDlt.check_packet() > 0)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}

	if (packetCsg.parse_packet() > 0)
	{
		pRtuChannel->Write((char *)packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetCsg.get_packet_buffer(), packetCsg.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_ReadHistory_Month(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	bool bWait = false;

	UINT32 dataitem = 0;
	sscanf_s(pTask->dataitem.c_str(), "%X", &dataitem);
	switch (dataitem)
	{
	// 最大需量及发生时间
	case 0x01010001:	
	case 0x01010101:
	case 0x01010201:
	case 0x01010301:
	case 0x01010401:
	case 0x01020001:
	case 0x01020101:
	case 0x01020201:
	case 0x01020301:
	case 0x01020401:
	case 0x01030001:
	case 0x01030101:
	case 0x01030201:
	case 0x01030301:
	case 0x01030401:
	case 0x01040001:
	case 0x01040101:
	case 0x01040201:
	case 0x01040301:
	case 0x01040401:
		bWait = ExcuteTask_01010001(pTask, pRtuChannel, pMeterChannel, dataitem);
		break;
	default:
		bWait = ExcuteTask_00010001(pTask, pRtuChannel, pMeterChannel);
		break;
	}

	return bWait;
}

bool CDlgAutoTest::ExcuteTask_ReadAlarm(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	bool bWait = false;

	UINT32 dataitem = 0;
	sscanf_s(pTask->dataitem.c_str(), "%X", &dataitem);
	switch (dataitem)
	{	
	case 0xE200003E:
	case 0xE200003F:
		bWait = ExcuteTask_E200003E(pTask, pRtuChannel, pMeterChannel);
		break;
	case 0xE200002F:
		bWait = ExcuteTask_E200002F(pTask, pRtuChannel, pMeterChannel);
		break;
	default:
		pTask->status = 4;
		break;
	}

	return bWait;
}

bool CDlgAutoTest::ExcuteTask_Other(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	bool bWait = false;

	UINT32 dataitem = 0;
	sscanf_s(pTask->dataitem.c_str(), "%X", &dataitem);
	switch (dataitem)
	{	
	case 0xE0000130:
		bWait = ExcuteTask_E0000130(pTask, pRtuChannel, pMeterChannel);
		break;	
	case 0xE3020001:
		bWait = ExcuteTask_E3020001(pTask, pRtuChannel, pMeterChannel);
		break;	
	default:
		pTask->status = 4;
		break;
	}

	return bWait;
}

bool CDlgAutoTest::ExcuteTask_ReadCurrent(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	bool bWait = false;

	UINT32 dataitem = 0;
	sscanf_s(pTask->dataitem.c_str(), "%X", &dataitem);
	switch (dataitem)
	{
	case 0x04000101://电能表日期
	case 0x04000102://电能表时间
		bWait = ExcuteTask_04000101(pTask, pRtuChannel, pMeterChannel);		
		break;
	default:
		bWait = ExcuteTask_00010000(pTask, pRtuChannel, pMeterChannel);
		break;
	}

	return bWait;
}

bool CDlgAutoTest::ExcuteTask_ReadHistory_Curve(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	bool bWait = false;

	UINT32 dataitem = 0;
	sscanf_s(pTask->dataitem.c_str(), "%X", &dataitem);
	switch (dataitem)
	{
	case 0x01010000:
	case 0x01010100:
	case 0x01010200:
	case 0x01010300:
	case 0x01010400:
	case 0x01020000:
	case 0x01020100:
	case 0x01020200:
	case 0x01020300:
	case 0x01020400:
		// 曲线数据自动测试无需量
		//bWait = ExcuteTask_RHC_FLOW2(pTask, pRtuChannel, pMeterChannel, dataitem);
		pTask->status = 4;
		break;
	default:
		bWait = ExcuteTask_RHC_FLOW1(pTask, pRtuChannel, pMeterChannel);
		break;
	}

	return bWait;
}

bool CDlgAutoTest::ExcuteTask_E2010004(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("A相失压测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 160, 220, 220, 1.51, 1.52, 1.53, 0x0F, 10);
			ShowOperateLog("下发A相失压事件模拟参数给虚拟表装置，设定A相电压：160V，持续时间10分钟！");
			pTask->starttime = time(NULL);
			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 190, 220, 220, 1.51, 1.52, 1.53, 0x0F, 5);		
			ShowOperateLog("下发A相失压恢复参数给虚拟表装置，设定A相电压：190V，持续时间10分钟！");
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 10 * 60;


			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测A相失压事件！");
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E2010005(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("B相失压测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 160, 220, 1.51, 1.52, 1.53, 0x0F, 10);
			ShowOperateLog("下发B相失压事件模拟参数给虚拟表装置，设定B相电压：160V，持续时间10分钟！"); // MODIFY BY WM 20131220
			pTask->starttime = time(NULL);
			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 190, 220, 1.51, 1.52, 1.53, 0x0F, 5);	
			ShowOperateLog("下发B相失压恢复参数给虚拟表装置，设定B相电压：190V，持续时间10分钟！");
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 10 * 60;

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测B相失压事件！");
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E2010006(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("C相失压测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 160, 1.51, 1.52, 1.53, 0x0F, 10);
			ShowOperateLog("下发C相失压事件模拟参数给虚拟表装置，设定C相电压：160V，持续时间10分钟！");
			pTask->starttime = time(NULL);
			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 190, 1.51, 1.52, 1.53, 0x0F, 5);			
			ShowOperateLog("下发C相失压恢复参数给虚拟表装置，设定C相电压：190V，持续时间10分钟！");
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 10 * 60;

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测C相失压事件！");
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E2010007(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("A相失流测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 220, 0.02, 1.52, 1.53, 0x0F, 10);
			ShowOperateLog("下发A相失流事件模拟参数给虚拟表装置，设定A相电流：0.02A，持续时间10分钟！");
			pTask->starttime = time(NULL);
			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 220, 1.51, 1.52, 1.53, 0x0F, 5);
			ShowOperateLog("下发A相失流恢复参数给虚拟表装置，设定A相电流：1.51A，持续时间10分钟！");
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 10 * 60;

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测A相失流事件！");
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E2010008(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("B相失流测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 220, 1.51, 0.02, 1.53, 0x0F, 10);
			ShowOperateLog("下发B相失流事件模拟参数给虚拟表装置，设定B相电流：0.02A，持续时间10分钟！");
			pTask->starttime = time(NULL);
			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 220, 1.51, 1.52, 1.53, 0x0F, 5);
			ShowOperateLog("下发B相失流恢复参数给虚拟表装置，设定B相电流：1.52A，持续时间10分钟！");
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 10 * 60;

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测B相失流事件！");
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E2010009(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("C相失流测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 220, 1.51, 1.52, 0.02, 0x0F, 10);
			ShowOperateLog("下发C相失流事件模拟参数给虚拟表装置，设定C相电流：0.02A，持续时间10分钟！");
			pTask->starttime = time(NULL);
			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 220, 1.51, 1.52, 1.53, 0x0F, 5);	
			ShowOperateLog("下发C相失流恢复参数给虚拟表装置，设定C相电流：1.53A，持续时间10分钟！");
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 10 * 60;

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测C相失流事件！");
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E201000B(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("A相断相测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 160, 220, 220, 0.02, 1.52, 1.53, 0x0F, 10);	
			ShowOperateLog("下发A相断相事件模拟参数给虚拟表装置，设定A相电压：160V,A相电流：0.02A，持续时间10分钟！");
			pTask->starttime = time(NULL);
			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 190, 220, 220, 1.51, 1.52, 1.53, 0x0F, 5);	
			ShowOperateLog("下发A相断相恢复参数给虚拟表装置，设定A相电压：190V,A相电流：1.51A，持续时间10分钟！");
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 10 * 60;

			
			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测A相断相事件！");
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E201000C(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("B相断相测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 160, 220, 1.51, 0.02, 1.53, 0x0F, 10);	
			ShowOperateLog("下发B相断相事件模拟参数给虚拟表装置，设定B相电压：160V,B相电流：0.02A，持续时间10分钟！");
			pTask->starttime = time(NULL);
			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 190, 220, 1.51, 1.52, 1.53, 0x0F, 5);	
			ShowOperateLog("下发B相断相恢复参数给虚拟表装置，设定B相电压：190V,B相电流：1.52A，持续时间10分钟！");
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 10 * 60;

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测B相断相事件！");
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E201000D(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("C相断相测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 160, 1.51, 1.52, 0.02, 0x0F, 10);	
			ShowOperateLog("下发C相断相事件模拟参数给虚拟表装置，设定C相电压：160V,C相电流：0.02A，持续时间10分钟！");
			pTask->starttime = time(NULL);
			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_I_U_data(&packetDlt, 0xB61F, 220, 220, 190, 1.51, 1.52, 1.53, 0x0F, 5);	
			ShowOperateLog("下发C相断相恢复参数给虚拟表装置，设定C相电压：190V,C相电流：1.53A，持续时间10分钟！");
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 10 * 60;

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测C相断相事件！");
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E2010001(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);	

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (++m_nEventTestCount > 1)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("编程记录/最大需量手动复零/电量清零/表盖开启测试任务开始！97规约电表不支持电量清零与表盖开启默认为“合格”");
			ShowOperateLog("终端校时到系统时间！等待2个抄表周期,即6分钟!");
			pTask->waittime = 6 * 60; //MODIFY BY WM 20131205 等待2个抄表周期
		}
		break;
	case 2:
		{
			if (m_nEventTestCount > 1)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_write_other_parameter(&packetDlt, 0xE81F, 1, 2, 3, 4, 0x0F);
			ShowOperateLog("下发编程记录/最大需量手动复零/电量清零/表盖开启参数给虚拟表装置，6分钟后开始召测事件！");
			pTask->starttime = time(NULL) - 1 * 60;
			pTask->endtime = time(NULL) + 6 * 60;
			pTask->waittime = 6 * 60;

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if ( pNextTask->dataitem.compare("E2010001") == 0 
					 || pNextTask->dataitem.compare("E2010003") == 0 
					 || pNextTask->dataitem.compare("E201000F") == 0 
					 || pNextTask->dataitem.compare("E2010012") == 0 )
				{					
					pNextTask->starttime = pTask->starttime;
					pNextTask->endtime = pTask->endtime;
				}
				pNextTask++;
			}
		}
		break;
	case 3:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测编程记录/最大需量手动复零/电量清零/表盖开启事件！");
			pTask->waittime = 0;	
		}
		break;
	case 4:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_00010001(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_dlt packetDlt;
	ptcl_packet_csg packetCsg;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packetCsg.set_device_addr(id);	

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
		case 1:
		{
			if (++m_nMonthTestCount > 1)  //此步骤只执行一次
				return false;

			//读取终端当前时间
			ptcl_module_csg::pnfn_data pnfn;
			pnfn.pn = 0;
			pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			ptcl_module_csg::get_instance()->format_packet_read_parameter(&packetCsg, &pnfn, 1);
			ShowOperateLog("月冻结数据测试任务开始！");
			ShowOperateLog("读取终端当前时间");
			pTask->bdataitemE0000130 = true;
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (m_nMonthTestCount > 1)  
				return false;
			CTime now(pTask->terminaltime);
			time_t dt;
			if(now.GetMonth() == 12)
			{
				CTime temp1(now.GetYear()+1, 1, 1, 23, 55, 0);
				dt = temp1.GetTime() - 24 * 60 * 60;
			}
			else
			{
				CTime temp1(now.GetYear(), now.GetMonth()+1, 1, 23, 55, 0);
				dt = temp1.GetTime() - 24 * 60 * 60;
			}
			 
			//time_t dt = temp1.GetTime() - 24 * 60 * 60;
			CTime temp(dt);
			pTask->starttime = temp.GetTime();
			pTask->endtime = temp.GetTime() + 5 * 60;

			// 设置后续任务（月冻结数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->type != pTask->type)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
			pNextTask--;
			pNextTask->starttime = 123456;	// 恢复终端及设备时钟的判定标志

			// 终端校时至月末23:55
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(temp.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(temp.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(temp.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(temp.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(temp.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(temp.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packetCsg, (unsigned char *)"", &dataunit, 1);	
			ShowOperateLog("终端校时至月末23:55");

			// 虚拟表装置校时至月末23:55
			packetDlt.set_meter_addr(0x999999999999);
			ptcl_module_dlt::get_instance()->format_packet_set_time(&packetDlt, temp.GetTime());
			ShowOperateLog("虚拟表装置校时至月末23:55,10分钟后开始召测数据。");

			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			bIsDltPacket = true;
			map<string, string>::iterator it = m_mapSg2Dlt.find(pTask->dataitem);
			if (it != m_mapSg2Dlt.end())
			{
				unsigned int dateitem = ptcl_packet_dlt::convert_fn(it->second);
				UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
				packetDlt.set_meter_addr(addr);
				ptcl_module_dlt::get_instance()->format_packet_read_data(&packetDlt, dateitem);
				pTask->waittime = 0;
			}
			else
			{
				return false;
			}
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);			
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_history(&packetCsg, pnfn_data, 1, pTask->endtime, pTask->endtime, pTask->desity);			
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (packetDlt.check_packet() > 0)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	if (packetCsg.parse_packet() > 0)
	{
		pRtuChannel->Write((char *)packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetCsg.get_packet_buffer(), packetCsg.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_01010001(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel, UINT32 nDataItem)
{
	string strItem1 = "", strItem2 = "";
	switch (nDataItem)
	{
	case 0x01010001:	// 最大需量及发生时间
	case 0x01010101:
	case 0x01010201:
	case 0x01010301:
	case 0x01010401:
		strItem1 = "A41F";
		strItem2 = "B41F";
		break;
	case 0x01020001:
	case 0x01020101:
	case 0x01020201:
	case 0x01020301:
	case 0x01020401:	
		strItem1 = "A42F";
		strItem2 = "B42F";
		break;
	case 0x01030001:
	case 0x01030101:
	case 0x01030201:
	case 0x01030301:
	case 0x01030401:	
		strItem1 = "A51F";
		strItem2 = "B51F";
		break;
	case 0x01040001:
	case 0x01040101:
	case 0x01040201:
	case 0x01040301:
	case 0x01040401:	
		strItem1 = "A52F";
		strItem2 = "B52F";
		break;
	}

	ptcl_packet_dlt packetDlt;
	ptcl_packet_csg packetCsg;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packetCsg.set_device_addr(id);
	
	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (++m_nMonthTestCount > 1)  //此步骤只执行一次
				return false;

			//读取终端当前时间
			ptcl_module_csg::pnfn_data pnfn;
			pnfn.pn = 0;
			pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			ptcl_module_csg::get_instance()->format_packet_read_parameter(&packetCsg, &pnfn, 1);
			ShowOperateLog("月冻结最大需量及发生时间自动测试开始！");
			ShowOperateLog("读取终端当前时间");
			pTask->bdataitemE0000130 = true;
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (m_nMonthTestCount > 1)  
				return false;

			CTime now(pTask->terminaltime);
			CTime temp1(now.GetYear(), now.GetMonth()+1, 1, 23, 55, 0);
			time_t dt = temp1.GetTime() - 24 * 60 * 60;
			CTime temp(dt);
			pTask->starttime = temp.GetTime();
			pTask->endtime = temp.GetTime() + 5 * 60;

			// 设置后续任务（月冻结数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->type != pTask->type)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
			pNextTask--;
			pNextTask->starttime = 123456;	// 恢复终端及设备时钟的判定标志

			// 终端校时至月末23:55
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(temp.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(temp.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(temp.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(temp.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(temp.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(temp.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packetCsg, (unsigned char *)"", &dataunit, 1);	
			ShowOperateLog("将终端对时到终端时间的本月未23：55分");

			// 虚拟表装置校时至月末23:55
			packetDlt.set_meter_addr(0x999999999999);
			ptcl_module_dlt::get_instance()->format_packet_set_time(&packetDlt, temp.GetTime());
			ShowOperateLog("将虚拟表装置对时到终端时间的本月未23：55分,10分钟后开始召测数据");

			pTask->waittime = 10 * 60;
		}
		break;
	case 3:
		{
			bIsDltPacket = true;			
			unsigned int dateitem = ptcl_packet_dlt::convert_fn(strItem1);
			UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
			packetDlt.set_meter_addr(addr);
			ptcl_module_dlt::get_instance()->format_packet_read_data(&packetDlt, dateitem);
			pTask->waittime = 0;
		}
		break;
	case 4:
		{
			bIsDltPacket = true;			
			unsigned int dateitem = ptcl_packet_dlt::convert_fn(strItem2);
			UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
			packetDlt.set_meter_addr(addr);
			ptcl_module_dlt::get_instance()->format_packet_read_data(&packetDlt, dateitem);
		}
		break;
	case 5:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_history(&packetCsg, pnfn_data, 1, pTask->endtime, pTask->endtime, pTask->desity);
		}
		break;
	case 6:
		pTask->status = 1;
		return false;
		break;
	}

	if (packetDlt.check_packet() > 0)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	if (packetCsg.parse_packet() > 0)
	{
		pRtuChannel->Write((char *)packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetCsg.get_packet_buffer(), packetCsg.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E200003E(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (++m_nAlarmTestCount > 1)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("电能表时钟异常/校时失败测试任务开始！");
            ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (m_nAlarmTestCount > 1)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_start_clock_communicate(&packetDlt, 0xE830, 0x0F, 10);
			ShowOperateLog("向虚拟表装置下发测试命令0xE830,15分钟后开始召测数据");//MODIFY BY WM 20131209 “10分钟后召测数据”改为“15分钟后召测数据”，以避免告警未恢复又重新开始测试导致终端不判断。
			pTask->starttime = time(NULL) - 60 ;
			pTask->endtime = time(NULL) + 10 * 60;
			pTask->waittime = 2 * 60;  //MODIFY BY WM 20160119

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare("E200003E") == 0 || pNextTask->dataitem.compare("E200003F") == 0)
				{				
					pNextTask->starttime = pTask->starttime;
					pNextTask->endtime = pTask->endtime;
				}
				pNextTask++;
			}
		}
		break;
	case 3://增加主站抄读电表时间；MODIFY BY WM 20160119
		{
			if (m_nAlarmTestCount > 1)
				return false;

			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn("04000102");
			ptcl_module_csg::get_instance()->format_packet_read_realtime_data(&packet, pnfn, 1);	
			ShowOperateLog("主站抄读电能表时钟！");
			pTask->waittime = 3 * 60;
		}
		break;
	case 4://增加主站下发对电表校时命令；MODIFY BY WM 20160119
		{
			if (m_nAlarmTestCount > 1)
				return false;

			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000131");
			dataunit.len_data = 4;
			dataunit.data_buffer[3] = 5;
			dataunit.data_buffer[2] = 0;
			dataunit.data_buffer[1] = 0xFF;
			dataunit.data_buffer[0] = 0xFF;
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("主站下发对电表校时命令！");
			pTask->waittime = 2 * 60; //MODIFY BY WM 20160204
		}
		break;
	case 5://增加主站抄读电表时间；MODIFY BY WM 20160204
		{
			if (m_nAlarmTestCount > 1)
				return false;

			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn("04000102");
			ptcl_module_csg::get_instance()->format_packet_read_realtime_data(&packet, pnfn, 1);	
			ShowOperateLog("主站抄读电能表时钟！");
			pTask->waittime = 8 * 60;
		}
		break;
	case 6:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_alarm_data(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测电能表时间异常/校时失败告警记录！");
			pTask->waittime = 0;
		}
		break;
	case 7:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E200002F(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);
	
	ptcl_packet_dlt packetDlt;
	packetDlt.set_meter_addr(0xAAAAAAAAAAAA);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = false;
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("电能表通讯失败测试任务开始！");
			ShowOperateLog("终端校时到系统时间！");
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			bIsDltPacket = true;
			ptcl_module_dlt::get_instance()->format_packet_start_clock_communicate(&packetDlt, 0xE820, 0x0F, 15);
			ShowOperateLog("向虚拟表装置下发测试命令0xE820,20分钟后开始召测数据");  //MODIFY BY WM 20131209 “15分钟后召测数据”改为“20分钟后召测数据”，以避免告警未恢复又重新开始测试导致终端不判断。
			pTask->starttime = time(NULL);
			pTask->endtime = time(NULL) + 15 * 60;
			pTask->waittime = 20 * 60;  //MODIFY BY WM 20131209 

			// 设置后续任务（同数据项，不同测量点）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->dataitem.compare(pTask->dataitem.c_str()) != 0)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
		}
		break;
	case 3:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];			
			data_format_csg::get_instance()->format_read_alarm_data(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测电能表通讯失败告警记录！");
			pTask->waittime = 0;
		}
		break;
	case 4:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E0000130(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);

	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{	
			//读取终端当前时间
			ptcl_module_csg::pnfn_data pnfn;
			pnfn.pn = 0;
			pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			ptcl_module_csg::get_instance()->format_packet_read_parameter(&packet, &pnfn, 1);
			ShowOperateLog("终端时间测试任务开始！");
			ShowOperateLog("读取终端时间！");
			pTask->bdataitemE0000130 = true;
			pTask->waittime = 0;
		}
		break;
	case 2:
		{
			CTime now(pTask->terminaltime);
			CTime temp(now.GetYear(),now.GetMonth(),now.GetDay(),now.GetHour(),now.GetMinute() + 2,now.GetSecond());
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(temp.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(temp.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(temp.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(temp.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(temp.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(temp.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("把终端时间往前对时2分钟！等待5分钟");
			pTask->starttime = temp.GetTime() + 5 * 60;
			pTask->waittime = 5 * 60;
		}
		break;
	case 3:
		{	
			ptcl_module_csg::pnfn_data pnfn;
			pnfn.pn = 0;
			pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			ptcl_module_csg::get_instance()->format_packet_read_parameter(&packet, &pnfn, 1);
			ShowOperateLog("召测终端当前时间！");
			pTask->waittime = 0;
		}
		break;
	case 4:
		pTask->status = 1;
		return false;
		break;
	}

	pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
	CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
	CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());

	return true;
}

bool CDlgAutoTest::ExcuteTask_00010000(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_dlt packetDlt;
	ptcl_packet_csg packetCsg;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packetCsg.set_device_addr(id);

	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (++m_nCurrentTestCount > 1)
				return false;
			
			packetDlt.set_meter_addr(0xAAAAAAAAAAAA);
			ptcl_module_dlt::get_instance()->format_packet_start_clock_communicate(&packetDlt, 0xE850, 0x0F, 15);//MODIFY BY WM 20131206 把装置停走时间从12分钟改为15分钟
			ShowOperateLog("当前数据测试任务开始！");
			ShowOperateLog("向虚拟表装置下发当前数据测试命令0xE850,5分钟后开始召测数据");
			pTask->waittime = 5 * 60;
			//pTask->waittime = 60;//需删除
		}
		break;
	case 2:
		{
			pTask->waittime = 0;
			map<string, string>::iterator it = m_mapSg2Dlt.find(pTask->dataitem);
			if (it != m_mapSg2Dlt.end())
			{
				unsigned int dateitem = ptcl_packet_dlt::convert_fn(it->second);
				UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
				packetDlt.set_meter_addr(addr);
				ptcl_module_dlt::get_instance()->format_packet_read_data(&packetDlt, dateitem);
			}
			else
			{
				pTask->status = 4;
				return false;
			}
		}
		break;
	case 3:
		{			
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::get_instance()->format_packet_read_realtime_data(&packetCsg, pnfn, 1);	
			pTask->waittime = 0;
		}
		break;
	case 4:
		pTask->status = 1;
		return false;
		break;
	}

	if (packetDlt.check_packet() > 0)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	if (packetCsg.parse_packet() > 0)
	{
		pRtuChannel->Write((char *)packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetCsg.get_packet_buffer(), packetCsg.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_04000101(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packetCsg;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packetCsg.set_device_addr(id);

	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::get_instance()->format_packet_read_realtime_data(&packetCsg, pnfn, 1);	
			pTask->waittime = 0;
		}
		break;
	case 2:
		pTask->status = 1;
		return false;
		break;
	}

	if (packetCsg.parse_packet() > 0)
	{
		pRtuChannel->Write((char *)packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetCsg.get_packet_buffer(), packetCsg.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_01010000(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel, UINT32 nDataItem)
{
	string strItem1 = "", strItem2 = "";
	switch (nDataItem)
	{
	case 0x01010000:
	case 0x01010100:
	case 0x01010200:
	case 0x01010300:
	case 0x01010400:
		strItem1 = "A01F";
		strItem2 = "B01F";
		break;
	case 0x01020000:
	case 0x01020100:
	case 0x01020200:
	case 0x01020300:
	case 0x01020400:
		strItem1 = "A02F";
		strItem2 = "B02F";
		break;
	}

	ptcl_packet_dlt packetDlt;
	ptcl_packet_csg packetCsg;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packetCsg.set_device_addr(id);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (++m_nCurrentTestCount > 1)
				return false;

			bIsDltPacket = true;			
			packetDlt.set_meter_addr(0xAAAAAAAAAAAA);
			ptcl_module_dlt::get_instance()->format_packet_start_clock_communicate(&packetDlt, 0xE850, 0x0F, 6);	
			pTask->waittime = 5 * 60;			
		}
		break;
	case 2:
		{
			bIsDltPacket = true;
			unsigned int dateitem = ptcl_packet_dlt::convert_fn(strItem1);
			UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
			packetDlt.set_meter_addr(addr);
			ptcl_module_dlt::get_instance()->format_packet_read_data(&packetDlt, dateitem);
			pTask->waittime = 0;
		}
		break;
	case 3:
		{
			bIsDltPacket = true;
			unsigned int dateitem = ptcl_packet_dlt::convert_fn(strItem2);
			UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
			packetDlt.set_meter_addr(addr);
			ptcl_module_dlt::get_instance()->format_packet_read_data(&packetDlt, dateitem);
			pTask->waittime = 0;
		}
		break;
	case 4:
		{
			bIsDltPacket = false;
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::get_instance()->format_packet_read_realtime_data(&packetCsg, pnfn, 1);	
			pTask->waittime = 0;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else
	{
		pRtuChannel->Write((char *)packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetCsg.get_packet_buffer(), packetCsg.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E2010014(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_csg packet;
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packet.set_device_addr(id);

	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E080000C");
			dataunit.len_data = 2;
			dataunit.data_buffer[0] = 0x10;
			dataunit.data_buffer[1] = 0;
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("终端参数变更测试任务开始！");
			ShowOperateLog("设置终端CT=10，等待2分钟！");
			//pTask->starttime = time(NULL);
			//pTask->endtime = pTask->starttime + 2 * 60;
			pTask->waittime = 2 * 60;
		}
		break;
	case 2:
		{
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E080000C");
			dataunit.len_data = 2;
			dataunit.data_buffer[0] = 1;
			dataunit.data_buffer[1] = 0;
			ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &dataunit, 1);
			ShowOperateLog("设置终端CT=1，等待2分钟！");
			pTask->starttime = time(NULL) - 60;
			pTask->endtime = pTask->starttime + 3 * 60;
			pTask->waittime = 2 * 60;
		}
		break;
	case 3:
		{	
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, pTask->starttime, pTask->endtime);
			ShowOperateLog("召测终端参数变更事件！");
			pTask->waittime = 0;
		}
		break;
	case 4:
		pTask->status = 1;
		return false;
		break;
	}

	pRtuChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
	CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
	CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());

	return true;
}

bool CDlgAutoTest::ExcuteTask_RHC_FLOW1(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_dlt packetDlt;
	ptcl_packet_csg packetCsg;
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packetCsg.set_device_addr(id);

	pTask->step++;
	switch (pTask->step)
	{
		case 1:
		{
			if (++m_nCurveTestCount > 1)
				return false;

			//读取终端当前时间
			ptcl_module_csg::pnfn_data pnfn;
			pnfn.pn = 0;
			pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			ptcl_module_csg::get_instance()->format_packet_read_parameter(&packetCsg, &pnfn, 1);
			pTask->bdataitemE0000130 = true;
			ShowOperateLog("曲线数据测试任务开始！");
			ShowOperateLog("读取终端当前时间");
			pTask->waittime = 0;
		}
		break;
		case 2:
			{
				if (m_nCurveTestCount > 1)
					return false;

				CTime now(pTask->terminaltime);
				CTime temp; //MODIFY BY WM 20140731  增加对年月日的判断,以防报错
				if(now.GetDay() == 31 && now.GetMonth() == 12 )
				{
					temp =CTime(now.GetYear()+1, 1, 1, 11, 54, 0);
				}
				else if(now.GetDay() == 31 && ((now.GetMonth()== 1) ||(now.GetMonth()== 3) ||(now.GetMonth()== 5) ||(now.GetMonth()== 7) || (now.GetMonth()== 8) ||(now.GetMonth()== 10)))
				{
					temp =CTime(now.GetYear(), now.GetMonth()+1, 1, 11, 54, 0);
				}
				else if(now.GetDay() == 30 && ((now.GetMonth()== 4) ||(now.GetMonth()== 6) ||(now.GetMonth()== 9) ||(now.GetMonth()== 11)))
				{
					temp =CTime(now.GetYear(), now.GetMonth()+1, 1, 11, 54, 0);
				}
				else if (now.GetDay() == 29 && now.GetMonth() == 2 && now.GetYear() % 4 == 0 && now.GetYear() % 100 != 0)
				{
					temp =CTime(now.GetYear(), 3, 1, 11, 54, 0);
				}
				else if(now.GetDay() == 28 && now.GetMonth() == 2 && now.GetYear() && !(now.GetYear() % 4 == 0 && now.GetYear() % 100 != 0))
				{
					temp =CTime(now.GetYear(), 3, 1, 11, 54, 0);
				}
				else
				{
					temp =CTime(now.GetYear(), now.GetMonth(), now.GetDay()+1, 11, 54, 0);
				}// END BY WM 20140731
				pTask->starttime = temp.GetTime();
				pTask->endtime = temp.GetTime() + 4 * 15 * 60;

				// 设置后续任务（曲线数据）的开始时间和结束时间
				AutoTask *pNextTask = pTask + 1;
				while (pNextTask <= &m_vTask[m_vTask.size() - 1])
				{
					if (pNextTask->type != pTask->type)
						break;
					pNextTask->starttime = pTask->starttime;
					pNextTask->endtime = pTask->endtime;
					pNextTask++;
				}
				pNextTask--;
				//pNextTask->starttime = 123456;		// 恢复终端及设备时钟的判定标志
				pNextTask->endtime = 123456;            // 恢复终端及设备时钟的判定标志


				// 终端校时至次日11:54
				ptcl_module_csg::pnfn_data_uint dataunit;
				dataunit.pnfn.pn = 0;
				dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
				dataunit.len_data = 6;
				dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(temp.GetYear() - 2000);
				dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(temp.GetMonth());
				dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(temp.GetDay());
				dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(temp.GetHour());
				dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(temp.GetMinute());
				dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(temp.GetSecond());
				ptcl_module_csg::get_instance()->format_packet_set_param(&packetCsg, (unsigned char *)"", &dataunit, 1);
				ShowOperateLog("终端校时至次日11:54");
			
				// 虚拟表装置校时至次日11:54
				packetDlt.set_meter_addr(0x999999999999);
				ptcl_module_dlt::get_instance()->format_packet_set_time(&packetDlt, temp.GetTime());
				ShowOperateLog("虚拟表装置校时至次日11:54");

				pTask->waittime = 30;
			}
			break;
		case 3:
			{
				if (m_nCurveTestCount > 1)
					return false;

				// 启动曲线测试任务命令
				packetDlt.set_meter_addr(0xAAAAAAAAAAAA);
				ptcl_module_dlt::get_instance()->format_packet_start_clock_communicate(&packetDlt, 0xE851, 0x0F, 4);		// 4为周期，1周期15分钟
				ShowOperateLog("下发曲线测试任务命令0xE851,60分钟后开始召测数据");
				pTask->waittime = 4 * 15 * 60; 
				//pTask->waittime = 60; //需删除
			}
			break;
		case 4:
			{
				pTask->waittime = 0;
				time_t datatime = pTask->starttime + 6 * 60 ;		// 12:00, 12:15, 12:30, 12:45
				map<string, string>::iterator it = m_mCurveSg2Dlt.find(pTask->dataitem);
				if (it != m_mCurveSg2Dlt.end())
				{
					unsigned int dateitem = ptcl_packet_dlt::convert_fn(it->second);
					UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
					packetDlt.set_meter_addr(addr);
					ptcl_module_dlt::get_instance()->format_packet_read_curve_data(&packetDlt, dateitem, datatime);
				}
				else
				{
					pTask->status = 4;
					return false;
				}
			}
			break;
		case 5:
			{
				ptcl_module_csg::pnfn_data pnfn[1];
				pnfn[0].pn = pTask->pn;
				pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
				ptcl_module_csg::pnfn_data_uint pnfn_data[1];
				pnfn_data[0].pnfn = pnfn[0];
				//data_format_csg::get_instance()->format_read_history(&packetCsg, pnfn_data, 1, pTask->starttime, pTask->endtime, pTask->desity);
				data_format_csg::get_instance()->format_read_history(&packetCsg, pnfn_data, 1, pTask->starttime, pTask->starttime + 15 * 60, pTask->desity);
				pTask->waittime = 0;
			}
			break;
		case 6:
			{
				pTask->waittime = 0;
				time_t datatime = pTask->starttime + 6 * 60 + 15 * 60;		// 12:00, 12:15, 12:30, 12:45
				map<string, string>::iterator it = m_mCurveSg2Dlt.find(pTask->dataitem);
				if (it != m_mCurveSg2Dlt.end())
				{
					unsigned int dateitem = ptcl_packet_dlt::convert_fn(it->second);
					UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
					packetDlt.set_meter_addr(addr);
					ptcl_module_dlt::get_instance()->format_packet_read_curve_data(&packetDlt, dateitem, datatime);
				}
				else
				{
					pTask->status = 4;
					return false;
				}
			}
			break;
		case 7:
			{
				ptcl_module_csg::pnfn_data pnfn[1];
				pnfn[0].pn = pTask->pn;
				pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
				ptcl_module_csg::pnfn_data_uint pnfn_data[1];
				pnfn_data[0].pnfn = pnfn[0];
				data_format_csg::get_instance()->format_read_history(&packetCsg, pnfn_data, 1, pTask->starttime + 15 * 60, pTask->starttime + 30 * 60, pTask->desity);
				pTask->waittime = 0;
			}
			break;
		case 8:
				{
				pTask->waittime = 0;
				time_t datatime = pTask->starttime + 6 * 60 + 2 * 15 * 60;		// 12:00, 12:15, 12:30, 12:45
				map<string, string>::iterator it = m_mCurveSg2Dlt.find(pTask->dataitem);
				if (it != m_mCurveSg2Dlt.end())
				{
					unsigned int dateitem = ptcl_packet_dlt::convert_fn(it->second);
					UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
					packetDlt.set_meter_addr(addr);
					ptcl_module_dlt::get_instance()->format_packet_read_curve_data(&packetDlt, dateitem, datatime);
				}
				else
				{
					pTask->status = 4;
					return false;
				}
			}
			break;
		case 9:
			{
				ptcl_module_csg::pnfn_data pnfn[1];
				pnfn[0].pn = pTask->pn;
				pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
				ptcl_module_csg::pnfn_data_uint pnfn_data[1];
				pnfn_data[0].pnfn = pnfn[0];
				data_format_csg::get_instance()->format_read_history(&packetCsg, pnfn_data, 1, pTask->starttime + 30 * 60, pTask->starttime + 45 * 60, pTask->desity);
				pTask->waittime = 0;
			}
			break;
		case 10:
			{
				pTask->waittime = 0;
				time_t datatime = pTask->starttime + 6 * 60 + 3 * 15 * 60;		// 12:00, 12:15, 12:30, 12:45
				map<string, string>::iterator it = m_mCurveSg2Dlt.find(pTask->dataitem);
				if (it != m_mCurveSg2Dlt.end())
				{
					unsigned int dateitem = ptcl_packet_dlt::convert_fn(it->second);
					UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
					packetDlt.set_meter_addr(addr);
					ptcl_module_dlt::get_instance()->format_packet_read_curve_data(&packetDlt, dateitem, datatime);
				}
				else
				{
					pTask->status = 4;
					return false;
				}
			}
			break;
		case 11:
			{
				ptcl_module_csg::pnfn_data pnfn[1];
				pnfn[0].pn = pTask->pn;
				pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
				ptcl_module_csg::pnfn_data_uint pnfn_data[1];
				pnfn_data[0].pnfn = pnfn[0];
				data_format_csg::get_instance()->format_read_history(&packetCsg, pnfn_data, 1, pTask->starttime + 45 * 60, pTask->starttime + 60 * 60, pTask->desity);
				pTask->waittime = 0;
			}
			break;
		case 12:
			pTask->waittime = 0;
			pTask->status = 1;
			return false;
			break;
	}

	if (packetDlt.check_packet() > 0)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}	
	if (packetCsg.parse_packet() > 0)
	{
		pRtuChannel->Write((char *)packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetCsg.get_packet_buffer(), packetCsg.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_RHC_FLOW2(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel, UINT32 nDataItem)
{
	ptcl_packet_dlt packetDlt;
	ptcl_packet_csg packetCsg;
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packetCsg.set_device_addr(id);

	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (++m_nCurveTestCount > 1)
				return false;

			CTime now(time(NULL));
			CTime temp(now.GetYear(), now.GetMonth(), now.GetDay(), 11, 54, 0);
			pTask->starttime = temp.GetTime();
			pTask->endtime = temp.GetTime() + 4 * 15 * 60;

			// 设置后续任务（曲线数据）的开始时间和结束时间
			AutoTask *pNextTask = pTask + 1;
			while (pNextTask <= &m_vTask[m_vTask.size() - 1])
			{
				if (pNextTask->type != pTask->type)
					break;
				pNextTask->starttime = pTask->starttime;
				pNextTask->endtime = pTask->endtime;
				pNextTask++;
			}
			pNextTask--;
			pNextTask->starttime = 123456;		// 恢复终端及设备时钟的判定标志

			// 终端校时至11:54
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(temp.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(temp.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(temp.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(temp.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(temp.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(temp.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packetCsg, (unsigned char *)"", &dataunit, 1);
			
			// 虚拟表装置校时至11:54
			packetDlt.set_meter_addr(0x999999999999);
			ptcl_module_dlt::get_instance()->format_packet_set_time(&packetDlt, temp.GetTime());

			pTask->waittime = 30;
		}
		break;
	case 2:
		{
			if (m_nCurveTestCount > 1)
				return false;

			// 启动曲线测试任务命令
			packetDlt.set_meter_addr(0xAAAAAAAAAAAA);
			ptcl_module_dlt::get_instance()->format_packet_start_clock_communicate(&packetDlt, 0xE851, 0x0F, 4);		// 4为周期，1周期15分钟
			pTask->waittime = 4 * 15 * 60;
		}
		break;
	case 3:
	case 4:
	case 5:
	case 6:
		{
			time_t datatime = pTask->starttime + 6 * 60 + (pTask->step - 3) * 15 * 60;		// 12:00, 12:15, 12:30, 12:45
			unsigned int dateitem = ptcl_packet_dlt::convert_fn("AA1F");
			UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
			packetDlt.set_meter_addr(addr);
			ptcl_module_dlt::get_instance()->format_packet_read_curve_data(&packetDlt, dateitem, datatime);
			pTask->waittime = 0;
		}
		break;	
	case 7:
		{						
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = pTask->pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(pTask->dataitem);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_history(&packetCsg, pnfn_data, 1, pTask->starttime, pTask->endtime, pTask->desity);
			pTask->waittime = 0;
		}
		break;
	case 8:
		{
			if (pTask->starttime != 123456)
				return false;
			
			// 终端校时至正常时钟
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packetCsg, (unsigned char *)"", &dataunit, 1);

			// 虚拟表装置校时至正常时钟
			packetDlt.set_meter_addr(0x999999999999);
			ptcl_module_dlt::get_instance()->format_packet_set_time(&packetDlt, time(NULL));

			pTask->waittime = 30;
		}
		break;
	case 9:
		pTask->status = 1;
		return false;
		break;
	}

	if (packetDlt.check_packet() > 0)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}	
	if (packetCsg.parse_packet() > 0)
	{
		pRtuChannel->Write((char *)packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetCsg.get_packet_buffer(), packetCsg.get_packet_length());
	}

	return true;
}

bool CDlgAutoTest::ExcuteTask_E3020001(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel)
{
	ptcl_packet_dlt packetDlt;
	ptcl_packet_csg packetCsg;	
	UINT64 id = ptcl_packet_csg::convert_device_addr(pTask->rtuAddr);
	packetCsg.set_device_addr(id);

	bool bIsDltPacket = false;
	pTask->step++;
	switch (pTask->step)
	{
	case 1:
		{
			if (!pTask->isfirst)
				return false;

			// 测试主站与被测终端校时			
			CTime now(time(NULL));
			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = 0;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0000130");
			dataunit.len_data = 6;
			dataunit.data_buffer[5] = ptcl_module::bin_to_bcd(now.GetYear() - 2000);
			dataunit.data_buffer[4] = ptcl_module::bin_to_bcd(now.GetMonth());
			dataunit.data_buffer[3] = ptcl_module::bin_to_bcd(now.GetDay());
			dataunit.data_buffer[2] = ptcl_module::bin_to_bcd(now.GetHour());
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(now.GetMinute());
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(now.GetSecond());
			ptcl_module_csg::get_instance()->format_packet_set_param(&packetCsg, (unsigned char *)"", &dataunit, 1);	
			ShowOperateLog("数据透传测试任务开始（仅支持97规约电表）！");
			ShowOperateLog("对终端进行系统时间校时！");
			pTask->waittime = 0;
			bIsDltPacket = false;
		}
		break;
	case 2:
		{
			if (!pTask->isfirst)
				return false;

			// 下发启动数据透传任务命令给虚拟表装置持续2分钟，虚拟表装置收到该任务命令时，停止走字2分钟			
			packetDlt.set_meter_addr(0xAAAAAAAAAAAA);
			ptcl_module_dlt::get_instance()->format_packet_start_clock_communicate(&packetDlt, 0xE853, 0x0F, 2);
			ShowOperateLog("向虚拟表装置下发数据透传任务命令0xE853，虚拟表装置停止走字2分钟！");
			pTask->waittime = 15;
			bIsDltPacket = true;
		}
		break;
	case 3:
		{
			// 抄读虚拟表装置中的正向有功电能量数据
			unsigned int dateitem = ptcl_packet_dlt::convert_fn("901F");
			UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
			packetDlt.set_meter_addr(addr);
			ptcl_module_dlt::get_instance()->format_packet_read_data(&packetDlt, dateitem);
			ShowOperateLog("抄读虚拟表装置中的正向有功电能量数据！");
			pTask->waittime = 0;	
			bIsDltPacket = true;
		}
		break;
	case 4:
		{	
			// 中继转发-抄读终端的正向有功电能量数据
			unsigned int dateitem = ptcl_packet_dlt::convert_fn("901F");
			UINT64 addr = ptcl_packet_dlt::convert_meter_addr(pTask->meterAddr);
			packetDlt.set_meter_addr(addr);
			ptcl_module_dlt::get_instance()->format_packet_read_data(&packetDlt, dateitem);						
			ptcl_module_csg::get_instance()->format_packet_transmit(&packetCsg, 0, pTask->port, pTask->baud, pTask->check, 
				pTask->databit, pTask->stopbit, 120, packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), (unsigned char *)"");
			ShowOperateLog("中继转发-抄读电表的正向有功电能量数据！");
			packetDlt.reset();
			pTask->waittime = 0;
			bIsDltPacket = false;
		}
		break;
	case 5:
		pTask->status = 1;
		return false;
		break;
	}

	if (bIsDltPacket)
	{
		pMeterChannel->Write((char *)packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), m_virtualDeviceId);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetDlt.get_packet_buffer(), packetDlt.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetDlt.get_packet_buffer(), packetDlt.get_packet_length());
	}
	else	
	{
		pRtuChannel->Write((char *)packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), id);
		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packetCsg.get_packet_buffer(), packetCsg.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packetCsg.get_packet_buffer(), packetCsg.get_packet_length());
	}

	return true;
}

void CDlgAutoTest::OnReceive(int state, char *buf, int len, u_int64 addr)
{
	if (g_nModuleControl != Module_AutoCeShi)
		return;

	if (len <= 0)
		return;

	CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf((unsigned char *)buf, len, "接收"));
	CLogMgr::GetInstanse().LogPacket("接收", (unsigned char *)buf, len);

	ptcl_packet_csg csgPacket;
	csgPacket.set_packet_buffer((unsigned char *)buf, len);
	if (csgPacket.parse_packet() > 0)
	{
		ProcessPacket(csgPacket, m_pAutoTask);		
		s_bWait = false;
		return;
	}

	ptcl_packet_dlt dltPacket;
	dltPacket.set_packet_buffer((unsigned char *)buf, len);
	if (dltPacket.parse_packet() > 0)
	{
		ProcessPacket(dltPacket, m_pAutoTask);
		s_bWait = false;
		return;
	}
}

void CDlgAutoTest::ProcessPacket(ptcl_packet_csg &packet, AutoTask *pTask)
{
	if (pTask == NULL)
		return;

	CString strPn = _T("");
	CString strChannelDesc = _T("");
	CString strTaskDesc = _T("");
	CString strResult = _T("");
	CString strDataItemType = _T("");
	CString strDataItem = _T("");
	CString strValue = _T("");
	CString strVirtualValue = _T("");
	CString strTime = _T("");

	strPn.Format("%d", pTask->pn);
	std::vector<ChannelParam>::iterator it = m_vChannelId.begin();
	while (it != m_vChannelId.end())
	{
		if (it->nId == pTask->channelId)
		{
			strChannelDesc = it->sChannelDesc;
			break;
		}
		++it;
	}
	std::vector<TaskParam>::iterator it1 = m_vTaskId.begin();
	while (it1 != m_vTaskId.end())
	{
		if (it1->nId == pTask->taskId)
		{
			strTaskDesc = it1->sTaskDesc;
			break;
		}
		++it1;
	}
	strDataItemType = GetChildTaskDesc(pTask->type);	
	//std::map<CString, CString>::iterator taskIt = m_mTaskDesc.find(pTask->strdataitem.c_str());//MODIFY BY WM 20140114
	//if (taskIt != m_mTaskDesc.end())
	//{
	//	//strDataItem = taskIt->second;	
	//	strDataItem = taskIt->first;
	//}
	strDataItem = pTask->strdataitem.c_str();

	if(packet.get_afn() == 0x0C)
	{	
		// ExcuteTask_E200003E: step[3]
		if (pTask->type == 8)
			return;

		vector<data_parse_csg_realtime::data_item_value> vData;	
		data_parse_csg_realtime::parse_realtime_data(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
		strResult = "不合格";

		if (vData.size() > 0)
		{
			if (IsValidData(vData[0].db_value))
			{
				if (pTask->dataitem.compare("04000101") == 0)
				{
					if (vData[0].db_value >= 0)
					{
						CTime dt(vData[0].db_value);
						strValue.Format("%d-%d-%d", dt.GetYear(), dt.GetMonth(), dt.GetDay());	// 日期
						strResult = "合格";
					}
				}
				else if (pTask->dataitem.compare("04000102") == 0)
				{
					if (vData[0].db_value >= 0)
					{
						CTime dt(vData[0].db_value);
						strValue.Format("%d:%d:%d", dt.GetHour(), dt.GetMinute(), dt.GetSecond());	// 时间
						strResult = "合格";
					}
				}
				else
				{
					strValue.Format("%f", vData[0].db_value);
					pTask->realdata.push_back(vData[0].db_value);
				}
			}
			else
			{
				strValue = "无效数据";
			}
		}
		if (pTask->virtualdata.size() > 0)
		{
			strVirtualValue.Format("%f", pTask->virtualdata[0]);
			if ( pTask->realdata.size() == pTask->virtualdata.size() && abs(pTask->realdata[0] - pTask->virtualdata[0]) < 0.1 )			
				strResult = "合格";			
		}


		int nRow = m_ctrlReportList.GetItemCount();
		m_ctrlReportList.InsertItem(nRow, strPn);
		m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
		m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
		m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
		m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
		m_ctrlReportList.SetItemText(nRow, 5, strValue);
		m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);		
		m_ctrlReportList.SetItemText(nRow, 7, strResult);
	}
	else if (packet.get_afn() == 0x0d)
	{

		GetHistoryData(packet, pTask, strValue, strVirtualValue, strResult, strTime);
		if(pTask->isfinish)//MODIFY BY WM 20140108
		{
			int nRow = m_ctrlReportList.GetItemCount();
			m_ctrlReportList.InsertItem(nRow, strPn);
			m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
			m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
			m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
			m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
			m_ctrlReportList.SetItemText(nRow, 5, strValue);
			m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);		
			m_ctrlReportList.SetItemText(nRow, 7, strResult);
		}

	}
	else if (packet.get_afn() == 0x0e)
	{
		vector<data_parse_event_record::data_item_value> vData;
		data_parse_event_record::parse_event_record(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
		strResult = "不合格";
		
		CTime tStart(pTask->starttime);
		CTime tEnd(pTask->endtime);
		strVirtualValue = "开始时间:" + tStart.Format("%y-%m-%d %H:%M:%S") + " 结束时间:" + tEnd.Format("%y-%m-%d %H:%M:%S");
		if (vData.size() > 0)
		{
			//strValue = vData[0].db_value.c_str();
			CTime sTime(vData[0].start_time);
			CTime eTime(vData[0].end_time);
			strValue = "开始时间:" + sTime.Format("%y-%m-%d %H:%M:%S") + " 结束时间:" + eTime.Format("%y-%m-%d %H:%M:%S");
			if (vData[0].start_time >= pTask->starttime && vData[0].end_time <= pTask->endtime)			
				strResult = "合格";
		}

		int nRow = m_ctrlReportList.GetItemCount();
		m_ctrlReportList.InsertItem(nRow, strPn);
		m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
		m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
		m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
		m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
		m_ctrlReportList.SetItemText(nRow, 5, strValue);
		m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);		
		m_ctrlReportList.SetItemText(nRow, 7, strResult);
	}
	else if (packet.get_afn() == 0x13)
	{
		vector<data_parse_alarm_data::data_item_value> vData;
		data_parse_alarm_data::parse_alarm_data(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
		strResult = "不合格";

		CTime tStart(pTask->starttime);
		CTime tEnd(pTask->endtime);
		strVirtualValue = "开始时间:" + tStart.Format("%y-%m-%d %H:%M:%S") + "; 结束时间:" + tEnd.Format("%y-%m-%d %H:%M:%S");
		if (vData.size() > 0)
		{
			CTime alarmTime(vData[0].db_time);
			strValue = "告警发生时间:" + alarmTime.Format("%y-%m-%d %H:%M:%S");
			//strValue = vData[0].db_value.c_str();
			if (vData[0].db_time >= pTask->starttime && vData[0].db_time <= pTask->endtime)
				strResult = "合格";
		}

		int nRow = m_ctrlReportList.GetItemCount();
		m_ctrlReportList.InsertItem(nRow, strPn);
		m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
		m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
		m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
		m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
		m_ctrlReportList.SetItemText(nRow, 5, strValue);
		m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);		
		m_ctrlReportList.SetItemText(nRow, 7, strResult);
	}
	else if (packet.get_afn() == 0x0a)
	{
		if (pTask->dataitem.compare("E0000130") == 0)
		{
			vector<data_parse_csg_parameter::data_item_value> vData;
			data_parse_csg_parameter::parse_parameter(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);				
			if (vData.size() > 0)
			{
				strValue = vData[0].db_value.c_str();
				int Y, M, D, h, m, s;
				sscanf_s(strValue, "%d-%d-%d %d:%d:%d", &Y, &M, &D, &h, &m, &s);
				if(pTask->bdataitemE0000130)
				{
					CTime dt(Y, M, D, h, m, s);
					pTask->terminaltime = dt.GetTime();
					pTask->bdataitemE0000130 = false;
				}
				else
				{
					strResult = "不合格";

					CTime tVirtual(pTask->starttime);
					strVirtualValue = tVirtual.Format("%Y-%m-%d %H:%M:%S");
					strValue = vData[0].db_value.c_str();
					int Y, M, D, h, m, s;
					sscanf_s(strValue, "%d-%d-%d %d:%d:%d", &Y, &M, &D, &h, &m, &s);
					CTime tRtu(Y, M, D, h, m, s);
					if (abs(tRtu.GetTime() - pTask->starttime) < 30)	
						strResult = "合格";

					int nRow = m_ctrlReportList.GetItemCount();
					m_ctrlReportList.InsertItem(nRow, strPn);
					m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
					m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
					m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
					m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
					m_ctrlReportList.SetItemText(nRow, 5, strValue);
					m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);		
					m_ctrlReportList.SetItemText(nRow, 7, strResult);
				}
			}


		}
		else
		{
			if(pTask->bdataitemE0000130)
			{
				vector<data_parse_csg_parameter::data_item_value> vData;
				data_parse_csg_parameter::parse_parameter(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);				
				if (vData.size() > 0)
				{
					strValue = vData[0].db_value.c_str();
					int Y, M, D, h, m, s;
					sscanf_s(strValue, "%d-%d-%d %d:%d:%d", &Y, &M, &D, &h, &m, &s);
					CTime dt(Y, M, D, h, m, s);
					pTask->terminaltime = dt.GetTime();
				}
				pTask->bdataitemE0000130 = false;
			}
			
		}
	}	
	else if (packet.get_afn() == 0x10)
	{
		strValue = "";
		strVirtualValue = "";
		strResult = "不合格";
		
		vector<parse_read_data::data_item_value> vData;
		parse_transmit::parse_transmit_data(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
		for (UINT i = 0; i < vData.size(); i++)
		{
			pTask->realdata.push_back(vData.at(i).db_value);			
			char temp[32] = {0};
			sprintf_s(temp, "%f; ", pTask->realdata.at(i));
			strValue.Append(temp);
		}
		for (UINT i = 0; i < pTask->virtualdata.size(); ++i)
		{
			char temp[32] = {0};
			sprintf_s(temp, "%f; ", pTask->virtualdata.at(i));
			strVirtualValue.Append(temp);
		}
		if (pTask->realdata.size() > 0 && pTask->realdata.size() == pTask->virtualdata.size())
		{
			bool bSame = true;
			for (UINT i = 0; i < pTask->realdata.size(); ++i)
			{
				if (abs(pTask->realdata[i] - pTask->virtualdata[i]) > 0.1)  //MODIFY BY WM 20131206 把误差从0.0001扩大到0.1
				{
					bSame = false;
					break;
				}
			}
			strResult = bSame ? "合格" : "不合格";
		}

		int nRow = m_ctrlReportList.GetItemCount();
		m_ctrlReportList.InsertItem(nRow, strPn);
		m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
		m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
		m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
		m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
		m_ctrlReportList.SetItemText(nRow, 5, strValue);
		m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);		
		m_ctrlReportList.SetItemText(nRow, 7, strResult);
	}
	else if (packet.get_afn() == 0x00)
	{
		CTime tmData(pTask->starttime);
		strTime = tmData.Format("%Y-%m-%d %H:%M:%S");
		int nRow = m_ctrlReportList.GetItemCount();
		strResult = "不合格";
		ptcl_module_csg::pnfn_data pnfn;
		ptcl_module_csg::parse_pnfn(packet.get_data_area_buffer(),&pnfn);
		if(pnfn.fn == 0x00080400)
		{
			strResult = "合格";
			UINT i = pTask->virtualdata.size() - 3;
			CTime dt1(pTask->virtualdata.at(i) < 0 ? 0 : pTask->virtualdata.at(i));
		    CTime dt2(pTask->virtualdata.at(i+2) < 0 ? 0 : pTask->virtualdata.at(i+2));
			char temp[128] = {0};
			sprintf_s(temp, "[%s]: %f%s; ", dt1.Format("%H:%M"), pTask->virtualdata.at(i+1), false? dt2.Format("(%Y-%m-%d %H:%M:%S)") : "");
			strVirtualValue.Append(temp);
			m_ctrlReportList.InsertItem(nRow, strPn);
			m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
			m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
			m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
			m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
			m_ctrlReportList.SetItemText(nRow, 5, strVirtualValue);
			m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);
			m_ctrlReportList.SetItemText(nRow, 7, strResult);
		}
		else
		{
			m_ctrlReportList.InsertItem(nRow, strPn);
			m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
			m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
			m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
			m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
			m_ctrlReportList.SetItemText(nRow, 5, strValue);
			m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);
			m_ctrlReportList.SetItemText(nRow, 7, strResult);
		}
	}
	if(pTask->isfinish) //ADD BY WM 20140108
	{
		WriteResultDb(m_batchId, m_termId, strPn, strChannelDesc, strTaskDesc, strVirtualValue, strValue, strDataItemType, strDataItem, strResult, strTime);	
	}
}

void CDlgAutoTest::GetHistoryData(ptcl_packet_csg &packet, AutoTask *pTask, CString &strValue, CString &strVirtualValue, CString &strResult, CString &strTime)
{
	strResult = "不合格";
	strValue = "";
	strVirtualValue = "";
	strTime = "";
	UINT32 dataitem = 0;
	sscanf_s(pTask->dataitem.c_str(), "%X", &dataitem);		// 当前任务的数据项标识
	
	if (2 == pTask->type)
	{
		vector<data_parse_csg_history::data_item_value> vData;
		data_parse_csg_history::parse_history_data(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
		pTask->isfinish = false; //ADD BY WM 20140108

		bool bIsDemand = false;
		switch (dataitem)
		{
		case 0x01010000:
		case 0x01010100:
		case 0x01010200:
		case 0x01010300:
		case 0x01010400:
		case 0x01020000:
		case 0x01020100:
		case 0x01020200:
		case 0x01020300:
		case 0x01020400:
			bIsDemand = true;
			break;
		}
		
		for (UINT i = 0; i < vData.size(); ++i)
		{
			pTask->realdata.push_back(vData.at(i).value_time);
			pTask->realdata.push_back(vData.at(i).db_value);	
			pTask->realdata.push_back(vData.at(i).db_time);
			
			CTime dt1(vData.at(i).value_time < 0 ? 0 : vData.at(i).value_time);
			CTime dt2(vData.at(i).db_time < 0 ? 0 : vData.at(i).db_time);
			char temp[128] = {0};			
			sprintf_s(temp, "[%s]: %f%s; ", dt1.Format("%H:%M"), vData.at(i).db_value, bIsDemand ? dt2.Format("(%Y-%m-%d %H:%M:%S)") : "");
			pTask->realcurstring.Append(temp); //ADD BY WM 20140108
			//strValue.Append(temp); //MODIFY BY WM 20140108
		}

		UINT num = pTask->realdata.size() - 3; 
		
		if ((num + 2) <= pTask->virtualdata.size()) //MODIFY BY WM 20140604 防止数组越界
		{
			CTime dt1(pTask->virtualdata.at(num) < 0 ? 0 : pTask->virtualdata.at(num));
			CTime dt2(pTask->virtualdata.at(num+2) < 0 ? 0 : pTask->virtualdata.at(num+2));
			char temp[128] = {0};
			sprintf_s(temp, "[%s]: %f%s; ", dt1.Format("%H:%M"), pTask->virtualdata.at(num+1), bIsDemand ? dt2.Format("(%Y-%m-%d %H:%M:%S)") : "");
			pTask->virtualcurstring.Append(temp);//ADD BY WM 20140108
			//strVirtualValue.Append(temp); //MODIFY BY WM 20140108
		}
		if (num == 9)  //MODIFY BY WM 20140108  用来判断曲线数据四个点是否解析完,解析完则进行测试结果判断
		{
			pTask->isfinish = true;//MODIFY BY WM 20140108
			strValue = pTask->realcurstring;
			strVirtualValue = pTask->virtualcurstring;
			
			if (pTask->realdata.size() > 0 && pTask->realdata.size() == pTask->virtualdata.size())  //MODIFY BY WM 20140604 防止数组越界
			{
				bool bSame = true;
				for (UINT i = 0; i < pTask->realdata.size(); i += 3)
				{
					if(((i + 2 ) <= pTask->realdata.size()) && ((i + 2) <= pTask->virtualdata.size()))
					{
						if ( abs(pTask->realdata[i] - pTask->virtualdata[i]) > 30 
							|| abs(pTask->realdata[i+1] - pTask->virtualdata[i+1]) > 0.1 
							|| abs(pTask->realdata[i+2] - pTask->virtualdata[i+2]) > 30 )
						{
							bSame = false;
							break;
						}
					}
				}
				strResult = bSame ? "合格" : "不合格";
			}
		}
	}
	else
	{
		vector<data_parse_csg_history::data_item_value> vData;
		data_parse_csg_history::parse_history_data(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
		switch (dataitem)
		{
		case 0x01010001:	// 最大需量及发生时间
		case 0x01010101:
		case 0x01010201:
		case 0x01010301:
		case 0x01010401:
		case 0x01020001:
		case 0x01020101:
		case 0x01020201:
		case 0x01020301:
		case 0x01020401:
		case 0x01030001:
		case 0x01030101:
		case 0x01030201:
		case 0x01030301:
		case 0x01030401:
		case 0x01040001:
		case 0x01040101:
		case 0x01040201:
		case 0x01040301:
		case 0x01040401:
			{
				int M,D,H,Min;
				int v_M,v_D,v_H,v_Min;
				if (vData.size() > 0)
				{
					if (IsValidData(vData[0].db_value))
					{
						CTime time(vData[0].value_time < 0 ? 0 : vData[0].value_time);
						strTime = time.Format("%Y-%m-%d %H:%M:%S");
						CTime rTime(vData[0].db_time < 0 ? 0 : vData[0].db_time);
						strValue.Format("最大需量:%f; 最大需量发生时间:%s", vData[0].db_value, rTime.Format("%m-%d %H:%M:%S"));
						pTask->realdata.push_back(vData[0].db_value);
						pTask->realdata.push_back(vData[0].db_time);
						M = rTime.GetMonth();
						D = rTime.GetDay();
						H = rTime.GetHour();
						Min = rTime.GetMinute();
					}
					else
						strValue = "无效数据";
				}
				if (pTask->virtualdata.size() > 1)
				{
					CTime vTime(pTask->virtualdata[1] < 0 ? 0 : pTask->virtualdata[1]);
					strVirtualValue.Format("最大需量:%f; 最大需量发生时间:%s", pTask->virtualdata[0], vTime.Format("%m-%d %H:%M:%S"));
					v_M = vTime.GetMonth();
					v_D = vTime.GetDay();
					v_H = vTime.GetHour();
					v_Min = vTime.GetMinute();
					
					if (pTask->realdata.size() > 1)
					{
						//long long iTemp = abs(pTask->realdata[1] - pTask->virtualdata[1]);
						if((pTask->realdata.size() == pTask->virtualdata.size() 
							&& (abs(pTask->realdata[0] - pTask->virtualdata[0]) < 0.1 )
							&& (M == v_M && D == v_D && H == v_H && Min == v_Min))|| pTask->realdata[0] == 0 )	// 最大需量发生时间的年份不判断
						{
							strResult = "合格";
						}
					}
				}
			}
			break;
		default:
			{
				if (vData.size() > 0)
				{
					if (IsValidData(vData[0].db_value))
					{
						CTime time(vData[0].value_time < 0 ? 0 : vData[0].value_time);
						strTime = time.Format("%Y-%m-%d %H:%M:%S");
						strValue.Format("%f", vData[0].db_value);
						pTask->realdata.push_back(vData[0].db_value);
					}
					else
						strValue = "无效数据";
				}
				if (pTask->virtualdata.size() > 0 )
				{
					strVirtualValue.Format("%f", pTask->virtualdata[0]);

					if (pTask->realdata.size() > 0)
					{
						if ( pTask->realdata.size() == pTask->virtualdata.size() 
							 && abs(vData[0].db_value - pTask->virtualdata[0]) < 0.1 )
						{
							strResult = "合格";
						}
					}
				}
			}
			break;
		}
	}
}

void CDlgAutoTest::ProcessPacket(ptcl_packet_dlt &packet, AutoTask *pTask)
{
	if (pTask == NULL)
		return;

	UINT32 dataitem = 0;
	sscanf_s(pTask->dataitem.c_str(), "%X", &dataitem);

	if (6 == pTask->type)
	{
		if (0xE3020001 == dataitem)
		{
			if (packet.get_ctrl_code() == 0x81)
			{
				vector <parse_read_data::data_item_value> vData;
				parse_read_data::parse_read_data_current(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
				for (UINT i = 0; i < vData.size(); i++)
				{
					pTask->virtualdata.push_back(vData.at(i).db_value);
				}
			}
		}
	}
	else if (2 == pTask->type)
	{
		if (packet.get_ctrl_code() == 0x81)
		{
			vector <parse_read_data::data_item_value> vData;
			parse_read_data::parse_read_data_curve(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
			for (UINT i = 0; i < vData.size(); i++)
			{
				string sDataItem = ptcl_packet_dlt::convert_fn(vData[i].data_item_id);
				map<string, string>::iterator it = m_mCurveDlt2sg.find(sDataItem);
				if (it == m_mCurveDlt2sg.end())
					continue;
				if (it->second.compare(pTask->dataitem.c_str()) != 0)
					continue;
				pTask->virtualdata.push_back(vData.at(i).value_time);
				pTask->virtualdata.push_back(vData.at(i).db_value);
				pTask->virtualdata.push_back(0);	
			}
		}
	}
	else if (3 == pTask->type)
	{
		if (packet.get_ctrl_code() == 0x81)
		{
			vector <parse_read_data::data_item_value> vData;
			parse_read_data::parse_read_data_current(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
			for (UINT i = 0; i < vData.size(); i++)
			{
				string sDataItem = ptcl_packet_dlt::convert_fn(vData[i].data_item_id);
				map<string, string>::iterator it = m_mapDlt2Sg.find(sDataItem);
				if (it == m_mapDlt2Sg.end())
					continue;
				if (it->second.compare(pTask->dataitem.c_str()) == 0)
				{
					pTask->virtualdata.push_back(vData.at(i).db_value);
				}
			}
		}
	}
	else
	{
		if (packet.get_ctrl_code() == 0x81)
		{
			vector <parse_read_data::data_item_value> vData;
			parse_read_data::parse_read_data_current(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
			for (UINT i = 0; i < vData.size(); i++)
			{
				string sDataItem = ptcl_packet_dlt::convert_fn(vData[i].data_item_id);
				map<string, string>::iterator it = m_mapDlt2Sg.find(sDataItem);
				if (it == m_mapDlt2Sg.end())
					continue;
				if (it->second.compare(pTask->dataitem.c_str()) == 0)
				{
					pTask->virtualdata.push_back(vData.at(i).db_value);
				}
			}
		}
	}
}

void CDlgAutoTest::ProcessFailed(AutoTask *pTask)
{
	if (pTask == NULL)
		return;

	CString strPn = _T("");
	CString strChannelDesc = _T("");
	CString strTaskDesc = _T("");
	CString strResult = _T("");
	CString strDataItemType = _T("");
	CString strDataItem = _T("");
	CString strValue = _T("");
	CString strVirtualValue = _T("");
	CString strTime = _T("");

	strPn.Format("%d", pTask->pn);
	std::vector<ChannelParam>::iterator it = m_vChannelId.begin();
	while (it != m_vChannelId.end())
	{
		if (it->nId == pTask->channelId)
		{
			strChannelDesc = it->sChannelDesc;
			break;
		}
		++it;
	}
	std::vector<TaskParam>::iterator it1 = m_vTaskId.begin();
	while (it1 != m_vTaskId.end())
	{
		if (it1->nId == pTask->taskId)
		{
			strTaskDesc = it1->sTaskDesc;
			break;
		}
		++it1;
	}
	strDataItemType = GetChildTaskDesc(pTask->type);	
	//std::map<CString, CString>::iterator taskIt = m_mTaskDesc.find(pTask->dataitem.c_str());
	//if (taskIt != m_mTaskDesc.end())
	//{
	//	strDataItem = taskIt->second;	
	//}	
	strDataItem = pTask->strdataitem.c_str();

	strResult = "不合格";

	int nRow = m_ctrlReportList.GetItemCount();	
	m_ctrlReportList.InsertItem(nRow, strPn);
	m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
	m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
	m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
	m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
	m_ctrlReportList.SetItemText(nRow, 5, strValue);
	m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);	
	m_ctrlReportList.SetItemText(nRow, 7, strResult);

	WriteResultDb(m_batchId, m_termId, strPn, strChannelDesc, strTaskDesc, strVirtualValue, strValue, strDataItemType, strDataItem, strResult, strTime);	
}

void CDlgAutoTest::ProcessSucceeded(AutoTask *pTask)
{
	if (pTask == NULL)
		return;

	CString strPn = _T("");
	CString strChannelDesc = _T("");
	CString strTaskDesc = _T("");
	CString strResult = _T("");
	CString strDataItemType = _T("");
	CString strDataItem = _T("");
	CString strValue = _T("");
	CString strVirtualValue = _T("");
	CString strTime = _T("");

	strPn.Format("%d", pTask->pn);
	std::vector<ChannelParam>::iterator it = m_vChannelId.begin();
	while (it != m_vChannelId.end())
	{
		if (it->nId == pTask->channelId)
		{
			strChannelDesc = it->sChannelDesc;
			break;
		}
		++it;
	}
	std::vector<TaskParam>::iterator it1 = m_vTaskId.begin();
	while (it1 != m_vTaskId.end())
	{
		if (it1->nId == pTask->taskId)
		{
			strTaskDesc = it1->sTaskDesc;
			break;
		}
		++it1;
	}
	strDataItemType = GetChildTaskDesc(pTask->type);	
	//std::map<CString, CString>::iterator taskIt = m_mTaskDesc.find(pTask->dataitem.c_str());
	//if (taskIt != m_mTaskDesc.end())
	//{
	//	strDataItem = taskIt->second;	
	//}	
	strDataItem = pTask->strdataitem.c_str();

	strResult = "合格";

	int nRow = m_ctrlReportList.GetItemCount();	
	m_ctrlReportList.InsertItem(nRow, strPn);
	m_ctrlReportList.SetItemText(nRow, 1, strChannelDesc);
	m_ctrlReportList.SetItemText(nRow, 2, strTaskDesc);
	m_ctrlReportList.SetItemText(nRow, 3, strDataItemType);
	m_ctrlReportList.SetItemText(nRow, 4, strDataItem);
	m_ctrlReportList.SetItemText(nRow, 5, strValue);
	m_ctrlReportList.SetItemText(nRow, 6, strVirtualValue);	
	m_ctrlReportList.SetItemText(nRow, 7, strResult);

	WriteResultDb(m_batchId, m_termId, strPn, strChannelDesc, strTaskDesc, strVirtualValue, strValue, strDataItemType, strDataItem, strResult, strTime);	
}

void CDlgAutoTest::OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	*pResult = CDRF_DODEFAULT;

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
    else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{
		if (7 == pLVCD->iSubItem)
		{
			COLORREF clrNewTextColor, clrNewBkColor;
			int nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );
			CString strTemp = m_ctrlReportList.GetItemText(nItem, 7);
			if(strTemp == "不合格")
			{
				clrNewTextColor = RGB(255,0,0);
				clrNewBkColor = RGB(255,255,255);
			}
			else
			{
				clrNewTextColor = RGB(0,0,0);
				clrNewBkColor = RGB(255,255,255);
			}
			pLVCD->clrText = clrNewTextColor;
			pLVCD->clrTextBk = clrNewBkColor; 
		}
        *pResult = CDRF_DODEFAULT;
	}
}

void CDlgAutoTest::InitMap()
{
	// 上日电量:正向有功
	m_mapSg2Dlt["05060100"] = "9C1F";
	m_mapSg2Dlt["05060101"] = "9C1F";
	m_mapSg2Dlt["05060102"] = "9C1F";
	m_mapSg2Dlt["05060103"] = "9C1F";
	m_mapSg2Dlt["05060104"] = "9C1F";
	m_mapDlt2Sg["9C10"] = "05060100";
	m_mapDlt2Sg["9C11"] = "05060101";
	m_mapDlt2Sg["9C12"] = "05060102";
	m_mapDlt2Sg["9C13"] = "05060103";
	m_mapDlt2Sg["9C14"] = "05060104";
	/*m_mapSg2Dlt["05060100"] = "901F";
	m_mapSg2Dlt["05060101"] = "901F";
	m_mapSg2Dlt["05060102"] = "901F";
	m_mapSg2Dlt["05060103"] = "901F";
	m_mapSg2Dlt["05060104"] = "901F";
	m_mapDlt2Sg["9010"] = "05060100";
	m_mapDlt2Sg["9011"] = "05060101";
	m_mapDlt2Sg["9012"] = "05060102";
	m_mapDlt2Sg["9013"] = "05060103";
	m_mapDlt2Sg["9014"] = "05060104";*/

	// 上日电量:反向有功
	m_mapSg2Dlt["05060200"] = "9C2F";
	m_mapSg2Dlt["05060201"] = "9C2F";
	m_mapSg2Dlt["05060202"] = "9C2F";
	m_mapSg2Dlt["05060203"] = "9C2F";
	m_mapSg2Dlt["05060204"] = "9C2F";
	m_mapDlt2Sg["9C20"] = "05060200";
	m_mapDlt2Sg["9C21"] = "05060201";
	m_mapDlt2Sg["9C22"] = "05060202";
	m_mapDlt2Sg["9C23"] = "05060203";
	m_mapDlt2Sg["9C24"] = "05060204";
	/*m_mapSg2Dlt["05060200"] = "902F";
	m_mapSg2Dlt["05060201"] = "902F";
	m_mapSg2Dlt["05060202"] = "902F";
	m_mapSg2Dlt["05060203"] = "902F";
	m_mapSg2Dlt["05060204"] = "902F";
	m_mapDlt2Sg["9020"] = "05060200";
	m_mapDlt2Sg["9021"] = "05060201";
	m_mapDlt2Sg["9022"] = "05060202";
	m_mapDlt2Sg["9023"] = "05060203";
	m_mapDlt2Sg["9024"] = "05060204";*/


	// 上日电量:正向无功
	m_mapSg2Dlt["05060300"] = "9C3F";
	m_mapSg2Dlt["05060301"] = "9C3F";
	m_mapSg2Dlt["05060302"] = "9C3F";
	m_mapSg2Dlt["05060303"] = "9C3F";
	m_mapSg2Dlt["05060304"] = "9C3F";
	m_mapDlt2Sg["9C30"] = "05060300";
	m_mapDlt2Sg["9C31"] = "05060301";
	m_mapDlt2Sg["9C32"] = "05060302";
	m_mapDlt2Sg["9C33"] = "05060303";
	m_mapDlt2Sg["9C34"] = "05060304";
	/*m_mapSg2Dlt["05060300"] = "911F";
	m_mapSg2Dlt["05060301"] = "911F";
	m_mapSg2Dlt["05060302"] = "911F";
	m_mapSg2Dlt["05060303"] = "911F";
	m_mapSg2Dlt["05060304"] = "911F";
	m_mapDlt2Sg["9110"] = "05060300";
	m_mapDlt2Sg["9111"] = "05060301";
	m_mapDlt2Sg["9112"] = "05060302";
	m_mapDlt2Sg["9113"] = "05060303";
	m_mapDlt2Sg["9114"] = "05060304";*/

	// 上日电量:反向无功
	m_mapSg2Dlt["05060400"] = "9C4F";
	m_mapSg2Dlt["05060401"] = "9C4F";
	m_mapSg2Dlt["05060402"] = "9C4F";
	m_mapSg2Dlt["05060403"] = "9C4F";
	m_mapSg2Dlt["05060404"] = "9C4F";
	m_mapDlt2Sg["9C40"] = "05060400";
	m_mapDlt2Sg["9C41"] = "05060401";
	m_mapDlt2Sg["9C42"] = "05060402";
	m_mapDlt2Sg["9C43"] = "05060403";
	m_mapDlt2Sg["9C44"] = "05060404";
	/*m_mapSg2Dlt["05060400"] = "912F";
	m_mapSg2Dlt["05060401"] = "912F";
	m_mapSg2Dlt["05060402"] = "912F";
	m_mapSg2Dlt["05060403"] = "912F";
	m_mapSg2Dlt["05060404"] = "912F";
	m_mapDlt2Sg["9120"] = "05060400";
	m_mapDlt2Sg["9121"] = "05060401";
	m_mapDlt2Sg["9122"] = "05060402";
	m_mapDlt2Sg["9123"] = "05060403";
	m_mapDlt2Sg["9124"] = "05060404";*/

	// 上日一象限无功电量
	m_mapSg2Dlt["05060500"] = "9C5F";
	m_mapSg2Dlt["05060501"] = "9C5F";
	m_mapSg2Dlt["05060502"] = "9C5F";
	m_mapSg2Dlt["05060503"] = "9C5F";
	m_mapSg2Dlt["05060504"] = "9C5F";
	m_mapDlt2Sg["9C50"] = "05060500";
	m_mapDlt2Sg["9C51"] = "05060501";
	m_mapDlt2Sg["9C52"] = "05060502";
	m_mapDlt2Sg["9C53"] = "05060503";
	m_mapDlt2Sg["9C54"] = "05060504";
	/*m_mapSg2Dlt["05060500"] = "913F";
	m_mapSg2Dlt["05060501"] = "913F";
	m_mapSg2Dlt["05060502"] = "913F";
	m_mapSg2Dlt["05060503"] = "913F";
	m_mapSg2Dlt["05060504"] = "913F";
	m_mapDlt2Sg["9130"] = "05060500";
	m_mapDlt2Sg["9131"] = "05060501";
	m_mapDlt2Sg["9132"] = "05060502";
	m_mapDlt2Sg["9133"] = "05060503";
	m_mapDlt2Sg["9134"] = "05060504";*/

	// 上日二象限无功电量
	m_mapSg2Dlt["05060600"] = "9C7F";
	m_mapSg2Dlt["05060601"] = "9C7F";
	m_mapSg2Dlt["05060602"] = "9C7F";
	m_mapSg2Dlt["05060603"] = "9C7F";
	m_mapSg2Dlt["05060604"] = "9C7F";
	m_mapDlt2Sg["9C70"] = "05060600";
	m_mapDlt2Sg["9C71"] = "05060601";
	m_mapDlt2Sg["9C72"] = "05060602";
	m_mapDlt2Sg["9C73"] = "05060603";
	m_mapDlt2Sg["9C74"] = "05060604";
	/*m_mapSg2Dlt["05060600"] = "915F";
	m_mapSg2Dlt["05060601"] = "915F";
	m_mapSg2Dlt["05060602"] = "915F";
	m_mapSg2Dlt["05060603"] = "915F";
	m_mapSg2Dlt["05060604"] = "915F";
	m_mapDlt2Sg["9150"] = "05060600";
	m_mapDlt2Sg["9151"] = "05060601";
	m_mapDlt2Sg["9152"] = "05060602";
	m_mapDlt2Sg["9153"] = "05060603";
	m_mapDlt2Sg["9154"] = "05060604";*/

	// 上日三象限无功电量
	m_mapSg2Dlt["05060700"] = "9C8F";
	m_mapSg2Dlt["05060701"] = "9C8F";
	m_mapSg2Dlt["05060702"] = "9C8F";
	m_mapSg2Dlt["05060703"] = "9C8F";
	m_mapSg2Dlt["05060704"] = "9C8F";
	m_mapDlt2Sg["9C80"] = "05060700";
	m_mapDlt2Sg["9C81"] = "05060701";
	m_mapDlt2Sg["9C82"] = "05060702";
	m_mapDlt2Sg["9C83"] = "05060703";
	m_mapDlt2Sg["9C84"] = "05060704";
	/*m_mapSg2Dlt["05060700"] = "916F";
	m_mapSg2Dlt["05060701"] = "916F";
	m_mapSg2Dlt["05060702"] = "916F";
	m_mapSg2Dlt["05060703"] = "916F";
	m_mapSg2Dlt["05060704"] = "916F";
	m_mapDlt2Sg["9160"] = "05060700";
	m_mapDlt2Sg["9161"] = "05060701";
	m_mapDlt2Sg["9162"] = "05060702";
	m_mapDlt2Sg["9163"] = "05060703";
	m_mapDlt2Sg["9164"] = "05060704";*/

	// 上日四象限无功电量
	m_mapSg2Dlt["05060800"] = "9C6F";
	m_mapSg2Dlt["05060801"] = "9C6F";
	m_mapSg2Dlt["05060802"] = "9C6F";
	m_mapSg2Dlt["05060803"] = "9C6F";
	m_mapSg2Dlt["05060804"] = "9C6F";
	m_mapDlt2Sg["9C60"] = "05060800";
	m_mapDlt2Sg["9C61"] = "05060801";
	m_mapDlt2Sg["9C62"] = "05060802";
	m_mapDlt2Sg["9C63"] = "05060803";
	m_mapDlt2Sg["9C64"] = "05060804";
	/*m_mapSg2Dlt["05060800"] = "914F";
	m_mapSg2Dlt["05060801"] = "914F";
	m_mapSg2Dlt["05060802"] = "914F";
	m_mapSg2Dlt["05060803"] = "914F";
	m_mapSg2Dlt["05060804"] = "914F";
	m_mapDlt2Sg["9140"] = "05060800";
	m_mapDlt2Sg["9141"] = "05060801";
	m_mapDlt2Sg["9142"] = "05060802";
	m_mapDlt2Sg["9143"] = "05060803";
	m_mapDlt2Sg["9144"] = "05060804";*/

	
	// 上月电量:正向有功
	m_mapSg2Dlt["00010001"] = "941F";
	m_mapSg2Dlt["00010101"] = "941F";
	m_mapSg2Dlt["00010201"] = "941F";
	m_mapSg2Dlt["00010301"] = "941F";
	m_mapSg2Dlt["00010401"] = "941F";
	m_mapDlt2Sg["9410"] = "00010001";
	m_mapDlt2Sg["9411"] = "00010101";
	m_mapDlt2Sg["9412"] = "00010201";
	m_mapDlt2Sg["9413"] = "00010301";
	m_mapDlt2Sg["9414"] = "00010401";

	// 上月电量:反向有功
	m_mapSg2Dlt["00020001"] = "942F";
	m_mapSg2Dlt["00020101"] = "942F";
	m_mapSg2Dlt["00020201"] = "942F";
	m_mapSg2Dlt["00020301"] = "942F";
	m_mapSg2Dlt["00020401"] = "942F";
	m_mapDlt2Sg["9420"] = "00020001";
	m_mapDlt2Sg["9421"] = "00020101";
	m_mapDlt2Sg["9422"] = "00020201";
	m_mapDlt2Sg["9423"] = "00020301";
	m_mapDlt2Sg["9424"] = "00020401";

	// 上月电量:正向无功
	m_mapSg2Dlt["00030001"] = "951F";
	m_mapSg2Dlt["00030101"] = "951F";
	m_mapSg2Dlt["00030201"] = "951F";
	m_mapSg2Dlt["00030301"] = "951F";
	m_mapSg2Dlt["00030401"] = "951F";
	m_mapDlt2Sg["9510"] = "00030001";
	m_mapDlt2Sg["9511"] = "00030101";
	m_mapDlt2Sg["9512"] = "00030201";
	m_mapDlt2Sg["9513"] = "00030301";
	m_mapDlt2Sg["9514"] = "00030401";

	// 上月电量:反向无功
	m_mapSg2Dlt["00040001"] = "952F";
	m_mapSg2Dlt["00040101"] = "952F";
	m_mapSg2Dlt["00040201"] = "952F";
	m_mapSg2Dlt["00040301"] = "952F";
	m_mapSg2Dlt["00040401"] = "952F";
	m_mapDlt2Sg["9520"] = "00040001";
	m_mapDlt2Sg["9521"] = "00040101";
	m_mapDlt2Sg["9522"] = "00040201";
	m_mapDlt2Sg["9523"] = "00040301";
	m_mapDlt2Sg["9524"] = "00040401";

	// 上月一象限无功电量
	m_mapSg2Dlt["00050001"] = "953F";
	m_mapSg2Dlt["00050101"] = "953F";
	m_mapSg2Dlt["00050201"] = "953F";
	m_mapSg2Dlt["00050301"] = "953F";
	m_mapSg2Dlt["00050401"] = "953F";
	m_mapDlt2Sg["9530"] = "00050001";
	m_mapDlt2Sg["9531"] = "00050101";
	m_mapDlt2Sg["9532"] = "00050201";
	m_mapDlt2Sg["9533"] = "00050301";
	m_mapDlt2Sg["9534"] = "00050401";

	// 上月二象限无功电量
	m_mapSg2Dlt["00060001"] = "955F";
	m_mapSg2Dlt["00060101"] = "955F";
	m_mapSg2Dlt["00060201"] = "955F";
	m_mapSg2Dlt["00060301"] = "955F";
	m_mapSg2Dlt["00060401"] = "955F";
	m_mapDlt2Sg["9550"] = "00060001";
	m_mapDlt2Sg["9551"] = "00060101";
	m_mapDlt2Sg["9552"] = "00060201";
	m_mapDlt2Sg["9553"] = "00060301";
	m_mapDlt2Sg["9554"] = "00060401";

	// 上月三象限无功电量
	m_mapSg2Dlt["00070001"] = "956F";
	m_mapSg2Dlt["00070101"] = "956F";
	m_mapSg2Dlt["00070201"] = "956F";
	m_mapSg2Dlt["00070301"] = "956F";
	m_mapSg2Dlt["00070401"] = "956F";
	m_mapDlt2Sg["9560"] = "00070001";
	m_mapDlt2Sg["9561"] = "00070101";
	m_mapDlt2Sg["9562"] = "00070201";
	m_mapDlt2Sg["9563"] = "00070301";
	m_mapDlt2Sg["9564"] = "00070401";

	// 上月四象限无功电量
	m_mapSg2Dlt["00080001"] = "954F";
	m_mapSg2Dlt["00080101"] = "954F";
	m_mapSg2Dlt["00080201"] = "954F";
	m_mapSg2Dlt["00080301"] = "954F";
	m_mapSg2Dlt["00080401"] = "954F";
	m_mapDlt2Sg["9540"] = "00080001";
	m_mapDlt2Sg["9541"] = "00080101";
	m_mapDlt2Sg["9542"] = "00080201";
	m_mapDlt2Sg["9543"] = "00080301";
	m_mapDlt2Sg["9544"] = "00080401";

	// 月冻结最大需量
	m_mapDlt2Sg["A410"] = "01010001";	// 正向有功
	m_mapDlt2Sg["A411"] = "01010101";
	m_mapDlt2Sg["A412"] = "01010201";
	m_mapDlt2Sg["A413"] = "01010301";
	m_mapDlt2Sg["A414"] = "01010401";
	m_mapDlt2Sg["A420"] = "01020001";	// 反向有功
	m_mapDlt2Sg["A421"] = "01020101";
	m_mapDlt2Sg["A422"] = "01020201";
	m_mapDlt2Sg["A423"] = "01020301";
	m_mapDlt2Sg["A424"] = "01020401";
	m_mapDlt2Sg["A510"] = "01030001";	// 正向无功
	m_mapDlt2Sg["A511"] = "01030101";
	m_mapDlt2Sg["A512"] = "01030201";
	m_mapDlt2Sg["A513"] = "01030301";
	m_mapDlt2Sg["A514"] = "01030401";
	m_mapDlt2Sg["A520"] = "01040001";	// 反向无功
	m_mapDlt2Sg["A521"] = "01040101";
	m_mapDlt2Sg["A522"] = "01040201";
	m_mapDlt2Sg["A523"] = "01040301";
	m_mapDlt2Sg["A524"] = "01040401";
	// 月冻结最大需量发生时间
	m_mapDlt2Sg["B410"] = "01010001";	// 正向有功
	m_mapDlt2Sg["B411"] = "01010101";
	m_mapDlt2Sg["B412"] = "01010201";
	m_mapDlt2Sg["B413"] = "01010301";
	m_mapDlt2Sg["B414"] = "01010401";
	m_mapDlt2Sg["B420"] = "01020001";	// 反向有功
	m_mapDlt2Sg["B421"] = "01020101";
	m_mapDlt2Sg["B422"] = "01020201";
	m_mapDlt2Sg["B423"] = "01020301";
	m_mapDlt2Sg["B424"] = "01020401";
	m_mapDlt2Sg["B510"] = "01030001";	// 正向无功
	m_mapDlt2Sg["B511"] = "01030101";
	m_mapDlt2Sg["B512"] = "01030201";
	m_mapDlt2Sg["B513"] = "01030301";
	m_mapDlt2Sg["B514"] = "01030401";
	m_mapDlt2Sg["B520"] = "01040001";	// 反向无功
	m_mapDlt2Sg["B521"] = "01040101";
	m_mapDlt2Sg["B522"] = "01040201";
	m_mapDlt2Sg["B523"] = "01040301";
	m_mapDlt2Sg["B524"] = "01040401";

		
	// 当前电量:正向有功
	m_mapSg2Dlt["00010000"] = "901F";
	m_mapSg2Dlt["00010100"] = "901F";
	m_mapSg2Dlt["00010200"] = "901F";
	m_mapSg2Dlt["00010300"] = "901F";
	m_mapSg2Dlt["00010400"] = "901F";
	m_mapDlt2Sg["9010"] = "00010000";
	m_mapDlt2Sg["9011"] = "00010100";
	m_mapDlt2Sg["9012"] = "00010200";
	m_mapDlt2Sg["9013"] = "00010300";
	m_mapDlt2Sg["9014"] = "00010400";
	// 当前电量:反向有功
	m_mapSg2Dlt["00020000"] = "902F";
	m_mapSg2Dlt["00020100"] = "902F";
	m_mapSg2Dlt["00020200"] = "902F";
	m_mapSg2Dlt["00020300"] = "902F";
	m_mapSg2Dlt["00020400"] = "902F";
	m_mapDlt2Sg["9020"] = "00020000";
	m_mapDlt2Sg["9021"] = "00020100";
	m_mapDlt2Sg["9022"] = "00020200";
	m_mapDlt2Sg["9023"] = "00020300";
	m_mapDlt2Sg["9024"] = "00020400";
	// 当前电量:正向无功
	m_mapSg2Dlt["00030000"] = "911F";
	m_mapSg2Dlt["00030100"] = "911F";
	m_mapSg2Dlt["00030200"] = "911F";
	m_mapSg2Dlt["00030300"] = "911F";
	m_mapSg2Dlt["00030400"] = "911F";
	m_mapDlt2Sg["9110"] = "00030000";
	m_mapDlt2Sg["9111"] = "00030100";
	m_mapDlt2Sg["9112"] = "00030200";
	m_mapDlt2Sg["9113"] = "00030300";
	m_mapDlt2Sg["9114"] = "00030400";
	// 当前电量:反向无功
	m_mapSg2Dlt["00040000"] = "912F";
	m_mapSg2Dlt["00040100"] = "912F";
	m_mapSg2Dlt["00040200"] = "912F";
	m_mapSg2Dlt["00040300"] = "912F";
	m_mapSg2Dlt["00040400"] = "912F";
	m_mapDlt2Sg["9120"] = "00040000";
	m_mapDlt2Sg["9121"] = "00040100";
	m_mapDlt2Sg["9122"] = "00040200";
	m_mapDlt2Sg["9123"] = "00040300";
	m_mapDlt2Sg["9124"] = "00040400";

	// 当前一象限无功
	m_mapSg2Dlt["00050000"] = "913F";
	m_mapSg2Dlt["00050100"] = "913F";
	m_mapSg2Dlt["00050200"] = "913F";
	m_mapSg2Dlt["00050300"] = "913F";
	m_mapSg2Dlt["00050400"] = "913F";
	m_mapDlt2Sg["9130"] = "00050000";
	m_mapDlt2Sg["9131"] = "00050100";
	m_mapDlt2Sg["9132"] = "00050200";
	m_mapDlt2Sg["9133"] = "00050300";
	m_mapDlt2Sg["9134"] = "00050400";
	// 当前二象限无功
	m_mapSg2Dlt["00060000"] = "915F";
	m_mapSg2Dlt["00060100"] = "915F";
	m_mapSg2Dlt["00060200"] = "915F";
	m_mapSg2Dlt["00060300"] = "915F";
	m_mapSg2Dlt["00060400"] = "915F";
	m_mapDlt2Sg["9150"] = "00060000";
	m_mapDlt2Sg["9151"] = "00060100";
	m_mapDlt2Sg["9152"] = "00060200";
	m_mapDlt2Sg["9153"] = "00060300";
	m_mapDlt2Sg["9154"] = "00060400";
	// 当前三象限无功
	m_mapSg2Dlt["00070000"] = "916F";
	m_mapSg2Dlt["00070100"] = "916F";
	m_mapSg2Dlt["00070200"] = "916F";
	m_mapSg2Dlt["00070300"] = "916F";
	m_mapSg2Dlt["00070400"] = "916F";
	m_mapDlt2Sg["9160"] = "00070000";
	m_mapDlt2Sg["9161"] = "00070100";
	m_mapDlt2Sg["9162"] = "00070200";
	m_mapDlt2Sg["9163"] = "00070300";
	m_mapDlt2Sg["9164"] = "00070400";
	// 当前四象限无功
	m_mapSg2Dlt["00080000"] = "914F";
	m_mapSg2Dlt["00080100"] = "914F";
	m_mapSg2Dlt["00080200"] = "914F";
	m_mapSg2Dlt["00080300"] = "914F";
	m_mapSg2Dlt["00080400"] = "914F";
	m_mapDlt2Sg["9140"] = "00080000";
	m_mapDlt2Sg["9141"] = "00080100";
	m_mapDlt2Sg["9142"] = "00080200";
	m_mapDlt2Sg["9143"] = "00080300";
	m_mapDlt2Sg["9144"] = "00080400";

	// 瞬时量:电压数据
	m_mapSg2Dlt["02010100"] = "B61F";
	m_mapSg2Dlt["02010200"] = "B61F";
	m_mapSg2Dlt["02010300"] = "B61F";
	m_mapDlt2Sg["B610"] = "02010100";
	m_mapDlt2Sg["B611"] = "02010200";
	m_mapDlt2Sg["B612"] = "02010300";
	// 瞬时量:电流数据
	m_mapSg2Dlt["02020100"] = "B62F";
	m_mapSg2Dlt["02020200"] = "B62F";
	m_mapSg2Dlt["02020300"] = "B62F";
	m_mapDlt2Sg["B620"] = "02020100";
	m_mapDlt2Sg["B621"] = "02020200";
	m_mapDlt2Sg["B622"] = "02020300";
	// 瞬时量:有功功率
	m_mapSg2Dlt["02030000"] = "B63F";
	m_mapSg2Dlt["02030100"] = "B63F";
	m_mapSg2Dlt["02030200"] = "B63F";
	m_mapSg2Dlt["02030300"] = "B63F";
	m_mapDlt2Sg["B630"] = "02030000";
	m_mapDlt2Sg["B631"] = "02030100";
	m_mapDlt2Sg["B632"] = "02030200";
	m_mapDlt2Sg["B633"] = "02030300";
	// 瞬时量:无功功率
	m_mapSg2Dlt["02040000"] = "B64F";
	m_mapSg2Dlt["02040100"] = "B64F";
	m_mapSg2Dlt["02040200"] = "B64F";
	m_mapSg2Dlt["02040300"] = "B64F";
	m_mapDlt2Sg["B640"] = "02040000";
	m_mapDlt2Sg["B641"] = "02040100";
	m_mapDlt2Sg["B642"] = "02040200";
	m_mapDlt2Sg["B643"] = "02040300";
	// 瞬时量:功率因数
	m_mapSg2Dlt["02060000"] = "B65F";
	m_mapSg2Dlt["02060100"] = "B65F";
	m_mapSg2Dlt["02060200"] = "B65F";
	m_mapSg2Dlt["02060300"] = "B65F";
	m_mapDlt2Sg["B650"] = "02060000";
	m_mapDlt2Sg["B651"] = "02060100";
	m_mapDlt2Sg["B652"] = "02060200";
	m_mapDlt2Sg["B653"] = "02060300";

	// 当前*最大需量
	m_mapDlt2Sg["A010"] = "01010000";
	m_mapDlt2Sg["A011"] = "01010100";
	m_mapDlt2Sg["A012"] = "01010200";
	m_mapDlt2Sg["A013"] = "01010300";
	m_mapDlt2Sg["A014"] = "01010400";
	m_mapDlt2Sg["A020"] = "01020000";
	m_mapDlt2Sg["A021"] = "01020100";
	m_mapDlt2Sg["A022"] = "01020200";
	m_mapDlt2Sg["A023"] = "01020300";
	m_mapDlt2Sg["A024"] = "01020400";
	//  当前*最大需量发生时间
	m_mapDlt2Sg["B010"] = "01010000";
	m_mapDlt2Sg["B011"] = "01010100";
	m_mapDlt2Sg["B012"] = "01010200";
	m_mapDlt2Sg["B013"] = "01010300";
	m_mapDlt2Sg["B014"] = "01010400";
	m_mapDlt2Sg["B020"] = "01020000";
	m_mapDlt2Sg["B021"] = "01020100";
	m_mapDlt2Sg["B022"] = "01020200";
	m_mapDlt2Sg["B023"] = "01020300";
	m_mapDlt2Sg["B024"] = "01020400";


	////////////////////////////////////////////////////////////////////////////////////
	// 曲线数据
	// 正有，反有，正无，反无
	m_mCurveSg2Dlt["00010000"] = "9B1F";
	m_mCurveSg2Dlt["00010100"] = "9B1F";
	m_mCurveSg2Dlt["00010200"] = "9B1F";
	m_mCurveSg2Dlt["00010300"] = "9B1F";
	m_mCurveSg2Dlt["00010400"] = "9B1F";
	m_mCurveSg2Dlt["00020000"] = "9B1F";
	m_mCurveSg2Dlt["00020100"] = "9B1F";
	m_mCurveSg2Dlt["00020200"] = "9B1F";
	m_mCurveSg2Dlt["00020300"] = "9B1F";
	m_mCurveSg2Dlt["00020400"] = "9B1F";
	m_mCurveSg2Dlt["00030000"] = "9B1F";
	m_mCurveSg2Dlt["00030100"] = "9B1F";
	m_mCurveSg2Dlt["00030200"] = "9B1F";
	m_mCurveSg2Dlt["00030300"] = "9B1F";
	m_mCurveSg2Dlt["00030400"] = "9B1F";
	m_mCurveSg2Dlt["00040000"] = "9B1F";
	m_mCurveSg2Dlt["00040100"] = "9B1F";
	m_mCurveSg2Dlt["00040200"] = "9B1F";
	m_mCurveSg2Dlt["00040300"] = "9B1F";
	m_mCurveSg2Dlt["00040400"] = "9B1F";
	m_mCurveDlt2sg["9B10"] = "00010000";
	m_mCurveDlt2sg["9B11"] = "00010100";
	m_mCurveDlt2sg["9B12"] = "00010200";
	m_mCurveDlt2sg["9B13"] = "00010300";
	m_mCurveDlt2sg["9B14"] = "00010400";
	m_mCurveDlt2sg["9B1A"] = "00020000";
	m_mCurveDlt2sg["9B1B"] = "00020100";
	m_mCurveDlt2sg["9B1C"] = "00020200";
	m_mCurveDlt2sg["9B1D"] = "00020300";
	m_mCurveDlt2sg["9B1E"] = "00020400";
	m_mCurveDlt2sg["9B15"] = "00030000";
	m_mCurveDlt2sg["9B16"] = "00030100";
	m_mCurveDlt2sg["9B17"] = "00030200";
	m_mCurveDlt2sg["9B18"] = "00030300";
	m_mCurveDlt2sg["9B19"] = "00030400";
	m_mCurveDlt2sg["9B1F"] = "00040000";
	m_mCurveDlt2sg["9B20"] = "00040100";
	m_mCurveDlt2sg["9B21"] = "00040200";
	m_mCurveDlt2sg["9B22"] = "00040300";
	m_mCurveDlt2sg["9B23"] = "00040400";
	
	// 一、二、三、四象限无功
	m_mCurveSg2Dlt["00050000"] = "9B5F";
	m_mCurveSg2Dlt["00050100"] = "9B5F";
	m_mCurveSg2Dlt["00050200"] = "9B5F";
	m_mCurveSg2Dlt["00050300"] = "9B5F";
	m_mCurveSg2Dlt["00050400"] = "9B5F";
	m_mCurveSg2Dlt["00060000"] = "9B5F";
	m_mCurveSg2Dlt["00060100"] = "9B5F";
	m_mCurveSg2Dlt["00060200"] = "9B5F";
	m_mCurveSg2Dlt["00060300"] = "9B5F";
	m_mCurveSg2Dlt["00060400"] = "9B5F";
	m_mCurveSg2Dlt["00070000"] = "9B5F";
	m_mCurveSg2Dlt["00070100"] = "9B5F";
	m_mCurveSg2Dlt["00070200"] = "9B5F";
	m_mCurveSg2Dlt["00070300"] = "9B5F";
	m_mCurveSg2Dlt["00070400"] = "9B5F";
	m_mCurveSg2Dlt["00080000"] = "9B5F";
	m_mCurveSg2Dlt["00080100"] = "9B5F";
	m_mCurveSg2Dlt["00080200"] = "9B5F";
	m_mCurveSg2Dlt["00080300"] = "9B5F";
	m_mCurveSg2Dlt["00080400"] = "9B5F";
	m_mCurveDlt2sg["9B50"] = "00050000";
	m_mCurveDlt2sg["9B51"] = "00050100";
	m_mCurveDlt2sg["9B52"] = "00050200";
	m_mCurveDlt2sg["9B53"] = "00050300";
	m_mCurveDlt2sg["9B54"] = "00050400";
	m_mCurveDlt2sg["9B55"] = "00060000";
	m_mCurveDlt2sg["9B56"] = "00060100";
	m_mCurveDlt2sg["9B57"] = "00060200";
	m_mCurveDlt2sg["9B58"] = "00060300";
	m_mCurveDlt2sg["9B59"] = "00060400";
	m_mCurveDlt2sg["9B5A"] = "00070000";
	m_mCurveDlt2sg["9B5B"] = "00070100";
	m_mCurveDlt2sg["9B5C"] = "00070200";
	m_mCurveDlt2sg["9B5D"] = "00070300";
	m_mCurveDlt2sg["9B5E"] = "00070400";
	m_mCurveDlt2sg["9B5F"] = "00080000";
	m_mCurveDlt2sg["9B60"] = "00080100";
	m_mCurveDlt2sg["9B61"] = "00080200";
	m_mCurveDlt2sg["9B62"] = "00080300";
	m_mCurveDlt2sg["9B63"] = "00080400";

	// 电压，电流，有功功率，无功功率，功率因数
	m_mCurveSg2Dlt["02010100"] = "BB1F";
	m_mCurveSg2Dlt["02010200"] = "BB1F";
	m_mCurveSg2Dlt["02010300"] = "BB1F";
	m_mCurveSg2Dlt["02020100"] = "BB1F";
	m_mCurveSg2Dlt["02020200"] = "BB1F";
	m_mCurveSg2Dlt["02020300"] = "BB1F";
	m_mCurveSg2Dlt["02030000"] = "BB1F";
	m_mCurveSg2Dlt["02030100"] = "BB1F";
	m_mCurveSg2Dlt["02030200"] = "BB1F";
	m_mCurveSg2Dlt["02030300"] = "BB1F";
	m_mCurveSg2Dlt["02040000"] = "BB1F";
	m_mCurveSg2Dlt["02040100"] = "BB1F";
	m_mCurveSg2Dlt["02040200"] = "BB1F";
	m_mCurveSg2Dlt["02040300"] = "BB1F";
	m_mCurveSg2Dlt["02060000"] = "BB1F";
	m_mCurveSg2Dlt["02060100"] = "BB1F";
	m_mCurveSg2Dlt["02060200"] = "BB1F";
	m_mCurveSg2Dlt["02060300"] = "BB1F";
	m_mCurveDlt2sg["BB10"] = "02010100";
	m_mCurveDlt2sg["BB11"] = "02010200";
	m_mCurveDlt2sg["BB12"] = "02010300";
	m_mCurveDlt2sg["BB13"] = "02020100";
	m_mCurveDlt2sg["BB14"] = "02020200";
	m_mCurveDlt2sg["BB15"] = "02020300";
	m_mCurveDlt2sg["BB16"] = "02030000";
	m_mCurveDlt2sg["BB17"] = "02030100";
	m_mCurveDlt2sg["BB18"] = "02030200";
	m_mCurveDlt2sg["BB19"] = "02030300";
	m_mCurveDlt2sg["BB1A"] = "02040000";
	m_mCurveDlt2sg["BB1B"] = "02040100";
	m_mCurveDlt2sg["BB1C"] = "02040200";
	m_mCurveDlt2sg["BB1D"] = "02040300";
	m_mCurveDlt2sg["BB1E"] = "02060000";
	m_mCurveDlt2sg["BB1F"] = "02060100";
	m_mCurveDlt2sg["BB20"] = "02060200";
	m_mCurveDlt2sg["BB21"] = "02060300";
}