// DlgReadEventData.cpp : 实现文件
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "MainFrm.h"
#include "DlgReadEventData.h"
#include ".\dlgreadeventdata.h"
#include "ItemData.h"
#include "ptcl_codec\data_parse_csg.h"
#include "ptcl_codec\data_format_csg.h"
#include "LogMgr.h"


// CDlgReadEventData 对话框

IMPLEMENT_DYNAMIC(CDlgReadEventData, CDialog)
CDlgReadEventData::CDlgReadEventData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReadEventData::IDD, pParent), m_nTermNo(0)
	, m_strTerminalAddr("")
	, m_layoutCtrl(this)
	,m_taskState(0)
	,m_retryCount(0)
	,m_timeout(30)
{
}

CDlgReadEventData::~CDlgReadEventData()
{
}

void CDlgReadEventData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATAITEM, m_ctrlDataItemList);
	DDX_Control(pDX, IDC_LIST_OPT_RESULT, m_ctrlOptResult);
	DDX_Control(pDX, IDC_LIST_DEVICE, m_ctrlDeviceList);
	//DDX_Control(pDX, IDC_LIST_DEVICE, m_ctrlDeviceList);
	DDX_Control(pDX, IDC_LIST_TERM_DATAITEM, m_ctrlTermEventItem);
	DDX_Control(pDX, IDC_EVENT_DATE_START, m_ctrlDateStart);
	DDX_Control(pDX, IDC_EVENT_TIME_START, m_ctrlTimeStart);
	DDX_Control(pDX, IDC_EVENT_DATE_END, m_ctrlDateEnd);
	DDX_Control(pDX, IDC_EVENT_END_TIME, m_ctrlTimeEnd);
}


BEGIN_MESSAGE_MAP(CDlgReadEventData, CDialog)
	ON_BN_CLICKED(IDC_BTN_START_READ, OnBnClickedBtnStart)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPT_RESULT, &CDlgReadEventData::OnNMDblclkListOptResult)
	ON_BN_CLICKED(IDC_BTN_EVENT_STOP, &CDlgReadEventData::OnBnClickedBtnEventStop)
END_MESSAGE_MAP()


// CDlgReadEventData 消息处理程序

BOOL CDlgReadEventData::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwExtendStyle = m_ctrlDataItemList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlDataItemList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlDataItemList.InsertColumn(0,"表计事件",LVCFMT_LEFT,65);
	m_ctrlDataItemList.InsertColumn(1,"数据项标识",LVCFMT_LEFT,80);
	m_ctrlDataItemList.InsertColumn(2,"数据项名称",LVCFMT_LEFT,400);

	dwExtendStyle = m_ctrlTermEventItem.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlTermEventItem.SetExtendedStyle(dwExtendStyle);	
	m_ctrlTermEventItem.InsertColumn(0,"终端事件",LVCFMT_LEFT,65);
	m_ctrlTermEventItem.InsertColumn(1,"数据项标识",LVCFMT_LEFT,80);
	m_ctrlTermEventItem.InsertColumn(2,"数据项名称",LVCFMT_LEFT,400);

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

	GetDlgItem(IDC_BTN_START_READ)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_EVENT_STOP)->EnableWindow(FALSE);

	CRect rect;
	GetClientRect(&rect);
	m_layoutCtrl.SetOldRect(rect);
	return TRUE;
}


void CDlgReadEventData::OnEvent(unsigned int nEventID, void *pParam)
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


