// DlgConnect.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgConnect.h"
#include "ItemData.h"
#include "ptcl_codec/ptcl_packet_csg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConnect dialog


CDlgConnect::CDlgConnect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConnect::IDD, pParent)
	, m_strTerminalAddr("")
	, m_strTerminalName(_T("双击索引树选择终端"))
	, m_pChannel(NULL)
{
	//{{AFX_DATA_INIT(CDlgConnect)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConnect)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT1, m_strTerminalName);
	DDX_Control(pDX, IDC_CMB_CHL_TYPE, m_cmbChlType);
}


BEGIN_MESSAGE_MAP(CDlgConnect, CDialog)
	//{{AFX_MSG_MAP(CDlgConnect)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	//}}AFX_MSG_MAP	
	ON_BN_CLICKED(IDC_BTN_CONNECT, OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT, OnBnClickedBtnDisconnect)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConnect message handlers

void CDlgConnect::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


void CDlgConnect::OnBnClickedBtnConnect()
{
	int nSel = m_cmbChlType.GetCurSel();
	if (nSel < 0)
		return;

	int nID = m_cmbChlType.GetItemData(nSel);
	m_pChannel = CChannelMgr::getInstance()->getChannel(nID);
	unsigned __int64 id = ptcl_packet_csg::convert_device_addr(m_strTerminalAddr.GetBuffer(0));
	this->BeginWaitCursor();
	if (m_pChannel && m_pChannel->ConnectTo(id) == 0)
	{
		GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);
		this->ShowWindow(SW_HIDE);
		
		MessageBox("连接成功");
	}
	else
	{
		MessageBox("连接失败");
	}
	this->EndWaitCursor();
}

void CDlgConnect::OnEvent(unsigned int nEventID, void *pParam)
{
	if (Event_Type_Item_Double_Click != nEventID || !::IsWindowVisible(this->GetSafeHwnd()))
		return;
	
	if (ITEM_TYPE_TERMINAL == ((ItemData *)pParam)->m_type)
	{
		TerminalItemData *data = (TerminalItemData *)pParam;
		m_strTerminalAddr = data->m_addr;
		//m_strTerminalAddr = data->t_addr; //MODIFY BY WM 20131126
		m_strTerminalName = data->m_addr;
		//m_strTerminalName = data->t_addr;
		UpdateData(FALSE);
	}
}

BOOL CDlgConnect::OnInitDialog()
{
	__super::OnInitDialog();

	InitChannelType();
	
	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);

	return TRUE;
}

void CDlgConnect::InitChannelType()
{
	m_cmbChlType.ResetContent();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;
	
	char strSQL[300] = {0};
	int nRow = 0;	
	sprintf(strSQL,"SELECT * FROM COMM_ARCH WHERE CH_ID <> 999999");	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;
	
	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nChannelID = 0; 		
		CString strChannelName = _T("");		
				
		record.GetFieldValue("CH_ID", nChannelID);
		record.GetFieldValue("CH_NAME", val);
		strChannelName = val.bstrVal;

		m_cmbChlType.InsertString(0, strChannelName);
		m_cmbChlType.SetItemData(0, nChannelID);

		record.RecordMoveNext();
	}
}


void CDlgConnect::OnBnClickedBtnDisconnect()
{
	if (m_pChannel)
	{
		unsigned __int64 id = ptcl_packet_csg::convert_device_addr(m_strTerminalAddr.GetBuffer(0));
		m_pChannel->Disonnect(id);
		m_pChannel = NULL;

		GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	}
}
