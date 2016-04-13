// ComModem.cpp: implementation of the CCom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Com.h"
#include "Def_Msg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CComWnd * pComWnd = NULL;
int InitCom()
{
	if(pComWnd != NULL)
		return 1;
	pComWnd = new CComWnd();
	BOOL result = pComWnd->CreateEx(WS_EX_OVERLAPPEDWINDOW,
		"EDIT","",
		WS_CHILD | WS_VISIBLE 
		,CRect(10,10,1,1),AfxGetApp()->GetMainWnd(),199999);
	return result;
}

DWORD WINAPI ThreadWatchComm(LPVOID pParm);

DWORD WINAPI ThreadWatchComm(LPVOID pParm)
{
	CCom * pCom = (CCom*)pParm;
	pCom->WatchComProc();
	ExitThread(0);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CComWnd

CComWnd::CComWnd()
{
}

CComWnd::~CComWnd()
{
}


BEGIN_MESSAGE_MAP(CComWnd, CWnd)
	//{{AFX_MSG_MAP(CComWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_COM_EVENT_INFO,OnMsgComEvent)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComWnd message handlers
/*
void CComWnd::OnMsgComEvent(WPARAM wParam, LPARAM lParam)
{
	CCom *pCom;
	DWORD event;
	pCom = (CCom*)wParam;
	event = (DWORD)lParam;
	pCom->ProcessMsgComEvent(event);
}
*/

int CComWnd::PostMessageComEvent(CCom *pCom, DWORD event)
{
	PostMessage(WM_COM_EVENT_INFO,(WPARAM)pCom,(LPARAM)event);
//	SendMessage(WM_COM_EVENT_INFO,(WPARAM)pCom,(LPARAM)event);

	return 0;
}

//DEL CCom::CCom(char * sComName,WORD baudRate,BYTE stopBits,BYTE parity,BYTE byteSize)
//DEL {
//DEL 	InitComInfo(sComName, baudRate, stopBits, parity, byteSize);
//DEL }

CCom::CCom()
{

}

CCom::~CCom()
{

}

int CCom::Open()
{	
	m_reqthreadexitcount = 0;
	int comMask,comBuf,comState;
	COMMTIMEOUTS  CommTimeOuts;
	DCB dcb;
	m_sComName = "\\\\.\\" + m_sComName;
	hComDev = CreateFile(m_sComName,
						GENERIC_READ|GENERIC_WRITE,
						0,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
						NULL);
	if(hComDev == (HANDLE)-1)
	{
		hComDev = NULL;
		OnError(GetLastError());
		return 0;
	}
	IsOpen = TRUE;
	DWORD dwTemp;

	dwTemp = 0;
	CommTimeOuts.ReadIntervalTimeout = 1000;
//	CommTimeOuts.ReadTotalTimeoutMultiplier = ((dwTemp > 0) ? dwTemp : 1);
	CommTimeOuts.ReadTotalTimeoutMultiplier =1;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000;
	if(m_BaudRate == 0)
		CommTimeOuts.WriteTotalTimeoutMultiplier = 1;
	else
		CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_9600/m_BaudRate;//( npTTYInfo ) ;
	CommTimeOuts.WriteTotalTimeoutConstant = 2000;//1000;
	SetCommTimeouts( hComDev, &CommTimeOuts);

	SetCommMask(hComDev, EV_RXCHAR);
    
	PurgeComm( hComDev, PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	comMask = SetCommMask(hComDev,EV_RXFLAG);
	comBuf = SetupComm(hComDev,4096,4096);
	comState = GetCommState(hComDev,&dcb);

	dcb.BaudRate = m_BaudRate;//CBR_19200
	dcb.ByteSize = m_ByteSize;//8

	dcb.Parity = m_Parity;
	dcb.StopBits = m_StopBits;
/*
	switch(m_Parity)
	{
	case 0:
		dcb.Parity = NOPARITY;//NOPARITY;
		break;
	case 1:
		dcb.Parity = ODDPARITY;
		break;
	case 2:
		dcb.Parity = EVENPARITY;
		break;
	}	
	switch(m_StopBits)
	{
	case 1:
		dcb.StopBits = ONESTOPBIT;
		break;
	case 2:
		dcb.StopBits = TWOSTOPBITS;
		break;
	default:
		dcb.StopBits = 0;
		break;
	}
*/
	dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
	dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	
	dcb.fBinary = 1;
	dcb.fParity = 1;

	comState = SetCommState(hComDev,&dcb);
	InitEvent();

	InitThread();//启动线程控制重叠操作
	return 1;
}


void CCom::Close()
{	
	if(hComDev)
	{
		ThreadExit = TRUE;
		bConnected = FALSE;
		CloseEvent();
		CloseHandle(hComDev);
		hComDev = NULL;
		IsOpen = FALSE;
		CloseThread();	
	}	
}

int CCom::InitEvent()
{
	m_hResponseEvent=CreateEvent(NULL,TRUE,TRUE,NULL);
	TRACE("m_hResponseEvent=CreateEvent(NULL,TRUE,TRUE,NULL)\n");
	
	memset(&read_os,0,sizeof(OVERLAPPED));
	read_os.hEvent = CreateEvent(NULL,TRUE,TRUE,NULL);
	
	memset(&write_os,0,sizeof(OVERLAPPED));
	write_os.hEvent = CreateEvent(NULL,TRUE,TRUE,NULL);

	memset(&os,0,sizeof(OVERLAPPED));
	os.hEvent = CreateEvent(NULL,TRUE,TRUE,NULL);
	return 1;
}


int CCom::InitThread()
{
	ThreadExit = FALSE;
	hThreadWatchComm = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadWatchComm,(LPVOID)this,0,&Thread_TimeOut_id);
	return 1;
}

int CCom::CloseEvent()
{
	if(read_os.hEvent)
	{
		CloseHandle(read_os.hEvent);
		read_os.hEvent = NULL;
	}
	if(write_os.hEvent)
	{
		CloseHandle(write_os.hEvent);
		write_os.hEvent = NULL;
	}
	if(os.hEvent)
	{
		CloseHandle(os.hEvent);
		os.hEvent = NULL;
	}
	if(m_hResponseEvent)
	{
		CloseHandle(m_hResponseEvent);
		m_hResponseEvent = NULL;
	}
	TRACE("CloseHandle(m_hResponseEvent)\n");

	return 0;
}

int CCom::CloseThread()
{
	DWORD ExitCode = 1;
	if(hThreadWatchComm)
	{	
		ThreadExit = 1;

		if(::WaitForSingleObject(hThreadWatchComm,500)==WAIT_TIMEOUT)
		{
			::TerminateThread(hThreadWatchComm,ExitCode);
		}
		CloseHandle(hThreadWatchComm);
		hThreadWatchComm = NULL;
		return 1;
	}
	return 1;
}




int CCom::OnError(DWORD err)
{
	DWORD err1;
	COMSTAT stat;
	ClearCommError(hComDev,&err1,&stat);

	return 0;
}


DWORD CCom::Write(BYTE *buff, DWORD bufflen,BOOL bOver)
{
	DWORD lenreturn;
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	if(!IsOpen)
		return 0;
	PurgeComm(hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
//	TRACE("Write Start:%d\n",bufflen);
	BOOL fState = WriteFile(hComDev,buff,bufflen,&lenreturn,&write_os);
	if(!fState)
	{
		if(GetLastError()==ERROR_IO_PENDING)
		{
			if(bOver)
			{
				while(!GetOverlappedResult(hComDev,&write_os,&lenreturn,TRUE)) 
				{ 
					if(GetLastError() == ERROR_IO_INCOMPLETE)
						continue; 
					else 
					{ 
						ClearCommError(hComDev,&dwErrorFlags,&ComStat);
						break; 
					} 
				}
			}
//			TRACE("write_os E\n");
		}
		else
		{
			lenreturn = -1;
			ClearCommError(hComDev,&dwErrorFlags,&ComStat);
		}
	}
//	else
//	{
//	Sleep(100);
//	}
//	PurgeComm( hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
//	TRACE("Write End:%d\n",lenreturn);
	return lenreturn;
}


DWORD CCom::Read(BYTE *pBuff, DWORD NumRead)
{
	DWORD numByte;
 	COMSTAT  ComStat ;
 	DWORD dwErrorFlags;
	DWORD len;
	BOOL fReadStat;
	if(!IsOpen)
		return 0;
	if(NumRead<=0)
		return 0;
	if(!ClearCommError(hComDev, &dwErrorFlags, &ComStat))
		return 0;
//  if(NumRead<ComStat.cbInQue)
	len = NumRead;
//	else
//		len = ComStat.cbInQue;

/*
	if(fReadStat = ReadFile(hComDev,pBuff,len,&numByte,NULL))
		return numByte;	
	return 0;
*/

	fReadStat = ReadFile(hComDev,pBuff,len,&numByte,&read_os);
	if (!fReadStat) 
	{
		if (GetLastError()==ERROR_IO_PENDING) 
		{ 
			while(!GetOverlappedResult(hComDev,&read_os,&numByte,TRUE)) 
			{ 
				if(GetLastError() == ERROR_IO_INCOMPLETE)
					continue; 
			} 
		} 
		else 
		{ 
			numByte=0; 
			ClearCommError(hComDev,&dwErrorFlags,&ComStat);
		} 
	}
	return numByte;
}
int CCom::ClearBuffError()
{
	DWORD len;
	COMSTAT comstat;
	PurgeComm(hComDev,PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);
	ClearCommError(hComDev,&len,&comstat);

	return 0;
}


void CCom::WatchComProc()
{
	DWORD dwTransfer,dwEvtMask;

	COMSTAT stat;
	
	DWORD err;
	DWORD mask;
	GetCommMask(mask);
	if(!SetCommMask(hComDev,mask)) //|EV_ERR
		return ;
	while(!ThreadExit)
	{
		dwEvtMask = 0;
		dwTransfer = 0;
		if(!WaitCommEvent(hComDev,&dwEvtMask,&os))
		{
			err = GetLastError();
			if(ERROR_IO_PENDING == err)				
			{
				GetOverlappedResult(hComDev,&os,&dwTransfer,TRUE);
			}
			else
			{
				ClearCommError(hComDev,&err,&stat);
				Sleep(100);
				continue;
			}
		}
		ProcessComEventInThread(dwEvtMask);
	}
}

BOOL CCom::Connect()
{
	return 0;
}

void CCom::OnReceive()
{

}

CString CCom::GetLastErrorInfo()
{
	return m_errorInfo;
}

void CCom::ProcessMsgComEvent(DWORD eventid)
{
	SetResponseEvent();
}

void CCom::SetResponseEvent()
{
	SetEvent(m_hResponseEvent);
	TRACE("SetEvent\n");
}

void CCom::ResetResponseEvent()
{
	ResetEvent(m_hResponseEvent);
	TRACE("ResetEvent\n");
}

void CCom::WaitResponseEvent()
{
	TRACE("WaitForSingleObject Start\n");
	WaitForSingleObject(m_hResponseEvent, INFINITE);
	TRACE("WaitForSingleObject End\n");
}

void CCom::InitComInfo(char * sComName,WORD baudRate,BYTE stopBits,BYTE parity,BYTE byteSize)
{
	InitCom();
    
	m_BaudRate=baudRate;
	m_StopBits=stopBits;
	m_Parity=parity;
	m_ByteSize=byteSize;
	m_sComName=sComName;

	m_reqthreadexitcount = 0;
	ThreadExit = TRUE;

	IsOpen = FALSE;
	bConnected = FALSE;

	read_os.hEvent  = NULL;

	write_os.hEvent = NULL;
	os.hEvent = NULL;
	hThreadWatchComm = NULL;
}

void CCom::PostMsgComEvent(int event)
{
	pComWnd->PostMessageComEvent(this,event); 
//	ProcessMsgComEvent(event);
}

void CCom::ProcessComEventInThread(DWORD dwEvtMask)
{
	WaitResponseEvent();
	ResetResponseEvent();
} 

void CCom::GetCommMask(DWORD & mask)
{

}
