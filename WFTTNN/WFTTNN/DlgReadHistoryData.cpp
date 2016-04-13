// DlgReadHistoryStory.cpp : 实现文件
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgReadHistoryData.h"
#include ".\dlgreadhistorydata.h"
#include "ItemData.h"
#include "MainFrm.h"
#include <time.h>
#include "ptcl_codec/data_parse_csg.h"
#include "LogMgr.h"

// CDlgReadHistoryData 对话框

IMPLEMENT_DYNAMIC(CDlgReadHistoryData, CDialog)
	CDlgReadHistoryData::CDlgReadHistoryData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReadHistoryData::IDD, pParent)
	, m_strTerminalAddr("")
	,m_retryCount(0)
	, m_timeout(30)
	, m_taskState(0)
	, m_pChannel(NULL)
	, m_layoutCtrl(this)
{
}

CDlgReadHistoryData::~CDlgReadHistoryData()
{
}

void CDlgReadHistoryData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATAITEM, m_ctrlDataItemList);
	DDX_Control(pDX, IDC_LIST_DAY_DATAITEM, m_ctrlDataItemList);
	DDX_Control(pDX, IDC_LIST_MON_DATAITEM, m_ctrlDataItemList);
	DDX_Control(pDX, IDC_COMBO1, m_cmbMeterList);
	DDX_Control(pDX, IDC_LIST_OPT_RESULT, m_ctrlOptResult);
	DDX_Control(pDX, IDC_LIST_METER2, m_ctrlMeterList);
	DDX_Control(pDX, IDC_PROGRESS1, m_pcHistory);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START_DATE, m_ctrlDate_start);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_ctrlTime_start);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START_DATE2, m_ctrlDate_end);
	DDX_Control(pDX, IDC_DATETIMEPICKER3, m_ctrlTime_end);
	DDX_Control(pDX, IDC_COMBO2, CComboBox_deny);
	DDX_Control(pDX, IDC_LIST_DAY_DATAITEM, m_ctrlDayDataItemList);
	DDX_Control(pDX, IDC_LIST_MON_DATAITEM, m_ctrlMonDataItemList);
}


BEGIN_MESSAGE_MAP(CDlgReadHistoryData, CDialog)
	ON_BN_CLICKED(IDC_BTN_START, OnBnClickedBtnStart)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_START1, OnBnClickedBtnStart2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPT_RESULT, &CDlgReadHistoryData::OnNMDblclkListOptResult)
END_MESSAGE_MAP()


BOOL CDlgReadHistoryData::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwExtendStyle = m_ctrlDataItemList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlDataItemList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlDataItemList.InsertColumn(0,"#",LVCFMT_LEFT,20);
	m_ctrlDataItemList.InsertColumn(1,"数据项标识",LVCFMT_LEFT,80);
	m_ctrlDataItemList.InsertColumn(2,"数据项名称",LVCFMT_LEFT,400);

	dwExtendStyle = m_ctrlDayDataItemList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlDayDataItemList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlDayDataItemList.InsertColumn(0,"#",LVCFMT_LEFT,20);
	m_ctrlDayDataItemList.InsertColumn(1,"数据项标识",LVCFMT_LEFT,80);
	m_ctrlDayDataItemList.InsertColumn(2,"数据项名称",LVCFMT_LEFT,400);

	dwExtendStyle = m_ctrlMonDataItemList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlMonDataItemList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlMonDataItemList.InsertColumn(0,"#",LVCFMT_LEFT,20);
	m_ctrlMonDataItemList.InsertColumn(1,"数据项标识",LVCFMT_LEFT,80);
	m_ctrlMonDataItemList.InsertColumn(2,"数据项名称",LVCFMT_LEFT,400);

	dwExtendStyle = m_ctrlOptResult.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ctrlOptResult.SetExtendedStyle(dwExtendStyle);	
	m_ctrlOptResult.InsertColumn(0,"电表序号",LVCFMT_LEFT,100);
	m_ctrlOptResult.InsertColumn(1,"数据项名称",LVCFMT_LEFT,400);
	m_ctrlOptResult.InsertColumn(2,"时间",LVCFMT_RIGHT,200);
	m_ctrlOptResult.InsertColumn(3,"数据值",LVCFMT_RIGHT,200);


	dwExtendStyle = m_ctrlMeterList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlMeterList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlMeterList.InsertColumn(0,"#",LVCFMT_CENTER,20);
	m_ctrlMeterList.InsertColumn(1,"电表序号",LVCFMT_LEFT,80);
	m_ctrlMeterList.InsertColumn(2,"电表名称",LVCFMT_LEFT,120);

	InitDataItem();	
	InitDayDataItem();
	InitMonDataItem();

	CRect rect;
	GetClientRect(&rect);
	m_layoutCtrl.SetOldRect(rect);
	return TRUE;
}

