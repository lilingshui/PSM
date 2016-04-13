// DlgTerminalInfo.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgTerminalInfo.h"

#include "MainFrm.h"
#include ".\dlgterminalinfo.h"
//#include "WFTTNNView.h"
#include "ItemData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTerminalInfo dialog


CDlgTerminalInfo::CDlgTerminalInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTerminalInfo::IDD, pParent) //,m_lstnSocket(6001)
	, m_layoutCtrl(this)
	, m_strTerminalFactory(_T(""))
	, m_nTerminalNo(0)
	, m_strCJBH(_T(""))
{
	//{{AFX_DATA_INIT(CDlgTerminalInfo)
	m_strTerminalNo = _T("");
	m_strTerminalFactory = _T("");
	m_strTerminalAddr = _T("");
	m_strAssertNo = _T("");
	m_strTerminalModel = _T("");//MODIFY BY WM 20131126
	//}}AFX_DATA_INIT
}


void CDlgTerminalInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTerminalInfo)
	DDX_Control(pDX, IDC_COMBO_TERMINALTYPE, m_cbTerminalType);
	DDX_Control(pDX, IDC_COMBO_BATCH_INFO, m_cbBatchInfo);
	DDX_Control(pDX, IDC_LIST_TERMINALINFO, m_ShowTerminalInfoListCtrl);	
	DDX_Text(pDX, IDC_EDIT_TERMINALNO, m_strTerminalNo);	
	DDX_Text(pDX, IDC_EDIT_TERMINALADDR, m_strTerminalAddr);
	DDX_Text(pDX, IDC_EDIT_ASSERTNO, m_strAssertNo);
	DDX_Text(pDX, IDC_EDIT_TERMINALMODEL, m_strTerminalModel);//MODIFY BY WM 20131126
	DDX_CBString(pDX, IDC_CMB_FACTORY, m_strTerminalFactory);
	//}}AFX_DATA_MAP	
	DDX_Control(pDX, IDC_CMB_FACTORY, m_cbFactory);
	DDX_Text(pDX, IDC_EDIT_CJBH, m_strCJBH);
}


BEGIN_MESSAGE_MAP(CDlgTerminalInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgTerminalInfo)
	ON_BN_CLICKED(IDC_BUTTON_ADDTERMIANL, OnButtonAddtermianl)
	ON_BN_CLICKED(IDC_BUTTON_DELTERMINAL, OnButtonDelterminal)
	ON_BN_CLICKED(IDC_BUTTON_MODIFYTERMINAL, OnButtonModifyterminal)
//	ON_NOTIFY(NM_CLICK, IDC_LIST_TERMINALINFO, OnClickListTerminalinfo)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_BATCH_INFO, OnCbnSelchangeComboBatchInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TERMINALINFO, OnLvnItemchangedListTerminalinfo)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTerminalInfo message handlers

