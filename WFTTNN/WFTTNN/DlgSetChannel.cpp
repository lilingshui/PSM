// DlgSetChannel.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgSetChannel.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetChannel dialog

static char *s_chlType[10] = { "TCP Server", "TCP Client", "专线MODEM", "拨号MODEM" };
static char *s_comName[6] = { "COM1", "COM2", "COM3", "COM4", "COM5", "COM6" };
static char *s_baud[7] = { "300", "600", "1200", "2400", "4800", "9600", "19200" };
static char *s_databit[2] = { "7", "8" };
static char *s_stopbit[3] = { "1", "1.5", "2" };
static char *s_checkway[3] = { "无校验", "奇校验", "偶校验" };

CDlgSetChannel::CDlgSetChannel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetChannel::IDD, pParent)
	, test(0)
	, m_layoutCtrl(this)
{
	//{{AFX_DATA_INIT(CDlgSetChannel)
	m_strChannelID = _T("");
	m_strChannelName = _T("");
	m_strPhone = _T("");
	m_strPort = _T("");
	m_strChar = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSetChannel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetChannel)
	DDX_Control(pDX, IDC_IPADDRESS, m_IpAddress);
	DDX_Control(pDX, IDC_LIST_SETCHANNELINFO, m_ShowSetChannelInfo);
	DDX_Control(pDX, IDC_COMBO_STOPBIT, m_cbStopBit);
	DDX_Control(pDX, IDC_COMBO_SERIAL, m_cbSerail);
	DDX_Control(pDX, IDC_COMBO_DATABIT, m_cbDataBit);
	DDX_Control(pDX, IDC_COMBO_CHECKWAY, m_cbCheckWay);
	DDX_Control(pDX, IDC_COMBO_CHANNELTYPE, m_cbChannelType);
	DDX_Control(pDX, IDC_COMBO_BAUD, m_cbBaud);
	DDX_Text(pDX, IDC_EDIT_CHANNELID, m_strChannelID);
	DDX_Text(pDX, IDC_EDIT_CHANNELNAME, m_strChannelName);
	DDX_Text(pDX, IDC_EDIT_PHONE, m_strPhone);
	DDX_Text(pDX, IDC_EDIT_PORT, m_strPort);
	DDX_Text(pDX, IDC_EDIT_CHAR, m_strChar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetChannel, CDialog)
	//{{AFX_MSG_MAP(CDlgSetChannel)
	ON_BN_CLICKED(IDC_BUTTON_ADDCHANNEL, OnButtonAddchannel)
	ON_BN_CLICKED(IDC_BUTTON_DELCHANNEL, OnButtonDelchannel)
	ON_BN_CLICKED(IDC_BUTTON_MODIFYCHANNEL, OnButtonModifychannel)
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNELTYPE, OnSelchangeComboChanneltype)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SETCHANNELINFO, &CDlgSetChannel::OnLvnItemchangedListSetchannelinfo)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetChannel message handlers

