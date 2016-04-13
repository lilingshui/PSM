// RS232.cpp: implementation of the CRS232 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RS232.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRS232::~CRS232()
{

}
void CRS232::OnReceive()
{	
	CCom::OnReceive();	
}

BOOL CRS232::Connect()
{
	bConnected=FALSE;
	if(Open())
		bConnected=TRUE;
	else
		m_errorInfo = "打开串口失败!";
	return bConnected;
}

CRS232::CRS232()
{

}

void CRS232::Close()
{
	CCom::Close();
}

void CRS232::ProcessMsgComEvent(DWORD eventid)
{
	switch(eventid)
	{
	case COM_EVT_RECV:
		OnReceive();
		break;
	}
	CCom::ProcessMsgComEvent(eventid);
}

void CRS232::ProcessComEventInThread(DWORD dwEvtMask)
{
	CCom::ProcessComEventInThread(dwEvtMask);
	
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL isSendMsg = FALSE;
	if((dwEvtMask & EV_RXCHAR)==EV_RXCHAR)//收到字符
	{
		ClearCommError(hComDev,&dwErrorFlags,&ComStat);
		if(ComStat.cbInQue > 0)
		{
		PostMsgComEvent(COM_EVT_RECV);

			isSendMsg = TRUE;
			TRACE("PostMsgComEvent\n");
			Sleep(50);
		}
	}
	if(!isSendMsg)
		SetResponseEvent();
}

void CRS232::GetCommMask(DWORD &mask)
{
	mask = EV_RXCHAR;
}
