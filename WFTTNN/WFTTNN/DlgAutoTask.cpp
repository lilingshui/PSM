// DlgAutoTask.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgAutoTask.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoTask dialog

int StrToDateTime(CString stime,SYSTEMTIME *systime,SYSTEMTIME *systime2);
CDlgAutoTask::CDlgAutoTask(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAutoTask::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAutoTask)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgAutoTask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TASK, m_TaskListCtrl);
	DDX_Control(pDX, IDC_TASKITEM, m_TaskItemListCtrl);
	DDX_Text(pDX, IDC_EDTASKNAME, m_EdTaskName);
	DDX_Control(pDX, IDC_ADDTASK, m_BtnAddTask);
	DDX_Control(pDX, IDC_DELTASK, m_BtnDelTask);
	DDX_Control(pDX, IDC_ADDSTEP, m_BtnAddStep);
	DDX_Control(pDX, IDC_SAVESTEP, m_BtnSaveStep);
	DDX_Control(pDX, IDC_DELSTEP, m_BtnDelStep);
	DDX_Control(pDX, IDC_DATETIME_ENDDATE, m_EndDate);
	DDX_Control(pDX, IDC_DATETIME_ENDTIME, m_EndTime);
	DDX_Control(pDX, IDC_DATETIME_STARTTIME, m_StartTime);
	DDX_Control(pDX, IDC_DATETIME_STARTDATE, m_StartDate);
	DDX_Control(pDX, IDC_COMBO_DATATYPE, m_cbDataType);
	DDX_Control(pDX, IDC_RDCUR, m_rdCur);
	DDX_Control(pDX, IDC_RDHIS, m_rdHis);
	DDX_Control(pDX, IDC_COMBO_DATATYPEHIS, m_cbDataTypeHis);




	//{{AFX_DATA_MAP(CDlgAutoTask)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAutoTask, CDialog)
	//{{AFX_MSG_MAP(CDlgAutoTask)
	ON_BN_CLICKED(IDC_ADDTASK, OnAddtask)
	ON_BN_CLICKED(IDC_DELTASK, OnDeltask)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TASK, OnClickListTask)
	ON_BN_CLICKED(IDC_RDCUR, OnRdcur)
	ON_BN_CLICKED(IDC_RDHIS, OnRdhis)
	ON_BN_CLICKED(IDC_ADDSTEP, OnAddstep)
	ON_BN_CLICKED(IDC_SAVESTEP, OnSavestep)
	ON_NOTIFY(NM_CLICK, IDC_TASKITEM, OnClickTaskitem)
	ON_BN_CLICKED(IDC_DELSTEP, OnDelstep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoTask message handlers

BOOL CDlgAutoTask::OnInitDialog() 
{
	CDialog::OnInitDialog();
	DWORD dwExtendStyle = m_TaskListCtrl.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_TaskListCtrl.SetExtendedStyle(dwExtendStyle);
	m_TaskItemListCtrl.SetExtendedStyle(dwExtendStyle);
	/**************初始化列表显示**************************************/
	m_TaskListCtrl.InsertColumn(0,"编号",LVCFMT_CENTER,0);
	m_TaskListCtrl.InsertColumn(1,"名称",LVCFMT_LEFT,150);


	m_TaskItemListCtrl.InsertColumn(0,"编号",LVCFMT_CENTER,0);
	m_TaskItemListCtrl.InsertColumn(1,"采集任务代号",LVCFMT_CENTER,0);
	m_TaskItemListCtrl.InsertColumn(2,"数据类型",LVCFMT_CENTER,200);
	m_TaskItemListCtrl.InsertColumn(3,"数据标识",LVCFMT_CENTER,100);
	m_TaskItemListCtrl.InsertColumn(4,"参数类型",LVCFMT_CENTER,0);
	
	m_TaskItemListCtrl.InsertColumn(5,"开始时间",LVCFMT_CENTER,150);
	m_TaskItemListCtrl.InsertColumn(6,"结束时间",LVCFMT_CENTER,150);
	

	InitTaskList();
	InitialDataType(0);
	InitialDataType(1);
	SetRdState(0);
	OnRdcur();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//添加任务
void CDlgAutoTask::OnAddtask() 
{
	int maxid=1;
	char sid[10];

	UpdateData(true);
	if (m_EdTaskName=="")
	{
		AfxMessageBox("任务名称不能为空！");
		return;
	}
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	char strSQL[300] = {0};
	sprintf(strSQL,"SELECT MAX(READING_ID) as READING_ID FROM READING_DESC ");

	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	if(!record.IsEndEOF())
	{
		_variant_t val;
		record.GetFieldValue("READING_ID",val);
		maxid=(long)val + 1;
	}
	sprintf(strSQL,"INSERT INTO READING_DESC(READING_ID,READING_ID_DESC) VALUES(%d,'%s') ",maxid,m_EdTaskName);
	pConnect->ExecSQL(strSQL);	
	sprintf(sid,"%d",maxid);
	int icount=m_TaskListCtrl.GetItemCount();
	m_TaskListCtrl.InsertItem(icount, sid);	
	m_TaskListCtrl.SetItemText(icount, 1, m_EdTaskName);	
	m_TaskListCtrl.SetSelectionMark(icount);

	//LRESULT pResult;
	//OnClickListTask(NULL, &pResult) ;
	//m_TaskListCtrl.SetFocus();
}

void CDlgAutoTask::InitTaskList()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};
	int nRow = 0;

	sprintf(strSQL,"SELECT * FROM READING_DESC ");

	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		
		CString strTaskid = _T("");
		CString strTaskName = _T("");
		

		record.GetFieldValue("READING_ID",val);
		strTaskid.Format("%d", (long)val);
		record.GetFieldValue("READING_ID_DESC",val);
		strTaskName=val.bstrVal;


		m_TaskListCtrl.InsertItem(nRow, strTaskid);
		m_TaskListCtrl.SetItemText(nRow, 1, strTaskName);		
		record.RecordMoveNext();
		nRow++;
	}
}

