#pragma once
#include "afxsock.h"

class CTcpServerHandler : public CSocket
{
public:
	CTcpServerHandler();
	virtual ~CTcpServerHandler(void);
	virtual void OnReceive(int nErrorCode);

private:
	char m_buffer[1024];
	unsigned int mId;

public:
	virtual void OnClose(int nErrorCode);
};

