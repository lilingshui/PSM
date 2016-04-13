// DlgDataBrowse.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgDataBrowse.h"
#include ".\dlgdatabrowse.h"
#include "ReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDataBrowse dialog


CDlgDataBrowse::CDlgDataBrowse(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDataBrowse::IDD, pParent)
	, m_layoutCtrl(this)
	, m_bInspectDT(FALSE)
	, m_strBatchNo(_T(""))
	, m_strInspectUnit(_T(""))
	, m_strSupplyYear(_T(""))
	, m_strCheckType(_T(""))
	, m_dtInspectStart(time(NULL))
	, m_dtInspectEnd(time(NULL))
	, m_strTermAddr(_T(""))
	, m_strFactory(_T(""))
	, m_strOutNo(_T(""))
	, m_nTermType(-1)
	, m_nResult(-1)	
	, m_nTermId(0)
	, m_nBatchId(0)
{
	//{{AFX_DATA_INIT(CDlgDataBrowse)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgDataBrowse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDataBrowse)
	DDX_Control(pDX, IDC_LIST_DATABROWSE, m_ShowDataBrowseInfoListCtrl);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_SUPPLY, m_bInspectDT);
	DDX_Text(pDX, IDC_TXT_BATCHNO, m_strBatchNo);
	DDX_Text(pDX, IDC_TXT_INSPECTUNIT, m_strInspectUnit);
	DDX_CBString(pDX, IDC_CMB_SUPPLY, m_strSupplyYear);
	DDX_CBString(pDX, IDC_CMB_CHECKTYPE, m_strCheckType);
	DDX_DateTimeCtrl(pDX, IDC_DTP_INSPECTDT_START, m_dtInspectStart);
	DDX_DateTimeCtrl(pDX, IDC_DTP_INSPECTDT_END, m_dtInspectEnd);
	DDX_Text(pDX, IDC_TXT_TERMADDR, m_strTermAddr);
	DDX_CBString(pDX, IDC_CMB_DM_FACTORY, m_strFactory);
	DDX_Text(pDX, IDC_TXT_OUTNO, m_strOutNo);
	DDX_CBIndex(pDX, IDC_CMB_TERM_TYPE, m_nTermType);
	DDX_CBIndex(pDX, IDC_CMB_RESULT, m_nResult);
	DDX_Control(pDX, IDC_LIST_DM_TERM, m_ctrlTermList);	
	DDX_Control(pDX, IDC_CMB_SUPPLY, m_cbSupplyYear);
	DDX_Control(pDX, IDC_CMB_CHECKTYPE, m_cbCheckType);
	DDX_Control(pDX, IDC_CMB_DM_FACTORY, m_cbFactory);
	DDX_Control(pDX, IDC_CMB_TERM_TYPE, m_cbTermType);
	DDX_Control(pDX, IDC_CMB_RESULT, m_cbResult);
}


BEGIN_MESSAGE_MAP(CDlgDataBrowse, CDialog)
	//{{AFX_MSG_MAP(CDlgDataBrowse)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CDlgDataBrowse::OnBnClickedBtnSearch)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DM_TERM, &CDlgDataBrowse::OnNMDblclkListDmTerm)
	ON_BN_CLICKED(IDC_BTN_DB_REPORT, &CDlgDataBrowse::OnBnClickedBtnDbReport)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_DATABROWSE, &CDlgDataBrowse::OnCustomdrawList)
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DM_TERM, &CDlgDataBrowse::OnLvnItemchangedListDmTerm)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDataBrowse message handlers

