// DlgReadAlarmData.cpp : 实现文件
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "MainFrm.h"
#include "DlgReadAlarmData.h"
#include ".\DlgReadAlarmData.h"
#include "ItemData.h"
#include "ptcl_codec\data_parse_csg.h"
#include "ptcl_codec\data_format_csg.h"
#include "LogMgr.h"

// CDlgReadAlarmData 对话框

IMPLEMENT_DYNAMIC(CDlgReadAlarmData, CDialog)
CDlgReadAlarmData::CDlgReadAlarmData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReadAlarmData::IDD, pParent), m_nTermNo(0)
	, m_strTerminalAddr("")
	, m_layoutCtrl(this)
	,m_taskState(0)
	,m_retryCount(0)
	,m_timeout(30)
{
}

CDlgReadAlarmData::~CDlgReadAlarmData()
{
}

void CDlgReadAlarmData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ALARM_DATAITEM, m_ctrlDataItemList);
	DDX_Control(pDX, IDC_LIST_OPT_ALARM_RESULT, m_ctrlOptResult);
	DDX_Control(pDX, IDC_LIST_ALARM_DEVICE, m_ctrlDeviceList);
	//DDX_Control(pDX, IDC_LIST_DEVICE, m_ctrlDeviceList);
	DDX_Control(pDX, IDC_LIST_ALARM_TERM_DATAITEM, m_ctrlTermAlarmItem);
	DDX_Control(pDX, IDC_ALARM_DATE_START, m_ctrlDateStart);
	DDX_Control(pDX, IDC_ALARM_TIME_START, m_ctrlTimeStart);
	DDX_Control(pDX, IDC_ALARM_DATE_END, m_ctrlDateEnd);
	DDX_Control(pDX, IDC_ALaRM_END_TIME, m_ctrlTimeEnd);
}


BEGIN_MESSAGE_MAP(CDlgReadAlarmData, CDialog)
	ON_BN_CLICKED(IDC_BTN_START_ALARM_READ, OnBnClickedBtnStart)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPT_ALARM_RESULT, &CDlgReadAlarmData::OnNMDblclkListOptAlarmResult)
	ON_BN_CLICKED(IDC_BTN_ALARM_STOP, &CDlgReadAlarmData::OnBnClickedBtnAlarmStop)
END_MESSAGE_MAP()


// CDlgReadAlarmData 消息处理程序

BOOL CDlgReadAlarmData::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwExtendStyle = m_ctrlDataItemList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlDataItemList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlDataItemList.InsertColumn(0,"表计事件",LVCFMT_LEFT,65);
	m_ctrlDataItemList.InsertColumn(1,"数据项标识",LVCFMT_LEFT,80);
	m_ctrlDataItemList.InsertColumn(2,"数据项名称",LVCFMT_LEFT,400);

	dwExtendStyle = m_ctrlTermAlarmItem.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlTermAlarmItem.SetExtendedStyle(dwExtendStyle);	
	m_ctrlTermAlarmItem.InsertColumn(0,"终端事件",LVCFMT_LEFT,65);
	m_ctrlTermAlarmItem.InsertColumn(1,"数据项标识",LVCFMT_LEFT,80);
	m_ctrlTermAlarmItem.InsertColumn(2,"数据项名称",LVCFMT_LEFT,400);

	dwExtendStyle = m_ctrlDeviceList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlDeviceList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlDeviceList.InsertColumn(0,"#",LVCFMT_LEFT,20);
	m_ctrlDeviceList.InsertColumn(1,"电表序号",LVCFMT_LEFT,80);
	m_ctrlDeviceList.InsertColumn(2,"表计名称",LVCFMT_LEFT,400);


	dwExtendStyle = m_ctrlOptResult.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ctrlOptResult.SetExtendedStyle(dwExtendStyle);	
	m_ctrlOptResult.InsertColumn(0,"电表序号",LVCFMT_LEFT,100);
	m_ctrlOptResult.InsertColumn(1,"数据项名称",LVCFMT_LEFT,400);
	m_ctrlOptResult.InsertColumn(2,"数据值",LVCFMT_RIGHT,200);

	InitDataItem();	

	GetDlgItem(IDC_BTN_START_ALARM_READ)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_ALARM_STOP)->EnableWindow(FALSE);

	CRect rect;
	GetClientRect(&rect);
	m_layoutCtrl.SetOldRect(rect);
	return TRUE;
}


