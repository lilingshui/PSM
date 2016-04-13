// DlgRndRead.cpp : implementation file
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgRndRead.h"
#include "PtclModule_NN.h"
#include "Protocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRndRead dialog


CDlgRndRead::CDlgRndRead(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRndRead::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRndRead)
	//}}AFX_DATA_INIT
	CPacketTransfer::GetInstance()->RegisterRndReadPacketWnd(this);
}


void CDlgRndRead::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRndRead)
	DDX_Control(pDX, IDC_DATETIME_ENDDATE, m_EndDate);
	DDX_Control(pDX, IDC_DATETIME_ENDTIME, m_EndTime);
	DDX_Control(pDX, IDC_DATETIME_STARTTIME, m_StartTime);
	DDX_Control(pDX, IDC_DATETIME_STARTDATE, m_StartDate);
	DDX_Control(pDX, IDC_COMBO_CURDATATYPE, m_cbCurDataType);
	DDX_Control(pDX, IDC_COMBO_DATATYPE, m_cbDataType);
	DDX_Control(pDX, IDC_LIST_RNDREADINFO, m_ShowRndReadInfoListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRndRead, CDialog)
	//{{AFX_MSG_MAP(CDlgRndRead)
	ON_BN_CLICKED(IDC_BUTTON_HISTORYDATA, OnButtonHistorydata)
	ON_BN_CLICKED(IDC_BUTTON_CURDATA, OnButtonCurdata)
	ON_BN_CLICKED(IDC_BUTTON_TERMINALTIME, OnButtonTerminaltime)
	ON_BN_CLICKED(IDC_BUTTON_TERMINALVERSION, OnButtonTerminalversion)
	ON_CBN_SELCHANGE(IDC_COMBO_CURDATATYPE, OnSelchangeComboCurdatatype)
	ON_MESSAGE(RNDREADPACKET_EVENT_NOTIFY,OnRndReadPacketEventNotify)
	ON_CBN_SELCHANGE(IDC_COMBO_DATATYPE, OnSelchangeComboDatatype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDlgRndRead message handlers

BOOL CDlgRndRead::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitialDataType();

	InitialCurDataType();

	DWORD dwExtendStyle = m_ShowRndReadInfoListCtrl.GetExtendedStyle();
	dwExtendStyle |= (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ShowRndReadInfoListCtrl.SetExtendedStyle(dwExtendStyle);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRndRead::OnButtonHistorydata() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_cbDataType.GetCurSel();

	if (nCurSel == -1)
	{
		AfxMessageBox("请选择数据类型！");
		return;
	}
	DATA_TYPE *pDataType = (DATA_TYPE*)m_cbDataType.GetItemData(nCurSel);

	CString strParamID =pDataType->strParamID;

	SYSTEMTIME systime,systime2;
	BYTE sTime[6],eTime[6];
	short int outlen;

	m_StartDate.GetTime(&systime);
	m_StartTime.GetTime(&systime2);
	int year=systime.wYear;//2000年之后的数据
	/*
	if(year<0)
		year=1;
	   
	sTime[0] = (BYTE)year;
	sTime[1] = (BYTE)systime.wMonth;
	sTime[2] = (BYTE)systime.wDay;
	
	m_StartTime.GetTime(&systime);
	sTime[3] = (BYTE)systime.wHour;
	sTime[4] = (BYTE)systime.wMinute;
	*/

	CString strBeginTime = _T("");
	strBeginTime.Format("%04d%02d%02d%02d%02d",year,systime.wMonth,systime.wDay,
		systime2.wHour,systime2.wMinute);

	//Convertstr(strBeginTime.GetBuffer(0),strlen(strBeginTime));
	StringtoByte(strBeginTime.GetBuffer(0),(char *)(sTime),&outlen);
	
	m_EndDate.GetTime(&systime);
	m_EndTime.GetTime(&systime2);
	year=systime.wYear;
	/*
	if(year<0)
		year=1;
	  
	eTime[0]=(BYTE)year;
	eTime[1] = (BYTE)systime.wMonth;
	eTime[2] = (BYTE)systime.wDay;
	
	m_EndTime.GetTime(&systime);
	eTime[3] = (BYTE)systime.wHour;
	eTime[4] = (BYTE)systime.wMinute;
	*/

	CString strEndTime = _T("");
	strEndTime.Format("%04d%02d%02d%02d%02d",year,systime.wMonth,systime.wDay,
		systime2.wHour,systime2.wMinute);

	//Convertstr(strEndTime.GetBuffer(0),strlen(strEndTime));
	StringtoByte(strEndTime.GetBuffer(0),(char *)(eTime),&outlen);
		
	
	unsigned int deviceAddr = GlobalFunc::convert_Terminal_StringToAddr("0031010700", 7);

	CPtclModule_NN::DataUint pDataUint;
	int nPnFnCount = 1;
	pDataUint.PnFn.Pn = 0;

	unsigned char chBuff[1024];
	int nDataTimeLen = 0;

	for(int i = 0; i <= 5; i++)
	{
		chBuff[nDataTimeLen] = sTime[i];
		nDataTimeLen++;
	}

	for(int j = 0; j <= 5; j++)
	{
		chBuff[nDataTimeLen] = eTime[j];
		nDataTimeLen++;
	}

	chBuff[nDataTimeLen++] = 0x02;

	string strSign = strParamID;

	GlobalFunc::convert_Sign(strSign,pDataUint.PnFn.Fn,50);
	
	memcpy(pDataUint.pDataBuf, chBuff,nDataTimeLen);
	pDataUint.lenDataBuf = nDataTimeLen;
	
	CPtclPacket_NN packet_NN;
	packet_NN.setDeviceAddr(deviceAddr);
	CPtclModule_NN::getInstance()->FormatPacket_ReadData_History(&packet_NN, &pDataUint, nPnFnCount);

	FRAME_BUF fb;
	fb.len = packet_NN.getPacketBufLen();
	memcpy(fb.buf, packet_NN.getPacketBuf(), packet_NN.getPacketBufLen());
	CHandlerMgr::GetInstance()->TransferToMaster(fb);
	
}

void CDlgRndRead::InitialDataType()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	char strSQL[300] = {0};
	int nRow = 0;
	
	sprintf(strSQL,"SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 2 OR ID = 3 ");
	
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
		
		m_cbDataType.InsertString(nRow, strParamName);

		DATA_TYPE *pDataType = new DATA_TYPE;
		pDataType->strParamID = strParamID;

		m_cbDataType.SetItemData(nRow, (DWORD)pDataType);
		
		record.RecordMoveNext();
		nRow++;
	}	
}

