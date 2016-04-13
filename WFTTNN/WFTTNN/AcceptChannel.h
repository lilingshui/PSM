// AcceptChannel.h: interface for the CAcceptChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCEPTCHANNEL_H__753E04DF_5AB6_47D0_93BE_8B07B8132D9D__INCLUDED_)
#define AFX_ACCEPTCHANNEL_H__753E04DF_5AB6_47D0_93BE_8B07B8132D9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Reactor/EventHandler.h"
#include "BufferData.h"

struct AcceptChannelInfo
{
	CString strIP;
	int		nPort;
	int		nHostID;
};

class IFrame;
class CAcceptChannel : public CServiceHandler 
{
public:
	CAcceptChannel();
	virtual ~CAcceptChannel();
    virtual void   HandleEvent(DWORD et);
	int GetHostID(){return m_nHostID;}
private:
	int m_nHostID;
	CBufferData m_bufData;
	void ProcessRecvData(BYTE buf[],int len);
	void ProcessFrame(FRAME_BUF fb);
};

//========================================================
class CListenSocket : public IEventHandler
{
public:
	CListenSocket(int nPort);
	virtual ~CListenSocket();
	virtual void   HandleEvent(DWORD et);
	virtual SOCKET GetSocket(){return m_acceptor.GetSocket();}
private:
	SOCK_Acceptor m_acceptor;
};

#endif // !defined(AFX_ACCEPTCHANNEL_H__753E04DF_5AB6_47D0_93BE_8B07B8132D9D__INCLUDED_)
