// PageEvent.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "PageEvent.h"
#include "afxdialogex.h"


// CPageEvent dialog

IMPLEMENT_DYNAMIC(CPageEvent, CDialogEx)

CPageEvent::CPageEvent(int nDataType, CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageEvent::IDD, pParent)
	, m_ctBeginDate(time(NULL))
	, m_ctBeginTime(time(NULL))
	, m_ctEndDate(time(NULL))
	, m_ctEndTime(time(NULL))
{
	m_nDataType = nDataType;
}

CPageEvent::~CPageEvent()
{
}

void CPageEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HISTORY, m_ctrlDataItem);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_STARTDATE, m_ctBeginDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_STARTTIME, m_ctBeginTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_ENDDATE, m_ctEndDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_ENDTIME, m_ctEndTime);
}


BEGIN_MESSAGE_MAP(CPageEvent, CDialogEx)
END_MESSAGE_MAP()


// CPageEvent message handlers

void CPageEvent::InitDataitem()
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
	char strSQL[300] = {0};	
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


BOOL CPageEvent::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitDataitem();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CPageEvent::GetTime(time_t &tBegin, time_t &tEnd, int &cycle)
{
	UpdateData(TRUE);
	
	CTime ctBegin(m_ctBeginDate.GetYear(), m_ctBeginDate.GetMonth(), m_ctBeginDate.GetDay(), m_ctBeginTime.GetHour(), m_ctBeginTime.GetMinute(), m_ctBeginTime.GetSecond());
	CTime ctEnd(m_ctEndDate.GetYear(), m_ctEndDate.GetMonth(), m_ctEndDate.GetDay(), m_ctEndTime.GetHour(), m_ctEndTime.GetMinute(), m_ctEndTime.GetSecond());
	tBegin = ctBegin.GetTime();
	tEnd = ctEnd.GetTime();
	cycle = 0;
}


void CPageEvent::RefreshDataItem(CString strTaskId)
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