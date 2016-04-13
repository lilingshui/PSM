// DlgTerminalParent.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgTerminalParent.h"
#include "DBAccess.h"
#include "ADODataBase.h"
#include "DlgBatchInfo.h"

#include "MainFrm.h"
#include "WFTTNNDoc.h"
#include "WFTTNNView.h"
#include ".\dlgterminalparent.h"
#include "ItemData.h"
#include "ptcl_codec/ptcl_module_csg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTerminalParent dialog


CDlgTerminalParent::CDlgTerminalParent()
	: CDialogBar()
{
	//{{AFX_DATA_INIT(CDlgTerminalParent)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgTerminalParent::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTerminalParent)
	DDX_Control(pDX, IDC_TREE_TERMINAL, m_TerminalTreeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTerminalParent, CDialogBar)
	//{{AFX_MSG_MAP(CDlgTerminalParent)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_TERMINAL, OnDblclkTreeTerminal)
	ON_NOTIFY(NM_CLICK, IDC_TREE_TERMINAL, OnClickTreeTerminal)
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TERMINAL, OnTvnSelchangedTreeTerminal)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTerminalParent message handlers

BOOL CDlgTerminalParent::OnInitDialog() 
{	
	// TODO: Add extra initialization here

	m_ImageList.Create(24, 24, ILC_COLOR16|ILC_MASK, 2, 2);
	m_ImageList.Add(AfxGetApp()->LoadIconA(IDI_ICON_TERM));
	m_ImageList.Add(AfxGetApp()->LoadIconA(IDI_ICON_FILE));
	
	m_TerminalTreeCtrl.SubclassDlgItem(IDC_TREE_TERMINAL, this);	
	m_TerminalTreeCtrl.SetImageList(&m_ImageList,TVSIL_NORMAL);
		

	m_hItem = m_TerminalTreeCtrl.InsertItem("厂站终端",0,0,TVI_ROOT,TVI_LAST);
	//
	ItemData *data = new ItemData(ITEM_TYPE_GONGBIAN, 2013);
	m_TerminalTreeCtrl.SetItemData(m_hItem, (DWORD)data);


	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return FALSE;

	char strSQL[300] = {0};
	int nRow = 0;
	HTREEITEM m_child,m_childex;
	
	sprintf(strSQL,"SELECT TOP 10 * FROM BATCH_INFO ORDER BY InspectDataTime DESC");
	
	CADORecordSet record,recordex;
	if (!pConnect->ExecSQL(strSQL,record))
		return FALSE;
	
	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nBatchNo;
		CString strBatchNo = _T("");
		CString strInspectUnit = _T("");
		
		CString strCheckType = _T("");
		CString strRtuAddr = _T("");
		CString strSupplyYear = _T("");
		SYSTEMTIME sys;
		CString strInspectTime = _T("");
				
		record.GetFieldValue("BatchNo",nBatchNo);
		strBatchNo.Format("%d", nBatchNo);
		
		record.GetFieldValue("InspectUnit",val);
		strInspectUnit = val.bstrVal;
		
		record.GetFieldValue("CheckType",val);
		strCheckType = val.bstrVal;
		
		record.GetFieldValue("InspectDataTime",sys);
		strInspectTime.Format("%04d-%02d-%02d %02d:%02d:%02d", 
			sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
		
		record.GetFieldValue("SupplyYear",val);
		strSupplyYear = val.bstrVal;
		
		record.RecordMoveNext();
		nRow++;
		
		CString strNodeName = _T("");
		strNodeName.Format("%s", strBatchNo);
		m_child = m_TerminalTreeCtrl.InsertItem(strNodeName,1,1,m_hItem,TVI_FIRST);
		//
		ItemData *data = new ItemData(ITEM_TYPE_BATCH, nBatchNo);
		m_TerminalTreeCtrl.SetItemData(m_child, (DWORD)data);

		sprintf(strSQL,"SELECT * FROM RTU_ARCH WHERE BatchNo = %s ORDER BY RTU_ID", strBatchNo);
		if (!pConnect->ExecSQL(strSQL,recordex))
			return FALSE;
		
		while (!recordex.IsEndEOF())
		{
			_variant_t val;
			int nRtuID;
			CString strRtuID = _T("");
			CString strRtuName = _T("");
			CString strRtuAddr = _T("");
			
			recordex.GetFieldValue("RTU_ID",nRtuID);
			strRtuID.Format("%d", nRtuID);

			recordex.GetFieldValue("RTU_NAME",val);
			strRtuName = val.bstrVal;

			recordex.GetFieldValue("RTU_ADDR",val);
			strRtuAddr = val.bstrVal;
			
			recordex.RecordMoveNext();
			nRow++;
			
			CString strNodeName = _T("");
			strNodeName.Format("%s", strRtuID);
			m_childex = m_TerminalTreeCtrl.InsertItem(strNodeName,0,0,m_child,TVI_LAST);
			//ItemData *data = new TerminalItemData(ITEM_TYPE_TERMINAL, nRtuID, strRtuAddr,strRtuAddr);//MODYFY BY WM 20131126
			ItemData *data = new TerminalItemData(ITEM_TYPE_TERMINAL, nRtuID, strRtuAddr);
			m_TerminalTreeCtrl.SetItemData(m_childex, (DWORD)data);
		}

		recordex.Close();
	}

	m_TerminalTreeCtrl.Expand(m_hItem, TVE_EXPAND);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgTerminalParent::Create(CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID) 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bRet = CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID );
	if(bRet)
	{
		//add your own code here!
		OnInitDialog();
	}
	return bRet;
}

