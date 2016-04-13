// DlgBatchInfo.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgBatchInfo.h"

#include "MainFrm.h"
#include "WFTTNNDoc.h"
#include "WFTTNNView.h"
#include ".\dlgbatchinfo.h"
#include "ItemData.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBatchInfo dialog


CDlgBatchInfo::CDlgBatchInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBatchInfo::IDD, pParent)
	, m_layout(this)
	, m_dtCheck(time(NULL))
	, m_strCheckType(_T(""))
	, m_strSupply(_T(""))
{
	//{{AFX_DATA_INIT(CDlgBatchInfo)
	m_strBatchNo = _T("");
	m_strInspectUnit = _T("");
	//}}AFX_DATA_INIT
}


void CDlgBatchInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBatchInfo)
	DDX_Control(pDX, IDC_LIST_BATCHINFO, m_ShowBatchInfoListCtrl);
	DDX_Text(pDX, IDC_EDIT_BATCHNO, m_strBatchNo);
	DDX_Text(pDX, IDC_EDIT_INSPECTUNIT, m_strInspectUnit);	
	//}}AFX_DATA_MAP
	DDX_DateTimeCtrl(pDX, IDC_DATE_CHECK, m_dtCheck);
	DDX_CBString(pDX, IDC_CMB_CHECKTYPE, m_strCheckType);
	DDX_Control(pDX, IDC_CMB_BC_SUPPLY, m_cbSupply);
	DDX_CBString(pDX, IDC_CMB_BC_SUPPLY, m_strSupply);
}


BEGIN_MESSAGE_MAP(CDlgBatchInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgBatchInfo)
	ON_BN_CLICKED(IDC_BUTTON_ADDBATCHINFO, OnButtonAddbatchinfo)
	ON_BN_CLICKED(IDC_BUTTON_DELBATCHINFO, OnBnClickedButtonDelbatchinfo)
	ON_BN_CLICKED(IDC_BUTTON_MODIFYBATCHINFO, OnBnClickedButtonModifybatchinfo)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BATCHINFO, OnLvnItemchangedListBatchinfo)
	//}}AFX_MSG_MAP	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBatchInfo message handlers

BOOL CDlgBatchInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD dwExtendStyle = m_ShowBatchInfoListCtrl.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ShowBatchInfoListCtrl.SetExtendedStyle(dwExtendStyle);		
	/*
	m_ShowBatchInfoListCtrl.InsertColumn(0,"批次号",LVCFMT_CENTER,100);
	m_ShowBatchInfoListCtrl.InsertColumn(1,"送检单位",LVCFMT_CENTER,120);
	m_ShowBatchInfoListCtrl.InsertColumn(2,"检测类别",LVCFMT_CENTER,100);
	m_ShowBatchInfoListCtrl.InsertColumn(3,"送检日期",LVCFMT_CENTER,160);
	m_ShowBatchInfoListCtrl.InsertColumn(4,"供货年度",LVCFMT_CENTER,120);
	*/
	m_ShowBatchInfoListCtrl.SetColumns("批次号,100;送检单位,120;检测类别,100;送检日期,160;供货年度,120");

	InitBatchInfoList();

	for (int i = 2012; i <= 2033; i++)
	{
		CString temp;
		temp.Format("%d", i);
		m_cbSupply.AddString(temp);
	}

	CRect rect;
	GetClientRect(&rect);
	m_layout.SetOldRect(rect);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBatchInfo::InitBatchInfoList()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	char strSQL[300] = {0};
	int nRow = 0;	
	sprintf(strSQL,"SELECT * FROM BATCH_INFO ORDER BY InspectDataTime DESC");	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;
	
	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nBatchNo;
		CString strBatchNo = _T("");
		CString strInspectUnit = _T("");
		CString strCheckType = _T("");
		CString strRtuAddr = _T("");
		SYSTEMTIME sys;
		CString strInspectTime = _T("");
		CString strSupplyYear = _T("");
				
		record.GetFieldValue("BatchNo",nBatchNo);
		strBatchNo.Format("%d", nBatchNo);
		
		record.GetFieldValue("InspectUnit",val);
		strInspectUnit = val.bstrVal;
		
		record.GetFieldValue("CheckType",val);
		strCheckType = val.bstrVal;

		record.GetFieldValue("InspectDataTime",sys);
		strInspectTime = CTime(sys).Format("%Y-%m-%d");
		
		record.GetFieldValue("SupplyYear",val);
		strSupplyYear = val.bstrVal;
		
		/*
		m_ShowBatchInfoListCtrl.InsertItem(0, strBatchNo);
		m_ShowBatchInfoListCtrl.SetItemText(0, 1, strInspectUnit);
		m_ShowBatchInfoListCtrl.SetItemText(0, 2, strCheckType);
		m_ShowBatchInfoListCtrl.SetItemText(0, 3, strInspectTime);
		m_ShowBatchInfoListCtrl.SetItemText(0, 4, strSupplyYear);
		*/
		m_ShowBatchInfoListCtrl.AddItem(0, strBatchNo, strInspectUnit, strCheckType, strInspectTime, strSupplyYear);
		
		record.RecordMoveNext();
		nRow++;
	}

	m_ShowBatchInfoListCtrl.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	m_ShowBatchInfoListCtrl.SetSelectionMark(0);
}

