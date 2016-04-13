#pragma once

// CTcpClientHandler command target
#include <afxsock.h>

class CTcpClientHandler : public CSocket
{
public:
	CTcpClientHandler(u_int64 id,int channelId);
	virtual ~CTcpClientHandler();
	virtual void OnReceive(int nErrorCode);

	u_int64 m_id;
	int m_channelId;

private:
	char m_buffer[1024];
	
public:
	//virtual void OnClose(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
};