BOOL CDlgTerminalInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	DWORD dwExtendStyle = m_ShowTerminalInfoListCtrl.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ShowTerminalInfoListCtrl.SetExtendedStyle(dwExtendStyle);

	/**************初始化列表显示**************************************/
	m_ShowTerminalInfoListCtrl.InsertColumn(0,"终端编号",LVCFMT_CENTER,100);	
	m_ShowTerminalInfoListCtrl.InsertColumn(1,"终端地址",LVCFMT_CENTER,100);
	m_ShowTerminalInfoListCtrl.InsertColumn(2,"生产厂家",LVCFMT_CENTER,120);	
	m_ShowTerminalInfoListCtrl.InsertColumn(3,"资产编号",LVCFMT_CENTER,120);
	m_ShowTerminalInfoListCtrl.InsertColumn(4,"终端类型",LVCFMT_CENTER,120);
	m_ShowTerminalInfoListCtrl.InsertColumn(5,"终端型号",LVCFMT_CENTER,120);//MODIFY BY WM 20131126
	m_ShowTerminalInfoListCtrl.InsertColumn(6,"出厂编号",LVCFMT_CENTER,120);

	m_cbTerminalType.InsertString(0, "机架式");
	m_cbTerminalType.InsertString(1, "壁挂式");

	InitBatchInfoList();
	InitTerminalInfoList(m_strBatchNo);
	InitFactory();
	m_nTerminalNo = GetNewTerminalID();
	
	CRect rect;
	GetClientRect(&rect);
	m_layoutCtrl.SetOldRect(rect);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTerminalInfo::OnButtonAddtermianl() 
{
	UpdateData(true);

	if (m_strTerminalFactory == "")
	{
		AfxMessageBox("请输入生产厂家！");
		return;
	}

	if (m_strTerminalAddr == "")
	{
		AfxMessageBox("请输入终端地址！");
		return;
	}

	if (m_strAssertNo == "")
	{
		AfxMessageBox("请输入资产编号！");
		return;
	}

	int nCurIndex = m_cbTerminalType.GetCurSel();
	if (nCurIndex == -1)
	{
		AfxMessageBox("请输入终端类型！");
		return;
	}

	if (m_strTerminalModel == "") //MODIFY BY WM 20131126
	{
		AfxMessageBox("请输入终端型号！");
		return;
	}

	if (m_strCJBH == "")
	{
		AfxMessageBox("请输入出厂编号!");
		return;
	}

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;	
	char strSQL[300] = {0};
	sprintf(strSQL,"INSERT INTO RTU_ARCH(RTU_ID,RTU_FACT,RTU_ADDR,ASSERTNO,RTU_TYPE,BATCHNO,RTU_MODEL,RTU_CJBH) VALUES(%d,'%s','%s','%s',%d,%s,'%s','%s')",
		m_nTerminalNo, m_strTerminalFactory, m_strTerminalAddr, m_strAssertNo, m_cbTerminalType.GetCurSel(),m_strBatchNo,m_strTerminalModel,m_strCJBH); //MODIFY BY WM 20131126
	if (!pConnect->ExecSQL(strSQL))
	{
		MessageBox(MSG_DUPLICATE, "提示", MB_OK|MB_ICONWARNING);
		return;
	}

	m_strTerminalNo.Format("%d", m_nTerminalNo);
	m_nTerminalNo++;

	int index = m_ShowTerminalInfoListCtrl.GetItemCount();
	m_ShowTerminalInfoListCtrl.InsertItem(index, m_strTerminalNo);
	m_ShowTerminalInfoListCtrl.SetItemText(index,1,m_strTerminalAddr);
	m_ShowTerminalInfoListCtrl.SetItemText(index,2,m_strTerminalFactory);	
	m_ShowTerminalInfoListCtrl.SetItemText(index,3,m_strAssertNo);
	CString strTerminalType = _T("");
	m_cbTerminalType.GetLBText(nCurIndex,strTerminalType);
	m_ShowTerminalInfoListCtrl.SetItemText(index,4,strTerminalType);
	m_ShowTerminalInfoListCtrl.SetItemText(index,5,m_strTerminalModel);//MODIFY BY WM 20131126
	m_ShowTerminalInfoListCtrl.SetItemText(index,6,m_strCJBH);
	CMainFrame* pMainWnd=(CMainFrame*)AfxGetApp()->m_pMainWnd;
	pMainWnd->m_TerminalParentDlgBar.RefreshTerminalTree(m_strBatchNo,m_strTerminalNo,0);
	//pMainWnd->m_TerminalParentDlgBar.RefreshTerminalTree(m_strBatchNo,m_strTerminalNo,m_strTerminalAddr,0);//MODIFY BY WM 20131126
	pMainWnd->RefreshTermArch();

	// 如生产厂家不存在，则在数据库中新增
	if (m_cbFactory.GetCurSel() < 0)
	{
		sprintf(strSQL, "INSERT INTO RTU_FACT_LIST(RTU_FACT) VALUES('%s')", m_strTerminalFactory);
		pConnect->ExecSQL(strSQL);
		m_cbFactory.AddString(m_strTerminalFactory);		
	}

	UpdateData(FALSE);
	ShowOperateLog("操作成功");
}

void CDlgTerminalInfo::OnButtonDelterminal() 
{
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;
	
	UpdateData(TRUE);

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};
	sprintf(strSQL,"DELETE FROM RTU_ARCH WHERE RTU_ID = %s", m_strTerminalNo);
	if (pConnect->ExecSQL(strSQL)) {

		CMainFrame* pMainWnd=(CMainFrame*)AfxGetApp()->m_pMainWnd;
		pMainWnd->m_TerminalParentDlgBar.RefreshTerminalTree(m_strBatchNo, m_strTerminalNo,1);
		//pMainWnd->m_TerminalParentDlgBar.RefreshTerminalTree(m_strBatchNo, m_strTerminalNo, m_strTerminalAddr,1);//MODIFY BY WM 20131126		
		pMainWnd->RefreshTermArch();

		int nIndex = m_ShowTerminalInfoListCtrl.GetSelectionMark();		
		m_ShowTerminalInfoListCtrl.DeleteItem(nIndex);
		m_ShowTerminalInfoListCtrl.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_ShowTerminalInfoListCtrl.SetSelectionMark(0);

		ShowOperateLog("删除成功");		
	}
}