BOOL CDlgSetChannel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for (int i = 0; i < 4; i++)
	{
		m_cbChannelType.InsertString(i, s_chlType[i]);
	}	
	m_cbChannelType.SetCurSel(0);
	OnSelchangeComboChanneltype();

	for (int i = 0; i < 6; i++)
	{
		m_cbSerail.InsertString(i, s_comName[i]);
	}
	m_cbSerail.SetCurSel(0);

	for (int i = 0; i < 7; i++)
	{
		m_cbBaud.InsertString(i, s_baud[i]);
	}
	m_cbBaud.SetCurSel(0);

	for (int i = 0; i < 2; i++)
	{
		m_cbDataBit.InsertString(i, s_databit[i]);
	}
	m_cbDataBit.SetCurSel(0);

	for (int i = 0; i < 3; i++)
	{
		m_cbStopBit.InsertString(i, s_stopbit[i]);
	}
	m_cbStopBit.SetCurSel(0);

	for (int i = 0; i < 3; i++)
	{
		m_cbCheckWay.InsertString(i, s_checkway[i]);
	}
	m_cbCheckWay.SetCurSel(0);
	

	DWORD dwExtendStyle = m_ShowSetChannelInfo.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ShowSetChannelInfo.SetExtendedStyle(dwExtendStyle);
	
	/**************初始化列表显示**************************************/
	m_ShowSetChannelInfo.InsertColumn(0,"通道ID",LVCFMT_CENTER,100);
	m_ShowSetChannelInfo.InsertColumn(1,"通道名称",LVCFMT_CENTER,120);
	m_ShowSetChannelInfo.InsertColumn(2,"通道类型",LVCFMT_CENTER,120);
	m_ShowSetChannelInfo.InsertColumn(3,"通讯IP",LVCFMT_CENTER,120);
	m_ShowSetChannelInfo.InsertColumn(4,"通讯端口号",LVCFMT_CENTER,100);
	m_ShowSetChannelInfo.InsertColumn(5,"电话号码",LVCFMT_CENTER,100);
	m_ShowSetChannelInfo.InsertColumn(6,"初始化串",LVCFMT_CENTER,100);
	m_ShowSetChannelInfo.InsertColumn(7,"通讯串口",LVCFMT_CENTER,100);
	m_ShowSetChannelInfo.InsertColumn(8,"波特率",LVCFMT_CENTER,100);
	m_ShowSetChannelInfo.InsertColumn(9,"数据位",LVCFMT_CENTER,100);
	m_ShowSetChannelInfo.InsertColumn(10,"停止位",LVCFMT_CENTER,100);
	m_ShowSetChannelInfo.InsertColumn(11,"校验方式",LVCFMT_CENTER,100);
	
	InitChannelInfoList();
	
	CRect rect;
	GetClientRect(&rect);
	m_layoutCtrl.SetOldRect(rect);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSetChannel::OnButtonAddchannel() 
{
	UpdateData(TRUE);

	if (m_strChannelID == "")
	{
		AfxMessageBox("请输入通道ID！");
		return ;
	}
	
	if (m_strChannelName == "")
	{
		AfxMessageBox("请输入通道名称！");
		return ;
	}

	int nChannelType = m_cbChannelType.GetCurSel();
	switch (nChannelType)
	{
	case 0:
	case 1:
	case 2:
	case 4:		
		if (m_strPort == "") 
		{
			AfxMessageBox("请输入端口号！");
			return;
		}
		break;		
	case 3:		
		if (m_strPhone == "") 
		{
			AfxMessageBox("请输入电话号码！");
			return;
		}					
		break;
	}

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	CString ipAddr = "";
	m_IpAddress.GetWindowTextA(ipAddr);

	char strSQL[300] = {0};	
	sprintf(strSQL, "INSERT INTO COMM_ARCH(CH_ID,CH_TYPE,CH_NAME,COM_ID,COM_BAUD,COM_DATA_BIT,COM_STOP_BIT,COM_CHECK_BIT,PHONE_NUM,PHONE_INITSTR,IP_ADDR,IP_PORT) "
		" VALUES(%s, %d, '%s', %d, %d, %d, %d, %d, '%s', '%s', '%s', %s)", m_strChannelID, m_cbChannelType.GetCurSel(), m_strChannelName, 
		m_cbSerail.GetCurSel(), m_cbBaud.GetCurSel(), m_cbDataBit.GetCurSel(), m_cbStopBit.GetCurSel(), m_cbCheckWay.GetCurSel(),
		m_strPhone, m_strChar, ipAddr, m_strPort);	
	if (pConnect->ExecSQL(strSQL))
	{		
		int nRow = m_ShowSetChannelInfo.GetItemCount();
		m_ShowSetChannelInfo.InsertItem(nRow, m_strChannelID);
		m_ShowSetChannelInfo.SetItemText(nRow, 1, m_strChannelName);
		m_ShowSetChannelInfo.SetItemText(nRow, 2, s_chlType[m_cbChannelType.GetCurSel()]);
		m_ShowSetChannelInfo.SetItemText(nRow, 3, ipAddr);
		m_ShowSetChannelInfo.SetItemText(nRow, 4, m_strPort);
		m_ShowSetChannelInfo.SetItemText(nRow, 5, m_strPhone);
		m_ShowSetChannelInfo.SetItemText(nRow, 6, m_strChar);

		ShowOperateLog("操作成功");
	}
	else
	{
		MessageBox(MSG_DUPLICATE, "提示", MB_OK|MB_ICONWARNING);
		return;
	}

	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	pMainFrm->RefreshChannelArch();
	pMainFrm->RefreshConnChannel();
}

void CDlgSetChannel::OnButtonDelchannel() 
{
	if (MessageBox("确认操作？", "提示", MB_YESNO) != IDYES)
		return;
	
	UpdateData(TRUE);

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};
	sprintf(strSQL,"DELETE FROM COMM_ARCH WHERE CH_ID=%s", m_strChannelID);
	if (pConnect->ExecSQL(strSQL)) {
		int nIndex = m_ShowSetChannelInfo.GetSelectionMark();
		m_ShowSetChannelInfo.DeleteItem(nIndex);		
	}		

	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	pMainFrm->RefreshChannelArch();
	pMainFrm->RefreshConnChannel();
}

void CDlgSetChannel::OnButtonModifychannel() 
{
	OnButtonDelchannel();
	OnButtonAddchannel();
}