void CDlgTerminalParent::OnDestroy() 
{
	CDialogBar::OnDestroy();
	
	// TODO: Add your message handler code here
//	m_TerminalTreeCtrl.Detach();
	
}

void CDlgTerminalParent::RefreshTerminalTree(CString strBatchNo, CString strTermNo, int nType /* = 0 */)
{
	HTREEITEM batchItem = NULL;
	HTREEITEM termItem = NULL;
	ItemData *data = NULL;
    
	if (nType)
	{
		if (m_hItem && m_TerminalTreeCtrl.ItemHasChildren(m_hItem))
		{
			batchItem = m_TerminalTreeCtrl.GetChildItem(m_hItem);
			while (batchItem)
			{
				data = (ItemData *)m_TerminalTreeCtrl.GetItemData(batchItem);
				if (data->m_id == atoi(strBatchNo))
					break;
				data = NULL;
				batchItem = m_TerminalTreeCtrl.GetNextItem(batchItem, TVGN_NEXT);
			}
		}

		if (batchItem && m_TerminalTreeCtrl.ItemHasChildren(batchItem))
		{
			termItem = m_TerminalTreeCtrl.GetChildItem(batchItem);
			while (termItem)
			{
				data = (TerminalItemData *)m_TerminalTreeCtrl.GetItemData(termItem);
				if (data->m_id == atoi(strTermNo))
					break;
				data = NULL;
				termItem = m_TerminalTreeCtrl.GetNextItem(termItem, TVGN_NEXT);
			}
		}

		if (termItem)
		{
			delete data;
			m_TerminalTreeCtrl.DeleteItem(termItem);
		}
	}
	else
	{
		if (m_hItem && m_TerminalTreeCtrl.ItemHasChildren(m_hItem))
		{
			batchItem = m_TerminalTreeCtrl.GetChildItem(m_hItem);
			while (batchItem)
			{
				data = (ItemData *)m_TerminalTreeCtrl.GetItemData(batchItem);
				if (data->m_id == atoi(strBatchNo))				
					break;
				batchItem = m_TerminalTreeCtrl.GetNextItem(batchItem, TVGN_NEXT);
			}
		}

		if (batchItem)
		{
			HTREEITEM item = m_TerminalTreeCtrl.InsertItem(strTermNo, 0, 0, batchItem, TVI_LAST);
			ItemData *data = new TerminalItemData(ITEM_TYPE_TERMINAL, atoi(strTermNo), strTermNo);
			//ItemData *data = new TerminalItemData(ITEM_TYPE_TERMINAL, atoi(strTermNo), strTermNo,strTermAddr);//MODYFY BY WM 20131126
			m_TerminalTreeCtrl.SetItemData(item,(DWORD)data);
		}		
	}

	m_TerminalTreeCtrl.UpdateWindow();
}

void CDlgTerminalParent::RefreshBatchInfoTree(CString strNodeNo, int nType /* = 0 */)
{
	HTREEITEM optItem = m_hItem;
	if (nType) 
	{
		if (m_TerminalTreeCtrl.ItemHasChildren(optItem)) 
		{
			HTREEITEM item = NULL;
			ItemData *data = NULL;

			item = m_TerminalTreeCtrl.GetChildItem(optItem);
			while (item)
			{
				data = (ItemData *)m_TerminalTreeCtrl.GetItemData(item);
				if (data->m_id == atoi(strNodeNo)) 
					break;				
				data = NULL;
				item = m_TerminalTreeCtrl.GetNextItem(item, TVGN_NEXT);
			}

			if (item) 
			{
				delete data;
				m_TerminalTreeCtrl.DeleteItem(item);
			}
		}
	}
	else
	{
		HTREEITEM item = m_TerminalTreeCtrl.InsertItem(strNodeNo, 1, 1, m_hItem, TVI_LAST);	
		ItemData *data = new ItemData(ITEM_TYPE_BATCH, atoi(strNodeNo));
		m_TerminalTreeCtrl.SetItemData(item,(DWORD)data);
	}

	m_TerminalTreeCtrl.UpdateWindow();
}