void CDlgTerminalInfo::OnButtonModifyterminal() 
{
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;

	UpdateData(TRUE);

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};
	sprintf(strSQL,"UPDATE RTU_ARCH SET RTU_ADDR='%s', RTU_FACT='%s', RTU_TYPE=%d, ASSERTNO='%s', BATCHNO=%s, RTU_MODEL='%s', RTU_CJBH='%s' WHERE RTU_ID=%s", 
		m_strTerminalAddr, m_strTerminalFactory, m_cbTerminalType.GetCurSel(), m_strAssertNo, m_strBatchNo,m_strTerminalModel, m_strCJBH, m_strTerminalNo);//MODIFY BY WM 20131126
	if (pConnect->ExecSQL(strSQL))
	{
		int nIndex = m_ShowTerminalInfoListCtrl.GetSelectionMark();
		m_ShowTerminalInfoListCtrl.SetItemText(nIndex, 1, m_strTerminalAddr);
		m_ShowTerminalInfoListCtrl.SetItemText(nIndex, 2, m_strTerminalFactory);		
		m_ShowTerminalInfoListCtrl.SetItemText(nIndex, 3, m_strAssertNo);
		m_ShowTerminalInfoListCtrl.SetItemText(nIndex, 5, m_strTerminalModel);
		m_ShowTerminalInfoListCtrl.SetItemText(nIndex, 6, m_strCJBH);
		
		CString strType;
		m_cbTerminalType.GetWindowText(strType);
		m_ShowTerminalInfoListCtrl.SetItemText(nIndex, 4, strType);
	}
}


void CDlgTerminalInfo::InitTerminalInfoList(CString strBatchNo)
{
	if (strBatchNo == "")	
		return;	

	m_ShowTerminalInfoListCtrl.DeleteAllItems();
	m_strBatchNo = strBatchNo;//保存档案树批次信息

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;
	char strSQL[300] = {0};
	int nRow = 0;
	sprintf(strSQL,"SELECT * FROM RTU_ARCH WHERE BATCHNO = %s ORDER BY RTU_ID", strBatchNo);	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nRtuID;
		CString strRtuID = _T("");
		CString strRtuFact = _T("");
		CString strRtuAddr = _T("");
		CString strAssertNo = _T("");
		CString strTerminalModel = _T("");//MODIFY BY WM 20131126
		CString strCJBH = _T("");
		int nRtuType;
		CString strRtuType = _T("");		

		record.GetFieldValue("RTU_ID", nRtuID);
		strRtuID.Format("%d", nRtuID);

		record.GetFieldValue("RTU_FACT",val);
		strRtuFact = val.bstrVal;

		record.GetFieldValue("RTU_ADDR",val);
		strRtuAddr = val.bstrVal;

		record.GetFieldValue("ASSERTNO",val);
		strAssertNo = val.bstrVal;

		record.GetFieldValue("RTU_TYPE",nRtuType);
		if (nRtuType==0)
		{
			strRtuType=_T("机架式");
		}
		else if (nRtuType==1) 
		{
			strRtuType=_T("壁挂式");
		}
		
		record.GetFieldValue("RTU_MODEL",val);//MODIFY BY WM 20131126
		strTerminalModel = val.bstrVal;
		
		char temp[512] = {0};
		record.GetFieldValue("RTU_CJBH", temp, sizeof(temp));
		strCJBH = temp;

		m_ShowTerminalInfoListCtrl.InsertItem(nRow, strRtuID);
		m_ShowTerminalInfoListCtrl.SetItemText(nRow, 1, strRtuAddr);
		m_ShowTerminalInfoListCtrl.SetItemText(nRow, 2, strRtuFact);		
		m_ShowTerminalInfoListCtrl.SetItemText(nRow, 3, strAssertNo);
		m_ShowTerminalInfoListCtrl.SetItemText(nRow, 4, strRtuType);
		m_ShowTerminalInfoListCtrl.SetItemText(nRow, 5, strTerminalModel);//MODIFY BY WM 20131126
		m_ShowTerminalInfoListCtrl.SetItemText(nRow, 6, strCJBH);
		
		record.RecordMoveNext();
		nRow++;
	}

	m_ShowTerminalInfoListCtrl.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	m_ShowTerminalInfoListCtrl.SetSelectionMark(0);
}

