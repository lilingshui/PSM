// TcpClientHandler.cpp : implementation file
//

#include "PSTNClientHandler.h"
#include "PSTNClientChannel.h"
#include "ChannelMgr.h"
#include <afxsock.h>

// CTcpClientHandler

CPSTNClientHandler::CPSTNClientHandler(u_int64 id,int channelId)
{
	Create();
	m_id = id;
	m_channelId = channelId;
}

CPSTNClientHandler::~CPSTNClientHandler()
{
}


// CTcpClientHandler member functions


void CPSTNClientHandler::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	int len = this->Receive(m_buffer,1024);

	if (len >= 0)
	{
		CPSTNClientChannel *channel = (CPSTNClientChannel*)CChannelMgr::getInstance()->getChannel(m_channelId);
		if (channel == NULL)
			return;
		//if (channel->GetState() == 1)
		//	CChannelMgr::getInstance()->OnReceive(CChannelMgr::RECEIVING, m_buffer,len, m_id);

		//int res = JudgeDialResult((unsigned char *)m_buffer, len);

		//if (res == 0)
		//{	
		//	channel->SetState(1);
		//}
		//else 
		//	channel->SetState(2);


	}

	CSocket::OnReceive(nErrorCode);
}


void CPSTNClientHandler::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	this->Close();
	CChannelMgr::getInstance()->OnReceive(CChannelMgr::OFFLINE, m_buffer,0, m_id);
	CPSTNClientChannel *channel = (CPSTNClientChannel*)CChannelMgr::getInstance()->getChannel(m_channelId);
	if (channel != NULL)
	{
		channel->SetClose();
	}


	CSocket::OnClose(nErrorCode);
}


void CPSTNClientHandler::OnConnect(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	CChannelMgr::getInstance()->OnReceive(CChannelMgr::ONLINE, m_buffer,0, 0);
	CSocket::OnConnect(nErrorCode);
}

//int CPSTNClientHandler::JudgeDialResult(unsigned char *rBuf, unsigned int rLen)
//{
//		unsigned char buf[512];
//		char szResult[512];
//		short value = 0;
//
//		memset(szResult,0,sizeof(szResult));
//		memset(buf,0,sizeof(buf));
//
//		int idx = 0;
//		
//		idx += rLen;
//		if (idx >= 512)
//		{
//			idx = 512;
//		}
//		for(int i=0; i<idx; i++)
//		{
//			memcpy(buf,rBuf,idx);
//			if((buf[i] == '\0') && (i != (idx - 2)))
//				buf[i]='\t';
//		}
//
//		char *temp = strchr((char*)buf,'\r');
//
//		if(temp)
//		{
//			int lenTmp = idx - (int)(temp - (char*)buf);
//			lenTmp = (lenTmp >= 512 ? 512 : lenTmp);
//			memcpy(szResult,temp,lenTmp);
//			value = 1;
//		}
//
//		_strupr(szResult);
//
//		int state = 0;
//		if(strstr(szResult,"CONNEC") != NULL)
//		{
//			return 0;
//		}
//		else
//			return -1;
//}
