// ModemCom.cpp: implementation of the CModem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Modem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include <mcx.h>
CModem::CModem()
{
	InitComInfo("COM1",9600,1,0,8);	
}

//DEL CModem::CModem(char * sComName,WORD baudRate,BYTE stopBits,BYTE parity,BYTE byteSize,
//DEL 			   BYTE dialMode,char *initStr,char *szCall):
//DEL 	CCom(sComName, baudRate, stopBits, parity, byteSize)
//DEL {
//DEL //	CCom::InitComInfo(sComName, baudRate, stopBits, parity, byteSize);
//DEL 
//DEL 	m_dialMode=dialMode;
//DEL 	m_initStr=initStr;
//DEL 	m_sCallTel=szCall;
//DEL }

CModem::~CModem()
{

}

//DEL void CModem::Close()
//DEL {
//DEL 	Write((BYTE *)"+++ATH\r",strlen("+++ATH\r"));
//DEL 	Sleep(500);
//DEL 	CCom::Close();
//DEL }

BOOL CModem::InitModem()
{
	m_connect_step = CONNECT_STEP_INIT_MODEM;
	CString temp;
	bConnected=FALSE;
	temp.Format("%s\r",m_initStr); 
	temp.MakeUpper(); 
	Write((BYTE*)temp.GetBuffer(temp.GetLength()),temp.GetLength());
	return TRUE;
}

//DEL BOOL CModem::Wait_For_Response()
//DEL {
//DEL 	BOOL ret=FALSE;
//DEL 	DWORD nRet=WaitForSingleObject(m_hResponseEvent, 10000);
//DEL 	if(nRet!=WAIT_OBJECT_0)
//DEL 	{
//DEL 		m_errorInfo="初始化Modem没有响应";
//DEL 		ResetEvent(m_hResponseEvent);
//DEL 		return FALSE;
//DEL 	}
//DEL 	m_replayInfo.MakeUpper();
//DEL 	if(m_replayInfo.Find("OK") >= 0)
//DEL 		ret=TRUE;
//DEL 	else
//DEL 		m_errorInfo="初始化Modem响应错误:"+m_replayInfo;
//DEL 	m_replayInfo.Empty();
//DEL 	ResetEvent(m_hResponseEvent);
//DEL 	return ret;
//DEL }

BOOL CModem::Dial()
{
	m_connect_step = CONNECT_STEP_DIALING;
	CString tel;
	bConnected=FALSE;
	if(m_dialMode != 1) 
		tel.Format("ATDT%s\r",m_sCallTel);
	else
		tel.Format("ATDP%s\r",m_sCallTel);
	Write((BYTE*)tel.GetBuffer(tel.GetLength()),tel.GetLength());
//	Sleep(5000);
//	return Wait_For_Connect();
	return TRUE;
}

//DEL BOOL CModem::Wait_For_Connect()
//DEL {
//DEL 	BOOL ret=FALSE;
//DEL 	DWORD nRet=WaitForSingleObject(m_hResponseEvent, 50000);
//DEL 	if(nRet!=WAIT_OBJECT_0)
//DEL 	{
//DEL 		m_errorInfo="Modem拨号没有响应";
//DEL 		ResetEvent(m_hResponseEvent);
//DEL 		return FALSE;
//DEL 	}
//DEL 	m_replayInfo.MakeUpper();
//DEL 	if(m_replayInfo.Find("CONNECT") >= 0)
//DEL 		ret=TRUE;
//DEL 	else
//DEL 		m_errorInfo="Modem拨号响应错误:"+m_replayInfo;
//DEL 	m_replayInfo.Empty();
//DEL 	ResetEvent(m_hResponseEvent);
//DEL 	return ret;
//DEL 
//DEL }

BOOL CModem::Connect()
{
	if(!Open())
	{
		Close();
		return FALSE;
	}
    Sleep(50);
	if(m_initStr.GetLength()>0)
	{
	if(!InitModem())
	{
		Close();
		return FALSE;
	}
	}

	if(!Dial())
	{
		Close();
		return FALSE;
	}
    bConnected=TRUE;
	return TRUE;
}

void CModem::ProcessData()
{
	switch(m_connect_step)
	{
	case CONNECT_STEP_INIT_MODEM:
	case CONNECT_STEP_DIALING:
	case CONNECT_STEP_TURNING_ORDER:
	case CONNECT_STEP_HUNGINGUP:
		JudgeResult();
		break;
	case CONNECT_STEP_DIAL_SUCCESS:
		OnReceive();
		break;
	}	
}

