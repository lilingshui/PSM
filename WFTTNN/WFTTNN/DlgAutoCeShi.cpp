// DlgAutoCeShi.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgAutoCeShi.h"
#include "PtclModule_NN.h"
#include "Protocol.h"
#include "DlgAutoTask.h"

bool g_nZiDongOrShouDongFlag;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoCeShi dialog


CDlgAutoCeShi::CDlgAutoCeShi(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAutoCeShi::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAutoCeShi)
		// NOTE: the ClassWizard will add member initialization here
	
	//}}AFX_DATA_INIT
	CPacketTransfer::GetInstance()->RegisterAutoCeShiPacketWnd(this);
}


void CDlgAutoCeShi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TERMINALINFO, m_ShowTerminalInfoListCtrl);
	DDX_Control(pDX, IDC_LIST_TERMINALINFO1, m_ShowTerminalInfoListCtrl1);
	DDX_Control(pDX, IDC_COMBO_DATATYPE, m_cbDataType);
/*	DDX_Control(pDX, IDC_CKPARAM, btnParam);
	DDX_Control(pDX, IDC_CKTIME, btnTime);
	DDX_Control(pDX, IDC_CKHIS, btnHis);
	DDX_Control(pDX, IDC_CKREAL, btnReal);
	*/
	DDX_Control(pDX, IDC_RESULT, m_Result);
	
	
	//{{AFX_DATA_MAP(CDlgAutoCeShi)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAutoCeShi, CDialog)
	//{{AFX_MSG_MAP(CDlgAutoCeShi)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_MESSAGE(AUTOCESHIPACKET_EVENT_NOTIFY,OnAutoCeShiPacketEventNotify)
	ON_BN_CLICKED(IDC_VIEWTASK, OnViewtask)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoCeShi message handlers



BOOL CDlgAutoCeShi::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	g_EventWaite = NULL;
	g_EventWaite = CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD dwExtendStyle = m_ShowTerminalInfoListCtrl.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ShowTerminalInfoListCtrl.SetExtendedStyle(dwExtendStyle);
	m_ShowTerminalInfoListCtrl1.SetExtendedStyle(dwExtendStyle);

	/**************初始化列表显示**************************************/
	m_ShowTerminalInfoListCtrl.InsertColumn(0,"终端编号",LVCFMT_CENTER,65);
	m_ShowTerminalInfoListCtrl.InsertColumn(1,"终端地址",LVCFMT_CENTER,100);
	m_ShowTerminalInfoListCtrl1.InsertColumn(0,"终端编号",LVCFMT_CENTER,65);
	m_ShowTerminalInfoListCtrl1.InsertColumn(1,"终端地址",LVCFMT_CENTER,100);
	InitTerminalInfoList();
	m_Result.SetExtendedStyle(dwExtendStyle);
	m_Result.InsertColumn(0,"时间",LVCFMT_CENTER,150);
	m_Result.InsertColumn(1,"终端编号",LVCFMT_CENTER,64);
	m_Result.InsertColumn(2,"测量点",LVCFMT_CENTER,51);
	m_Result.InsertColumn(3,"操作类型",LVCFMT_CENTER,200);
	m_Result.InsertColumn(4,"数据标识",LVCFMT_CENTER,68);
	m_Result.InsertColumn(5,"执行结果",LVCFMT_CENTER,64);
	m_Result.InsertColumn(6,"值",LVCFMT_CENTER,220);
	//----------------------------------------

	InitialTask();
	if(!InitTask(10000)) return FALSE;
	memset(g_st,0,sizeof(g_st));
	memset(g_et,0,sizeof(g_et));
	sprintf(g_st,GetTestId());
	sprintf(g_et,GetTestId());
	bbusy = 0;	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAutoCeShi::InitTerminalInfoList()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;

	char strSQL[300] = {0};
	int nRow = 0;

	sprintf(strSQL,"SELECT * FROM RTU_ARCH ");

	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;

	while (!record.IsEndEOF())
	{
		_variant_t val;
		int nRtuID;
		CString strRtuID = _T("");
		CString strRtuName = _T("");
		CString strRtuAddr = _T("");
		CString strRtuFact = _T("");

		record.GetFieldValue("RTU_ID",nRtuID);
		strRtuID.Format("%d", nRtuID);
		record.GetFieldValue("RTU_ADDR",val);
		strRtuAddr = val.bstrVal;		
		m_ShowTerminalInfoListCtrl.InsertItem(nRow, strRtuID);
		m_ShowTerminalInfoListCtrl.SetItemText(nRow, 1, strRtuAddr);		
		record.RecordMoveNext();
		nRow++;
	}
	m_nTerminalNum = nRow;
}