void CDlgTerminalInfo::SetCurrentTreeNode(HTREEITEM node)
{
	m_hCurrentTreeNode = node;
}


void CDlgTerminalInfo::OnEvent(unsigned int nEventID, void *pParam)
{
	if (Event_Type_Item_Double_Click != nEventID || !::IsWindowVisible(this->GetSafeHwnd()))
		return;	

	ItemData *data = (ItemData *)pParam;
	if (ITEM_TYPE_BATCH == data->m_type)
	{
		int batchNo = data->m_id;		
		m_strBatchNo.Format("%d", batchNo);
		m_cbBatchInfo.SelectString(0, m_strBatchNo);
		m_strTerminalNo.Format("%d", m_nTerminalNo);
		m_strTerminalAddr = "";
		m_strTerminalFactory = "";
		m_strAssertNo = "";
		m_strTerminalModel = "";//MODIFY BY WM 20131126
		m_strCJBH = "";
		m_cbTerminalType.SetCurSel(0);
		UpdateData(FALSE);

		InitTerminalInfoList(m_strBatchNo);
	}
	else if (ITEM_TYPE_TERMINAL == data->m_type)
	{
		int termNo = data->m_id;
		CString strTermNo = "";
		strTermNo.Format("%d", termNo);

		for (int i = 0; i < m_ShowTerminalInfoListCtrl.GetItemCount(); i++)
		{
			CString temp = m_ShowTerminalInfoListCtrl.GetItemText(i, 0);
			if (temp == strTermNo) 
			{
				m_ShowTerminalInfoListCtrl.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				m_ShowTerminalInfoListCtrl.SetSelectionMark(i);
				break;
			}
		}
	}
}

void CDlgTerminalInfo::InitBatchInfoList()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	const char * strSQL = "SELECT * FROM BATCH_INFO ORDER BY InspectDataTime DESC";
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;

	while (!record.IsEndEOF())
	{
		int nBatchNo;
		CString strBatchNo = "";

		record.GetFieldValue("BatchNo",nBatchNo);
		strBatchNo.Format("%d", nBatchNo);
		m_cbBatchInfo.AddString(strBatchNo);		

		record.RecordMoveNext();
	}

	m_cbBatchInfo.SetCurSel(0);
}

void CDlgTerminalInfo::OnCbnSelchangeComboBatchInfo()
{
	int nIndex = m_cbBatchInfo.GetCurSel();
	m_cbBatchInfo.GetLBText(nIndex, m_strBatchNo);
}

void CDlgTerminalInfo::OnLvnItemchangedListTerminalinfo(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);	
	if(pNMLV->uChanged == LVIF_STATE)
	{
		if(pNMLV->uNewState & LVIS_SELECTED)
		{
			int nItem=pNMLV->iItem;

			m_strTerminalNo = m_ShowTerminalInfoListCtrl.GetItemText(nItem, 0);
			m_strTerminalAddr = m_ShowTerminalInfoListCtrl.GetItemText(nItem, 1);
			m_strTerminalFactory = m_ShowTerminalInfoListCtrl.GetItemText(nItem, 2);			
			m_strAssertNo  = m_ShowTerminalInfoListCtrl.GetItemText(nItem, 3);
			m_cbTerminalType.SelectString(0, m_ShowTerminalInfoListCtrl.GetItemText(nItem, 4));
			m_strTerminalModel = m_ShowTerminalInfoListCtrl.GetItemText(nItem, 5);//ADD BY WM 20131126
			m_strCJBH = m_ShowTerminalInfoListCtrl.GetItemText(nItem, 6);

			UpdateData(FALSE);
		}
	}

	*pResult = 0;
}

long CDlgTerminalInfo::GetNewTerminalID()
{
	long id = 1;

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return id;

	const char * strSQL = "SELECT MAX(RTU_ID) AS ID FROM RTU_ARCH";
	CADORecordSet record;
	if (pConnect->ExecSQL(strSQL, record) && !record.IsEndEOF())
	{
		record.GetFieldValue("ID", id);
		id++;
	}

	return id;
}

void CDlgTerminalInfo::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (!GetDlgItem(IDC_LIST_TERMINALINFO)->GetSafeHwnd())
		return;
	m_layoutCtrl.SetPos(IDC_LIST_TERMINALINFO, 	LT_LEFT_TOP, RB_RIGHT_BOTTOM);
	m_layoutCtrl.SetOldRect(CRect(0, 0, cx, cy));
}

void CDlgTerminalInfo::InitFactory()
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
	m_cbFactory.SetCurSel(0);	
}