void CDlgBatchInfo::OnButtonAddbatchinfo() 
{	
	UpdateData(TRUE);

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	CString strInspect = m_dtCheck.Format("%Y-%m-%d");
	char strSQL[300] = {0};
	sprintf(strSQL,"INSERT INTO BATCH_INFO(BatchNo,InspectUnit,CheckType,InspectDataTime,SupplyYear,InputTime) \
				   VALUES(%d, '%s','%s', '%s', '%s', %ld)",
				   atoi(m_strBatchNo), m_strInspectUnit, m_strCheckType, strInspect, m_strSupply, time(NULL));
	if (pConnect->ExecSQL(strSQL)) {
		/*
		int index = m_ShowBatchInfoListCtrl.GetItemCount();
		m_ShowBatchInfoListCtrl.InsertItem(index,m_strBatchNo);
		m_ShowBatchInfoListCtrl.SetItemText(index,1,m_strInspectUnit);
		m_ShowBatchInfoListCtrl.SetItemText(index,2,m_strCheckType);
		m_ShowBatchInfoListCtrl.SetItemText(index,3,strInspect);
		m_ShowBatchInfoListCtrl.SetItemText(index,4,strSupply);
		*/
		m_ShowBatchInfoListCtrl.AddItem(0, m_strBatchNo, m_strInspectUnit, m_strCheckType, strInspect, m_strSupply);
		
		CMainFrame* pMainWnd=(CMainFrame*)AfxGetApp()->m_pMainWnd;
		pMainWnd->m_TerminalParentDlgBar.RefreshBatchInfoTree(m_strBatchNo);
		pMainWnd->RefreshBatchArch();

		ShowOperateLog("操作成功");
	}
	else
	{
		MessageBox(MSG_DUPLICATE, "提示", MB_OK|MB_ICONWARNING);
	}
}

void CDlgBatchInfo::OnEvent(unsigned int nEventID, void *pParam)
{	
	if (Event_Type_Item_Double_Click != nEventID || !::IsWindowVisible(this->GetSafeHwnd()))
		return;

	ItemData *data = (ItemData *)pParam;
	if (ITEM_TYPE_BATCH  == data->m_type)
	{
		int batchNo = data->m_id;
		CString strBatchNo = "";
		strBatchNo.Format("%d", batchNo);
		
		for (int i = 0; i < m_ShowBatchInfoListCtrl.GetItemCount(); i++)
		{
			CString temp = m_ShowBatchInfoListCtrl.GetItemText(i, 0);
			if (temp == strBatchNo) 
			{
				m_ShowBatchInfoListCtrl.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED | LVIS_DROPHILITED);				
				m_ShowBatchInfoListCtrl.SetSelectionMark(i);
				m_ShowBatchInfoListCtrl.EnsureVisible(i, TRUE);
				break;
			}
		}
	}
}