void CDlgAutoCeShi::OnAdd() 
{
	int nRow = m_ShowTerminalInfoListCtrl.GetSelectionMark();
	if(nRow<0)
	{
		AfxMessageBox("请选择终端编号！");
		return;
	}
	CString strTerminalNo = m_ShowTerminalInfoListCtrl.GetItemText(nRow,0);
	CString strTerminalAddr = m_ShowTerminalInfoListCtrl.GetItemText(nRow,1);
	int icount=m_ShowTerminalInfoListCtrl1.GetItemCount();
	m_ShowTerminalInfoListCtrl1.InsertItem(icount, strTerminalNo);	
	m_ShowTerminalInfoListCtrl1.SetItemText(icount, 1, strTerminalAddr);
	m_ShowTerminalInfoListCtrl.DeleteItem(nRow);

}



void CDlgAutoCeShi::OnDel() 
{
	int nRow = m_ShowTerminalInfoListCtrl1.GetSelectionMark();
	if(nRow<0)
	{
		AfxMessageBox("请选择终端编号！");
		return;
	}
	CString strTerminalNo = m_ShowTerminalInfoListCtrl1.GetItemText(nRow,0);
	CString strTerminalAddr = m_ShowTerminalInfoListCtrl1.GetItemText(nRow,1);
	int icount=m_ShowTerminalInfoListCtrl.GetItemCount();
	m_ShowTerminalInfoListCtrl.InsertItem(icount, strTerminalNo);	
	m_ShowTerminalInfoListCtrl.SetItemText(icount, 1, strTerminalAddr);
	m_ShowTerminalInfoListCtrl1.DeleteItem(nRow);	
}

void CDlgAutoCeShi::OnStart() 
{
	KillTimer(1);

	int nCurSel = m_cbDataType.GetCurSel();

	if (nCurSel == -1)
	{
		AfxMessageBox("请选择测试任务！");
		return;
	}
	TASK_TYPE *pTaskType = (TASK_TYPE*)m_cbDataType.GetItemData(nCurSel);

	long TaskID = pTaskType->ParamID;
	g_testid=GetTestId();
	//--------------
	ResetTask();
	//----------------
	m_Result.DeleteAllItems();
	g_nZiDongOrShouDongFlag = true;
	
	
	for (int i=0;i<m_ShowTerminalInfoListCtrl1.GetItemCount();i++)
	{
		CString strTerminalAddr = m_ShowTerminalInfoListCtrl1.GetItemText(i,1);
		strTerminalAddr = _T("0031010700");
		CString strTerminalNo = m_ShowTerminalInfoListCtrl1.GetItemText(i,0);
		EexeTask(strTerminalNo,strTerminalAddr,TaskID);

	/*	if(btnParam.GetCheck())//参数设置
		{
			SetParam(strTerminalNo,strTerminalAddr);
		}

		if(btnTime.GetCheck())//读取/设置时间
		{
			SetTime(strTerminalNo,strTerminalAddr);
		}


		if(btnHis.GetCheck())//历史数据采集
		{
			ReadHis(strTerminalNo,strTerminalAddr);
		}

		if(btnReal.GetCheck())//当前数据采集
		{
			ReadReal(strTerminalNo,strTerminalAddr);
		}
		*/

		
	}
	if(m_TASK.maxcurcount>0)
	{
		SetTimer(1,1000,NULL);
		bbusy =0;
	}
}


//发送数据
int CDlgAutoCeShi::SendFrame(unsigned char *inbuf,int inlen)
{
	FRAME_BUF fb;
	fb.len = inlen;
	memcpy(fb.buf, inbuf, inlen);
	CHandlerMgr::GetInstance()->TransferToMaster(fb);
	return 1;
}