void CDlgReadHistoryData::InitDataItem()
{
	m_ctrlDataItemList.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	const char *strSQL = "SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 2	ORDER BY PARAM_ID DESC";
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		CString strParamID = _T("");
		CString strParamName = _T("");

		record.GetFieldValue("PARAM_ID",val);
		strParamID = val.bstrVal;

		record.GetFieldValue("PARAM_NAME",val);
		strParamName = val.bstrVal;

		m_ctrlDataItemList.InsertItem(0, "");
		m_ctrlDataItemList.SetItemText(0, 1, strParamID);
		m_ctrlDataItemList.SetItemText(0, 2, strParamName);

		DataTypeParam param;
		param.type = atoi(strParamID);
		param.desc = strParamName;
		m_mDataType[strParamID] = param;

		record.RecordMoveNext();
	}
}

void CDlgReadHistoryData::InitDayDataItem()
{
	m_ctrlDayDataItemList.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	const char *strSQL = "SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 3 ORDER BY PARAM_ID DESC";
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		CString strParamID = _T("");
		CString strParamName = _T("");

		record.GetFieldValue("PARAM_ID",val);
		strParamID = val.bstrVal;

		record.GetFieldValue("PARAM_NAME",val);
		strParamName = val.bstrVal;

		m_ctrlDayDataItemList.InsertItem(0, "");
		m_ctrlDayDataItemList.SetItemText(0, 1, strParamID);
		m_ctrlDayDataItemList.SetItemText(0, 2, strParamName);

		DataTypeParam param;
		param.type = atoi(strParamID);
		param.desc = strParamName;
		m_mDataType[strParamID] = param;

		record.RecordMoveNext();
	}
}

void CDlgReadHistoryData::InitMonDataItem()
{
	m_ctrlMonDataItemList.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	const char *strSQL = "SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 4 ORDER BY PARAM_ID DESC";
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		CString strParamID = _T("");
		CString strParamName = _T("");

		record.GetFieldValue("PARAM_ID",val);
		strParamID = val.bstrVal;

		record.GetFieldValue("PARAM_NAME",val);
		strParamName = val.bstrVal;

		m_ctrlMonDataItemList.InsertItem(0, "");
		m_ctrlMonDataItemList.SetItemText(0, 1, strParamID);
		m_ctrlMonDataItemList.SetItemText(0, 2, strParamName);

		DataTypeParam param;
		param.type = atoi(strParamID);
		param.desc = strParamName;
		m_mDataType[strParamID] = param;

		record.RecordMoveNext();
	}
}

void CDlgReadHistoryData::InitMeterList()
{	
	m_ctrlMeterList.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[256];
	sprintf(strSQL, "SELECT * FROM MP_ARCH WHERE RTU_ID = %d ORDER BY MP_ID", m_nTermNo);	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;

	int nRow = 0;
	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nMeterId = 0;
		CString strMeterName = "";
		CString strMeterId = "";

		record.GetFieldValue("MP_ID", nMeterId);
		strMeterId.Format("%d", nMeterId);
		record.GetFieldValue("MP_NAME",val);
		strMeterName = val.bstrVal;

		m_ctrlMeterList.InsertItem(nRow, "");
		m_ctrlMeterList.SetItemText(nRow, 1, strMeterId);
		m_ctrlMeterList.SetItemText(nRow, 2, strMeterName);		

		record.RecordMoveNext();
		nRow++;
	}

	m_cmbMeterList.SetCurSel(0);
}

void CDlgReadHistoryData::OnEvent(unsigned int nEventID, void *pParam)
{
	if (Event_Type_Item_Double_Click != nEventID || !::IsWindowVisible(this->GetSafeHwnd()))
		return;	

	if (ITEM_TYPE_TERMINAL == ((ItemData *)pParam)->m_type)
	{
		TerminalItemData *data = (TerminalItemData *)pParam;
		m_strTerminalAddr = data->m_addr;
		m_nTermNo = data->m_id;

		CString strTermNo = "";
		strTermNo.Format("当前终端：%d", data->m_id);
		GetDlgItem(IDC_TERMINAL)->SetWindowTextA(strTermNo);	

		InitMeterList();
	}	
}