void CDlgRndRead::InitialCurDataType()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	
	char strSQL[300] = {0};
	int nRow = 0;
	
	sprintf(strSQL,"SELECT * FROM ALL_PARAMETER_INFO WHERE ID = 1 ");
	
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
		
		m_cbCurDataType.InsertString(nRow, strParamName);
		
		DATA_TYPE *pDataType = new DATA_TYPE;
		pDataType->strParamID = strParamID;
		
		m_cbCurDataType.SetItemData(nRow, (DWORD)pDataType);
		
		record.RecordMoveNext();
		nRow++;
	}
}

void CDlgRndRead::OnButtonCurdata() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_cbCurDataType.GetCurSel();

	if (nCurSel == -1)
	{
		AfxMessageBox("请选择数据类型！");
		return;
	}
	DATA_TYPE *pDataType = (DATA_TYPE*)m_cbCurDataType.GetItemData(nCurSel);

	CString strParamID = pDataType->strParamID;

	unsigned int deviceAddr = GlobalFunc::convert_Terminal_StringToAddr("0031010700", 7);

	CPtclModule_NN::FormatPnFnData vPnFn[20];
	int nPnFnCount = 1;

	string strSign = strParamID;

	vPnFn[0].Pn = 0;

	GlobalFunc::convert_Sign(strSign,vPnFn[0].Fn,50);
	
	CPtclPacket_NN packet_NN;
	packet_NN.setDeviceAddr(deviceAddr);
	CPtclModule_NN::getInstance()->FormatPacket_ReadData_RealTime(&packet_NN,vPnFn,nPnFnCount);

	FRAME_BUF fb;
	fb.len = packet_NN.getPacketBufLen();

	memcpy(fb.buf, packet_NN.getPacketBuf(), packet_NN.getPacketBufLen());

	CHandlerMgr::GetInstance()->TransferToMaster(fb);
}