//执行方案任务
void CDlgAutoCeShi::EexeTask(CString strTerminalNo,CString strTerminalAddr,long TaskID)
{
	char strSQL[400];
	char sparamname[100];
	unsigned int deviceAddr;
	int pn;

	//------------------------------
	TASKNODE tasknode;
	memset(&tasknode,0,sizeof(TASKNODE));
	//-------------------------------

	//得到方案编号
	deviceAddr= GlobalFunc::convert_Terminal_StringToAddr(strTerminalAddr, 7);
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	CADORecordSet pnrs;
	if (pConnect == NULL)
		return ;

	//查找测量点
	memset(strSQL,0,sizeof(strSQL));
	sprintf(strSQL,"select * from MP_ARCH where RTU_ID=%s",strTerminalNo);
	if (!pConnect->ExecSQL(strSQL,pnrs))
		return ;
	while (!pnrs.IsEndEOF())
	{
		_variant_t val1;
		pnrs.GetFieldValue("MP_ID",val1);
		pn=(long)val1;


		memset(strSQL,0,sizeof(strSQL));
		sprintf(strSQL,"select * from AUTOTEST_READING_ARCH where READING_ID=%d",TaskID);
		CADORecordSet record;
		if (!pConnect->ExecSQL(strSQL,record))
			return ;
		while (!record.IsEndEOF())
		{
			_variant_t val;
			//unsigned long lID;
			CString strParamID = _T("");
			CString strParamName;
			long Paramtype;//参数类型


			//record.GetFieldValue("ID",val);
			//lID = (unsigned long)val.bstrVal;		
			record.GetFieldValue("PARAM_ID",val);
			strParamID = val.bstrVal;
			record.GetFieldValue("CRS_DATA_TYPE",val);
			strParamName = val.bstrVal;
			sprintf(sparamname,"%s",strParamName);

			record.GetFieldValue("PARAM_TYPE",val);
			Paramtype = (long)val;

			string strSign = strParamID;

			switch(Paramtype)
			{
				case 1://当前数据
					{
					CPtclModule_NN::FormatPnFnData vPnFn[20];
					int nPnFnCount = 1;
					vPnFn[0].Pn = pn;
					
					GlobalFunc::convert_Sign(strSign,vPnFn[0].Fn,50);

					CPtclPacket_NN packet_NN;
					packet_NN.setDeviceAddr(deviceAddr);
					CPtclModule_NN::getInstance()->FormatPacket_ReadData_RealTime(&packet_NN,vPnFn,nPnFnCount);	
					//-------------------------------------
					sprintf(tasknode.sterminalno,"%s",strTerminalNo);//终端编号
					sprintf(tasknode.saddr,"%s",strTerminalAddr);//终端地址
					tasknode.pn=pn;//测量点号
					sprintf(tasknode.sparamid,"%s",strParamID);//数据标识
					//sprintf(tasknode.soper,"%s","抄读实时数据");//操作类型
					sprintf(tasknode.soper,"%s",sparamname);//操作类型
					tasknode.ilen =packet_NN.getPacketBufLen();
					memcpy(tasknode.buf,packet_NN.getPacketBuf(), packet_NN.getPacketBufLen());
					AddTaskNode(&tasknode);
					//-------------------------------------
					}
					break;
				case 2://历史数据
				case 3:
					{
						CPtclModule_NN::DataUint pDataUint;
						int nPnFnCount = 1;
						short int outlen=0;

						char stmp[50];
						//开始时间
						record.GetFieldValue("CRS_S_TIME",val);
	
						memset(stmp,0,sizeof(stmp));
						VarianttoStr(val,stmp);						
						//Convertstr(stmp,strlen(stmp));
						StringtoByte(stmp,(char *)pDataUint.pDataBuf,&outlen);
						pDataUint.lenDataBuf = outlen;

						//结束时间
						record.GetFieldValue("CRS_E_TIME",val);
						memset(stmp,0,sizeof(stmp));
						VarianttoStr(val,stmp);	
						//Convertstr(stmp,strlen(stmp));
						StringtoByte(stmp,(char *)(pDataUint.pDataBuf+pDataUint.lenDataBuf),&outlen);
						pDataUint.lenDataBuf += outlen;
						if(Paramtype==2)//日
						{
							*(pDataUint.pDataBuf+pDataUint.lenDataBuf) = 0x06;
						}
						else //月
						{
							*(pDataUint.pDataBuf+pDataUint.lenDataBuf) = 0x07;
						}
						pDataUint.lenDataBuf++;


						pDataUint.PnFn.Pn =pn;
				
						GlobalFunc::convert_Sign(strSign,pDataUint.PnFn.Fn,50);

						CPtclPacket_NN packet_NN;
						packet_NN.setDeviceAddr(deviceAddr);
						CPtclModule_NN::getInstance()->FormatPacket_ReadData_History(&packet_NN, &pDataUint, nPnFnCount);

						//-------------------------------------
						sprintf(tasknode.sterminalno,"%s",strTerminalNo);//终端编号
						sprintf(tasknode.saddr,"%s",strTerminalAddr);//终端地址
						tasknode.pn=pn;//测量点号
						sprintf(tasknode.sparamid,"%s",strParamID);//数据标识
						//sprintf(tasknode.soper,"%s","抄读历史数据");//操作类型
						sprintf(tasknode.soper,"%s",sparamname);//操作类型
						tasknode.ilen =packet_NN.getPacketBufLen();
						memcpy(tasknode.buf,packet_NN.getPacketBuf(), packet_NN.getPacketBufLen());
						AddTaskNode(&tasknode);
						//-------------------------------------	
					}
					break;
			}			
			record.RecordMoveNext();
		}
		pnrs.RecordMoveNext();
	}
}