void CDlgReadHistoryData::OnBnClickedBtnStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
	if (pMainFrm)
	{
		m_pChannel = pMainFrm->m_pDlgConn->m_pChannel;
	}

	if (!m_pChannel)
	{
		MessageBox("未连接终端，请先建立连接！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	m_ctrlOptResult.DeleteAllItems();

	while (!m_qTask.empty())
		m_qTask.pop();

	UpdateData(TRUE);

	int nPn = 0;
	CString strPn = "";
	CString strDataitem = "";
	for (int i = 0; i < m_ctrlMeterList.GetItemCount(); i++)
	{
		if (!m_ctrlMeterList.GetCheck(i))
			continue;

		strPn = m_ctrlMeterList.GetItemText(i, 1);
		nPn = atoi(strPn.GetBuffer(0));

		SYSTEMTIME tDate_start, tTime_start, tDate_end, tTime_end;
		m_ctrlDate_start.GetTime(&tDate_start);
		m_ctrlTime_start.GetTime(&tTime_start);	

		m_ctrlDate_end.GetTime(&tDate_end);
		m_ctrlTime_end.GetTime(&tTime_end);

		CTime cStart,cEnd;
		cStart = CTime((int)tDate_start.wYear, (int)tDate_start.wMonth, (int)tDate_start.wDay,
					(int)tTime_start.wHour, (int)tTime_start.wMinute, 0,-1);
		cEnd = CTime((int)tDate_end.wYear, (int)tDate_end.wMonth, (int)tDate_end.wDay,
					(int)tTime_end.wHour, (int)tTime_end.wMinute, 0,-1);


		//cStart = GetOnDay(cStart);

		int deny = CComboBox_deny.GetCurSel() + 1;
		if(deny == 0)
		{
			MessageBox("请选择数据密度！");
			return;
		}

		if (cStart > cEnd)
		{
			MessageBox("开始时间大于结束时间！");
			return;
		}

		int denyTime = getDenyTime(deny);


		for (int j = 0; j < m_ctrlDataItemList.GetItemCount(); j++)
		{
			if (!m_ctrlDataItemList.GetCheck(j))
				continue;

			strDataitem = m_ctrlDataItemList.GetItemText(j, 1);

			time_t start_t = cStart.GetTime();
			time_t end_t = cEnd.GetTime();
			start_t = start_t/denyTime * denyTime;
			// 规约规定曲线时间段是前闭后开区间
			while (start_t < end_t)
			{					
				Task task(TASK_READ_HISTORY, m_strTerminalAddr.GetBuffer(0), nPn, strDataitem.GetBuffer(0), start_t, start_t + 60, deny);
				m_qTask.push(task);
				start_t = start_t  + denyTime;				
			}
			
		}
		for (int j = 0; j < m_ctrlDayDataItemList.GetItemCount(); j++)
		{
			if (!m_ctrlDayDataItemList.GetCheck(j))
				continue;

			strDataitem = m_ctrlDayDataItemList.GetItemText(j, 1);
			cStart = GetOnDay(cStart);
			time_t start_t = cStart.GetTime();
			time_t end_t = cEnd.GetTime();
			while (start_t < end_t)
			{
				Task task(TASK_READ_HISTORY, m_strTerminalAddr.GetBuffer(0), nPn, strDataitem.GetBuffer(0), start_t, start_t, deny);
				m_qTask.push(task);
				start_t += 86400;
			}
		
		}
		for (int j = 0; j < m_ctrlMonDataItemList.GetItemCount(); j++)
		{
			if (!m_ctrlMonDataItemList.GetCheck(j))
				continue;

			strDataitem = m_ctrlMonDataItemList.GetItemText(j, 1);
			cStart = GetOnMonth(cStart);
			CTime cTempTime = cStart;
			time_t start_t = cStart.GetTime();
			time_t end_t = cEnd.GetTime();
			while (start_t < end_t)
			{
				Task task(TASK_READ_HISTORY, m_strTerminalAddr.GetBuffer(0), nPn, strDataitem.GetBuffer(0), start_t, start_t, deny);
				m_qTask.push(task);
				//start_t += 86400;
				cTempTime = GetNextMonth(cTempTime);
				start_t = cTempTime.GetTime();
			}
		
		}
	}	

	if (m_qTask.empty())
	{
		MessageBox("请选择抄读数据项！");
	}
	else
	{
		this->SetTimer(1, 1000, NULL);
		GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_START1)->EnableWindow(TRUE);
		m_taskState = 0;
		m_retryCount = 0;
		m_pcHistory.SetPos(0);
		m_pcHistory.SetRange(0, m_qTask.size());
		m_pcHistory.SetStep(1);
	}
}




