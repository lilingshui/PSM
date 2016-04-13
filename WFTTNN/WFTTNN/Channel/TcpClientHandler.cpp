// TcpClientHandler.cpp : implementation file
//

#include "TcpClientHandler.h"
#include "NetHandlerMgr.h"
#include "ChannelMgr.h"
#include "..\ptcl_codec\ptcl_packet_csg.h"
#include "..\ptcl_codec\ptcl_module_csg.h"
#include "TcpClientChannel.h"
#include <afxsock.h>

// CTcpClientHandler

CTcpClientHandler::CTcpClientHandler(u_int64 id,int channelId)
{
	Create();
	m_id = id;
	m_channelId = channelId;
}

CTcpClientHandler::~CTcpClientHandler()
{
}


// CTcpClientHandler member functions


void CTcpClientHandler::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	int len = this->Receive(m_buffer,1024);

	if (len >= 0)
	{
		CChannelMgr::getInstance()->OnReceive(CChannelMgr::RECEIVING, m_buffer,len, m_id);
	}

	CSocket::OnReceive(nErrorCode);
}

void CTcpClientHandler::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	//CNetHandlerMgr::getInstance()->RemoveTcpClientHandler(m_id);	
	this->Close();
	CChannelMgr::getInstance()->OnReceive(CChannelMgr::OFFLINE, m_buffer,0, m_id);
	CTcpClientChannel *channel = (CTcpClientChannel*)CChannelMgr::getInstance()->getChannel(m_channelId);
	if (channel != NULL)
	{
		channel->SetClose();
	}
	CSocket::OnClose(nErrorCode);
}


void CTcpClientHandler::OnConnect(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	CChannelMgr::getInstance()->OnReceive(CChannelMgr::ONLINE, m_buffer,0, 0);
	CSocket::OnConnect(nErrorCode);
}
