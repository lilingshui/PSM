// DlgSetParam.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgSetParam.h"
#include "PtclModule_NN.h"
#include "Protocol.h"
#include ".\dlgsetparam.h"
#include "ItemData.h"
#include "MainFrm.h"
#include "LogMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetParam dialog


CDlgSetParam::CDlgSetParam(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetParam::IDD, pParent) 
	, m_layoutCtrl(this)
	, m_fRatedVoltage(220)
	, m_fRatedCurrent(5)
{
	//{{AFX_DATA_INIT(CDlgSetParam)
	m_strMeterNo = _T("");
	m_strMeterAddr = _T("");
	m_strMeterName = _T("");
	CPacketTransfer::GetInstance()->RegisterSetParamPacketWnd(this);
	//}}AFX_DATA_INIT
	m_nBeginIndex = 0;
	m_nEndIndex = 0;
	m_nMeterIndex = 0;
	m_nOptType = 0;
	m_tCmdTime = 0;
	m_nRetryCount = 0;
	m_bWaiting = false;
}


void CDlgSetParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetParam)
	DDX_Control(pDX, IDC_COMBO_ENERGYTYPE, m_cbEnergyType);
	DDX_Control(pDX, IDC_COMBO_METERPROTOCOL, m_cbMeterProtocol);
	DDX_Control(pDX, IDC_COMBO_METERTYPE, m_cbMeterType);
	DDX_Control(pDX, IDC_COMBO_TERMINALNO, m_cbTerminalNo);
	DDX_Control(pDX, IDC_LIST_SETPARAMINFO, m_ShowSetParamListCtrl);
	DDX_Text(pDX, IDC_EDIT_METERNO, m_strMeterNo);
	DDX_Text(pDX, IDC_EDIT_METERADDR, m_strMeterAddr);
	DDX_Text(pDX, IDC_EDIT_METERNAME, m_strMeterName);
	DDX_Text(pDX, IDC_EDIT_PORT, m_strPort);
	DDX_Control(pDX, IDC_CMB_BAUD, m_cbBaud);
	DDX_Control(pDX, IDC_CMB_CHECK, m_cbCheck);
	DDX_Control(pDX, IDC_CMB_DATABIT, m_cbDataBit);
	DDX_Control(pDX, IDC_CMB_STOPBIT, m_cbStopBit);
	DDX_Text(pDX, IDC_TXT_VOLTAGE, m_fRatedVoltage);
	DDX_Text(pDX, IDC_TXT_ELECTRICITY, m_fRatedCurrent);
	//}}AFX_DATA_MAP	
	DDX_Text(pDX, IDC_EDIT_BEGINMETER, m_nBeginIndex);
	DDX_Text(pDX, IDC_EDIT_ENDMETER, m_nEndIndex);
}


BEGIN_MESSAGE_MAP(CDlgSetParam, CDialog)
	//{{AFX_MSG_MAP(CDlgSetParam)
	ON_BN_CLICKED(IDC_BUTTON_METERADD, OnButtonMeteradd)
	ON_BN_CLICKED(IDC_BUTTON_METERDEL, OnButtonMeterdel)
//	ON_NOTIFY(NM_CLICK, IDC_LIST_SETPARAMINFO, OnClickListSetparaminfo)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_READ, OnButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_METERMODIFY, OnButtonMetermodify)
	ON_MESSAGE(SETPARAMPACKET_EVENT_NOTIFY,OnSetParamPacketEventNotify)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SETPARAMINFO, OnLvnItemchangedListSetparaminfo)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDlgSetParam::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BTN_TOTEMPLATE, &CDlgSetParam::OnBnClickedBtnTotemplate)
	ON_BN_CLICKED(IDC_BTN_FROMTEMPLATE, &CDlgSetParam::OnBnClickedBtnFromtemplate)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_STOP, &CDlgSetParam::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CDlgSetParam::OnBnClickedBtnClear)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetParam message handlers

BOOL CDlgSetParam::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	m_cbMeterType.AddString("485表");
	m_cbMeterType.AddString("模拟量");
	m_cbMeterType.AddString("脉冲量");
	m_cbMeterType.AddString("计算值");
	m_cbMeterType.AddString("交流采样");	
	m_cbMeterType.SetCurSel(0);

	m_cbMeterProtocol.AddString("DL/T645-1997规约");
	m_cbMeterProtocol.AddString("DL/T645-2007规约");
	m_cbMeterProtocol.AddString("广东97电表规约");
	m_cbMeterProtocol.AddString("广东07电表规约");
	m_cbMeterProtocol.AddString("威胜自定义规约");
	m_cbMeterProtocol.AddString("兰吉尔B表规约");
	m_cbMeterProtocol.AddString("兰吉尔D表规约");
	m_cbMeterProtocol.AddString("EDMI表规约");
	m_cbMeterProtocol.AddString("ABB方表规约");
	m_cbMeterProtocol.AddString("埃托利表规约");
	m_cbMeterProtocol.AddString("EMAIL表规约");
	m_cbMeterProtocol.AddString("其他规约");	
	m_cbMeterProtocol.AddString(0);

	m_cbEnergyType.AddString("单相电子表");
	m_cbEnergyType.AddString("多功能表");
	m_cbEnergyType.AddString("其他类型");	
	m_cbEnergyType.SetCurSel(0);
	
	DWORD dwExtendStyle = m_ShowSetParamListCtrl.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ShowSetParamListCtrl.SetExtendedStyle(dwExtendStyle);

	m_ShowSetParamListCtrl.InsertColumn(0,"测量点编号",LVCFMT_CENTER,100);
	m_ShowSetParamListCtrl.InsertColumn(1,"测量点名称",LVCFMT_CENTER,100);
	m_ShowSetParamListCtrl.InsertColumn(2,"测量点性质",LVCFMT_CENTER,120);
	m_ShowSetParamListCtrl.InsertColumn(3,"测量点地址",LVCFMT_CENTER,120);
	m_ShowSetParamListCtrl.InsertColumn(4,"测量点通讯规约",LVCFMT_CENTER,140);
	m_ShowSetParamListCtrl.InsertColumn(5,"电能表类型",LVCFMT_CENTER,120);
	m_ShowSetParamListCtrl.InsertColumn(6,"端口号",LVCFMT_CENTER,80);
	m_ShowSetParamListCtrl.InsertColumn(7,"波特率",LVCFMT_CENTER,80);
	m_ShowSetParamListCtrl.InsertColumn(8,"校验位",LVCFMT_CENTER,80);
	m_ShowSetParamListCtrl.InsertColumn(9,"数据位",LVCFMT_CENTER,80);
	m_ShowSetParamListCtrl.InsertColumn(10,"停止位",LVCFMT_CENTER,80);
	m_ShowSetParamListCtrl.InsertColumn(11,"额定电压",LVCFMT_CENTER,80);
	m_ShowSetParamListCtrl.InsertColumn(12,"额定电流",LVCFMT_CENTER,80);
	
	// 通讯参数
	m_strPort = "2";
	// 波特率
	m_cbBaud.AddString("300");
	m_cbBaud.AddString("600");
	m_cbBaud.AddString("1200");
	m_cbBaud.AddString("2400");
	m_cbBaud.AddString("4800");
	m_cbBaud.AddString("9600");
	m_cbBaud.AddString("19200");
	m_cbBaud.SetCurSel(2);
	// 校验位
	m_cbCheck.AddString("无校验");
	m_cbCheck.AddString("偶校验");
	m_cbCheck.AddString("奇校验");
	m_cbCheck.SetCurSel(2);
	// 数据位
	m_cbDataBit.AddString("5");
	m_cbDataBit.AddString("6");
	m_cbDataBit.AddString("7");
	m_cbDataBit.AddString("8");
	m_cbDataBit.SetCurSel(3);
	// 停止位
	m_cbStopBit.AddString("1");
	m_cbStopBit.AddString("1.5");
	m_cbStopBit.AddString("2");
	m_cbStopBit.SetCurSel(0);

	InitialTerminalInfo();

	InitMeterParamInfoList(m_strTerminalNo);
	
	CRect rect;
	GetClientRect(&rect);
	m_layoutCtrl.SetOldRect(rect);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSetParam::RefreshUI(int nNodeValue)
{
	CString temp;
	temp.Format("%d", nNodeValue);
	InitMeterParamInfoList(temp);
}