void CDlgReadHistoryData::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (m_taskState)
	{
	case 0:		// 待执行
		{
			if (m_qTask.empty())
			{
				KillTimer(1);
				GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_START1)->EnableWindow(FALSE);
				return;
			}

			Task data = m_qTask.front();			

			UINT64 id = ptcl_packet_csg::convert_device_addr(data.addr);
			ptcl_module_csg::pnfn_data_uint pnfn_data_uint[1];
			ptcl_module_csg::pnfn_data pnfn;
			pnfn.pn = data.pn;
			pnfn.fn = ptcl_packet_csg::convert_fn(data.dataitem);
			packet.set_device_addr(id);
			pnfn_data_uint[0].pnfn = pnfn;

			data_format_csg::get_instance()->format_read_history(&packet,pnfn_data_uint,1,data.starttime,data.endtime,data.densy);
			m_pChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
			CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
			CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());


			m_taskState = 1;
			m_taskTime = time(NULL);
		}
		break;
	case 1:		// 执行中
		{
			if (time(NULL) - m_taskTime > m_timeout) 
			{
				if (++m_retryCount < 3)  // 超时3次
				{
					Task data = m_qTask.front();

					UINT64 id = ptcl_packet_csg::convert_device_addr(data.addr);
					packet.set_device_addr(id);

					ptcl_module_csg::pnfn_data_uint pnfn_data_uint[1];
					ptcl_module_csg::pnfn_data pnfn;
					pnfn.pn = data.pn;
					pnfn.fn = ptcl_packet_csg::convert_fn(data.dataitem);
					pnfn_data_uint[0].pnfn = pnfn;

					data_format_csg::get_instance()->format_read_history(&packet,pnfn_data_uint,1,data.starttime,data.endtime,data.densy);
					m_pChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
					CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
					CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());

					m_taskState = 1;
					m_taskTime = time(NULL);
				}
				else
				{				
					if (!m_qTask.empty())
						m_qTask.pop();
					m_taskState = 0;
					m_retryCount = 0;
					m_pcHistory.StepIt();
				}
			}
		}
		break;
	case 2:
		{
			if (!m_qTask.empty())
				m_qTask.pop();
			m_taskState = 0;
			m_retryCount = 0;
			m_pcHistory.StepIt();
		}
		break;
	}	

	__super::OnTimer(nIDEvent);
}