void CModem::OnResponse()
{
	BYTE buf[100];
	CString temp;
	memset(buf,0,100);
	Read(buf,100);
	temp=buf;
	m_replayInfo += temp;

	if(m_replayInfo.Find('\r',0)==0) 
	
		m_replayInfo.TrimLeft('\r');
	m_replayInfo.MakeUpper();
	if((m_replayInfo.Find("OK",0)>0)||(m_replayInfo.Find("CONNECT",0)>0))
		SetEvent(m_hResponseEvent);
	
}

void CModem::ProcessComEventInThread(DWORD dwEvtMask)
{
	CCom::ProcessComEventInThread(dwEvtMask);
	
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL isSendMsg = FALSE;

	if((dwEvtMask & EV_RXCHAR)==EV_RXCHAR)//收到字符
	{
		Sleep(50);
		ClearCommError(hComDev,&dwErrorFlags,&ComStat);
		if(ComStat.cbInQue > 0)
		{
			PostMsgComEvent(COM_EVT_RECV);
			isSendMsg = TRUE;
		}	
	}

	if((dwEvtMask & EV_TXEMPTY) == EV_TXEMPTY)//发送字符
	{
		isSendMsg = TRUE;
		PostMsgComEvent(COM_EVT_SEND);
	}

	if((dwEvtMask & EV_RLSD) == EV_RLSD)//DCD信号改变状态
	{
		isSendMsg = TRUE;
		PostMsgComEvent(COM_EVT_RLSD);
	}

	if((dwEvtMask & EV_RING) == EV_RING)//线路错误
	{
		isSendMsg = TRUE;
		PostMsgComEvent(EV_RING);
	}

	if(!isSendMsg)
		SetResponseEvent();
}

void CModem::GetCommMask(DWORD &mask)
{
	mask = EV_RXCHAR|EV_TXEMPTY|EV_ERR|EV_RLSD|EV_RING;
}

void CModem::ProcessMsgComEvent(DWORD eventid)
{
	switch(eventid)
	{
	case COM_EVT_RECV:
		ProcessData();
		break;
	case COM_EVT_SEND:
		break;
	case COM_EVT_RLSD:
		SeekConnect();
		break;
	case COM_EVT_RING:
		break;
	}
	CCom::ProcessMsgComEvent(eventid);	
}