void CDlgSetParam::OnButtonMeteradd() 
{
	UpdateData(TRUE);

	int nCurSel = m_cbTerminalNo.GetCurSel();
	if (nCurSel == -1)
	{
		AfxMessageBox("双击索引树选择终端！");
		return ;
	}
	m_cbTerminalNo.GetLBText(nCurSel, m_strTerminalNo);	
		
	if (m_strMeterNo == "")
	{
		AfxMessageBox("测量点编号不能为空！");
		return ;
	}
	if (m_strMeterName == "")
	{
		AfxMessageBox("测量点名称不能为空！");
		return ;
	}
	if (m_strMeterAddr == "")
	{
		AfxMessageBox("测量点地址不能为空！");
		return ;
	}
	
	CString strMeterType, strMeterProtocol, strEnergyType;
	int nMeterType, nMeterProtocol, nEnergyType;
	//
	nMeterType = m_cbMeterType.GetCurSel();
	nMeterProtocol = m_cbMeterProtocol.GetCurSel();
	nEnergyType = m_cbEnergyType.GetCurSel();
	//
	m_cbMeterType.GetLBText(nMeterType, strMeterType);
	m_cbMeterProtocol.GetLBText(nMeterProtocol, strMeterProtocol);
	m_cbEnergyType.GetLBText(nEnergyType, strEnergyType);

	int nBaud, nCheck, nDatabit, nStopbit;
	CString strBaud, strCheck, strDatabit, strStopbit;
	//
	nBaud = m_cbBaud.GetCurSel();
	nCheck = m_cbCheck.GetCurSel();
	nDatabit = m_cbDataBit.GetCurSel();
	nStopbit = m_cbStopBit.GetCurSel();
	//
	m_cbBaud.GetLBText(nBaud, strBaud);
	m_cbCheck.GetLBText(nCheck, strCheck);
	m_cbDataBit.GetLBText(nDatabit, strDatabit);
	m_cbStopBit.GetLBText(nStopbit, strStopbit);

	CString strVoltage, strCurrent;
	strVoltage.Format("%.1f", m_fRatedVoltage);
	strCurrent.Format("%.3f", m_fRatedCurrent);

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	char strSQL[300] = {0};
	sprintf(strSQL, "INSERT INTO MP_ARCH(RTU_ID,MP_ID,MP_NAME,MP_ADDR,MP_PROTOCOL,MP_TYPE,MP_ENERGYTYPE," \
		"MP_PORT,MP_BAUD,MP_CHECKBIT,MP_DATABIT,MP_STOPBIT,MP_VOLTAGE,MP_CURRENT)" \
		" VALUES(%s, %s, '%s', '%s', %d, %d, %d, %s, %d, %d, %d, %d, %s, %s)",
		m_strTerminalNo, m_strMeterNo, m_strMeterName, m_strMeterAddr, nMeterProtocol, 
		nMeterType, nEnergyType, m_strPort, nBaud, nCheck, nDatabit, nStopbit, strVoltage, strCurrent);
	if (pConnect->ExecSQL(strSQL))
	{
		int index = m_ShowSetParamListCtrl.GetItemCount();
		m_ShowSetParamListCtrl.InsertItem(index, m_strMeterNo);			
		m_ShowSetParamListCtrl.SetItemText(index, 1, m_strMeterName);
		m_ShowSetParamListCtrl.SetItemText(index, 2, strMeterType);
		m_ShowSetParamListCtrl.SetItemText(index, 3, m_strMeterAddr);
		m_ShowSetParamListCtrl.SetItemText(index, 4, strMeterProtocol);
		m_ShowSetParamListCtrl.SetItemText(index, 5, strEnergyType);
		m_ShowSetParamListCtrl.SetItemText(index, 6, m_strPort);
		m_ShowSetParamListCtrl.SetItemText(index, 7, strBaud);
		m_ShowSetParamListCtrl.SetItemText(index, 8, strCheck);
		m_ShowSetParamListCtrl.SetItemText(index, 9, strDatabit);
		m_ShowSetParamListCtrl.SetItemText(index, 10, strStopbit);
		m_ShowSetParamListCtrl.SetItemText(index, 11, strVoltage);
		m_ShowSetParamListCtrl.SetItemText(index, 12, strCurrent);
		ShowOperateLog("操作成功");

		CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
		if (pMainFrm)
			pMainFrm->RefreshMeterArch();
	} 
	else
	{
		MessageBox(MSG_DUPLICATE, "提示", MB_OK|MB_ICONWARNING);
		return;
	}
}

void CDlgSetParam::InitialTerminalInfo()
{
	m_cbTerminalNo.ResetContent();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	char strSQL[300] = {0};
	sprintf(strSQL,"SELECT * FROM RTU_ARCH ORDER BY RTU_ID");	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;
	
	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nTerminalID = 0;
		CString strTerminalNo = "";
		
		record.GetFieldValue("RTU_ID", nTerminalID);
		strTerminalNo.Format("%d", nTerminalID);
		m_cbTerminalNo.AddString(strTerminalNo);
		
		record.RecordMoveNext();
	}

	m_cbTerminalNo.SetCurSel(0);
}

void CDlgSetParam::SearchByField(int nTerminalNo, int nMeterNo, CString &strMeterName)
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	char strSQL[300] = {0};
	int nRow = 0;
	
	sprintf(strSQL,"SELECT * FROM MP_ARCH WHERE RTU_ID = %d AND MP_ID = %d ", nTerminalNo, nMeterNo);
	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;
	
	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nRtuID;
		CString strRtuID = _T("");
		
		record.GetFieldValue("RTU_ID",nRtuID);
		strRtuID.Format("%d", nRtuID);
		
		record.GetFieldValue("MP_NAME",val);
		strMeterName = val.bstrVal;

		record.RecordMoveNext();
	}
}