void CDlgAutoTask::OnDeltask() 
{
	int nRow = m_TaskListCtrl.GetSelectionMark();
	if(nRow<0)
	{
		AfxMessageBox("请选择任务！");
		return;
	}
	CString strTasId = m_TaskListCtrl.GetItemText(nRow,0);
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};


	sprintf(strSQL,"DELETE FROM READING_DESC where  READING_ID = %s",strTasId);
	pConnect->ExecSQL(strSQL);
	sprintf(strSQL,"DELETE FROM AUTOTEST_READING_ARCH where  READING_ID = %s",strTasId);
	pConnect->ExecSQL(strSQL);	
	m_TaskListCtrl.DeleteItem(nRow);
	
}


void CDlgAutoTask::OnClickListTask(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	int nRow = m_TaskListCtrl.GetSelectionMark();
	if(nRow<0)
	{
		AfxMessageBox("请选择任务！");
		return;
	}
	CString strTasId = m_TaskListCtrl.GetItemText(nRow,0);
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	m_TaskItemListCtrl.DeleteAllItems();

	char strSQL[300] = {0};
	sprintf(strSQL,"SELECT * FROM AUTOTEST_READING_ARCH WHERE READING_ID=%s",strTasId);

	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;
	m_TaskItemListCtrl.DeleteAllItems();

	while (!record.IsEndEOF())
	{
		_variant_t val;		
		
		CString strCRS_NO = _T("");
		CString strCRS_DATA_TYPE = _T("");
		CString strPARAM_ID = _T("");
		CString strPARAM_TYPE = _T("");
		CString strCRS_S_TIME = _T("");
		CString strCRS_E_TIME = _T("");		

		record.GetFieldValue("CRS_NO",val);
		strCRS_NO.Format("%d", (long)val);
		record.GetFieldValue("CRS_DATA_TYPE",val);
		strCRS_DATA_TYPE=val.bstrVal;
		record.GetFieldValue("PARAM_ID",val);
		strPARAM_ID=val.bstrVal;
		record.GetFieldValue("PARAM_TYPE",val);
		strPARAM_TYPE.Format("%d", (long)val);

		char sttmp[50];
		record.GetFieldValue("CRS_S_TIME",val);
		VarianttoStr(val,sttmp);
		strCRS_S_TIME = sttmp;

		record.GetFieldValue("CRS_E_TIME",val);
		VarianttoStr(val,sttmp);
		strCRS_E_TIME = sttmp;

		AddOneTaskItem(strTasId,strCRS_NO,strCRS_DATA_TYPE,strPARAM_ID,strPARAM_TYPE,strCRS_S_TIME,strCRS_E_TIME);
		
		record.RecordMoveNext();

	}
	
	*pResult = 0;
}

