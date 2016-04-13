// ClientChannel.h: interface for the CClientChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTCHANNEL_H__1687992B_CD08_4951_BB3B_EA4B1F45B570__INCLUDED_)
#define AFX_CLIENTCHANNEL_H__1687992B_CD08_4951_BB3B_EA4B1F45B570__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Reactor/EventHandler.h"
#include "BufferData.h"

struct ClientChannelInfo
{
	CString strIP;
	int		nPort;
	int		nConnState;
};

class CClientChannel : public CServiceHandler 
{
public:
	CClientChannel(CString strIP,int nPort);
	virtual ~CClientChannel();
    virtual void   HandleEvent(DWORD et);
	virtual int GetConnectState();
	/*ClientChannelInfo GetShowInfo();*/
private:
	int m_nConnectState;
	SOCK_NetAddr m_netAddr;
	CBufferData m_bufData;
	void ProcessRecvData(BYTE buf[],int len);
};

#endif // !defined(AFX_CLIENTCHANNEL_H__1687992B_CD08_4951_BB3B_EA4B1F45B570__INCLUDED_)