BOOL CDlgDataBrowse::OnInitDialog() 
{
	CDialog::OnInitDialog();

	DWORD dwExtendStyle = 0;
	dwExtendStyle= m_ShowDataBrowseInfoListCtrl.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ShowDataBrowseInfoListCtrl.SetExtendedStyle(dwExtendStyle);		
	m_ShowDataBrowseInfoListCtrl.InsertColumn(0,"测量点",LVCFMT_LEFT,80);
	m_ShowDataBrowseInfoListCtrl.InsertColumn(1,"通道名称",LVCFMT_LEFT,100);
	m_ShowDataBrowseInfoListCtrl.InsertColumn(2,"任务名称",LVCFMT_LEFT,150);
	m_ShowDataBrowseInfoListCtrl.InsertColumn(3,"数据项类型",LVCFMT_LEFT,150);
	m_ShowDataBrowseInfoListCtrl.InsertColumn(4,"数据项名称",LVCFMT_LEFT,150);
	m_ShowDataBrowseInfoListCtrl.InsertColumn(5,"实测值",LVCFMT_LEFT,100);
	m_ShowDataBrowseInfoListCtrl.InsertColumn(6,"参考值",LVCFMT_LEFT,100);		
	m_ShowDataBrowseInfoListCtrl.InsertColumn(7,"执行结果",LVCFMT_LEFT,100);

	dwExtendStyle= m_ctrlTermList.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ctrlTermList.SetExtendedStyle(dwExtendStyle);	
	m_ctrlTermList.InsertColumn(0,"#", LVCFMT_CENTER,36);
	m_ctrlTermList.InsertColumn(1, "批次号", LVCFMT_CENTER, 100);
	m_ctrlTermList.InsertColumn(2, "送检单位", LVCFMT_LEFT, 100);
	m_ctrlTermList.InsertColumn(3, "检测类别", LVCFMT_LEFT, 100);
	m_ctrlTermList.InsertColumn(4, "送检日期", LVCFMT_LEFT, 100);
	m_ctrlTermList.InsertColumn(5, "供货年度", LVCFMT_LEFT, 100);
	m_ctrlTermList.InsertColumn(6, "序号", LVCFMT_LEFT, 100);
	m_ctrlTermList.InsertColumn(7, "终端地址", LVCFMT_LEFT, 100);
	m_ctrlTermList.InsertColumn(8, "生产厂家", LVCFMT_LEFT, 100);
	m_ctrlTermList.InsertColumn(9, "出厂编号", LVCFMT_LEFT, 100);
	m_ctrlTermList.InsertColumn(10, "终端类型", LVCFMT_LEFT, 100);
	//m_ctrlTermList.InsertColumn(11, "终端型号", LVCFMT_LEFT, 100);//MODIFY BY WM 20131126


	// 供货年度
	CString strYear;
	for (int i = 2012; i <= 2033; i++)
	{
		strYear.Format("%d", i);
		m_cbSupplyYear.AddString(strYear);
	}

	InitFactory();


	CRect rect;
	GetClientRect(&rect);
	m_layoutCtrl.SetOldRect(rect);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDataBrowse::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if (!GetDlgItem(IDC_LIST_DATABROWSE)->GetSafeHwnd())
		return;
	m_layoutCtrl.SetPos(IDC_LIST_DATABROWSE, LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	m_layoutCtrl.SetPos(IDC_LIST_DM_TERM, LT_LEFT_TOP, RB_RIGHT);
	m_layoutCtrl.SetOldRect(CRect(0, 0, cx, cy));
}


void CDlgDataBrowse::OnBnClickedBtnSearch()
{
	UpdateData(TRUE);
	if (m_strBatchNo == "" && m_strInspectUnit == "" && m_strSupplyYear == "" && m_strCheckType == "" 
		&& !m_bInspectDT && m_strTermAddr == "" && m_strFactory == "" && m_strOutNo == ""
		&& m_nTermType < 0	&& m_nResult < 0)
	{
		MessageBox("请输入检索条件！", "提示", MB_OK|MB_ICONWARNING);
		return;
	}	
	
	m_ctrlTermList.DeleteAllItems();		// 清除

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;

	string strSQL = "SELECT RTU_ID, BATCH_INFO.BatchNo AS BatchNo, InspectUnit, CheckType, InspectDataTime, SupplyYear," \
		"RTU_ADDR, RTU_FACT, ASSERTNO, RTU_TYPE, RTU_MODEL FROM RTU_ARCH LEFT JOIN BATCH_INFO ON RTU_ARCH.BatchNo=BATCH_INFO.BatchNo WHERE ";	//MODIFY BY WM 20131126 增加“终端类型”
	CString strCondition = "";	
	CString strPart = "";
	if (m_strBatchNo != "")
	{
		strPart.Format(" %s BATCH_INFO.BatchNo=%s", (strCondition.GetLength() > 0 ? "AND" : ""), m_strBatchNo);	
		strCondition.Append(strPart);
	}
	if (m_strInspectUnit != "")
	{
		strPart.Format(" %s InspectUnit='%s'", (strCondition.GetLength() > 0 ? "AND" : ""), m_strInspectUnit);	
		strCondition.Append(strPart);
	}
	if (m_strSupplyYear != "")
	{
		strPart.Format(" %s SupplyYear='%s'", (strCondition.GetLength() > 0 ? "AND" : ""), m_strSupplyYear);	
		strCondition.Append(strPart);
	}
	if (m_strCheckType != "")
	{
		strPart.Format(" %s CheckType='%s'", (strCondition.GetLength() > 0 ? "AND" : ""), m_strCheckType);	
		strCondition.Append(strPart);
	}
	if (m_bInspectDT)
	{
		strPart.Format(" %s (InspectDataTime >= #%s# AND InspectDataTime <= #%s#) ", 
			(strCondition.GetLength() > 0 ? "AND" : ""), m_dtInspectStart.Format("%Y-%m-%d"), m_dtInspectEnd.Format("%Y-%m-%d"));
		strCondition.Append(strPart);
	}
	if (m_strTermAddr != "")
	{
		strPart.Format(" %s RTU_ADDR='%s'", (strCondition.GetLength() > 0 ? "AND" : ""), m_strTermAddr);	
		strCondition.Append(strPart);
	}
	if (m_strFactory != "")
	{
		strPart.Format(" %s RTU_FACT='%s'", (strCondition.GetLength() > 0 ? "AND" : ""), m_strFactory);	
		strCondition.Append(strPart);
	}
	if (m_strOutNo != "")
	{
		strPart.Format(" %s ASSERTNO='%s'", (strCondition.GetLength() > 0 ? "AND" : ""), m_strOutNo);	
		strCondition.Append(strPart);
	}
	if (m_nTermType >= 0)
	{
		strPart.Format(" %s RTU_TYPE=%d", (strCondition.GetLength() > 0 ? "AND" : ""), m_nTermType);
		strCondition.Append(strPart);
	}
	if (m_nResult >= 0)
	{
		strPart.Format(" %s RTU_ID IN (SELECT TERM_ID FROM AUTOTEST_TOTAL_RESULT WHERE RESULT=%s)", 
			(strCondition.GetLength() > 0 ? "AND" : ""), (m_nResult > 0) ? "no" : "yes");
		strCondition.Append(strPart);
	}
	strSQL.append(strCondition.GetBuffer(0));
	strSQL.append(" ORDER BY RTU_ID");
	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL.c_str(), record))
		return;	
	int nRow = 0;
	while (!record.IsEndEOF())
	{
		_variant_t val;
		SYSTEMTIME sys;
		long lTermNo = 0;
		long lBatchNo = 0;
		int nTermType = 0;
		CString strTermNo = "";
		CString strBatchNo = "";
		CString strInspectUnit = "";
		CString strCheckType = "";
		CString strInspectDT = "";
		CString strSupplyYear = "";
		CString strTermAddr = "";
		CString strFactory = "";
		CString strOutNo = "";
		CString strTermType = "";


		record.GetFieldValue("RTU_ID", lTermNo);
		strTermNo.Format("%d", lTermNo);

		record.GetFieldValue("BatchNo", lBatchNo);
		strBatchNo.Format("%d", lBatchNo);

		record.GetFieldValue("InspectUnit", val);
		strInspectUnit = val.bstrVal;

		record.GetFieldValue("CheckType", val);
		strCheckType = val.bstrVal;

		record.GetFieldValue("InspectDataTime", sys);
		strInspectDT = CTime(sys).Format("%Y-%m-%d");

		record.GetFieldValue("SupplyYear", val);
		strSupplyYear = val.bstrVal;

		record.GetFieldValue("RTU_ADDR", val);
		strTermAddr = val.bstrVal;

		record.GetFieldValue("RTU_FACT", val);
		strFactory = val.bstrVal;

		record.GetFieldValue("ASSERTNO", val);
		strOutNo = val.bstrVal;

		record.GetFieldValue("RTU_TYPE", nTermType);
		m_cbTermType.GetLBText(nTermType, strTermType);

		CString strRow;
		strRow.Format("%d", nRow + 1);

		m_ctrlTermList.InsertItem(nRow, strRow);
		m_ctrlTermList.SetItemText(nRow, 1, strBatchNo);
		m_ctrlTermList.SetItemText(nRow, 2, strInspectUnit);
		m_ctrlTermList.SetItemText(nRow, 3, strCheckType);
		m_ctrlTermList.SetItemText(nRow, 4, strInspectDT);
		m_ctrlTermList.SetItemText(nRow, 5, strSupplyYear);
		m_ctrlTermList.SetItemText(nRow, 6, strTermNo);
		m_ctrlTermList.SetItemText(nRow, 7, strTermAddr);
		m_ctrlTermList.SetItemText(nRow, 8, strFactory);
		m_ctrlTermList.SetItemText(nRow, 9, strOutNo);
		m_ctrlTermList.SetItemText(nRow, 10, strTermType);
		m_ctrlTermList.SetItemData(nRow, lTermNo);
		nRow++;

		record.RecordMoveNext();
	}

	CString strMsg;
	strMsg.Format("共检索到%d个终端", nRow);
	MessageBox(strMsg, 0, MB_OK|MB_ICONINFORMATION);
}