//向结果表格写入数据
void CDlgAutoCeShi::AddResultOneLine(char *stime,char *terminaladdr,long pn,char *stype,char *paramid,char *sresult,char *svalue)
{	
	char spn[10];
	sprintf(spn,"%d",pn);
	int icount=m_Result.GetItemCount();
	m_Result.InsertItem(icount, stime);	
	m_Result.SetItemText(icount, 1, terminaladdr);
	m_Result.SetItemText(icount, 2, spn);
	m_Result.SetItemText(icount, 3, stype);
	m_Result.SetItemText(icount, 4, paramid);
	m_Result.SetItemText(icount, 5, sresult);
	m_Result.SetItemText(icount, 6, svalue);

}



CString CDlgAutoCeShi::GetTestId()
{
	CTime tm=CTime::GetCurrentTime();
	CString str;
	str.Format("%04d%02d%02d%02d%02d%02d",tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),tm.GetSecond());
	return str;
}

void CDlgAutoCeShi::GetCurTimeFmt(char *sfmttm)
{
	CTime tm=CTime::GetCurrentTime();
	
	sprintf(sfmttm,"%04d-%02d-%02d %02d:%02d:%02d",tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),tm.GetSecond());
}

LRESULT CDlgAutoCeShi::OnAutoCeShiPacketEventNotify(WPARAM wParam,LPARAM lParam)
{
	//FILE *fp;
	if(m_TASK.flag == 1)
	{
		FRAME_BUF *pInfo=(FRAME_BUF*)lParam;
		//fp=fopen("xxx.txt","a+");
		//fprintf(fp,"curpos=%d,maxcurcount=%d,%s\n",m_TASK.curpos,m_TASK.maxcurcount,m_TASK.pTASKNODE[m_TASK.curpos].sparamid);
		//fclose(fp);

		memcpy(m_TASK.buf,pInfo->buf,pInfo->len);
		m_TASK.ilen = pInfo->len;
		m_TASK.flag=2;
	}	
	return 0;
}

void CDlgAutoCeShi::DoMessage()
{
	MSG msg;
	       
	while(PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))        
	{        
		//if (msg.message == WM_QUIT)        
		//	   break ;        
		TranslateMessage (&msg) ;        
		DispatchMessage (&msg) ;        
	}        

}


void CDlgAutoCeShi::OnButton5() 
{
	unsigned char inbuf[50],outbuf[100];
	char sinbuf[]="681200120068A8003107010000120C6200000002010200006616";
	short int ilen;
	int inlen,outlen;
	StringtoByte(sinbuf,(char *)inbuf,&ilen);
	inlen = ilen;
	ParseRetFrame(inbuf,inlen,outbuf,&outlen);
}


//-------------------------------------------
int CDlgAutoCeShi::InitTask(long maxcount)
{
	if ((m_TASK.pTASKNODE = (LPTASKNODE) GlobalAlloc(GPTR,sizeof(TASKNODE)*maxcount)) == NULL) 
	{
         return 0; 
	}
	m_TASK.maxcount=maxcount;
	m_TASK.maxcurcount=0;
	m_TASK.curpos=-1;
	m_TASK.flag=0;
	m_TASK.execout=0;
	return 1;
}

