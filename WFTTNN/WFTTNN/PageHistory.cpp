// PageHistory.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "PageHistory.h"
#include "afxdialogex.h"


// CPageHistory dialog

IMPLEMENT_DYNAMIC(CPageHistory, CDialogEx)

CPageHistory::CPageHistory(int nDataType, CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageHistory::IDD, pParent)
	, m_tBeginDate(time(NULL))
	, m_tBeginTime(time(NULL))
	, m_tEndDate(time(NULL))
	, m_tEndtime(time(NULL))
{
	m_nDataType = nDataType;
}

CPageHistory::~CPageHistory()
{
}

void CPageHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HISTORY, m_ctrlDataItem);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_STARTDATE, m_tBeginDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_STARTTIME, m_tBeginTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_ENDDATE, m_tEndDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_ENDTIME, m_tEndtime);
	DDX_Control(pDX, IDC_COMBO1, m_cbCycle);
}


BEGIN_MESSAGE_MAP(CPageHistory, CDialogEx)
END_MESSAGE_MAP()


// CPageHistory message handlers


BOOL CPageHistory::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitDataitem();

	m_cbCycle.AddString("默认");
	m_cbCycle.AddString("1分钟");
	m_cbCycle.AddString("5分钟");
	m_cbCycle.AddString("15分钟");
	m_cbCycle.AddString("30分钟");
	m_cbCycle.AddString("60分钟");
	m_cbCycle.AddString("1日");
	m_cbCycle.AddString("1月");
	m_cbCycle.SetCurSel(0);

	return TRUE;  
}

void CPageHistory::InitDataitem()
{
	DWORD dwExtendStyle = m_ctrlDataItem.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_ctrlDataItem.SetExtendedStyle(dwExtendStyle);
	m_ctrlDataItem.InsertColumn(0, "#",LVCFMT_CENTER, 20 );
	m_ctrlDataItem.InsertColumn(1, "数据项标识", LVCFMT_CENTER, 160);
	m_ctrlDataItem.InsertColumn(2, "数据项名称", LVCFMT_LEFT, 300);


	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	int nRow = m_ctrlDataItem.GetItemCount();
	char strSQL[1024] = {0};	
	
	// 曲线数据自动测试无需量
	if (2 == m_nDataType)
		sprintf(strSQL, "SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 2 " \
		" AND PARAM_ID <> '01010000' AND PARAM_ID <> '01010100' AND PARAM_ID <> '01010200' "\
		" AND PARAM_ID <> '01010300' AND PARAM_ID <> '01010400' AND PARAM_ID <> '01020000' "\
		" AND PARAM_ID <> '01020100' AND PARAM_ID <> '01020200' AND PARAM_ID <> '01020300' "\
		" AND PARAM_ID <> '01020400' ORDER BY PARAM_ID DESC");
	else
		sprintf(strSQL, "SELECT * FROM ALL_PARAMETER_INFO WHERE ID = %d ORDER BY PARAM_ID DESC", m_nDataType);

	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;
	while (!record.IsEndEOF())
	{
		_variant_t val;
		CString strID = _T("");
		CString strParamID = _T("");
		CString strParamName = _T("");

		record.GetFieldValue("ID",val);
		strID.Format("%d",(long)val) ;

		record.GetFieldValue("PARAM_ID",val);
		strParamID = val.bstrVal;

		record.GetFieldValue("PARAM_NAME",val);
		strParamName = val.bstrVal;
		
		m_ctrlDataItem.InsertItem(0, "");
		m_ctrlDataItem.SetItemText(0, 1, strParamID);
		m_ctrlDataItem.SetItemText(0, 2, strParamName);

		record.RecordMoveNext();
		nRow++;
	}
}

void CPageHistory::GetTime(time_t &tBegin, time_t &tEnd, int &cycle)
{
	UpdateData(TRUE);
	
	CTime ctBegin(m_tBeginDate.GetYear(), m_tBeginDate.GetMonth(), m_tBeginDate.GetDay(), m_tBeginTime.GetHour(), m_tBeginTime.GetMinute(), m_tBeginTime.GetSecond());
	CTime ctEnd(m_tEndDate.GetYear(), m_tEndDate.GetMonth(), m_tEndDate.GetDay(), m_tEndtime.GetHour(), m_tEndtime.GetMinute(), m_tEndtime.GetSecond());
	tBegin = ctBegin.GetTime();
	tEnd = ctEnd.GetTime();
	cycle = m_cbCycle.GetCurSel();
}


void CPageHistory::RefreshDataItem(CString strTaskId)
{
	for (int i = 0; i < m_ctrlDataItem.GetItemCount(); i++)
	{
		m_ctrlDataItem.SetCheck(i, FALSE);
	}

	vector<CString> vDataItem;
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	int nRow = m_ctrlDataItem.GetItemCount();
	char strSQL[300] = {0};	
	sprintf(strSQL, "SELECT * FROM AUTOTEST_READING_ARCH WHERE READING_ID=%s AND PARAM_TYPE=%d", strTaskId, m_nDataType);
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;
	while (!record.IsEndEOF())
	{		
		_variant_t val;
		CString strParamID = _T("");

		record.GetFieldValue("PARAM_ID",val);
		strParamID = val.bstrVal;
		vDataItem.push_back(strParamID);

		record.RecordMoveNext();
	}

	for (int i = 0; i < vDataItem.size(); i++)
	{
		for (int j = 0; j < m_ctrlDataItem.GetItemCount(); j++)
		{
			CString paramId = m_ctrlDataItem.GetItemText(j, 1);
			if (paramId.CompareNoCase(vDataItem.at(i)) == 0)
			{
				m_ctrlDataItem.SetCheck(j);
				break;
			}
		}
	}
}