void CDlgSetParam::InitMeterParamInfoList(CString strTerminalNo)
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	char strSQL[300] = {0};
	int nRow = 0;	
	sprintf(strSQL,"SELECT * FROM MP_ARCH WHERE RTU_ID = %s ORDER BY MP_ID", strTerminalNo);
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;
	
	m_ShowSetParamListCtrl.DeleteAllItems();
	while (!record.IsEndEOF())
	{
		_variant_t val;
		CString strRtuID = _T("");
		int nMpID;
		CString strMpID = _T("");
		CString strMpName = _T("");		
		CString strMpAddr = _T("");	

		record.GetFieldValue("MP_ID",nMpID);
		strMpID.Format("%d", nMpID);

		record.GetFieldValue("MP_NAME",val);
		strMpName = val.bstrVal;

		record.GetFieldValue("MP_ADDR",val);
		strMpAddr = val.bstrVal;

		
		int nMpType, nMpProtocol, nEnergyType;		
		CString strMpType, strMpProtocol, strEnergyType;
		//
		record.GetFieldValue("MP_TYPE",nMpType);		
		record.GetFieldValue("MP_PROTOCOL",nMpProtocol);
		record.GetFieldValue("MP_ENERGYTYPE",nEnergyType);
		//
		m_cbMeterType.GetLBText(nMpType, strMpType);
		m_cbMeterProtocol.GetLBText(nMpProtocol, strMpProtocol);
		m_cbEnergyType.GetLBText(nEnergyType, strEnergyType);


		int nPort, nBaud, nCheck, nDatabit, nStopbit;
		CString strPort, strBaud, strCheck, strDatabit, strStopbit;
		//
		record.GetFieldValue("MP_PORT", nPort);
		record.GetFieldValue("MP_BAUD", nBaud);
		record.GetFieldValue("MP_CHECKBIT", nCheck);
		record.GetFieldValue("MP_DATABIT", nDatabit);
		record.GetFieldValue("MP_STOPBIT", nStopbit);
		strPort.Format("%d", nPort);
		//
		m_cbBaud.GetLBText(nBaud, strBaud);
		m_cbCheck.GetLBText(nCheck, strCheck);
		m_cbDataBit.GetLBText(nDatabit, strDatabit);
		m_cbStopBit.GetLBText(nStopbit, strStopbit);

		float fVoltage, fCurrent;
		CString strVoltage, strCurrent;
		record.GetFieldValue("MP_VOLTAGE", fVoltage);
		record.GetFieldValue("MP_CURRENT", fCurrent);
		strVoltage.Format("%.1f", fVoltage);
		strCurrent.Format("%.3f", fCurrent);

		int nID = 0;
		record.GetFieldValue("ID", nID);

		m_ShowSetParamListCtrl.InsertItem(nRow, strMpID);
		m_ShowSetParamListCtrl.SetItemText(nRow, 1, strMpName);
		m_ShowSetParamListCtrl.SetItemText(nRow, 2, strMpType);
		m_ShowSetParamListCtrl.SetItemText(nRow, 3, strMpAddr);
		m_ShowSetParamListCtrl.SetItemText(nRow, 4, strMpProtocol);
		m_ShowSetParamListCtrl.SetItemText(nRow, 5, strEnergyType);
		m_ShowSetParamListCtrl.SetItemText(nRow, 6, strPort);
		m_ShowSetParamListCtrl.SetItemText(nRow, 7, strBaud);
		m_ShowSetParamListCtrl.SetItemText(nRow, 8, strCheck);
		m_ShowSetParamListCtrl.SetItemText(nRow, 9, strDatabit);
		m_ShowSetParamListCtrl.SetItemText(nRow, 10, strStopbit);
		m_ShowSetParamListCtrl.SetItemText(nRow, 11, strVoltage);
		m_ShowSetParamListCtrl.SetItemText(nRow, 12, strCurrent);
		m_ShowSetParamListCtrl.SetItemData(nRow, nID);
		
		record.RecordMoveNext();
		nRow++;
	}
	
	m_nMeterNum = nRow;

	m_ShowSetParamListCtrl.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	m_ShowSetParamListCtrl.SetSelectionMark(0);
}

void CDlgSetParam::OnButtonMeterdel() 
{
	int nIndex = m_ShowSetParamListCtrl.GetSelectionMark();
	if (nIndex < 0)
		return;
	
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;

	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;

	char strSQL[300] = {0};	
	int nID = (int)m_ShowSetParamListCtrl.GetItemData(nIndex);
	sprintf(strSQL, "DELETE FROM MP_ARCH WHERE ID = %d", nID);
	if (pConnect->ExecSQL(strSQL)) {	
		m_ShowSetParamListCtrl.DeleteItem(nIndex);
		ShowOperateLog("删除成功");

		CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
		if (pMainFrm)		
			pMainFrm->RefreshMeterArch();
	}
}

void string_to_buffer(char *src_string, unsigned char *dst_buffer, int size_buffer)
{
	if (src_string == NULL || dst_buffer == NULL || size_buffer > 20)
		return;
 
	//去空格
	int len_src_string = (int)strlen(src_string);
	for (int n=0;n<len_src_string;n++)
	{
		if (src_string[n] == ' ')
			src_string[n] = 0;   
	}

	//太长截断
	len_src_string = (int)strlen(src_string);
	if (len_src_string > size_buffer*2)
		src_string[size_buffer*2] = 0;

	//不足头部补0
	char src[40];
	memset(src, 0, 40);
	for (int n=len_src_string;n<size_buffer*2;n++)
	{
		strcat(src,"0");
	}
	strcat(src,src_string);

	for (int n=0;n<size_buffer;n++)
	{
		unsigned int b = 0;
		sscanf(src+n*2,"%02X",&b);
		dst_buffer[size_buffer-1-n] = (unsigned char)b;
	}
}