void CDlgAutoCeShi::FreeTask()
{
	if(m_TASK.pTASKNODE != NULL) GlobalFree(m_TASK.pTASKNODE);
	m_TASK.maxcount=0;
	m_TASK.maxcurcount=0;
	m_TASK.curpos=-1;
	m_TASK.flag=0;
	m_TASK.execout=0;
}

//重置任务状态
int CDlgAutoCeShi::ResetTask()
{
	m_TASK.maxcurcount=0;
	m_TASK.curpos=-1;
	m_TASK.flag=0;
	m_TASK.execout=0;
	return 1;
}

//得到当前正在执行的任务
LPTASKNODE  CDlgAutoCeShi::GetCurTask()
{
	LPTASKNODE pTASKNODE = NULL;
	if(m_TASK.curpos > -1)
	{
		pTASKNODE = &m_TASK.pTASKNODE[m_TASK.curpos];
	}
	return pTASKNODE;
}

//得到下一任务
LPTASKNODE  CDlgAutoCeShi::GetNextTask()
{
	LPTASKNODE pTASKNODE = NULL;
	if(m_TASK.curpos < m_TASK.maxcurcount-1)
	{
		m_TASK.curpos++;
		m_TASK.flag=0;
		m_TASK.execout=0;
		pTASKNODE = &m_TASK.pTASKNODE[m_TASK.curpos];
	}
	return pTASKNODE;
}

int  CDlgAutoCeShi::AddTaskNode(LPTASKNODE pTASKNODE)
{
	if(m_TASK.maxcurcount >= m_TASK.maxcount)
	{
		return 0;
	}
	else
	{
		memcpy(&m_TASK.pTASKNODE[m_TASK.maxcurcount],pTASKNODE,sizeof(TASKNODE));
		m_TASK.maxcurcount++;
	}
	return 1;
}


//-------------------------------------------
void CDlgAutoCeShi::OnTimer(UINT nIDEvent) 
{
	LPTASKNODE pTASKNODE;
	int iret;
	int dtmout=4;//超时时间
	int	MaxExeCount=1;//执行时间
	char sresult[10],svalue[40];
	

	memset(sresult,0,sizeof(sresult));
	memset(svalue,0,sizeof(svalue));
	if(!bbusy)
	{
		bbusy=1;
		if(m_TASK.flag==0)//未执行
		{
			pTASKNODE = GetNextTask();
			if(pTASKNODE == NULL)//所有任务执行完
			{
				KillTimer(1);
				return;
			}
			m_TASK.flag= 1;
			m_TASK.execout = 1;
			m_TASK.tm = time(NULL);
			SendFrame(pTASKNODE->buf,pTASKNODE->ilen);
		}
		else if(m_TASK.flag==1)//在执行
		{
			if(time(NULL) - m_TASK.tm > dtmout)//超时
			{
				if(m_TASK.execout < MaxExeCount)//还有次数未执行
				{
					pTASKNODE = GetCurTask();
					if(pTASKNODE == NULL) 
					{
						return;
					}

					m_TASK.flag= 1;
					m_TASK.execout ++;
					m_TASK.tm = time(NULL);
					SendFrame(pTASKNODE->buf,pTASKNODE->ilen);
				}
				else
				{
					//数据入库
					sprintf(sresult,"失败");
					pTASKNODE = GetCurTask();
					{
						if(pTASKNODE==NULL) 
						{
							return;
						}
					}
					ShowAndToDB(pTASKNODE,sresult,svalue);
					pTASKNODE = GetNextTask();
					if(pTASKNODE == NULL)//所有任务执行完
					{
						KillTimer(1);
						return;
					}
					m_TASK.flag= 1;
					m_TASK.execout = 1;
					m_TASK.tm = time(NULL);
					SendFrame(pTASKNODE->buf,pTASKNODE->ilen);
				}
			}
		}
		else if(m_TASK.flag==2)//本次任务完成，有数据
		{
			//解析数据
			unsigned char outdata[1024];
			int outlen;
			memset(outdata,0,sizeof(outdata));

			pTASKNODE = GetCurTask();
			if(pTASKNODE==NULL) 
			{
				return;
			}
			iret = ParseRetFrame(m_TASK.buf,m_TASK.ilen,outdata,&outlen);
			if(iret==RTSUCCESS)
			{
				LPUPDATA pUPDATA=LPUPDATA(outdata+sizeof(UPDATAHEAD));
				sprintf(svalue,"%s",pUPDATA->sdata);
				sprintf(sresult,"成功");			
			}
			else if((iret==RTALLCONFIRM)||(iret==RTCONFIRM))//确认
			{
				sprintf(sresult,"成功");
				if(strlen(pTASKNODE->sparam)>0)
				{
					sprintf(svalue,"%s",pTASKNODE->sparam);
				}
			}
			else
			{
				sprintf(sresult,"失败");
			}
			//数据入库
			ShowAndToDB(pTASKNODE,sresult,svalue);	
			pTASKNODE = GetNextTask();
			if(pTASKNODE == NULL)//所有任务执行完
			{
				KillTimer(1);
				return;
			}
			m_TASK.flag= 1;
			m_TASK.execout = 1;
			m_TASK.tm = time(NULL);
			SendFrame(pTASKNODE->buf,pTASKNODE->ilen);
		}
		bbusy=0;
	}	
	CDialog::OnTimer(nIDEvent);
}