void CDlgBatchInfo::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();		
	}
}

void CDlgBatchInfo::OnLvnItemchangedListBatchinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);	
	if(pNMLV->uChanged == LVIF_STATE)
	{
		if(pNMLV->uNewState & LVIS_SELECTED)
		{
			int nItem=pNMLV->iItem;

			CString strInspect, strSupply;
			m_strBatchNo = m_ShowBatchInfoListCtrl.GetItemText(nItem, 0);
			m_strInspectUnit = m_ShowBatchInfoListCtrl.GetItemText(nItem, 1);
			m_strCheckType = m_ShowBatchInfoListCtrl.GetItemText(nItem, 2);
			strInspect = m_ShowBatchInfoListCtrl.GetItemText(nItem, 3);
			m_strSupply = m_ShowBatchInfoListCtrl.GetItemText(nItem, 4);		

			UpdateData(FALSE);
		}
	}

	*pResult = 0;
}

void CDlgBatchInfo::OnBnClickedButtonDelbatchinfo()
{	
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;
	
	UpdateData(TRUE);

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};
	sprintf(strSQL,"DELETE FROM BATCH_INFO WHERE BatchNo=%s", m_strBatchNo);
	if (pConnect->ExecSQL(strSQL)) {

		CMainFrame* pMainWnd=(CMainFrame*)AfxGetApp()->m_pMainWnd;// (CMainFrame*)AfxGetMainWnd();		
		pMainWnd->m_TerminalParentDlgBar.RefreshBatchInfoTree(m_strBatchNo, 1);
		pMainWnd->RefreshBatchArch();

		int nIndex = m_ShowBatchInfoListCtrl.GetSelectionMark();		
		m_ShowBatchInfoListCtrl.DeleteItem(nIndex);

		ShowOperateLog("操作成功");
	}
}

void CDlgBatchInfo::OnBnClickedButtonModifybatchinfo()
{
	if (MessageBox("确认操作？", "提示", MB_YESNO) != IDYES)
		return;

	UpdateData(TRUE);

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	CString strInspect = m_dtCheck.Format("%Y-%m-%d");
	char strSQL[300] = {0};
	sprintf(strSQL,"UPDATE BATCH_INFO SET InspectUnit='%s',CheckType='%s',InspectDataTime='%s',SupplyYear='%s' WHERE BatchNo=%s",
		m_strInspectUnit, m_strCheckType, strInspect, m_strSupply, m_strBatchNo);
	if (pConnect->ExecSQL(strSQL)) {
		
		int nIndex = m_ShowBatchInfoListCtrl.GetSelectionMark();		
		m_ShowBatchInfoListCtrl.SetItemText(nIndex, 1, m_strInspectUnit);
		m_ShowBatchInfoListCtrl.SetItemText(nIndex, 2, m_strCheckType);
		m_ShowBatchInfoListCtrl.SetItemText(nIndex, 3, strInspect);
		m_ShowBatchInfoListCtrl.SetItemText(nIndex, 4, m_strSupply);

		ShowOperateLog("操作成功");
	}
}

void CDlgBatchInfo::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (!m_ShowBatchInfoListCtrl.GetSafeHwnd())
		return;
	m_layout.SetPos(IDC_LIST_BATCHINFO, LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	//m_layout.SetPos(IDC_GROUP, LT_LEFT_TOP, RB_RIGHT);
	//	m_layout.SetPos(IDC_EDIT_CHECKTYPE, LT_LEFT_TOP, RB_RIGHT);

	//	m_layout.SetPos(IDC_BUTTON_ADDBATCHINFO, LT_TOP, RB_RIGHT);
	//	m_layout.SetPos(IDC_BUTTON_DELBATCHINFO, LT_TOP, RB_RIGHT);
	//	m_layout.SetPos(IDC_BUTTON_MODIFYBATCHINFO, LT_TOP, RB_RIGHT);

	m_layout.SetOldRect(CRect(0, 0, cx, cy));
}