//向步骤明细加入一个步骤
void CDlgAutoTask::AddOneTaskItem(CString strTasId,CString strCRS_NO,CString strCRS_DATA_TYPE,CString strPARAM_ID,CString strPARAM_TYPE,CString strCRS_S_TIME,CString strCRS_E_TIME)
{
	int icount=m_TaskItemListCtrl.GetItemCount();

	m_TaskItemListCtrl.InsertItem(icount, strTasId);
	m_TaskItemListCtrl.SetItemText(icount, 1, strCRS_NO);	
	m_TaskItemListCtrl.SetItemText(icount, 2, strCRS_DATA_TYPE);
	m_TaskItemListCtrl.SetItemText(icount, 3, strPARAM_ID);
	m_TaskItemListCtrl.SetItemText(icount, 4, strPARAM_TYPE);
	m_TaskItemListCtrl.SetItemText(icount, 5, strCRS_S_TIME);
	m_TaskItemListCtrl.SetItemText(icount, 6, strCRS_E_TIME);
}
//修改步骤明细一个步骤
void CDlgAutoTask::ModOneTaskItem(int icount,CString strCRS_DATA_TYPE,CString strPARAM_ID,CString strPARAM_TYPE,CString strCRS_S_TIME,CString strCRS_E_TIME)
{	
	m_TaskItemListCtrl.SetItemText(icount, 2, strCRS_DATA_TYPE);
	m_TaskItemListCtrl.SetItemText(icount, 3, strPARAM_ID);
	m_TaskItemListCtrl.SetItemText(icount, 4, strPARAM_TYPE);
	m_TaskItemListCtrl.SetItemText(icount, 5, strCRS_S_TIME);
	m_TaskItemListCtrl.SetItemText(icount, 6, strCRS_E_TIME);
}

//向数据库添加一个步骤
void CDlgAutoTask::AddOneTaskItemToDB(CString strTasId,CString strCRS_NO,CString strCRS_DATA_TYPE,CString strPARAM_ID,CString strPARAM_TYPE,CString strCRS_S_TIME,CString strCRS_E_TIME)
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};	
	sprintf(strSQL,"DELETE FROM AUTOTEST_READING_ARCH where  READING_ID = %s and CRS_NO=%s",strTasId,strCRS_NO);
	pConnect->ExecSQL(strSQL);	
	sprintf(strSQL,"INSERT INTO AUTOTEST_READING_ARCH(READING_ID,CRS_NO,CRS_DATA_TYPE,PARAM_ID,PARAM_TYPE,CRS_S_TIME,CRS_E_TIME) VALUES (%s,%s,'%s','%s','%s','%s','%s')",
		                                              strTasId,strCRS_NO,strCRS_DATA_TYPE,strPARAM_ID,strPARAM_TYPE,strCRS_S_TIME,strCRS_E_TIME);
	pConnect->ExecSQL(strSQL);

}

int CDlgAutoTask::VarianttoStr(_variant_t val,char *str)
{
	
	char syear[10],smon[10],sday[5],shour[5],smin[5],sd[20];
	if(val.vt==VT_BSTR)
	{		
		sprintf(sd,"%s",(LPCSTR)(_bstr_t)val);
		memset(syear,0,sizeof(syear));
		memcpy(syear,sd,4);
		memset(smon,0,sizeof(smon));
		memcpy(smon,sd+4,2);
		memset(sday,0,sizeof(sday));
		memcpy(sday,sd+6,2);
		memset(shour,0,sizeof(shour));
		memcpy(shour,sd+8,2);
		memset(smin,0,sizeof(smin));
		memcpy(smin,sd+10,2);
		sprintf(str,"%s-%s-%s %s:%s",syear,smon,sday,shour,smin);
	}
	
	return 1;
}


void CDlgAutoTask::SetRdState(int itype)
{
	switch(itype)
	{
		case 0:
			m_rdCur.SetCheck(1);			
			break;
		case 1:
			m_rdHis.SetCheck(1);		
			break;
	}
}

