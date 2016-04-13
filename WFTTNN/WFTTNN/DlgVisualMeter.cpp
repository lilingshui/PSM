// DlgVisualMeter.cpp : 实现文件
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgVisualMeter.h"
#include "ItemData.h"


// CDlgVisualMeter 对话框

IMPLEMENT_DYNAMIC(CDlgVisualMeter, CDialog)
CDlgVisualMeter::CDlgVisualMeter(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVisualMeter::IDD, pParent)
	, m_nTermNo(0)
{
}

CDlgVisualMeter::~CDlgVisualMeter()
{
}

void CDlgVisualMeter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATAITEM, m_ctrlDataItemList);
	DDX_Control(pDX, IDC_LIST_OPT_RESULT, m_ctrlResult);
	DDX_Control(pDX, IDC_COMBO1, m_cmbMeterList);
}

BEGIN_MESSAGE_MAP(CDlgVisualMeter, CDialog)
END_MESSAGE_MAP()


// CDlgVisualMeter 消息处理程序
BOOL CDlgVisualMeter::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwExtendStyle = 0;
	dwExtendStyle = m_ctrlDataItemList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlDataItemList.SetExtendedStyle(dwExtendStyle);
	m_ctrlDataItemList.InsertColumn(0,"",LVCFMT_CENTER,20);
	m_ctrlDataItemList.InsertColumn(1,"数据项标识",LVCFMT_CENTER,100);
	m_ctrlDataItemList.InsertColumn(2,"数据项名称",LVCFMT_LEFT,300);
	InitDataItemList();

	dwExtendStyle = m_ctrlResult.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ctrlResult.SetExtendedStyle(dwExtendStyle);	
	m_ctrlResult.InsertColumn(0,"时间",LVCFMT_CENTER,100);
	m_ctrlResult.InsertColumn(1,"数据项名称",LVCFMT_LEFT,100);
	m_ctrlResult.InsertColumn(2,"表名",LVCFMT_CENTER,100);	
	m_ctrlResult.InsertColumn(3,"值",LVCFMT_CENTER,100);
	m_ctrlResult.InsertColumn(4,"虚拟表",LVCFMT_LEFT,100);
	m_ctrlResult.InsertColumn(5,"虚拟表值",LVCFMT_CENTER,100);
	m_ctrlResult.InsertColumn(6,"对比结果",LVCFMT_LEFT,100);	


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgVisualMeter::InitDataItemList()
{
	m_ctrlDataItemList.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	const char *strSQL = "SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 2 or ID = 3";
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

		record.RecordMoveNext();
	}
}

void CDlgVisualMeter::InitMeterList()
{	
	while (m_cmbMeterList.GetCount())
	{
		m_cmbMeterList.DeleteString(0);
	}

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[256];
	sprintf(strSQL, "SELECT * FROM MP_ARCH WHERE RTU_ID = %d", m_nTermNo);	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nMeterId = 0;
		CString strMeterName = "";

		record.GetFieldValue("MP_ID", nMeterId);
		record.GetFieldValue("MP_NAME",val);
		strMeterName = val.bstrVal;

		m_cmbMeterList.InsertString(m_cmbMeterList.GetCount(), strMeterName);
		m_cmbMeterList.SetItemData(m_cmbMeterList.GetCount(), nMeterId);

		record.RecordMoveNext();
	}

	m_cmbMeterList.SetCurSel(0);
}


void CDlgVisualMeter::OnEvent(unsigned int nEventID, void *pParam)
{	
	if (Event_Type_Item_Double_Click != nEventID || !::IsWindowVisible(this->GetSafeHwnd()))
		return;

	ItemData *data = (ItemData *)pParam;
	if (ITEM_TYPE_TERMINAL  == data->m_type)
	{
		m_nTermNo = data->m_id;	
		InitMeterList();		
	}
}