void CDlgReadHistoryData::OnReceive(int state, char *buf, int len, u_int64 strTermAddr)
{
	if (g_nModuleControl != Moudle_RndRead_HistoryData)
		return;

	if (state == 1 || state == 2)
		return;

	CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf((unsigned char *)buf, len, "接收"));
	CLogMgr::GetInstanse().LogPacket("接收", (unsigned char *)buf, len);

	packet.set_packet_buffer((unsigned char *)buf,len);
	if (packet.parse_packet() <= 0)
	{        
		return ;
	}

	if(packet.get_afn() == 0x0D)
	{
		vector<data_parse_csg_history::data_item_value> vData;	
		data_parse_csg_history::parse_history_data(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);


		for (int i = 0; i < vData.size(); i++)
		{
			UINT dtid = vData[i].data_item_id;
			string strdtid = ptcl_packet_csg::convert_fn(dtid);

			CString strDatatype = "";
			std::map <CString, DataTypeParam>::iterator it = m_mDataType.find(strdtid.c_str());
			if (it!=m_mDataType.end())
			{
				strDatatype = it->second.desc;
			}

			CString strMeterID = "";
			CString strValue = "";
			CString strTime = "";
			strMeterID.Format("%d", vData[i].meter_index);

			//if (vData[i].db_time >= 0)
			// 判断是否为需量数据
			if ((vData[i].data_item_id >> 24) == 0x01)
			{
				if (IsValidData(vData[i].db_value))
				{
					CString strTime = "";
					CTime ctime(vData[i].db_time < 0 ? 0 : vData[i].db_time);
					strTime = ctime.Format("%Y-%m-%d %H:%M:%S");
					strValue.Format("最大需量：%f  最大需量发生时间：%s", vData[i].db_value, strTime);					
				}
				else
					strValue = "无效数据";
			}
			else
			{
				if (IsValidData(vData[i].db_value))
					strValue.Format("%f", vData[i].db_value);
				else
					strValue = "无效数据";
			}			

			CTime a(vData[i].value_time);
			strTime = a.Format("%Y-%m-%d %H:%M:%S");

			int nRow = m_ctrlOptResult.GetItemCount();
			m_ctrlOptResult.InsertItem(nRow, strMeterID);
			m_ctrlOptResult.SetItemText(nRow, 1, strDatatype);
			m_ctrlOptResult.SetItemText(nRow, 2, strTime);
			m_ctrlOptResult.SetItemText(nRow, 3, strValue);
		}
	}
	else if (packet.get_afn() == 0x00)
	{
		string result = "[终端";
		result += m_strTerminalAddr.GetBuffer(0);
		result += "]读历史数据: ";
		int nlen = 0;	
		while (len - nlen > 6)
		{
			// pnfn
			ptcl_module_csg::pnfn_data pnfn;
			ptcl_module_csg::parse_pnfn((unsigned char *)buf + nlen, &pnfn);
			nlen += 6;

			string strCode = "";
			BYTE ret = *(buf + nlen);
			if (ret == 0x00)		
				strCode = "确定";		
			else if (ret == 0x01)
				strCode = "否定";
		
			char temp[256] = {0};		
			sprintf(temp, "测量点号%d:%s:%s; ", pnfn.pn, ptcl_packet_csg::convert_fn(pnfn.fn).c_str(), strCode.c_str());
			result.append(temp);

			nlen += 1;
		}			
		ShowOperateLog(result.c_str());
	}

	m_taskState = 2;
}



int  CDlgReadHistoryData::getDenyTime(int denyType)  
{  
	int denyTime = 0;
	switch(denyType)
	{
	case 1:
		denyTime = 60;
		break;
	case 2:
		denyTime = 300;
		break;
	case 3:
		denyTime = 900;
		break;
	case 4:
		denyTime = 1800;
		break;
	case 5:
		denyTime = 3600;
		break;
	case 6:
		denyTime = 86400;
		break;
	}

	return denyTime;

}  

CTime CDlgReadHistoryData::GetNextMonth(const CTime tmSrc)  
{  
	int newYear = tmSrc.GetMonth()==12 ? tmSrc.GetYear()+1 : tmSrc.GetYear();  
	int newMonth = tmSrc.GetMonth()==12 ? 1 : tmSrc.GetMonth()+1;  


	return CTime(newYear, newMonth, 1,0,0,0);  
}  

CTime CDlgReadHistoryData::GetOnHour(const CTime tmSrc)
{

	return CTime(tmSrc.GetYear(),tmSrc.GetMonth(),tmSrc.GetDay(),tmSrc.GetHour(),0,0);
}

CTime CDlgReadHistoryData::GetOnDay(const CTime tmSrc)
{

	return CTime(tmSrc.GetYear(),tmSrc.GetMonth(),tmSrc.GetDay(),0,0,0);
}

CTime CDlgReadHistoryData::GetOnMonth(const CTime tmSrc)  
{
	return CTime(tmSrc.GetYear(), tmSrc.GetMonth(), 1,0,0,0);  
}



void CDlgReadHistoryData::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (!GetDlgItem(IDC_LIST_OPT_RESULT)->GetSafeHwnd())
		return;
	m_layoutCtrl.SetPos(IDC_LIST_OPT_RESULT, 	LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	m_layoutCtrl.SetPos(IDC_LIST_MON_DATAITEM, 	LT_LEFT_TOP, RB_RIGHT);
	m_layoutCtrl.SetOldRect(CRect(0, 0, cx, cy));
}



void CDlgReadHistoryData::OnBnClickedBtnStart2()
{
	KillTimer(1);
	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_START1)->EnableWindow(FALSE);
	m_pcHistory.SetPos(0);
}


void CDlgReadHistoryData::OnNMDblclkListOptResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString text = "";
	text = m_ctrlOptResult.GetItemText(pNMItemActivate->iItem,pNMItemActivate->iSubItem);
	AfxMessageBox(text);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