void CDlgSetChannel::InitChannelInfoList()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;
	
	char strSQL[300] = {0};
	int nRow = 0;	
	sprintf(strSQL,"SELECT * FROM COMM_ARCH ORDER BY CH_ID");	//  WHERE CH_ID <> 999999
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;
	
	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nChannelID=0, nChannelType=0, nComId=0, nComBaud=0, nComDataBit=0, nComStopBit=0, nCheckBit=0, nPort=0;

		CString strChannelID = _T("");		
		CString strChannelName = _T("");
		CString strPhoneNumber = _T("");
		CString strPhoneInitStr = _T("");
		CString strIP = _T("");
		CString strPort = _T("");

		record.GetFieldValue("CH_ID",nChannelID);
		record.GetFieldValue("CH_TYPE",nChannelType);
		record.GetFieldValue("COM_ID",nComId);
		record.GetFieldValue("COM_BAUD",nComBaud);
		record.GetFieldValue("COM_DATA_BIT",nComDataBit);
		record.GetFieldValue("COM_STOP_BIT",nComStopBit);
		record.GetFieldValue("COM_CHECK_BIT",nCheckBit);
		record.GetFieldValue("IP_PORT",nPort);
		
		record.GetFieldValue("CH_NAME",val);
		if (val.vt != VT_NULL)	
			strChannelName = val.bstrVal;
		
		record.GetFieldValue("PHONE_NUM",val);
		if (val.vt != VT_NULL)	
			strPhoneNumber = val.bstrVal;
		
		record.GetFieldValue("PHONE_INITSTR",val);
		if (val.vt != VT_NULL)	
			strPhoneInitStr = val.bstrVal;
		
		record.GetFieldValue("IP_ADDR",val);
		if (val.vt != VT_NULL)	
			strIP = val.bstrVal;
		
		strChannelID.Format("%d", nChannelID);
		strPort.Format("%d", nPort);
				
		m_ShowSetChannelInfo.InsertItem(nRow, strChannelID);
		m_ShowSetChannelInfo.SetItemText(nRow, 1, strChannelName);
		m_ShowSetChannelInfo.SetItemText(nRow, 2, s_chlType[nChannelType]);
		m_ShowSetChannelInfo.SetItemText(nRow, 3, strIP);
		m_ShowSetChannelInfo.SetItemText(nRow, 4, strPort);
		m_ShowSetChannelInfo.SetItemText(nRow, 5, strPhoneNumber);
		m_ShowSetChannelInfo.SetItemText(nRow, 6, strPhoneInitStr);
		m_ShowSetChannelInfo.SetItemText(nRow, 7, s_comName[nComId]);
		m_ShowSetChannelInfo.SetItemText(nRow, 8, s_baud[nComBaud]);
		m_ShowSetChannelInfo.SetItemText(nRow, 9, s_databit[nComDataBit]);
		m_ShowSetChannelInfo.SetItemText(nRow, 10, s_stopbit[nComStopBit]);
		m_ShowSetChannelInfo.SetItemText(nRow, 11, s_checkway[nCheckBit]);
	
		record.RecordMoveNext();
		nRow++;
	}
}

void CDlgSetChannel::OnSelchangeComboChanneltype() 
{
	// TODO: Add your control notification handler code here

	switch(m_cbChannelType.GetCurSel())
	{
	case 0://TCP/IP
	case 1://多功能装置
	case 2://装置转专线MODEM
	case 4://装置转RS232
		{
			GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);

			GetDlgItem(IDC_EDIT_PHONE)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_CHAR)->EnableWindow(FALSE);				
			m_cbSerail.EnableWindow(FALSE);
			m_cbBaud.EnableWindow(FALSE);
			m_cbDataBit.EnableWindow(FALSE);
			m_cbStopBit.EnableWindow(FALSE);
			m_cbCheckWay.EnableWindow(FALSE);
		}
		break;
	case 3://装置转拨号MODEM
		{
			GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_PHONE)->EnableWindow(TRUE);									
			GetDlgItem(IDC_EDIT_CHAR)->EnableWindow(TRUE);	
			m_cbSerail.EnableWindow(FALSE);
			m_cbBaud.EnableWindow(FALSE);
			m_cbDataBit.EnableWindow(FALSE);
			m_cbStopBit.EnableWindow(FALSE);
			m_cbCheckWay.EnableWindow(FALSE);			
		}
		break;
	default:
		break;
	}
}

void CDlgSetChannel::OnLvnItemchangedListSetchannelinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(pNMLV->uChanged == LVIF_STATE)
	{
		if(pNMLV->uNewState & LVIS_SELECTED)
		{
			int nItem=pNMLV->iItem;

			m_strChannelID = m_ShowSetChannelInfo.GetItemText(nItem, 0);
			m_strChannelName = m_ShowSetChannelInfo.GetItemText(nItem, 1);
			m_cbChannelType.SelectString(0, m_ShowSetChannelInfo.GetItemText(nItem, 2));
			m_IpAddress.SetWindowTextA(m_ShowSetChannelInfo.GetItemText(nItem, 3));
			m_strPort = m_ShowSetChannelInfo.GetItemText(nItem, 4);
			m_strPhone = m_ShowSetChannelInfo.GetItemText(nItem, 5);
			m_strChar = m_ShowSetChannelInfo.GetItemText(nItem, 6);

			OnSelchangeComboChanneltype();
			UpdateData(FALSE);
		}
	}
	
	*pResult = 0;
}


void CDlgSetChannel::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (!GetDlgItem(IDC_LIST_SETCHANNELINFO)->GetSafeHwnd())
		return;
	m_layoutCtrl.SetPos(IDC_LIST_SETCHANNELINFO, 	LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	m_layoutCtrl.SetOldRect(CRect(0, 0, cx, cy));
}