void CDlgSetParam::OnButtonSend() 
{	
	CChannel *pChannel = NULL;
	CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
	if (pMainFrm)
		pChannel = pMainFrm->m_pDlgConn->m_pChannel;	
	if (!pChannel)
	{
		MessageBox("未连接终端，请先建立连接！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (m_strTerminalAddr == "")
	{
		MessageBox("未选择终端，双击索引树选择终端！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	BeginTask(1);
}

void CDlgSetParam::OnButtonRead() 
{
	CChannel *pChannel = NULL;
	CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
	if (pMainFrm)
		pChannel = pMainFrm->m_pDlgConn->m_pChannel;	
	if (!pChannel)
	{
		MessageBox("未连接终端，请先建立连接！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (m_strTerminalAddr == "")
	{
		MessageBox("未选择终端，双击索引树选择终端！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	BeginTask(2);
}

void CDlgSetParam::OnBnClickedButtonDel()
{
	CChannel *pChannel = NULL;
	CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
	if (pMainFrm)
		pChannel = pMainFrm->m_pDlgConn->m_pChannel;	
	if (!pChannel)
	{
		MessageBox("未连接终端，请先建立连接！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (m_strTerminalAddr == "")
	{
		MessageBox("未选择终端，双击索引树选择终端！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	BeginTask(3);
}

void CDlgSetParam::OnButtonMetermodify() 
{
	int nRow = m_ShowSetParamListCtrl.GetSelectionMark();
	if (nRow < 0)
		return;
	int nId = 0;// m_ShowSetParamListCtrl.GetItemData(nRow);
	nId = m_ShowSetParamListCtrl.GetItemData(nRow);
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;

	UpdateData(true);

	CString strMeterType, strMeterProtocol, strEnergyType;
	int nMeterType, nMeterProtocol, nEnergyType;
	//
	nMeterType = m_cbMeterType.GetCurSel();
	nMeterProtocol = m_cbMeterProtocol.GetCurSel();
	nEnergyType = m_cbEnergyType.GetCurSel();
	//
	m_cbMeterType.GetLBText(nMeterType, strMeterType);
	m_cbMeterProtocol.GetLBText(nMeterProtocol, strMeterProtocol);
	m_cbEnergyType.GetLBText(nEnergyType, strEnergyType);

	int nBaud, nCheck, nDatabit, nStopbit;
	CString strBaud, strCheck, strDatabit, strStopbit;
	//
	nBaud = m_cbBaud.GetCurSel();
	nCheck = m_cbCheck.GetCurSel();
	nDatabit = m_cbDataBit.GetCurSel();
	nStopbit = m_cbStopBit.GetCurSel();
	//
	m_cbBaud.GetLBText(nBaud, strBaud);
	m_cbCheck.GetLBText(nCheck, strCheck);
	m_cbDataBit.GetLBText(nDatabit, strDatabit);
	m_cbStopBit.GetLBText(nStopbit, strStopbit);	

	int nCurSel = m_cbTerminalNo.GetCurSel();
	m_cbTerminalNo.GetLBText(nCurSel, m_strTerminalNo);	

	CString strVoltage, strCurrent;
	strVoltage.Format("%.1f", m_fRatedVoltage);
	strCurrent.Format("%.3f", m_fRatedCurrent);
	
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};	
	sprintf(strSQL,"UPDATE MP_ARCH SET MP_ID=%s, MP_NAME='%s',  MP_ADDR='%s', MP_PROTOCOL=%d, MP_TYPE=%d, MP_ENERGYTYPE=%d, "
		" MP_PORT=%s, MP_BAUD=%d, MP_CHECKBIT=%d, MP_DATABIT=%d, MP_STOPBIT=%d, MP_VOLTAGE=%s, MP_CURRENT=%s WHERE ID=%d", 
		m_strMeterNo, m_strMeterName, m_strMeterAddr, nMeterProtocol, nMeterType, nEnergyType, m_strPort, nBaud, 
		nCheck, nDatabit, nStopbit, strVoltage, strCurrent, nId);
	if (pConnect->ExecSQL(strSQL))
	{
		m_ShowSetParamListCtrl.SetItemText(nRow, 0, m_strMeterNo);
		m_ShowSetParamListCtrl.SetItemText(nRow, 1, m_strMeterName);
		m_ShowSetParamListCtrl.SetItemText(nRow, 2, strMeterType);
		m_ShowSetParamListCtrl.SetItemText(nRow, 3, m_strMeterAddr);
		m_ShowSetParamListCtrl.SetItemText(nRow, 4, strMeterProtocol);
		m_ShowSetParamListCtrl.SetItemText(nRow, 5, strEnergyType);
		m_ShowSetParamListCtrl.SetItemText(nRow, 6, m_strPort);
		m_ShowSetParamListCtrl.SetItemText(nRow, 7, strBaud);
		m_ShowSetParamListCtrl.SetItemText(nRow, 8, strCheck);
		m_ShowSetParamListCtrl.SetItemText(nRow, 9, strDatabit);
		m_ShowSetParamListCtrl.SetItemText(nRow, 10, strStopbit);
		m_ShowSetParamListCtrl.SetItemText(nRow, 11, strVoltage);
		m_ShowSetParamListCtrl.SetItemText(nRow, 12, strCurrent);

		CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
		if (pMainFrm)		
			pMainFrm->RefreshMeterArch();

		ShowOperateLog("操作成功");
	}
}

LRESULT CDlgSetParam::OnSetParamPacketEventNotify(WPARAM wParam,LPARAM lParam)
{
	FRAME_BUF *pFb=(FRAME_BUF*)lParam;
	
	CPtclPacket_NN packet_nn;
	packet_nn.setPacketBuf(pFb->buf,pFb->len);
	if (packet_nn.parsePacket() <= 0) ;
	
	unsigned char AFN = packet_nn.getAFN();

	/*01 01 01 00 80 E0 01*/
	
	if (AFN == 0x0A) //心跳帧
	{
		unsigned char pDataBuf[1024];
		int nDataLen = 1024;
		packet_nn.getDataAreaBuf(pDataBuf, nDataLen);
		ParseSetParam((unsigned char*)pDataBuf, nDataLen);

		return TRUE;
	}

	return TRUE;
}

void CDlgSetParam::ParseSetParam(unsigned char *pDataBuff, int nDataLen)
{
	unsigned char pMeterType[4] = {0x01, 0x00, 0x80, 0xE0}; //测量点性质
	unsigned char pMeterAddr[4] = {0x02, 0x00, 0x80, 0xE0}; //测量点地址
	unsigned char pMeterProtocol[4] = {0x03, 0x00, 0x80, 0xE0}; //测量点协议
	unsigned char pEnergyType[4] = {0x04, 0x00, 0x80, 0xE0}; //电能表类型

	CString strMeterNo,strMeterType,strMeterAddr,strMeterProtocol,strEnergyType;
	unstring sCode;
	sCode.erase(sCode.begin(), sCode.end());
	sCode.append(pDataBuff,nDataLen);

	m_ShowSetParamListCtrl.DeleteAllItems();

	int nCnt = 0, nRow = 0;
	while (sCode.size() > 6)
	{
		CPtclModule_NN::ParsePnFnData PnFn;
		CPtclModule_NN::ParsePnFn((unsigned char*)sCode.c_str(),&PnFn);
 
		strMeterNo.Format("%d", PnFn.vPn[0]);

		int nValueLen = 0;
		if (memcmp(sCode.c_str()+2, pMeterType, 4) == 0)
		{
			char chMeterType;
			memcpy(&chMeterType, sCode.c_str()+6, 1);
			if (chMeterType == 1) 
			{
				strMeterType = _T("485表");
			}
			else if (chMeterType == 2)
			{
				strMeterType = _T("模拟量");
			}
			else if (chMeterType == 3)
			{
				strMeterType = _T("脉冲量");
			}
			else if (chMeterType == 4)
			{
				strMeterType = _T("计算值");
			}
			else if (chMeterType == 5)
			{
				strMeterType = _T("交流采样");
			}

			nValueLen = 1;
		}
		else if (memcmp(sCode.c_str()+2, pMeterAddr, 4) == 0)
		{
			strMeterAddr.Format("%02x%02x%02x%02x%02x%02x", 
				sCode.at(11), sCode.at(10), sCode.at(9), 
				sCode.at(8), sCode.at(7), sCode.at(6));
			nValueLen = 6;
		}
		else if (memcmp(sCode.c_str()+2, pMeterProtocol, 4) == 0)
		{
			char chMeterProtocol;
			memcpy(&chMeterProtocol, sCode.c_str()+6, 1);

			if (chMeterProtocol == 0)
			{
				strMeterProtocol = _T("DL/T645-1997规约");
			}
			else if (chMeterProtocol == 1)
			{
				strMeterProtocol = _T("DL/T645-2007规约");
			}
			else if (chMeterProtocol == 2)
			{
				strMeterProtocol = _T("广东97电表规约");
			}
			else if (chMeterProtocol == 3)
			{
				strMeterProtocol = _T("广东07电表规约");
			}
			else if (chMeterProtocol == 4)
			{
				strMeterProtocol = _T("威胜自定义规约");
			}
			else if (chMeterProtocol == 5)
			{
				strMeterProtocol = _T("兰吉尔B表规约");
			}
			else if (chMeterProtocol == 6)
			{
				strMeterProtocol = _T("兰吉尔D表规约");
			}
			else if (chMeterProtocol == 7)
			{
				strMeterProtocol = _T("EDMI表规约");
			}
			else if (chMeterProtocol == 8)
			{
				strMeterProtocol = _T("ABB方表规约");
			}
			else if (chMeterProtocol == 9)
			{
				strMeterProtocol = _T("埃托利表规约");
			}
			else if (chMeterProtocol == 10)
			{
				strMeterProtocol = _T("EMAIL表规约");
			}
			else if (chMeterProtocol == 11)
			{
				strMeterProtocol = _T("其他规约");
			}
			
			nValueLen = 1;
		}
		else if (memcmp(sCode.c_str()+2, pEnergyType, 4) == 0)
		{
			char chEnergyType;
			memcpy(&chEnergyType, sCode.c_str()+6, 1);

			if (chEnergyType == 1)
			{
				strEnergyType = _T("单相电子表");
			}
			else if (chEnergyType == 2)
			{
				strEnergyType = _T("多功能表");
			}
			else if (chEnergyType == 3)
			{
				strEnergyType = _T("其他类型");
			}
			
			nValueLen = 1;
		}

		sCode.erase(sCode.begin(), sCode.begin() + 6 + nValueLen);

		nCnt++;

		if (nCnt%4==0)
		{
			m_ShowSetParamListCtrl.InsertItem(nRow, "1");
			m_ShowSetParamListCtrl.SetItemText(nRow, 1, strMeterNo);

			CString strMeterName=_T("");
			SearchByField(1, atoi(strMeterNo), strMeterName);
			m_ShowSetParamListCtrl.SetItemText(nRow, 2, strMeterName);
			m_ShowSetParamListCtrl.SetItemText(nRow, 3, strMeterType);
			m_ShowSetParamListCtrl.SetItemText(nRow, 4, strMeterAddr);
			m_ShowSetParamListCtrl.SetItemText(nRow, 5, strMeterProtocol);
			m_ShowSetParamListCtrl.SetItemText(nRow, 6, strEnergyType);
			nRow++;
		}
	}
}

void CDlgSetParam::OnEvent(unsigned int nEventID, void *pParam)
{
	if (g_nModuleControl != Module_SetParam)
		return;

	if (Event_Type_Item_Double_Click != nEventID || !::IsWindowVisible(this->GetSafeHwnd()))
		return;	
		
	if (ITEM_TYPE_TERMINAL == ((ItemData *)pParam)->m_type)
	{	
		
		TerminalItemData *data = (TerminalItemData *)pParam;
		m_strTerminalAddr = data->m_addr;
		m_strTerminalNo.Format("%d", data->m_id);
		m_cbTerminalNo.SelectString(0, m_strTerminalNo);

		m_strMeterNo = "";
		m_strMeterName = "";
		m_strMeterAddr = "";
		m_cbMeterType.SetCurSel(0);
		m_cbMeterProtocol.SetCurSel(0);
		m_cbEnergyType.SetCurSel(0);
		m_strPort = "2";
		m_cbBaud.SetCurSel(2);
		m_cbCheck.SetCurSel(2);
		m_cbDataBit.SetCurSel(3);
		m_cbStopBit.SetCurSel(0);
		m_fRatedVoltage = 220;
		m_fRatedCurrent = 5;

		UpdateData(FALSE);

		InitMeterParamInfoList(m_strTerminalNo);		
	}	
}

void CDlgSetParam::OnLvnItemchangedListSetparaminfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);	
	if(pNMLV->uChanged == LVIF_STATE)
	{
		if(pNMLV->uNewState & LVIS_SELECTED)
		{
			int nItem=pNMLV->iItem;

			m_strMeterNo = m_ShowSetParamListCtrl.GetItemText(nItem, 0);
			m_strMeterName = m_ShowSetParamListCtrl.GetItemText(nItem, 1);
			m_cbMeterType.SelectString(0, m_ShowSetParamListCtrl.GetItemText(nItem, 2));
			m_strMeterAddr = m_ShowSetParamListCtrl.GetItemText(nItem, 3);
			m_cbMeterProtocol.SelectString(0, m_ShowSetParamListCtrl.GetItemText(nItem, 4));
			m_cbEnergyType.SelectString(0, m_ShowSetParamListCtrl.GetItemText(nItem, 5));
			m_strPort = m_ShowSetParamListCtrl.GetItemText(nItem, 6);
			m_cbBaud.SelectString(0, m_ShowSetParamListCtrl.GetItemText(nItem, 7));
			m_cbCheck.SelectString(0, m_ShowSetParamListCtrl.GetItemText(nItem, 8));
			m_cbDataBit.SelectString(0, m_ShowSetParamListCtrl.GetItemText(nItem, 9));
//			m_cbStopBit.SelectString(0, m_ShowSetParamListCtrl.GetItemText(nItem, 10));
			CString temp = m_ShowSetParamListCtrl.GetItemText(nItem, 10);
			if (temp == "1")
				m_cbStopBit.SetCurSel(0);
			else if (temp == "1.5")
				m_cbStopBit.SetCurSel(1);
			else
				m_cbStopBit.SetCurSel(2);

			CString strVoltage, strCurrent;
			strVoltage = m_ShowSetParamListCtrl.GetItemText(nItem, 11);
			strCurrent = m_ShowSetParamListCtrl.GetItemText(nItem, 12);
			m_fRatedVoltage = atof(strVoltage.GetBuffer(0));
			m_fRatedCurrent = atof(strCurrent.GetBuffer(0));

			UpdateData(FALSE);
		}
	}

	*pResult = 0;
}

static const char *GetErrorInfo(unsigned char errcode)
{
	switch (errcode)
	{
	case 0x00:
		return "正确";		
	case 0x01:
		return "中继命令没有返回";		
	case 0x02:
		return "设置内容非法";		
	case 0x03:
		return "密码权限不足";		
	case 0x04:
		return "无此数据项";		
	case 0x05:
		return "命令时间失效";		
	case 0x06:
		return "目标地址不存在";		
	case 0x07:
		return "校验失败";		
	}

	return "";
}

void CDlgSetParam::OnReceive(int state,char *buf, int len, u_int64 strTermAddr)
{
	if (g_nModuleControl != Module_SetParam)
		return;

	if (len <= 0)
		return;

	CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf((unsigned char *)buf, len, "接收"));
	CLogMgr::GetInstanse().LogPacket("接收", (unsigned char *)buf, len);

	ptcl_packet_csg packet;
    packet.set_packet_buffer((unsigned char *)buf,len);
    if (packet.parse_packet() <= 0)
		return;

	if (m_bWaiting)	
		m_bWaiting = false;
	m_tCmdTime = 0;
    
	switch (packet.get_afn())
	{
	case 0x0A:
		{
			ProcessAFN0A(packet.get_data_area_buffer(), packet.get_data_area_length());		
		}
		break;
	case 0x04:
		{
			ProcessAFN04(packet.get_data_area_buffer(), packet.get_data_area_length());			
		}
		break;
	}
}

bool CDlgSetParam::GetMeterInfo(int id, CString &addr, int &protocol, int &mtype, int &etype, int &port, 
								int &baud, int &check, int &databit, int &stopbit, float &fVoltage, float &fCurrent)
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return false;	
	char strSQL[300] = {0};	
	sprintf(strSQL,"SELECT * FROM MP_ARCH WHERE RTU_ID = %s AND MP_ID = %d", m_strTerminalNo, id);
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return false;	
	if (record.IsEndEOF())
		return false;

	_variant_t val;
	record.GetFieldValue("MP_ADDR", val);
	addr = val.bstrVal;
	record.GetFieldValue("MP_TYPE", mtype);	
	record.GetFieldValue("MP_PROTOCOL", protocol);
	record.GetFieldValue("MP_ENERGYTYPE", etype);
	record.GetFieldValue("MP_PORT", port);
	record.GetFieldValue("MP_BAUD", baud);
	record.GetFieldValue("MP_CHECKBIT", check);
	record.GetFieldValue("MP_DATABIT", databit);
	record.GetFieldValue("MP_STOPBIT", stopbit);
	record.GetFieldValue("MP_VOLTAGE", fVoltage);
	record.GetFieldValue("MP_CURRENT", fCurrent);

	return true;
}

void CDlgSetParam::ProcessAFN04(unsigned char *buf, int len)
{
	// 写参数返回
	string result = "写参数：";
	int nlen = 0;	
	while (len - nlen > 6)
	{
		// pnfn
		ptcl_module_csg::pnfn_data pnfn;
		ptcl_module_csg::parse_pnfn(buf + nlen, &pnfn);
		nlen += 6;

		char temp[256] = {0};		
		sprintf(temp, "电表编号%d[%s:%s]; ", pnfn.pn, ptcl_packet_csg::convert_fn(pnfn.fn).c_str(), GetErrorInfo(*(buf + nlen)));
		result.append(temp);

		nlen += 1;
	}			
	ShowOperateLog(result.c_str());
}

void CDlgSetParam::ProcessAFN0A(unsigned char *buf, int len)
{
	// 读参数返回
	vector<data_parse_csg_parameter::data_item_value> vdata;
	data_parse_csg_parameter::parse_parameter(buf, len, vdata);
		
	for (int i = 0; i < vdata.size(); i++)
	{
		int nPn = vdata[i].meter_index;
		string sFn = ptcl_packet_csg::convert_fn(vdata[i].data_item_id);
		string strValue = vdata[i].db_value;
				
		int index = -1;
		for (int m = 0; m < m_ShowSetParamListCtrl.GetItemCount(); m++)
		{
			CString strID = m_ShowSetParamListCtrl.GetItemText(m, 0);
			int id = atoi(strID.GetBuffer(0));
			if (id == nPn) 
			{
				index = m;
				break;
			}
		}

		if (index < 0)
		{
			CString temp;
			temp.Format("%d", nPn);
			index = m_ShowSetParamListCtrl.GetItemCount();
			m_ShowSetParamListCtrl.InsertItem(index, temp);			
			m_ShowSetParamListCtrl.SetItemText(index, 1, "meter");
		}

		if ("E0800001" == sFn)
		{
			CString temp;
			int val = atoi(strValue.c_str());
			if (val < 1 || val > 5)
				temp = "无效数据";
			else
				m_cbMeterType.GetLBText(val - 1, temp);						
			m_ShowSetParamListCtrl.SetItemText(index, 2, temp);
		}
		else if ("E0800002" == sFn)
		{
			if (strValue.compare("FFFFFFFFFFFF") == 0)
				strValue = "无效数据";
			m_ShowSetParamListCtrl.SetItemText(index, 3, strValue.c_str());
		}
		else if ("E0800003" == sFn)
		{
			CString temp;
			int val = atoi(vdata[i].db_value.c_str());
			if (val < 0 || val > 11)
				temp = "无效数据";
			else
				m_cbMeterProtocol.GetLBText(val, temp);
			m_ShowSetParamListCtrl.SetItemText(index, 4, temp);
		}
		else if ("E0800004" == sFn)
		{
			CString temp;
			int val = atoi(vdata[i].db_value.c_str());
			if (val < 1 || val > 3)
				temp = "无效数据";
			else
				m_cbEnergyType.GetLBText(val - 1, temp);
			m_ShowSetParamListCtrl.SetItemText(index, 5, temp);
		}
		else if ("E080000A" == sFn)
		{
			if (strValue.compare("256") == 0)
				strValue = "无效数据";
			m_ShowSetParamListCtrl.SetItemText(index, 6, strValue.c_str());
		}
		else if ("E080000B" == sFn)
		{
			int baud = 4, check = 1, databit = 8, stopbit = 0;
			CString strBaud, strCheck, strDatabit, strStopbit;
			sscanf(strValue.c_str(), "%d,%d,%d,%d", &stopbit, &databit, &check, &baud);
			baud *= 300;
			strBaud.Format("%d", baud);
			m_cbCheck.GetLBText(check, strCheck);
			strDatabit.Format("%d", databit);
			m_cbStopBit.GetLBText(stopbit, strStopbit);
			m_ShowSetParamListCtrl.SetItemText(index, 7, strBaud);
			m_ShowSetParamListCtrl.SetItemText(index, 8, strCheck);
			m_ShowSetParamListCtrl.SetItemText(index, 9, strDatabit);
			m_ShowSetParamListCtrl.SetItemText(index, 10, strStopbit);
		}
		else if ("E0800011" == sFn)
		{
			if (strValue.compare("FFFF") == 0)
				strValue = "无效数据";
			else
				m_ShowSetParamListCtrl.SetItemText(index, 11,strValue.c_str());
		}
		else if ("E0800012" == sFn)
		{
			if (strValue.compare("FFFFFF") == 0)
				strValue = "无效数据";
			else
				m_ShowSetParamListCtrl.SetItemText(index, 12, strValue.c_str());

		}
	}
	ShowOperateLog("读参数成功");
}

void CDlgSetParam::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (!GetDlgItem(IDC_LIST_SETPARAMINFO)->GetSafeHwnd())
		return;
	m_layoutCtrl.SetPos(IDC_LIST_SETPARAMINFO, 	LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	m_layoutCtrl.SetOldRect(CRect(0, 0, cx, cy));
}

void CDlgSetParam::OnBnClickedBtnTotemplate()
{
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;
	
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[1024] = {0};
	sprintf(strSQL, "DELETE FROM TEMPLATE_MP_ARCH");
	pConnect->ExecSQL(strSQL);

	sprintf(strSQL, "INSERT INTO TEMPLATE_MP_ARCH(MP_ID,MP_NAME,MP_ADDR,MP_PROTOCOL,MP_TYPE,MP_ENERGYTYPE,MP_PORT,MP_BAUD," \
		"MP_CHECKBIT,MP_DATABIT,MP_STOPBIT) SELECT MP_ID,MP_NAME,MP_ADDR,MP_PROTOCOL,MP_TYPE,MP_ENERGYTYPE,MP_PORT,MP_BAUD," \
		"MP_CHECKBIT,MP_DATABIT,MP_STOPBIT FROM MP_ARCH WHERE RTU_ID=%s", m_strTerminalNo);
	if (pConnect->ExecSQL(strSQL))
	{
		ShowOperateLog("保存模板成功！");
	}
}

void CDlgSetParam::OnBnClickedBtnFromtemplate()
{
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[1024] = {0};
	sprintf(strSQL, "DELETE FROM MP_ARCH WHERE RTU_ID=%s", m_strTerminalNo);
	pConnect->ExecSQL(strSQL);

	sprintf(strSQL, "INSERT INTO MP_ARCH(RTU_ID,MP_ID,MP_NAME,MP_ADDR,MP_PROTOCOL,MP_TYPE,MP_ENERGYTYPE,MP_PORT,MP_BAUD," \
		"MP_CHECKBIT,MP_DATABIT,MP_STOPBIT) SELECT %s,MP_ID,MP_NAME,MP_ADDR,MP_PROTOCOL,MP_TYPE,MP_ENERGYTYPE,MP_PORT,MP_BAUD," \
		"MP_CHECKBIT,MP_DATABIT,MP_STOPBIT FROM TEMPLATE_MP_ARCH", m_strTerminalNo);
	if (pConnect->ExecSQL(strSQL))
	{
		ShowOperateLog("引用模板成功！");
	}
	InitMeterParamInfoList(m_strTerminalNo);
}

void CDlgSetParam::OnTimer(UINT_PTR nIDEvent)
{
	if (time(NULL) - m_tCmdTime > 60)
	{
		switch(nIDEvent)
		{
		case 1:	
			NextSend();			
			break;
		case 2:
			NextCall();
			break;
		case 3:
			NextDel();
			break;
		}	
	}

	__super::OnTimer(nIDEvent);
}

void CDlgSetParam::NextSend() 
{	
	UpdateData(TRUE);

	CChannel *pChannel = NULL;
	CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
	if (pMainFrm)
		pChannel = pMainFrm->m_pDlgConn->m_pChannel;	
	if (!pChannel)
	{
		MessageBox("未连接终端，请先建立连接！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (m_strTerminalAddr == "")
	{
		MessageBox("未选择终端，双击索引树选择终端！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}
	UINT64 id = ptcl_packet_csg::convert_device_addr(m_strTerminalAddr.GetBuffer(0));
	
	if (!m_bWaiting)
	{
		if (m_nMeterIndex > m_nEndIndex)
		{
			EndTask(m_nOptType);
			return;
		}

		vector<string>vParamID;
		vParamID.push_back("E0800000");	//测量点状态			1bytes
		vParamID.push_back("E0800001");	//测量点性质			1
		vParamID.push_back("E0800002");	//测量点地址			6
		vParamID.push_back("E0800003");	//测量点通信规约		1
		vParamID.push_back("E0800004");	//电能表类型			1		
		vParamID.push_back("E080000A");	//测量点端口号		1
		vParamID.push_back("E080000B");	//端口参数			4
		vParamID.push_back("E0800010");	//测量点接线方式 	1
		vParamID.push_back("E0800011");	//额定电压			2
		vParamID.push_back("E0800012");	//额定电流			3

		vector<ptcl_module_csg::pnfn_data_uint> vDataunit;
		int nCount = 0;
		while (true)
		{
			if (m_nMeterIndex > m_nEndIndex)
				break;

			CString addr;
			int protocol, mtype, etype, port, baud, check, databit, stopbit;
			float fVoltage, fCurrent;
			if (GetMeterInfo(m_nMeterIndex, addr, protocol, mtype, etype, port, baud, check, databit, stopbit, fVoltage, fCurrent))
			{
				for (int j = 0; j < vParamID.size(); j++)
				{
					ptcl_module_csg::pnfn_data_uint dataunit;
					dataunit.pnfn.pn = m_nMeterIndex;
					dataunit.pnfn.fn = ptcl_packet_csg::convert_fn(vParamID.at(j));
					switch (j)
					{
					case 0:		// 测量点状态
						{						
							dataunit.len_data = 1;
							dataunit.data_buffer[0] = 1;
						}
						break;
					case 1:		// 测量点性质
						{						
							dataunit.len_data = 1;
							dataunit.data_buffer[0] = mtype + 1;
						}
						break;
					case 2:		// 测量点地址
						{					
							dataunit.len_data = 6;
							string_to_buffer(addr.GetBuffer(0), dataunit.data_buffer, 6);
						}
						break;
					case 3:		// 测量点通信规约
						{					
							dataunit.len_data = 1;
							dataunit.data_buffer[0] = protocol;
						}
						break;
					case 4:		// 电能表类型
						{
							dataunit.len_data = 1;
							dataunit.data_buffer[0] = etype + 1;
						}
						break;
					case 5:		// 测量点端口号
						{					
							dataunit.len_data = 1;
							dataunit.data_buffer[0] = port - 1;
						}
						break;
					case 6:		// 端口参数
						{
							CString strTemp;
							m_cbBaud.GetLBText(baud, strTemp);
							baud = atoi(strTemp);

							dataunit.len_data = 4;
							dataunit.data_buffer[0] = baud / 300;	// 通信波特率
							dataunit.data_buffer[1] = check;		// 校验方式
							dataunit.data_buffer[2] = databit + 5;	// 数据位
							dataunit.data_buffer[3] = stopbit;		// 停止位
						}
						break;
					case 7:		// 测量点接线方式 
						{						
							dataunit.len_data = 1;
							dataunit.data_buffer[0] = 4;
						}
						break;
					case 8:		// 额定电压 220V
						{
							int temp = fVoltage * 10;
							dataunit.len_data = 2;
							dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(temp / 100);
							dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(temp % 100);
						}
						break;
					case 9:		// 额定电流 5A
						{
							int temp = fCurrent * 1000;
							dataunit.len_data = 3;
							dataunit.data_buffer[2] = temp / 10000;
							temp %= 10000;
							dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(temp / 100);
							dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(temp % 100);
						}
						break;
					default:	
						{
							continue;				
						}
						break;
					}
					vDataunit.push_back(dataunit);			
				}
			}

			m_nMeterIndex++;
			if (++nCount >= 10)
				break;
		}

		if (vDataunit.empty())
			return;
				
		packet.set_device_addr(id);
		ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &vDataunit[0], vDataunit.size());
		pChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		m_tCmdTime = time(NULL);
		m_bWaiting = true;
		m_nRetryCount = 1;

		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}
	else
	{
		if (++m_nRetryCount > 3)
		{
			m_bWaiting = false;
			m_tCmdTime = 0;			
		}
		else
		{
			pChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
			m_tCmdTime = time(NULL);
			m_bWaiting = true;

			CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
			CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
		}
	}	
}

void CDlgSetParam::NextCall()
{
	UpdateData(TRUE);

	CChannel *pChannel = NULL;
	CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
	if (pMainFrm)
		pChannel = pMainFrm->m_pDlgConn->m_pChannel;	
	if (!pChannel)
	{
		MessageBox("未连接终端，请先建立连接！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (m_strTerminalAddr == "")
	{
		MessageBox("未选择终端，双击索引树选择终端！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}
	UINT64 id = ptcl_packet_csg::convert_device_addr(m_strTerminalAddr.GetBuffer(0));

	if (!m_bWaiting)
	{
		if (m_nMeterIndex > m_nEndIndex)
		{
			EndTask(m_nOptType);
			return;
		}

		vector<string>vParamID;
		vParamID.push_back("E0800001");	//测量点性质			1bytes
		vParamID.push_back("E0800002");	//测量点地址			6
		vParamID.push_back("E0800003");	//测量点通信规约		1
		vParamID.push_back("E0800004");	//电能表类型			1
		vParamID.push_back("E080000A");	//测量点端口号		1
		vParamID.push_back("E080000B");	//端口参数			4
		vParamID.push_back("E0800011");	//额定电压			2
		vParamID.push_back("E0800012");	//额定电流			3

		vector<ptcl_module_csg::pnfn_data> vPnFn;
		int nCount = 0;
		while (true)
		{
			if (m_nMeterIndex > m_nEndIndex)
				break;

			for (int j = 0; j < vParamID.size(); j++)
			{
				ptcl_module_csg::pnfn_data pnfn;
				pnfn.pn = m_nMeterIndex;
				pnfn.fn = ptcl_packet_csg::convert_fn(vParamID.at(j));;
				vPnFn.push_back(pnfn);
			}

			m_nMeterIndex++;
			if (++nCount >= 10)
				break;
		}	

		if (vPnFn.empty())
			return;
		
		packet.set_device_addr(id);
		ptcl_module_csg::get_instance()->format_packet_read_parameter(&packet, &vPnFn[0], vPnFn.size());
		pChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		m_tCmdTime = time(NULL);
		m_bWaiting = true;
		m_nRetryCount = 1;

		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}
	else
	{
		if (++m_nRetryCount > 3)
		{
			m_bWaiting = false;
			m_tCmdTime = 0;
		}
		else
		{
			pChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
			m_tCmdTime = time(NULL);
			m_bWaiting = true;

			CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
			CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
		}
	}	
}

void CDlgSetParam::NextDel()
{
	UpdateData(TRUE);

	CChannel *pChannel = NULL;
	CMainFrame *pMainFrm  = (CMainFrame *)AfxGetMainWnd();
	if (pMainFrm)
		pChannel = pMainFrm->m_pDlgConn->m_pChannel;	
	if (!pChannel)
	{
		MessageBox("未连接终端，请先建立连接！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (m_strTerminalAddr == "")
	{
		MessageBox("未选择终端，双击索引树选择终端！", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}
	UINT64 id = ptcl_packet_csg::convert_device_addr(m_strTerminalAddr.GetBuffer(0));

	if (!m_bWaiting)
	{
		if (m_nMeterIndex > m_nEndIndex)
		{
			EndTask(m_nOptType);
			return;
		}

		vector<ptcl_module_csg::pnfn_data_uint> vDataunit;
		int nCount = 0;
		while (true)
		{
			if (m_nMeterIndex > m_nEndIndex)
				break;

			CString addr;
			int protocol, mtype, etype, port, baud, check, databit, stopbit;
			float fVoltage, fCurrent;
			if (!GetMeterInfo(m_nMeterIndex, addr, protocol, mtype, etype, port, baud, check, databit, stopbit, fVoltage, fCurrent))		
				continue;
			UINT64 u64Addr = ptcl_packet_csg::convert_device_addr(addr.GetBuffer(0));

			ptcl_module_csg::pnfn_data_uint dataunit;
			dataunit.pnfn.pn = m_nMeterIndex;
			dataunit.pnfn.fn = ptcl_packet_csg::convert_fn("E0001104");
			dataunit.len_data = 8;
			// 测量点号
			dataunit.data_buffer[1] = ptcl_module::bin_to_bcd(m_nMeterIndex / 100);	
			dataunit.data_buffer[0] = ptcl_module::bin_to_bcd(m_nMeterIndex % 100);
			// 测量点地址
			char temp[16] = {0};
			sprintf(temp, "%012s", addr.GetBuffer(0));
			int a0, a1, a2, a3, a4, a5;
			sscanf(temp, "%2x%2x%2x%2x%2x%2x", &a5, &a4, &a3, &a2, &a1, &a0);
			dataunit.data_buffer[2] = a0;
			dataunit.data_buffer[3] = a1;
			dataunit.data_buffer[4] = a2;
			dataunit.data_buffer[5] = a3;
			dataunit.data_buffer[6] = a4;
			dataunit.data_buffer[7] = a5;
			// 
			vDataunit.push_back(dataunit);

			m_nMeterIndex++;
			if (++nCount >= 10)
				break;
		}

		if (vDataunit.empty())
			return;

		packet.set_device_addr(id);
		ptcl_module_csg::get_instance()->format_packet_set_param(&packet, (unsigned char *)"", &vDataunit[0], vDataunit.size());
		pChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
		m_tCmdTime = time(NULL);
		m_bWaiting = true;
		m_nRetryCount = 1;

		CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
		CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
	}
	else
	{
		if (++m_nRetryCount > 3)
		{
			m_bWaiting = false;
			m_tCmdTime = 0;
		}
		else
		{
			pChannel->Write((char *)packet.get_packet_buffer(), packet.get_packet_length(), id);
			m_tCmdTime = time(NULL);
			m_bWaiting = true;

			CMsgOutput::GetInstance()->ShowpPacketInfo("", 0, TraceBuf(packet.get_packet_buffer(), packet.get_packet_length(), "下发"));
			CLogMgr::GetInstanse().LogPacket("下发", packet.get_packet_buffer(), packet.get_packet_length());
		}
	}
}

void CDlgSetParam::BeginTask(int type)
{
	UpdateData(TRUE);
	if (m_nBeginIndex <= 0 || m_nBeginIndex > m_nEndIndex)
		return;
	
	GetDlgItem(IDC_EDIT_BEGINMETER)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ENDMETER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_READ)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);	
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);

	m_nOptType = type;
	m_tCmdTime = 0;
	m_nRetryCount = 0;
	m_bWaiting = false;
	m_nMeterIndex = m_nBeginIndex;
	SetTimer(m_nOptType, 1000, NULL);}

void CDlgSetParam::EndTask(int type)
{
	KillTimer(type);
	m_nOptType = 0;
	m_tCmdTime = 0;
	m_nRetryCount = 0;
	m_bWaiting = false;
	m_nBeginIndex = 0;

	GetDlgItem(IDC_EDIT_BEGINMETER)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_ENDMETER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_READ)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
}

void CDlgSetParam::OnBnClickedBtnStop()
{
	EndTask(m_nOptType);
}


void CDlgSetParam::OnBnClickedBtnClear()
{
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[1024] = {0};
	sprintf(strSQL, "DELETE FROM MP_ARCH WHERE RTU_ID=%s", m_strTerminalNo);
	pConnect->ExecSQL(strSQL);

	InitMeterParamInfoList(m_strTerminalNo);
}
