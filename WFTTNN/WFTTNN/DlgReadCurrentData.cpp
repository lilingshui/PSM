// DlgReadCurrentData.cpp : 实现文件
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgReadCurrentData.h"
#include ".\dlgreadcurrentdata.h"
#include "MainFrm.h"
#include "ItemData.h"
#include "ptcl_codec/ptcl_module_csg.h"
#include "ptcl_codec/data_parse_csg.h"
#include "LogMgr.h"

// CDlgReadCurrentData 对话框

IMPLEMENT_DYNAMIC(CDlgReadCurrentData, CDialog)
CDlgReadCurrentData::CDlgReadCurrentData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReadCurrentData::IDD, pParent)
	, m_strTerminalAddr("")
	, m_retryCount(0)
	, m_timeout(30)
	, m_taskState(0)
	, m_pChannel(NULL)
	, m_layoutCtrl(this)
{
}

CDlgReadCurrentData::~CDlgReadCurrentData()
{
}

void CDlgReadCurrentData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATAITEM, m_ctrlDataItemList);
	DDX_Control(pDX, IDC_LIST_OPT_RESULT, m_ctrlOptResult);
	DDX_Control(pDX, IDC_COMBO1, m_cmbMeterList);
	DDX_Control(pDX, IDC_PROGRESS_REALTIME, m_pcRealtime);
	DDX_Control(pDX, IDC_LIST_METER, m_ctrlMeterList);
}


BEGIN_MESSAGE_MAP(CDlgReadCurrentData, CDialog)
	ON_BN_CLICKED(IDC_BTN_START, OnBnClickedBtnStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_STOP, &CDlgReadCurrentData::OnBnClickedBtnStop)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPT_RESULT, &CDlgReadCurrentData::OnNMDblclkListOptResult)
END_MESSAGE_MAP()

// CDlgReadCurrentData 消息处理程序

BOOL CDlgReadCurrentData::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwExtendStyle = m_ctrlDataItemList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlDataItemList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlDataItemList.InsertColumn(0,"#",LVCFMT_CENTER,20);
	m_ctrlDataItemList.InsertColumn(1,"数据项标识",LVCFMT_LEFT,80);
	m_ctrlDataItemList.InsertColumn(2,"数据项名称",LVCFMT_LEFT,200);
	InitDataItem();	

	dwExtendStyle = m_ctrlMeterList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlMeterList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlMeterList.InsertColumn(0,"#",LVCFMT_CENTER,20);
	m_ctrlMeterList.InsertColumn(1,"电表序号",LVCFMT_LEFT,80);
	m_ctrlMeterList.InsertColumn(2,"电表名称",LVCFMT_LEFT,120);

	dwExtendStyle = m_ctrlOptResult.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ctrlOptResult.SetExtendedStyle(dwExtendStyle);	
	m_ctrlOptResult.InsertColumn(0,"电表序号",LVCFMT_LEFT,100);
	m_ctrlOptResult.InsertColumn(1,"数据项名称",LVCFMT_LEFT,400);
	m_ctrlOptResult.InsertColumn(2,"数据值",LVCFMT_RIGHT,400);	

	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	
	CRect rect;
	GetClientRect(&rect);
	m_layoutCtrl.SetOldRect(rect);
	return TRUE;
}