int CDlgTerminalParent::GetTreeNodeValue()
{
	//m_TerminalTreeCtrl.GetRootItem();
	HTREEITEM node = m_TerminalTreeCtrl.GetSelectedItem();  //取得当前选择的节点
	int nNodeValue = (DWORD)m_TerminalTreeCtrl.GetItemData(node);

	return nNodeValue;
}

int CDlgTerminalParent::GetTreeRootNodeValue()
{
	HTREEITEM node = m_TerminalTreeCtrl.GetRootItem();
	HTREEITEM subnode = m_TerminalTreeCtrl.GetNextItem(node, TVGN_CHILD);
	HTREEITEM onesubnode = m_TerminalTreeCtrl.GetNextItem(subnode, TVGN_CHILD);
	int nOneSubNodeValue = (DWORD)m_TerminalTreeCtrl.GetItemData(onesubnode);
	return nOneSubNodeValue;
}


void CDlgTerminalParent::OnDblclkTreeTerminal(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM item = m_TerminalTreeCtrl.GetSelectedItem();
	ItemData *data = (ItemData *)m_TerminalTreeCtrl.GetItemData(item);

	DeliveryEvent(Event_Type_Item_Double_Click, data);	
}

void CDlgTerminalParent::OnClickTreeTerminal(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*
	// TODO: Add your control notification handler code here
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	CWFTTNNView *pView = (CWFTTNNView*)pMainWnd->GetActiveView();
	pView->pDlgBatchInfo->ShowWindow(SW_SHOW);
	
	pView->pDlgSetParam->ShowWindow(SW_HIDE);
	pView->pDlgDataBrowse->ShowWindow(SW_HIDE);
	pView->pDlgRndRead->ShowWindow(SW_HIDE);
	pView->pDlgTerminalInfo->ShowWindow(SW_HIDE);
	pView->pDlgAutoCeShi->ShowWindow(SW_HIDE);
	pView->pDlgSetChannel->ShowWindow(SW_HIDE);

	*pResult = 0;
	*/
}

void CDlgTerminalParent::OnTvnSelchangedTreeTerminal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CDlgTerminalParent::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

    CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(rect);
 	
 	if (m_TerminalTreeCtrl.GetSafeHwnd())
 	{
 		m_TerminalTreeCtrl.MoveWindow(rect);
 	}
}

void CDlgTerminalParent::OnReceive(int state, char *buf, int len, u_int64 strTermAddr)
{
	std::string addr = ptcl_packet_csg::convert_device_addr(strTermAddr);
	RrefreshState(addr.c_str(), state);
}

void CDlgTerminalParent::RrefreshState(const char *addr, int state)
{
	if (!m_TerminalTreeCtrl.ItemHasChildren(m_hItem)) 
		return;
	
	HTREEITEM hBatchItem = NULL;
	HTREEITEM hTerminalItem = NULL;
	ItemData *pData = NULL;	

	hBatchItem = m_TerminalTreeCtrl.GetChildItem(m_hItem);
	while (hBatchItem)
	{
		pData = (ItemData *)m_TerminalTreeCtrl.GetItemData(hBatchItem);
		if (pData->m_type == ITEM_TYPE_BATCH && m_TerminalTreeCtrl.ItemHasChildren(hBatchItem))
		{
			hTerminalItem = m_TerminalTreeCtrl.GetChildItem(hBatchItem);			
			while (hTerminalItem)
			{
				TerminalItemData *pTermData = (TerminalItemData *)m_TerminalTreeCtrl.GetItemData(hTerminalItem);	
				if (pTermData->m_addr.CompareNoCase(addr) == 0)
				{
					CString temp = m_TerminalTreeCtrl.GetItemText(hTerminalItem);
					if (temp.Find("[") != -1)
					{						
						temp = temp.Left(temp.Find("["));
						temp = temp.Trim();
					}
					if (state == CChannelMgr::ONLINE || state == CChannelMgr::RECEIVING)
					{
						temp.Append(" [在线]");
					}
					m_TerminalTreeCtrl.SetItemText(hTerminalItem, temp);
					return;
				}
				hTerminalItem = m_TerminalTreeCtrl.GetNextItem(hTerminalItem, TVGN_NEXT);
			}
		}

		hBatchItem = m_TerminalTreeCtrl.GetNextItem(hBatchItem, TVGN_NEXT);
	}	
}