LRESULT CDlgRndRead::OnRndReadPacketEventNotify(WPARAM wParam,LPARAM lParam)
{
	FRAME_BUF *pFb=(FRAME_BUF*)lParam;

	CPtclPacket_NN packet_nn;
	packet_nn.setPacketBuf(pFb->buf,pFb->len);
	if (packet_nn.parsePacket() <= 0) ;
	
	unsigned char AFN = packet_nn.getAFN();

	unsigned char pDataBuf[1024];
	int nDataLen = 1024;
	packet_nn.getDataAreaBuf(pDataBuf, nDataLen);
	
	if (AFN == 0x0A) 
	{
		unsigned char pTerminalTime[4] = {0x30, 0x01, 0x00, 0xE0}; //终端时间

		if (memcmp(pDataBuf+2,pTerminalTime, 4) == 0)
		{
			ParseTerminalTime((char*)pDataBuf, nDataLen);
		}
	}
	else
	{
		unsigned char pTerminalVersion[4] = {0x02, 0x0B, 0x00, 0xE0}; //终端硬件版本号

		if (memcmp(pDataBuf+2,pTerminalVersion, 4) == 0)
		{
			ParseTerminalVersion((char*)pDataBuf, nDataLen);
		}
		else
		{
			unsigned char outbuf[1024]={0}, svalue[512];
			int outlen;
			ParseRetFrame(pFb->buf,pFb->len,outbuf,&outlen);
			
			LPUPDATA pUPDATA=LPUPDATA(outbuf+sizeof(UPDATAHEAD));
			sprintf((char*)svalue,"%s",pUPDATA->sdata);

			CString strPn =_T(""), strDataMark=_T("");
			strPn.Format("%02x", (char*)pUPDATA->pn);
			strDataMark.Format("%ld", pUPDATA->datamark);

			m_ShowRndReadInfoListCtrl.DeleteAllItems();

			m_ShowRndReadInfoListCtrl.InsertItem(0, "07310001");
			m_ShowRndReadInfoListCtrl.SetItemText(0, 1, "1");
			m_ShowRndReadInfoListCtrl.SetItemText(0, 2, strDataMark);
			m_ShowRndReadInfoListCtrl.SetItemText(0, 3, (char*)svalue);
		}
	}
	
	//delete pInfo;

	return 0;
}

void CDlgRndRead::OnButtonTerminaltime() 
{
	// TODO: Add your control notification handler code here
	CString strParamID = _T("E0000130");
	
	unsigned int deviceAddr = GlobalFunc::convert_Terminal_StringToAddr("0031010700", 7);
	
	CPtclModule_NN::FormatPnFnData vPnFn[20];
	int nPnFnCount = 1;
	
	string strSign = strParamID;
	
	vPnFn[0].Pn = 0;
	
	GlobalFunc::convert_Sign(strSign,vPnFn[0].Fn,50);
	
	CPtclPacket_NN packet_NN;
	packet_NN.setDeviceAddr(deviceAddr);
	CPtclModule_NN::getInstance()->FormatPacket_ReadParam(&packet_NN,vPnFn,nPnFnCount);
	
	FRAME_BUF fb;
	fb.len = packet_NN.getPacketBufLen();
	
	memcpy(fb.buf, packet_NN.getPacketBuf(), packet_NN.getPacketBufLen());
	CHandlerMgr::GetInstance()->TransferToMaster(fb);
}

void CDlgRndRead::ParseTerminalTime(char *pDataBuff, int nDataLen)
{

	DWORD dwYear,dwMon,dwDay,dwHour,dwMin,dwSecond;

	dwSecond = (pDataBuff[6]/16)*10+pDataBuff[6]%16;
	dwMin = (pDataBuff[7]/16)*10+pDataBuff[7]%16;
	dwHour = (pDataBuff[8]/16)*10+pDataBuff[8]%16;
	dwDay = (pDataBuff[9]/16)*10+pDataBuff[9]%16;
	dwMon = (pDataBuff[10]/16)*10+pDataBuff[10]%16;
	dwYear = 2000+(pDataBuff[11]/16)*10+pDataBuff[11]%16;
	
	CString m_strTerminalTime = _T(""), strSystemTime = _T("");
	m_strTerminalTime.Format("%4d-%02d-%02d %02d:%02d:%02d",
		dwYear,dwMon,dwDay,dwHour,dwMin,dwSecond);
	
	SYSTEMTIME time;
	::GetLocalTime(&time);
	strSystemTime.Format("%d-%02d-%02d %02d:%02d:%02d", 
		time.wYear,time.wMonth,time.wDay, 
		time.wHour,time.wMinute,time.wSecond);
	
	SetDlgItemText(IDC_EDIT_TERMINALTIME, m_strTerminalTime);
	SetDlgItemText(IDC_EDIT_SYSTEMTIME, strSystemTime);
}

void CDlgRndRead::ParseTerminalVersion(char *pDataBuff, int nDataLen)
{
	CString strTerminalVersion = _T("");
	strTerminalVersion.Format("%02x%02x", pDataBuff[7], pDataBuff[6]);
	SetDlgItemText(IDC_EDIT_VERSION, strTerminalVersion);
}

//将字符串转换为二进制流
int CDlgRndRead::StringtoByte(char *instr,char *outstr,short int *outlen)
{
	int iret=1,i,ilen;
	ilen = strlen(instr);
	if(ilen % 2 !=0) return -1;//字符个数为奇数
	for(i=0;i<int(ilen/2);i++)
	{
		*(outstr+i) = HextoValue(*(instr+2*i),*(instr+2*i+1));
	}
	*outlen = int(strlen(instr)/2);
	return 1;
}