void CDlgAutoCeShi::ShowAndToDB(LPTASKNODE pTASKNODE,char *sresult,char *svalue)
{
	char ssql[500],stime[20];
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	GetCurTimeFmt(stime);
	sprintf(ssql,"INSERT INTO RD_AUTOTEST_RT(SID,STIME,RTU_ADDR,MP_ID,OPTYPE,PARAM_ID,SRESULT,SVALUE) VALUES ");
	sprintf(ssql+strlen(ssql),"('%s','%s',%s,%d,'%s','%s','%s','%s')",g_testid,stime,pTASKNODE->sterminalno,pTASKNODE->pn,pTASKNODE->soper,pTASKNODE->sparamid,sresult,svalue);
	pConnect->ExecSQL(ssql);	
	AddResultOneLine(stime,pTASKNODE->sterminalno,pTASKNODE->pn,pTASKNODE->soper,pTASKNODE->sparamid,sresult,svalue);

//	m_Result.SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
	m_Result.PostMessage(WM_VSCROLL, SB_BOTTOM, NULL);
//	int nCount = m_Result.GetItemCount();
//	if (nCount > 0)
//		m_Result.EnsureVisible(nCount-1, FALSE);
}

void CDlgAutoCeShi::OnClose() 
{
	KillTimer(1);
	FreeTask();	
	CDialog::OnClose();
} 



void CDlgAutoCeShi::InitialTask()
{
	m_cbDataType.Clear();
	m_cbDataType.ResetContent();
	
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	char strSQL[300] = {0};
	int nRow = 0;
	
	sprintf(strSQL,"SELECT * FROM READING_DESC ");//主站采集测试方案描述
	
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return ;
	
	while (!record.IsEndEOF())
	{
		_variant_t val;
		long ParamID;
		CString strParamName = _T("");
		
		record.GetFieldValue("READING_ID",val);
		ParamID=(long)val;

		//sprintf(strParamID,"%d",(long)val);
		
		record.GetFieldValue("READING_ID_DESC",val);
		strParamName = val.bstrVal;
		
		m_cbDataType.InsertString(nRow, strParamName);		
		TASK_TYPE *pTaskType = new TASK_TYPE;
		pTaskType->ParamID = ParamID;
		
		m_cbDataType.SetItemData(nRow, (DWORD)pTaskType);		
		record.RecordMoveNext();
		nRow++;
	}
	
}


int CDlgAutoCeShi::VarianttoStr(_variant_t val,char *str)
{
	char sd[50];

	if(val.vt==VT_BSTR)
	{		
		sprintf(sd,"%s",(LPCSTR)(_bstr_t)val);
		
		sprintf(str,sd);
	}
	return 1;
}

void CDlgAutoCeShi::OnViewtask() 
{
	/*CTestdllDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{		
	}
	else if (nResponse == IDCANCEL)
	{
		
	}
	*/
	CDlgAutoTask dlg;
	dlg.DoModal();	
	InitialTask();
}