void CDlgDataBrowse::OnNMDblclkListDmTerm(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int nSel = m_ctrlTermList.GetSelectionMark();
	if (nSel < 0)
		return;

	CString strBatchNo, strTerminalNo;
	strBatchNo = m_ctrlTermList.GetItemText(nSel, 1);
	strTerminalNo = m_ctrlTermList.GetItemText(nSel, 6);
	InitTermData(strBatchNo, strTerminalNo);

	m_nTermId = atoi(strTerminalNo);
	m_nBatchId = atoi(strBatchNo);
	
	*pResult = 0;
}

void CDlgDataBrowse::InitFactory()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;
	const char * strSQL = "SELECT * FROM RTU_FACT_LIST";
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;
	if (record.IsEndEOF())
		return;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		CString strFactory = "";

		record.GetFieldValue("RTU_FACT", val);
		strFactory = val.bstrVal;
		m_cbFactory.AddString(strFactory);	

		record.RecordMoveNext();
	}
}

void CDlgDataBrowse::InitTermData(CString strBatchNo, CString strTerminalNo)
{
	/*
	static CString strNowBatchNo = "";
	static CString strNowTermNo = "";
	if (strNowBatchNo.CompareNoCase(strBatchNo) == 0 && strNowTermNo.CompareNoCase(strTerminalNo) == 0)
	{
		return;
	}
	else
	{
		strNowBatchNo = strBatchNo;
		strNowTermNo = strTerminalNo;
	}
	*/

	m_ShowDataBrowseInfoListCtrl.DeleteAllItems();

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;
	char strSQL[1024];
	//sprintf(strSQL, "SELECT * FROM RD_AUTOTEST_RT WHERE BANTCH_ID=%s AND SID=%s ORDER BY MP_ID ASC,TASK_NAME ASC,PARAM_ID ASC", strBatchNo, strTerminalNo);	
	sprintf(strSQL, "SELECT RD_AUTOTEST_RT.* FROM \
					((RD_AUTOTEST_RT INNER JOIN READING_DESC ON RD_AUTOTEST_RT.TASK_NAME = READING_DESC.READING_ID_DESC) \
					INNER JOIN ALL_PARAMETER_INFO ON RD_AUTOTEST_RT.PARAM_ID = ALL_PARAMETER_INFO.PARAM_NAME) \
					WHERE RD_AUTOTEST_RT.BANTCH_ID=%s AND RD_AUTOTEST_RT.SID=%s \
					ORDER BY RD_AUTOTEST_RT.MP_ID ASC, \
					READING_DESC.READING_ID ASC,ALL_PARAMETER_INFO.PARAM_ID ASC",
					strBatchNo, 
					strTerminalNo);//MODIFY BY WM 20140102 数据排列默认通过测量点、任务名称、数据项类型三级条件排列
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;
	int nRow = 0;
	while (!record.IsEndEOF())
	{
		_variant_t val;
		long lMeterIndex = 0;
		CString strMeterIndex = "";
		CString strChannelName = "";
		CString strTaskName = "";
		CString strVisualVal = "";
		CString strRealVal = "";
		CString strParamType = "";
		CString strParamId = "";
		CString strResult = "";
		CString strTime = "";

		record.GetFieldValue("MP_ID", lMeterIndex);
		strMeterIndex.Format("%d", lMeterIndex);
		record.GetFieldValue("CHANNEL_NAME", val);
		strChannelName = val.bstrVal;
		record.GetFieldValue("TASK_NAME", val);
		strTaskName = val.bstrVal;
		record.GetFieldValue("VIRTUAL_VALUE", val);
		strVisualVal = val.bstrVal;
		record.GetFieldValue("SVALUE", val);
		strRealVal = val.bstrVal;
		record.GetFieldValue("PARAM_TYPE", val);
		strParamType = val.bstrVal;
		record.GetFieldValue("PARAM_ID", val);
		strParamId = val.bstrVal;
		record.GetFieldValue("SRESULT", val);
		if (val.boolVal)
			strResult = "合格";
		else
			strResult = "不合格";
		record.GetFieldValue("STIME", val);
		strTime = val.bstrVal;

		m_ShowDataBrowseInfoListCtrl.InsertItem(nRow, strMeterIndex);
		m_ShowDataBrowseInfoListCtrl.SetItemText(nRow, 1, strChannelName);
		m_ShowDataBrowseInfoListCtrl.SetItemText(nRow, 2, strTaskName);
		m_ShowDataBrowseInfoListCtrl.SetItemText(nRow, 3, strParamType);
		m_ShowDataBrowseInfoListCtrl.SetItemText(nRow, 4, strParamId);
		m_ShowDataBrowseInfoListCtrl.SetItemText(nRow, 5, strRealVal);
		m_ShowDataBrowseInfoListCtrl.SetItemText(nRow, 6, strVisualVal);		
		m_ShowDataBrowseInfoListCtrl.SetItemText(nRow, 7, strResult);
				
		nRow++;		

		record.RecordMoveNext();
	}
}

void CDlgDataBrowse::OnBnClickedBtnDbReport()
{
	CReportDlg dlg(m_nTermId, m_nBatchId, GetDesktopWindow());
	dlg.DoModal();
}

void CDlgDataBrowse::OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	*pResult = CDRF_DODEFAULT;

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
    else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{
		if (7 == pLVCD->iSubItem)
		{
			COLORREF clrNewTextColor, clrNewBkColor;
			int nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );
			CString strTemp = m_ShowDataBrowseInfoListCtrl.GetItemText(nItem, 7);
			if(strTemp == "不合格")
			{
				clrNewTextColor = RGB(255,0,0);
				clrNewBkColor = RGB(255,255,255);
			}
			else
			{
				clrNewTextColor = RGB(0,0,0);
				clrNewBkColor = RGB(255,255,255);
			}
			pLVCD->clrText = clrNewTextColor;
			pLVCD->clrTextBk = clrNewBkColor;
		}	
        *pResult = CDRF_DODEFAULT;
	}
}