void CDlgReadAlarmData::OnEvent(unsigned int nEventID, void *pParam)
{
	if (Event_Type_Item_Double_Click != nEventID || !::IsWindowVisible(this->GetSafeHwnd()))
		return;		

	TerminalItemData *data = (TerminalItemData *)pParam;
	if (ITEM_TYPE_TERMINAL == data->m_type && m_nTermNo != data->m_id)
	{
		m_nTermNo = data->m_id;
		m_strTerminalAddr = data->m_addr;
		m_ctrlDeviceList.DeleteAllItems();

		InitMeterList();
	}
}


void CDlgReadAlarmData::InitDataItem()
{
	m_ctrlDataItemList.DeleteAllItems();
	m_ctrlTermAlarmItem.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	const char *strSQL = "SELECT * FROM ALL_PARAMETER_INFO WHERE ID=8 OR ID=9 ORDER BY PARAM_ID DESC";
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		CString strParamID = _T("");
		CString strParamName = _T("");
		int nId = 0;

		record.GetFieldValue("PARAM_ID",val);
		strParamID = val.bstrVal;

		record.GetFieldValue("PARAM_NAME",val);
		strParamName = val.bstrVal;

		record.GetFieldValue("ID",nId);

		if (nId == 8)
		{
			m_ctrlDataItemList.InsertItem(0, "");
			m_ctrlDataItemList.SetItemText(0, 1, strParamID);
			m_ctrlDataItemList.SetItemText(0, 2, strParamName);
		}
		else if (nId == 9)
		{
			m_ctrlTermAlarmItem.InsertItem(0, "");
			m_ctrlTermAlarmItem.SetItemText(0, 1, strParamID);
			m_ctrlTermAlarmItem.SetItemText(0, 2, strParamName);
		}
		else
			;
	
		DataTypeParam param;
		param.type = nId;
		param.desc = strParamName;

		m_mDataType[strParamID] = param;

		record.RecordMoveNext();
	}

}

void CDlgReadAlarmData::InitMeterList()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[256];
	sprintf(strSQL, "SELECT * FROM MP_ARCH WHERE RTU_ID = %d", m_nTermNo);	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;

	m_ctrlDeviceList.DeleteAllItems();
	int nRow = 0;
	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nMeterId = 0;
		CString strMeterName = "";
		CString strMeterId = "";

		record.GetFieldValue("MP_ID", nMeterId);
		record.GetFieldValue("MP_NAME",val);
		strMeterName = val.bstrVal;

		strMeterId.Format("%d", nMeterId);

		m_ctrlDeviceList.InsertItem(nRow, "");
		m_ctrlDeviceList.SetItemText(nRow, 1, strMeterId);
		m_ctrlDeviceList.SetItemText(nRow, 2, strMeterName);

		record.RecordMoveNext();
		nRow++;
	}
}

