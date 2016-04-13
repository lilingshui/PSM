#pragma once

// CTcpClientHandler command target
#include <afxsock.h>

class CPSTNClientHandler : public CSocket
{
public:
	CPSTNClientHandler(u_int64 id, int channelId);
	virtual ~CPSTNClientHandler();
	virtual void OnReceive(int nErrorCode);

	u_int64 m_id;
	int m_channelId;
	

private:
	char m_buffer[1024];
//	int JudgeDialResult(char *rBuf, unsigned int rLen);

public:
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
};