void CDlgAutoTask::InitialDataType(int itype)
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	char strSQL[300] = {0};
	int nRow = 0;


	
	switch(itype)
	{
		case 0:
			m_cbDataType.Clear();
			m_cbDataType.ResetContent();
			sprintf(strSQL,"SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 1 ");
			break;
		case 1:
			m_cbDataTypeHis.Clear();
			m_cbDataTypeHis.ResetContent();
			sprintf(strSQL,"SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 2 OR ID = 3 ");
			break;
	}
	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;
	
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
		PARAM_TYPE *pPARAM_TYPE;
		
		switch(itype)
		{
			case 0:
				m_cbDataType.InsertString(nRow, strParamName);

				pPARAM_TYPE = new PARAM_TYPE;
				pPARAM_TYPE->strParamID = strParamID;
				pPARAM_TYPE->striID =strID;
				pPARAM_TYPE->strParmName = strParamName;

				m_cbDataType.SetItemData(nRow, (DWORD)pPARAM_TYPE);
				break;
			case 1:
				m_cbDataTypeHis.InsertString(nRow, strParamName);

				pPARAM_TYPE = new PARAM_TYPE;
				pPARAM_TYPE->strParamID = strParamID;
				pPARAM_TYPE->striID =strID;
				pPARAM_TYPE->strParmName = strParamName;

				m_cbDataTypeHis.SetItemData(nRow, (DWORD)pPARAM_TYPE);
				break;
		}
		
		record.RecordMoveNext();
		nRow++;
	}	
}





void CDlgAutoTask::OnRdcur() 
{
	//InitialDataType(0);
	m_cbDataType.ShowWindow(SW_SHOW);
	m_cbDataTypeHis.ShowWindow(SW_HIDE);
	
}

void CDlgAutoTask::OnRdhis() 
{
	//InitialDataType(1);
	m_cbDataType.ShowWindow(SW_HIDE);
	m_cbDataTypeHis.ShowWindow(SW_SHOW);
	
}

void CDlgAutoTask::OnAddstep() 
{
	CString strCRS_NO = _T("");
	CString strCRS_DATA_TYPE = _T("");
	CString strPARAM_ID = _T("");
	CString strPARAM_TYPE = _T("");
	CString strCRS_S_TIME = _T("");
	CString strCRS_S_TIME_D = _T("");
	CString strCRS_E_TIME = _T("");
	CString strCRS_E_TIME_D = _T("");
	int nRow = m_TaskListCtrl.GetSelectionMark();
	if(nRow<0)
	{
		AfxMessageBox("请选择任务！");
		return;
	}
	CString strTaskId = m_TaskListCtrl.GetItemText(nRow,0);



	int nCurSel=-1;
	if(m_rdCur.GetCheck())
	{
		int nCurSel = m_cbDataType.GetCurSel();
		if (nCurSel == -1)
		{
			AfxMessageBox("请选择数据类型！");
			return;
		}
		strCRS_NO.Format("%d",GetTaskidMaxItmNo(strTaskId));

		PARAM_TYPE *pPARAM_TYPE = (PARAM_TYPE*)m_cbDataType.GetItemData(nCurSel);
		strPARAM_ID=pPARAM_TYPE->strParamID ;
		strPARAM_TYPE=pPARAM_TYPE->striID;
		strCRS_DATA_TYPE=pPARAM_TYPE->strParmName;
	}
	else
	{
		int nCurSel = m_cbDataTypeHis.GetCurSel();
		if (nCurSel == -1)
		{
			AfxMessageBox("请选择数据类型！");
			return;
		}
		strCRS_NO.Format("%d",GetTaskidMaxItmNo(strTaskId));

		PARAM_TYPE *pPARAM_TYPE = (PARAM_TYPE*)m_cbDataTypeHis.GetItemData(nCurSel);
		strPARAM_ID=pPARAM_TYPE->strParamID ;
		strPARAM_TYPE=pPARAM_TYPE->striID;
		strCRS_DATA_TYPE=pPARAM_TYPE->strParmName;
	}



	SYSTEMTIME systime,systime2;
	m_StartDate.GetTime(&systime);
	m_StartTime.GetTime(&systime2);
	strCRS_S_TIME_D.Format("%04d%02d%02d%02d%02d",systime.wYear,systime.wMonth,systime.wDay,systime2.wHour,systime2.wMinute);
	strCRS_S_TIME.Format("%04d-%02d-%02d %02d;%02d",systime.wYear,systime.wMonth,systime.wDay,systime2.wHour,systime2.wMinute);

	m_EndDate.GetTime(&systime);
	m_EndTime.GetTime(&systime2);
	strCRS_E_TIME_D.Format("%04d%02d%02d%02d%02d",systime.wYear,systime.wMonth,systime.wDay,systime2.wHour,systime2.wMinute);
	strCRS_E_TIME.Format("%04d-%02d-%02d %02d;%02d",systime.wYear,systime.wMonth,systime.wDay,systime2.wHour,systime2.wMinute);

	AddOneTaskItem(strTaskId,strCRS_NO,strCRS_DATA_TYPE,strPARAM_ID,strPARAM_TYPE,strCRS_S_TIME,strCRS_E_TIME);
	AddOneTaskItemToDB(strTaskId,strCRS_NO,strCRS_DATA_TYPE,strPARAM_ID,strPARAM_TYPE,strCRS_S_TIME_D,strCRS_E_TIME_D);

	
}