void CDlgReadEventData::InitDataItem()
{
	m_ctrlDataItemList.DeleteAllItems();
	m_ctrlTermEventItem.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	//const char *strSQL = "SELECT * FROM ALL_PARAMETER_INFO WHERE (ID=5 OR ID=7) AND PARAM_ID <> 'E2010014' ORDER BY PARAM_ID DESC";
	const char *strSQL = "SELECT * FROM ALL_PARAMETER_INFO WHERE (ID=5 OR ID=7) ORDER BY PARAM_ID DESC";    // MODIFY BY WM  20140102 手动召测加上“参数变更记录”
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

		if (nId == 5 || nId == 8)
		{
			m_ctrlDataItemList.InsertItem(0, "");
			m_ctrlDataItemList.SetItemText(0, 1, strParamID);
			m_ctrlDataItemList.SetItemText(0, 2, strParamName);
		}
		else if (nId == 7 || nId == 9)
		{
			m_ctrlTermEventItem.InsertItem(0, "");
			m_ctrlTermEventItem.SetItemText(0, 1, strParamID);
			m_ctrlTermEventItem.SetItemText(0, 2, strParamName);
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

void CDlgReadEventData::InitMeterList()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[256];
	sprintf(strSQL, "SELECT * FROM MP_ARCH WHERE RTU_ID = %d ORDER BY MP_ID", m_nTermNo);	
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

void CDlgReadEventData::OnBnClickedBtnStart()
{
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
			int type=5;
			std::map <CString,DataTypeParam>::iterator it = m_mDataType.find(strDataitem);
			if (it != m_mDataType.end())
				type = it->second.type;
			
			Task task(type, m_strTerminalAddr.GetBuffer(0), nPn, strDataitem.GetBuffer(0), uStartTime, uEndTime, 0);
			m_qTask.push_back(task);		
		}
	}

	int a = m_ctrlTermEventItem.GetItemCount();
	for (int k=0; k<m_ctrlTermEventItem.GetItemCount();k++)
	{
		if (!m_ctrlTermEventItem.GetCheck(k))
			continue;

		strDataitem = m_ctrlTermEventItem.GetItemText(k,1);
		int type=7;
		std::map <CString,DataTypeParam>::iterator it = m_mDataType.find(strDataitem);
		if (it != m_mDataType.end())
			type = it->second.type;
		Task task(type, m_strTerminalAddr.GetBuffer(0), 0, strDataitem.GetBuffer(0), uStartTime, uEndTime, 0);
		m_qTask.push_back(task);
	}

	if (m_qTask.empty())
	{
		MessageBox("请选择抄读数据项！");
		return;
	}

	CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
	if ((NULL == pMainFrm) || NULL == (m_pChannel = pMainFrm->m_pDlgConn->m_pChannel))
	{
		MessageBox("未连接终端，请建立连接！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	this->SetTimer(1, 1000, NULL);	
	GetDlgItem(IDC_BTN_START_READ)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EVENT_STOP)->EnableWindow(TRUE);
	m_taskState = 0;
	m_retryCount = 0;	
}

void CDlgReadEventData::OnTimer(UINT_PTR nIDEvent)
{
	switch (m_taskState)
	{
	case 0:		// 待执行
		{
			if (m_qTask.empty())
			{
				KillTimer(1);
				GetDlgItem(IDC_BTN_START_READ)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_EVENT_STOP)->EnableWindow(FALSE);
				return;
			}

			Task data = m_qTask.front();			
			UINT64 id = ptcl_packet_csg::convert_device_addr(data.addr);
			ptcl_module_csg::pnfn_data pnfn[1];

			if (data.type == 5)
			{
				pnfn[0].pn = data.pn;
			}
			else 
				pnfn[0].pn = 0;
			
			pnfn[0].fn = ptcl_packet_csg::convert_fn(data.dataitem);


			packet.set_device_addr(id);
			ptcl_module_csg::pnfn_data_uint pnfn_data[1];
			pnfn_data[0].pnfn = pnfn[0];
			data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, data.starttime, data.endtime);

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
					if (data.type == 5)
					{
						pnfn[0].pn = data.pn;
					}
					else 
						pnfn[0].pn = 0;
					pnfn[0].fn = ptcl_packet_csg::convert_fn(data.dataitem);
					ptcl_module_csg::pnfn_data_uint pnfn_data[1];
					pnfn_data[0].pnfn = pnfn[0];
					packet.set_device_addr(id);
					data_format_csg::get_instance()->format_read_event_record(&packet, pnfn_data, 1, data.starttime, data.endtime);

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

void CDlgReadEventData::OnReceive(int state, char *buf, int len, u_int64 termAddr)
{
	if (g_nModuleControl != Moudle_RndRead_EventData)
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

	if(packet.get_afn() == 0x0e)
	{
		vector<data_parse_event_record::data_item_value> vData;	
		data_parse_event_record::parse_event_record(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
		
		for (int i = 0; i < vData.size(); i++)
		{
			UINT dtid = vData[i].data_item_id;
			string strdtid = ptcl_packet_csg::convert_fn(dtid);
		
			CString strMeterID = "";
			CString strValue = "";
			CString strDatatype = "";
			std::map <CString, DataTypeParam>::iterator it = m_mDataType.find(strdtid.c_str());
			if (it!=m_mDataType.end())
			{
				strDatatype = it->second.desc;
			}

			strMeterID.Format("%d", vData[i].meter_index);
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

void CDlgReadEventData::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (!GetDlgItem(IDC_LIST_OPT_RESULT)->GetSafeHwnd())
		return;
	m_layoutCtrl.SetPos(IDC_LIST_OPT_RESULT, 	LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	m_layoutCtrl.SetPos(IDC_LIST_DATAITEM, 	LT_LEFT_TOP, RB_RIGHT);
	m_layoutCtrl.SetPos(IDC_LIST_TERM_DATAITEM, LT_LEFT_TOP, RB_RIGHT);
	m_layoutCtrl.SetOldRect(CRect(0, 0, cx, cy));
}


void CDlgReadEventData::OnNMDblclkListOptResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString text = "";
	text = m_ctrlOptResult.GetItemText(pNMItemActivate->iItem,pNMItemActivate->iSubItem);
	AfxMessageBox(text);
	*pResult = 0;
}


void CDlgReadEventData::OnBnClickedBtnEventStop()
{
	KillTimer(1);
	GetDlgItem(IDC_BTN_START_READ)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_EVENT_STOP)->EnableWindow(FALSE);
}
