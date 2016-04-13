// ClientChannel.cpp: implementation of the CClientChannel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClientChannel.h"
#include "Reactor/Reactor.h"
#include "HandlerMgr.h"
//#include "FR_GuoDian.h"
//#include "FR_NanWang.h"

//#pragma comment(lib,"ws2_32.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientChannel::CClientChannel(CString strIP,int nPort)
:m_netAddr(nPort,strIP)
{
	m_nConnectState=CS_NULL;
	Reactor::GetInstance()->RegisterHandler(this,ET_READ | ET_WRITE | ET_EXCEPT);
	CHandlerMgr::GetInstance()->m_vClientChannel.push_back(this);
}

CClientChannel::~CClientChannel()
{
	Reactor::GetInstance()->RemoveHandler(this);
	CHandlerMgr::GetInstance()->m_vClientChannel.erase(find(CHandlerMgr::GetInstance()->m_vClientChannel.begin(),CHandlerMgr::GetInstance()->m_vClientChannel.end(),this));
}

void CClientChannel::HandleEvent(DWORD et)
{
	switch (et)
	{
	case ET_READ:
		{
			FRAME_BUF fb;
			memset(&fb,0,sizeof(FRAME_BUF));
			GetStream().Recv(fb);
			if (fb.len>0)
			{				
				ProcessRecvData(fb.buf,fb.len);
			}
			else
			{
				m_nConnectState=CS_NULL;
				GetStream().CloseSocket();
			}
			break;
		}
	case ET_WRITE:
		{
			if (m_nConnectState==CS_CONNECTING)
				m_nConnectState=CS_CONNECTED;
			else if (m_nConnectState==CS_NULL)
			{
				GetStream().CreateClientSocket(m_netAddr);
				m_nConnectState=CS_CONNECTING;
				TRACE("===========connect====\r\n");
			}
			break;
		}
	case ET_EXCEPT:
		{
			int nValue;
			int nLen=sizeof(nValue);
			getsockopt(GetSocket(),SOL_SOCKET,SO_ERROR,(char*)&nValue,&nLen);
			switch (nValue)
			{
			case 10061:
				m_nConnectState=CS_NULL;
				GetStream().CloseSocket();
				break;
			}
			break;
		}
	}
}

void CClientChannel::ProcessRecvData(BYTE buf[],int len)
{
	m_bufData.Add(buf,len);
	while (m_bufData.GetSize()>0)
	{
		/*
		IFrame *pFrame=FR_GuoDian::ExtractFrame(m_bufData);
		if (pFrame)
		{
			CHandlerMgr::GetInstance()->TransferToMaster(pFrame);
			continue;
		}
		pFrame=FR_NanWang::ExtractFrame(m_bufData);
		if (pFrame)
		{
			CHandlerMgr::GetInstance()->TransferToMaster(pFrame);
			continue;
		}
		*/
		break;
	}
}

int CClientChannel::GetConnectState()
{
	return m_nConnectState;
}
/*
ClientChannelInfo CClientChannel::GetShowInfo()
{
	ClientChannelInfo cci;
	SOCK_NetAddr netAddr=m_sockStream.GetPeerAddr();
	cci.strIP=netAddr.GetIP();
	cci.nPort=netAddr.GetPort();
	cci.nConnState=GetConnectState();
	return cci;
}
*/