void CDlgAutoTask::OnSavestep() 
{
	CString strTaskId =  _T("");
	CString strCRS_NO = _T("");
	CString strCRS_DATA_TYPE = _T("");
	CString strPARAM_ID = _T("");
	CString strPARAM_TYPE = _T("");
	CString strCRS_S_TIME = _T("");
	CString strCRS_S_TIME_D = _T("");
	CString strCRS_E_TIME = _T("");
	CString strCRS_E_TIME_D = _T("");
	int nRow = m_TaskListCtrl.GetSelectionMark();
	if(nRow<0)
	{
		AfxMessageBox("请选择任务！");
		return;
	}
	int nrowitm=m_TaskItemListCtrl.GetSelectionMark();
	if(nrowitm<0)
	{
		AfxMessageBox("请选择步骤！");
		return;
	}
	strTaskId=m_TaskItemListCtrl.GetItemText(nrowitm,0);
	strCRS_NO=m_TaskItemListCtrl.GetItemText(nrowitm,1);

	int nCurSel=-1;
	if(m_rdCur.GetCheck())
	{
		int nCurSel = m_cbDataType.GetCurSel();
		if (nCurSel == -1)
		{
			AfxMessageBox("请选择数据类型！");
			return;
		}
		

		PARAM_TYPE *pPARAM_TYPE = (PARAM_TYPE*)m_cbDataType.GetItemData(nCurSel);
		strPARAM_ID=pPARAM_TYPE->strParamID ;
		strPARAM_TYPE=pPARAM_TYPE->striID;
		strCRS_DATA_TYPE=pPARAM_TYPE->strParmName;
	}
	else
	{
		int nCurSel = m_cbDataTypeHis.GetCurSel();
		if (nCurSel == -1)
		{
			AfxMessageBox("请选择数据类型！");
			return;
		}
		

		PARAM_TYPE *pPARAM_TYPE = (PARAM_TYPE*)m_cbDataTypeHis.GetItemData(nCurSel);
		strPARAM_ID=pPARAM_TYPE->strParamID ;
		strPARAM_TYPE=pPARAM_TYPE->striID;
		strCRS_DATA_TYPE=pPARAM_TYPE->strParmName;
	}

	SYSTEMTIME systime,systime2;
	m_StartDate.GetTime(&systime);
	m_StartTime.GetTime(&systime2);
	strCRS_S_TIME_D.Format("%04d%02d%02d%02d%02d",systime.wYear,systime.wMonth,systime.wDay,systime2.wHour,systime2.wMinute);
	strCRS_S_TIME.Format("%04d-%02d-%02d %02d;%02d",systime.wYear,systime.wMonth,systime.wDay,systime2.wHour,systime2.wMinute);

	m_EndDate.GetTime(&systime);
	m_EndTime.GetTime(&systime2);
	strCRS_E_TIME_D.Format("%04d%02d%02d%02d%02d",systime.wYear,systime.wMonth,systime.wDay,systime2.wHour,systime2.wMinute);
	strCRS_E_TIME.Format("%04d-%02d-%02d %02d;%02d",systime.wYear,systime.wMonth,systime.wDay,systime2.wHour,systime2.wMinute);

	ModOneTaskItem(nrowitm,strCRS_DATA_TYPE,strPARAM_ID,strPARAM_TYPE,strCRS_S_TIME,strCRS_E_TIME);
	AddOneTaskItemToDB(strTaskId,strCRS_NO,strCRS_DATA_TYPE,strPARAM_ID,strPARAM_TYPE,strCRS_S_TIME_D,strCRS_E_TIME_D);	
}

//得到本任务的最大任务代号+1
int CDlgAutoTask::GetTaskidMaxItmNo(CString strTaskId)
{
	int maxid=1;
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return 0;
	
	char strSQL[300] = {0};
	sprintf(strSQL,"SELECT MAX(CRS_NO) AS CRS_NO FROM AUTOTEST_READING_ARCH WHERE READING_ID = %s ",strTaskId);
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return 0;

	if(!record.IsEndEOF())
	{
		_variant_t val;
		record.GetFieldValue("CRS_NO",val);
		if(val.vt !=VT_NULL)
		{
			maxid=(long)val + 1;
		}
	}
	return maxid;
}