void CDlgReadAlarmData::OnBnClickedBtnStart()
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
		m_qTask.pop_front();

	UpdateData(TRUE);

	int nPn = 0;
	CString strPn = "";
	CString strDataitem = "";
	SYSTEMTIME sysDateStart, sysTimeStart, sysDateEnd, sysTimeEnd;
	m_ctrlDateStart.GetTime(&sysDateStart);
	m_ctrlTimeStart.GetTime(&sysTimeStart);
	m_ctrlDateEnd.GetTime(&sysDateEnd);
	m_ctrlTimeEnd.GetTime(&sysTimeEnd);

	CTime cStartTime,cEndTime;
	cStartTime = CTime((int)sysDateStart.wYear, (int)sysDateStart.wMonth, (int)sysDateStart.wDay,
		(int)sysTimeStart.wHour, (int)sysTimeStart.wMinute, (int)sysTimeStart.wSecond,-1);

	cEndTime = CTime((int)sysDateEnd.wYear, (int)sysDateEnd.wMonth, (int)sysDateEnd.wDay,
		(int)sysTimeEnd.wHour, (int)sysTimeEnd.wMinute, (int)sysTimeEnd.wSecond,-1);

	time_t uStartTime,uEndTime = 0;
	uStartTime = cStartTime.GetTime();
	uEndTime = cEndTime.GetTime();
	if (uStartTime > uEndTime)
	{
		MessageBox("开始时间大于结束时间！");
		return;
	}


	for (int i = 0; i < m_ctrlDeviceList.GetItemCount(); i++)
	{
		if (!m_ctrlDeviceList.GetCheck(i))
			continue;

		strPn = m_ctrlDeviceList.GetItemText(i, 1);
		nPn = atoi(strPn.GetBuffer(0));

		for (int j = 0; j < m_ctrlDataItemList.GetItemCount(); j++)
		{
			if (!m_ctrlDataItemList.GetCheck(j))
				continue;


			strDataitem = m_ctrlDataItemList.GetItemText(j, 1);
			int type=8;
			std::map <CString,DataTypeParam>::iterator it = m_mDataType.find(strDataitem);
			if (it != m_mDataType.end())
				type = it->second.type;
			
			Task task(type, m_strTerminalAddr.GetBuffer(0), nPn, strDataitem.GetBuffer(0), uStartTime, uEndTime, 0);
			m_qTask.push_back(task);		
		}		
	}	

	for (int i=0; i<m_ctrlTermAlarmItem.GetItemCount(); i++)
	{
		if (!m_ctrlTermAlarmItem.GetCheck(i))
			continue;

		strDataitem = m_ctrlTermAlarmItem.GetItemText(i,1);
		int type=9;
		std::map <CString,DataTypeParam>::iterator it = m_mDataType.find(strDataitem);
		if (it != m_mDataType.end())
			type = it->second.type;
		Task task(type, m_strTerminalAddr.GetBuffer(0), 0, strDataitem.GetBuffer(0), uStartTime, uEndTime, 0);
		m_qTask.push_back(task);
	}

	if (m_qTask.empty())
	{
		MessageBox("请选择抄读数据项！");
	}
	else
	{
		this->SetTimer(1, 1000, NULL);		
		GetDlgItem(IDC_BTN_START_ALARM_READ)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_ALARM_STOP)->EnableWindow(TRUE);
		m_taskState = 0;
		m_retryCount = 0;
	}
}