int CDlgRndRead::Convertstr(char *str,int ilen)
{
	char tmp[3];
	int i; 
	if(ilen % 2 ==1)return -1;
	for(i=0;i<(int)(ilen/4);i++)
	{
		tmp[0] = *(str+2*i);
		tmp[1] = *(str+2*i+1);
		*(str+2*i) = *(str+ilen-2*i-2);
		*(str+2*i+1) = *(str+ilen-2*i-1);
		*(str+ilen-2*i-2) = tmp[0];
		*(str+ilen-2*i-1) = tmp[1];
	}
	return 1;
}

//将2个字符转换为一个十六进制数
int   CDlgRndRead::HextoValue(char ch1,char ch2)
{
	int tint1,tint2;
	tint1=chrtobyte(ch1);
	tint2=chrtobyte(ch2);
	if((tint1>=0)&&(tint2>=0))
		return (tint1 << 4) + tint2;
	else
		return -1;
}
//字符转换为数字
int CDlgRndRead::chrtobyte(char ch)
{
	if (ch>='0' && ch<='9')
	{
		return ch-'0';
	}
	else if (ch>='a' && ch<='z')
	{
		return ch-'a'+10;
	}
	else if (ch>='A' && ch<='Z')
	{
		return ch-'A'+10;
	}
	else
		return -1;
	
}



void CDlgRndRead::OnButtonTerminalversion() 
{
	// TODO: Add your control notification handler code here
	/*
	CString strParamID = _T("E0000B02");
	
	unsigned int deviceAddr = GlobalFunc::convert_Terminal_StringToAddr("0031010700", 7);
	
	CPtclModule_NN::FormatPnFnData vPnFn[20];
	int nPnFnCount = 1;
	
	string strSign = strParamID;
	
	vPnFn[0].Pn = 0;
	
	GlobalFunc::convert_Sign(strSign,vPnFn[0].Fn,50);
	
	CPtclPacket_NN packet_NN;
	packet_NN.setDeviceAddr(deviceAddr);
	CPtclModule_NN::getInstance()->FormatPacket_ReadParam(&packet_NN,vPnFn,nPnFnCount);
	*/

	CString strParamID =  _T("E0000B02");
	
	unsigned int deviceAddr = GlobalFunc::convert_Terminal_StringToAddr("0031010700", 7);
	
	CPtclModule_NN::FormatPnFnData vPnFn[1];
	int nPnFnCount = 1;
	
	string strSign = strParamID;
	
	vPnFn[0].Pn = 0;
	
	GlobalFunc::convert_Sign(strSign,vPnFn[0].Fn,50);
	
	CPtclPacket_NN packet_NN;
	packet_NN.setDeviceAddr(deviceAddr);
	CPtclModule_NN::getInstance()->FormatPacket_ReadData_RealTime(&packet_NN,vPnFn,nPnFnCount);
	
	FRAME_BUF fb;
	fb.len = packet_NN.getPacketBufLen();
	
	memcpy(fb.buf, packet_NN.getPacketBuf(), packet_NN.getPacketBufLen());
	
	CHandlerMgr::GetInstance()->TransferToMaster(fb);
}

void CDlgRndRead::OnSelchangeComboCurdatatype() 
{
	// TODO: Add your control notification handler code here

	m_ShowRndReadInfoListCtrl.DeleteAllItems();
	
	while(!(m_ShowRndReadInfoListCtrl.DeleteColumn(0))==0)
	{}

	m_ShowRndReadInfoListCtrl.InsertColumn(0,"终端地址",LVCFMT_CENTER,100);
	m_ShowRndReadInfoListCtrl.InsertColumn(1,"测量点",LVCFMT_CENTER,120);
	m_ShowRndReadInfoListCtrl.InsertColumn(2,"数据标识",LVCFMT_CENTER,120);
	m_ShowRndReadInfoListCtrl.InsertColumn(3,"数值",LVCFMT_CENTER,140);
}

void CDlgRndRead::OnSelchangeComboDatatype() 
{
	// TODO: Add your control notification handler code here
	m_ShowRndReadInfoListCtrl.DeleteAllItems();

	while(!(m_ShowRndReadInfoListCtrl.DeleteColumn(0))==0)
	{}

	m_ShowRndReadInfoListCtrl.InsertColumn(0,"终端地址",LVCFMT_CENTER,100);
	m_ShowRndReadInfoListCtrl.InsertColumn(1,"测量点",LVCFMT_CENTER,120);
	m_ShowRndReadInfoListCtrl.InsertColumn(2,"数据标识",LVCFMT_CENTER,120);
	m_ShowRndReadInfoListCtrl.InsertColumn(3,"时间",LVCFMT_CENTER,100);
	m_ShowRndReadInfoListCtrl.InsertColumn(4,"数值",LVCFMT_CENTER,140);
	
}