void CDlgReadCurrentData::InitDataItem()
{
	m_ctrlDataItemList.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	const char *strSQL = "SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 1 ORDER BY PARAM_ID DESC";
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

void CDlgReadCurrentData::OnEvent(unsigned int nEventID, void *pParam)
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

void CDlgReadCurrentData::InitMeterList()
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

void CDlgReadCurrentData::OnBnClickedBtnStart()
{	
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

		for (int j = 0; j < m_ctrlDataItemList.GetItemCount(); j++)
		{
			if (!m_ctrlDataItemList.GetCheck(j))
				continue;

			strDataitem = m_ctrlDataItemList.GetItemText(j, 1);
			Task task(TASK_READ_REALTIME, m_strTerminalAddr.GetBuffer(0), nPn, strDataitem.GetBuffer(0), 0, 0, 0);
			m_qTask.push(task);
		}
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
	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	m_taskState = 0;
	m_retryCount = 0;
	m_pcRealtime.SetPos(0);
	m_pcRealtime.SetRange(0, m_qTask.size());
	m_pcRealtime.SetStep(1);
}

void CDlgReadCurrentData::OnTimer(UINT_PTR nIDEvent)
{	
	switch (m_taskState)
	{
	case 0:		// 待执行
		{
			if (m_qTask.empty())
			{
				KillTimer(1);
				GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
				return;
			}

			Task data = m_qTask.front();			
			
			UINT64 id = ptcl_packet_csg::convert_device_addr(data.addr);
			ptcl_module_csg::pnfn_data pnfn[1];
			pnfn[0].pn = data.pn;
			pnfn[0].fn = ptcl_packet_csg::convert_fn(data.dataitem);
			packet.set_device_addr(id);
			ptcl_module_csg::get_instance()->format_packet_read_realtime_data(&packet, pnfn, 1);
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
					ptcl_module_csg::pnfn_data pnfn[1];
					pnfn[0].pn = data.pn;
					pnfn[0].fn = ptcl_packet_csg::convert_fn(data.dataitem);
					packet.set_device_addr(id);
					ptcl_module_csg::get_instance()->format_packet_read_realtime_data(&packet, pnfn, 1);
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
					m_pcRealtime.StepIt();
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
			m_pcRealtime.StepIt();			
		}
		break;
	}	

	__super::OnTimer(nIDEvent);
}


void CDlgReadCurrentData::OnBnClickedBtnStop()
{
	KillTimer(1);
	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	m_pcRealtime.SetPos(0);
}

void CDlgReadCurrentData::OnReceive(int state, char *buf, int len, u_int64 strTermAddr)
{
	if (g_nModuleControl != Module_RndRead_CurrentData)
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

	if(packet.get_afn() == 0x0C)
	{
		vector<data_parse_csg_realtime::data_item_value> vData;	
		data_parse_csg_realtime::parse_realtime_data(packet.get_data_area_buffer(), packet.get_data_area_length(), vData);
		
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
			strMeterID.Format("%d", vData[i].meter_index);
			if (vData[i].db_time > 0)
			{				
				if (IsValidData(vData[i].db_value))
				{
					CString strTime = "";
					CTime ctime(vData[i].db_time);
					strTime = ctime.Format("%Y-%m-%d %H:%M:%S");
					strValue.Format("最大需量：%f  最大需量发生时间：%s", vData[i].db_value, strTime);
				}
				else
				{
					strValue = "无效数据";
				}
			}
			else
			{
				if (IsValidData(vData[i].db_value))
				{
					if (strdtid.compare("04000101") == 0)
					{						
						CTime dt(vData[i].db_value);
						strValue.Format("%d-%d-%d", dt.GetYear(), dt.GetMonth(), dt.GetDay());	// 日期
					}
					else if (strdtid.compare("04000102") == 0)
					{						
						CTime dt(vData[i].db_value);
						strValue.Format("%d:%d:%d", dt.GetHour(), dt.GetMinute(), dt.GetSecond());	// 时间
					}
					else
					{
						strValue.Format("%f",  vData[i].db_value);
					}
				}
				else
				{
					strValue = "无效数据";
				}
			}
			int nRow = m_ctrlOptResult.GetItemCount();
			m_ctrlOptResult.InsertItem(nRow, strMeterID);
			m_ctrlOptResult.SetItemText(nRow, 1, strDatatype);
			m_ctrlOptResult.SetItemText(nRow, 2, strValue);
		}
	}
	else if (packet.get_afn() == 0x00)
	{
		ProcessAFN00(packet.get_data_area_buffer(), packet.get_data_area_length());
	}

	m_taskState = 2;
}

void CDlgReadCurrentData::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (!GetDlgItem(IDC_LIST_OPT_RESULT)->GetSafeHwnd())
		return;
	m_layoutCtrl.SetPos(IDC_LIST_OPT_RESULT, 	LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	m_layoutCtrl.SetPos(IDC_LIST_DATAITEM, 	LT_LEFT_TOP, RB_RIGHT);

	m_layoutCtrl.SetOldRect(CRect(0, 0, cx, cy));
}

void CDlgReadCurrentData::ProcessAFN00(unsigned char *buf, int len)
{
	string result = "[终端";
	result += m_strTerminalAddr.GetBuffer(0);
	result += "]读当前数据: ";
	int nlen = 0;	
	while (len - nlen > 6)
	{
		// pnfn
		ptcl_module_csg::pnfn_data pnfn;
		ptcl_module_csg::parse_pnfn(buf + nlen, &pnfn);
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



void CDlgReadCurrentData::OnNMDblclkListOptResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString text = "";
	text = m_ctrlOptResult.GetItemText(pNMItemActivate->iItem,pNMItemActivate->iSubItem);
	AfxMessageBox(text);
	
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