void CDlgReadAlarmData::OnTimer(UINT_PTR nIDEvent)
{
	switch (m_taskState)
	{
	case 0:		// 待执行
		{
			if (m_qTask.empty())
			{
				KillTimer(1);
				GetDlgItem(IDC_BTN_START_ALARM_READ)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_ALARM_STOP)->EnableWindow(FALSE);				
				return;
			}

			Task data = m_qTask.front();			
			UINT64 id = ptcl_packet_csg::convert_device_addr(data.addr);
			ptcl_module_csg::pnfn_data pnfn[1];

			if (data.type == 8)
			{
				pnfn[0].pn = data.pn;
			}
			else 
				pnfn[0].pn = 0;
			
			pnfn[0].fn = ptcl_packet_csg::convert_fn(data.dataitem);


			packet.set_device_addr(id);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_alarm_data(&packet, pnfn_data, 1, data.starttime, data.endtime);
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
				if (m_retryCount++ < 3)  // 超时3次
				{
					Task data = m_qTask.front();

					UINT64 id = ptcl_packet_csg::convert_device_addr(data.addr);
					ptcl_module_csg::pnfn_data pnfn[1];
					if (data.type == 8)
					{
						pnfn[0].pn = data.pn;
					}
					else 
						pnfn[0].pn = 0;
					pnfn[0].fn = ptcl_packet_csg::convert_fn(data.dataitem);
					ptcl_module_csg::pnfn_data_uint pnfn_data[1];
					pnfn_data[0].pnfn = pnfn[0];
					packet.set_device_addr(id);
					data_format_csg::get_instance()->format_read_alarm_data(&packet, pnfn_data, 1, data.starttime, data.endtime);
					m_pChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
					CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
					CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());

					m_taskState = 1;
					m_taskTime = time(NULL);
				}
				else
				{				
					if (!m_qTask.empty())
						m_qTask.pop_front();
					m_taskState = 0;
		
				}
			}
		}
		break;
	case 2:
		{
			if (!m_qTask.empty())
				m_qTask.pop_front();
			m_taskState = 0;

		}
		break;
	}	

	__super::OnTimer(nIDEvent);
}

void CDlgReadAlarmData::OnReceive(int state, char *buf, int len, u_int64 termAddr)
{
	if (g_nModuleControl != Moudle_RndRead_AlarmData)
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

	if(packet.get_afn() == 0x13)
	{
		vector<data_parse_alarm_data::data_item_value> vData;	
		data_parse_alarm_data::parse_alarm_data(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);

		for (int i = 0; i < vData.size(); i++)
		{
			UINT dtid = vData[i].data_item_id;
			string strdtid = ptcl_packet_csg::convert_fn(dtid);
			CString strDatatype = "";
			CString strMeterID = "";
			CString strValue = "";
			strMeterID.Format("%d", vData[i].meter_index);
			//strValue.Format("%f", vData[i].db_value);
			std::map <CString, DataTypeParam>::iterator it = m_mDataType.find(strdtid.c_str());
			if (it!=m_mDataType.end())
			{
				strDatatype = it->second.desc;
			}

			strValue = vData[i].db_value.c_str();
			int nRow = m_ctrlOptResult.GetItemCount();
			m_ctrlOptResult.InsertItem(nRow, strMeterID);
			m_ctrlOptResult.SetItemText(nRow, 1, strDatatype);
			m_ctrlOptResult.SetItemText(nRow, 2, strValue);
		}
	}
	else if (packet.get_afn() == 0x00)
	{
		//ProcessAFN00(packet.get_data_area_buffer(), packet.get_data_area_length());
		string result = "[终端";
		result += m_strTerminalAddr.GetBuffer(0);
		result += "]读报警事件: ";
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

void CDlgReadAlarmData::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (!GetDlgItem(IDC_LIST_OPT_ALARM_RESULT)->GetSafeHwnd())
		return;
	m_layoutCtrl.SetPos(IDC_LIST_OPT_ALARM_RESULT, 	LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	m_layoutCtrl.SetPos(IDC_LIST_ALARM_DATAITEM, 	LT_LEFT_TOP, RB_RIGHT);
	m_layoutCtrl.SetPos(IDC_LIST_ALARM_TERM_DATAITEM, LT_LEFT_TOP, RB_RIGHT);
	m_layoutCtrl.SetOldRect(CRect(0, 0, cx, cy));
}


void CDlgReadAlarmData::OnNMDblclkListOptAlarmResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString text = "";
	text = m_ctrlOptResult.GetItemText(pNMItemActivate->iItem,pNMItemActivate->iSubItem);
	AfxMessageBox(text);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CDlgReadAlarmData::OnBnClickedBtnAlarmStop()
{
	KillTimer(1);
	GetDlgItem(IDC_BTN_START_ALARM_READ)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_ALARM_STOP)->EnableWindow(FALSE);
}