int CModem::JudgeResult()
{
	Sleep(200);
	BYTE buf[100];
	CString sresult;
	short v = -1;
	memset(buf,0,100);
	Read(buf,100);
	sresult = buf;
//	short l;

#ifdef _DEBUG
//	TRACE("%d,%s\n",m_dialsuccess,sresult);
#endif

/*	
	if(sresult.Find('\r',0)==0) 
	{
		v = 1;
	}
	else
	{
		l = sresult.Find('\r',0);
		if(l>0)
		{
			sresult = sresult.Left(l); 
			v = 0;
		}		 
	}
*/
	sresult.MakeUpper();

	if(sresult.Find("CONNEC")>=0)
	{					
		ProcessResult(1,sresult,0);
		return 0;
	}
	else
	{
		if(sresult.Find("NO CARRIER")>=0)
		{
			ProcessResult(0,sresult,CONN_RESULT_NOCARRIER);
			return 0;
		}
		else
		{
			if(sresult.Find("OK") >= 0)
			{
				ProcessResult(1,sresult,0);
				return 0;
			}
			else
			{
				if(sresult.Find("NO DIALTONE")>=0)
				{
					ProcessResult(0,sresult,CONN_RESULT_NODIALONE);
					return 0;
				}
				else
				{
					if(sresult.Find("BUSY")>=0)
					{
						ProcessResult(0,sresult,CONN_RESULT_BUSY);
						return 0;
					}
					else
					{
						if(sresult.Find("NO ANSWER")>=0)
						{
							ProcessResult(0,sresult,CONN_RESULT_NOANSWER);
							return 0;
						}
					}
				}
			}		
		}
		if(v == 0)
		{
			if(sresult == "1" || sresult == "5" || sresult == "9" || sresult == "10" || 
			   sresult == "11" || sresult == "12" || sresult == "13" || sresult == "14" || 
			   sresult == "15" || sresult == "16" || sresult == "17" || sresult == "18" || 
			   sresult == "19" || sresult == "20" || sresult == "22" || sresult == "23" || 
			   sresult == "40" || sresult == "44" || sresult == "45" || sresult == "46" || 
			   sresult == "47" || sresult == "48" || sresult == "49" || sresult == "50" || 
			   sresult == "51" || sresult == "52" || sresult == "53" || sresult == "54" || 
			   sresult == "55" || sresult == "56" || sresult == "57" || sresult == "58" || 				   
			   
			   sresult == "59" || sresult == "61" || sresult == "62" || sresult == "63" || 
			   sresult == "64" || sresult == "78" || sresult == "79" || sresult == "84" || 
			   sresult == "91" || sresult == "150" || sresult == "151" || sresult == "152" || 
			   sresult == "153" || sresult == "154" || sresult == "155" || sresult == "156" || 
			   sresult == "157" || sresult == "158" || sresult == "159" || sresult == "160" || 
			   sresult == "161" || sresult == "162" ||
			   sresult == "166" || sresult == "167" || sresult == "168" || sresult == "169" || 
			   sresult == "170" || sresult == "171" || sresult == "172" || sresult == "173" || 
			   sresult == "174" || sresult == "175" || sresult == "176" || sresult == "177" ||
			   sresult == "178" || sresult == "180" || sresult == "181" || sresult == "182" ||
			   sresult == "183" || sresult == "184" || sresult == "185" || sresult == "186" ||
			   sresult == "187" || sresult == "188" || sresult == "189" || sresult == "190" ||
			   sresult == "191" || sresult == "192" || sresult == "193" || sresult == "194" ||
			   
			   sresult == "195" || sresult == "196" || sresult == "197" || sresult == "198" || 
			   sresult == "199" || sresult == "200" || sresult == "201" || sresult == "202" || 
			   sresult == "203" || sresult == "204" || sresult == "205" || sresult == "206" || 
			   sresult == "207" || sresult == "208" || sresult == "209" )
			{						
				ProcessResult(1,sresult,0);						
				return 0;
			}
			else
			{
				if(sresult == "3")
				{
					ProcessResult(0,"NO CARRIER",CONN_RESULT_NOCARRIER);
					return 0;
				}
				else				
				{
					if(sresult == "6")
					{
						ProcessResult(0,"NO DIALTONE",CONN_RESULT_NODIALONE);
						return 0;
					}
					else
					{
						if(sresult == "7")
						{
							ProcessResult(0,"BUSY",CONN_RESULT_BUSY);
							return 0;
						}
						else
						{
							if(sresult == "8")
							{
								ProcessResult(0,"NO ANSWER",CONN_RESULT_NOANSWER);
								return 0;
							}
							else
							{
								if(sresult == "0")
								{
									ProcessResult(1,"OK",0);
									return 0;
								}
								else
								{
									ProcessResult(0,"ERROR",CONN_RESULT_ERROR);
									return 0;
								}
							}
						}
					}
				}
			}
		}
	}
	return 1;
}

int CModem::ProcessResult(DWORD connstate, CString sResult, BYTE nResult)
{
	switch(m_connect_step)
	{
	case CONNECT_STEP_INIT_MODEM:            //初始化MODEM	
	case DIAL_METHOD_TONE:
		OnInitResult(connstate,sResult);
		break;
	case CONNECT_STEP_DIALING:            //正在连接
		if(connstate == 1)
			m_connect_step = CONNECT_STEP_DIAL_SUCCESS;
		else
			m_connect_step = CONNECT_STEP_DIAL_FAIL;

		if(sResult.Find("OK") < 0)
			OnConnecting(connstate,sResult,nResult);
		else
		{
			OnConnecting(0,"ERROR",nResult);
			m_connect_step = CONNECT_STEP_DIAL_FAIL;
		}
		break;
	}
	return 0;
}

int CModem::OnInitResult(DWORD result, CString sResult)
{
	return 0;
}

int CModem::OnConnecting(DWORD connstate, CString sResult, BYTE nResult)
{
	return 0;
}

void CModem::InitModemInfo(char * sComName,WORD baudRate,BYTE stopBits,BYTE parity,BYTE byteSize,BYTE dialMode,char *initStr,char *szCall)
{
	InitComInfo(sComName,baudRate,stopBits,parity,byteSize);
	m_dialMode=dialMode;
 	m_initStr=initStr;
 	m_sCallTel=szCall;
}

void CModem::SeekConnect()
{
	DWORD ModemStat;
	GetCommModemStatus(hComDev,&ModemStat);
	if((ModemStat & MS_RLSD_ON) == MS_RLSD_ON)
	{
		OnConnecting(1,"CONNECT",0);
		IsConnect = CONNECT_STEP_DIAL_SUCCESS;
	}
	else
	{
		OnConnecting(0,"ERROR",CONN_RESULT_NOCARRIER);
		IsConnect = CONNECT_STEP_DIAL_FAIL;
	}
}

void CModem::Close()
{
	Write((BYTE *)"+++ATH\r",strlen("+++ATH\r"));
 	Sleep(500);
	CCom::Close();
}