void CDlgAutoTask::OnClickTaskitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString strCRS_NO = _T("");
	CString strCRS_DATA_TYPE = _T("");
	CString strPARAM_ID = _T("");
	CString strPARAM_TYPE = _T("");
	CString strCRS_S_TIME = _T("");
	CString strCRS_S_TIME_D = _T("");
	CString strCRS_E_TIME = _T("");
	CString strCRS_E_TIME_D = _T("");


	int iCursel=-1;
	int nRow = m_TaskItemListCtrl.GetSelectionMark();
	if(nRow<0)
	{		
		return;
	}
	strPARAM_TYPE=m_TaskItemListCtrl.GetItemText(nRow,4);
	strCRS_DATA_TYPE=m_TaskItemListCtrl.GetItemText(nRow,2);
	strCRS_S_TIME = m_TaskItemListCtrl.GetItemText(nRow,5);
	strCRS_E_TIME = m_TaskItemListCtrl.GetItemText(nRow,6);
	if(strPARAM_TYPE=='1')
	{
			m_rdCur.SetCheck(1);
			m_rdHis.SetCheck(0);
			OnRdcur();
			iCursel=m_cbDataType.FindString(-1,strCRS_DATA_TYPE);
			m_cbDataType.SetCurSel(iCursel);


	}
	else if((strPARAM_TYPE=='2')||(strPARAM_TYPE=='3'))
	{
		//if(!m_rdHis.GetCheck())
		{
			m_rdHis.SetCheck(1);
			m_rdCur.SetCheck(0);
			OnRdhis();
			iCursel=m_cbDataTypeHis.FindString(-1,strCRS_DATA_TYPE);
			m_cbDataTypeHis.SetCurSel(iCursel);

		}
	}	
	SYSTEMTIME systime, systime2;
	memset(&systime,0,sizeof(SYSTEMTIME));
	memset(&systime2,0,sizeof(SYSTEMTIME));
	StrToDateTime(strCRS_S_TIME,&systime, &systime2);
	m_StartDate.SetTime(&systime);
	m_StartTime.SetTime(&systime2);

	memset(&systime,0,sizeof(SYSTEMTIME));
	memset(&systime2,0,sizeof(SYSTEMTIME));
	StrToDateTime(strCRS_E_TIME,&systime, &systime2);
	m_EndDate.SetTime(&systime);
	m_EndTime.SetTime(&systime2);

	UpdateData(true);

	*pResult = 0;
}

int StrToDateTime(CString stime,SYSTEMTIME *systime,SYSTEMTIME *systime2)
{
	char syear[10],smon[10],sday[5],shour[5],smin[5],sd[20];
	sprintf(sd,stime);

		memset(syear,0,sizeof(syear));
		memcpy(syear,sd,4);
		memset(smon,0,sizeof(smon));
		memcpy(smon,sd+5,2);
		memset(sday,0,sizeof(sday));
		memcpy(sday,sd+8,2);
		memset(shour,0,sizeof(shour));
		memcpy(shour,sd+11,2);
		memset(smin,0,sizeof(smin));
		memcpy(smin,sd+14,2);

		systime->wYear = atoi(syear);
		systime->wMonth = atoi(smon);
		systime->wDay = atoi(sday);

		systime2->wYear = atoi(syear);
		systime2->wMonth = atoi(smon);
		systime2->wDay = atoi(sday);
		systime2->wHour = atoi(shour);
		systime2->wMinute = atoi(smin);
		return 1;
}

void CDlgAutoTask::OnDelstep() 
{
	CString strTaskId =  _T("");
	CString strCRS_NO = _T("");
	int nRow = m_TaskListCtrl.GetSelectionMark();
	if(nRow<0)
	{
		AfxMessageBox("请选择任务！");
		return;
	}
	int nrowitm=m_TaskItemListCtrl.GetSelectionMark();
	if(nrowitm<0)
	{
		AfxMessageBox("请选择步骤！");
		return;
	}
	strTaskId=m_TaskItemListCtrl.GetItemText(nrowitm,0);
	strCRS_NO=m_TaskItemListCtrl.GetItemText(nrowitm,1);
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};	
	sprintf(strSQL,"DELETE FROM AUTOTEST_READING_ARCH where  READING_ID = %s and CRS_NO=%s",strTaskId,strCRS_NO);
	pConnect->ExecSQL(strSQL);	
	m_TaskItemListCtrl.DeleteItem(nrowitm);
	

	
}
