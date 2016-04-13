// AcceptChannel.cpp: implementation of the CAcceptChannel class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "AcceptChannel.h"
#include "Reactor/Reactor.h"
#include "HandlerMgr.h"
#include "PtclPacket_NN.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern bool g_nZiDongOrShouDongFlag;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAcceptChannel::CAcceptChannel()
{
	m_nHostID=0;
	
	Reactor::GetInstance()->RegisterHandler(this);
	CHandlerMgr::GetInstance()->m_vAcceptChannel.push_back(this);
	
}

CAcceptChannel::~CAcceptChannel()
{
	Reactor::GetInstance()->RemoveHandler(this);
	CHandlerMgr::GetInstance()->m_vAcceptChannel.erase(find(CHandlerMgr::GetInstance()->m_vAcceptChannel.begin(),CHandlerMgr::GetInstance()->m_vAcceptChannel.end(),this));
}

void CAcceptChannel::HandleEvent(DWORD et)
{
	switch (et)
	{
	case ET_READ:
		FRAME_BUF fb;
		memset(&fb,0,sizeof(FRAME_BUF));
		m_sockStream.Recv(fb);
		if (fb.len>0)
		{
			//ProcessRecvData(fb.buf,fb.len);
			ProcessFrame(fb);
		}
		else
		{
			m_sockStream.CloseSocket();
			delete this;
		}
		break;
	}
}

void CAcceptChannel::ProcessFrame(FRAME_BUF fb)
{
	ShowPacketLog(TraceBuf(fb.buf,fb.len,"接收"));
	
	CPtclPacket_NN packet_nn;
	packet_nn.setPacketBuf(fb.buf,fb.len);
	if (packet_nn.parsePacket() <= 0) ;

	unsigned char AFN = packet_nn.getAFN();

	if (AFN == 0x02 ||AFN == 0x0E) //心跳帧
	{
		CHandlerMgr::GetInstance()->TransferToFep(fb);
		return ;
	}

	if (g_nZiDongOrShouDongFlag == false)
	{
		if (g_nModuleControl == Module_RndRead || AFN == 0x0C || AFN == 0x0D)//0C读取当前数据 0D读一类数据
		{
			CPacketTransfer::GetInstance()->RndReadPacketTransfer((FRAME_BUFFER*)&fb);
		}
		else if (AFN == 0x0A || AFN == 0x0C || AFN == 0x0D)
		{
			CPacketTransfer::GetInstance()->SetParamPacketTransfer((FRAME_BUFFER*)&fb);
		}
	}
	else
	{
		CPacketTransfer::GetInstance()->AutoCeShiPacketTransfer((FRAME_BUFFER*)&fb);
	}
}

void CAcceptChannel::ProcessRecvData(BYTE buf[],int len)
{
	m_bufData.Add(buf,len);
	while (m_bufData.GetSize()>0)
	{
		/*
		IFrame *pFrame=FR_GuoDian::ExtractFrame(m_bufData);
		if (pFrame)
		{
			ProcessFrame(pFrame);
			continue;
		}

		pFrame=FR_NanWang::ExtractFrame(m_bufData);
		if (pFrame)
		{
			ProcessFrame(pFrame);
			continue;
		}
		
		break;
		*/
	}
}


//======================================================
CListenSocket::CListenSocket(int nPort)
:m_acceptor(nPort)
{
	Reactor::GetInstance()->RegisterHandler(this);
}

CListenSocket::~CListenSocket()
{
	Reactor::GetInstance()->RemoveHandler(this);
}

void CListenSocket::HandleEvent(DWORD et)
{
	if (et==ET_READ)
	{
		CAcceptChannel *pChannel=new CAcceptChannel();
		m_acceptor.Accept(pChannel->GetStream());
	}
}
