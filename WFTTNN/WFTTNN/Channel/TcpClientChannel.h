#pragma once

#include "Channel.h"
#include "TcpClientHandler.h"
#include <afxsock.h>
#include <map>
// CTcpClientChannel command target

class CTcpClientChannel : public CChannel//, public CSocket
{
public:
	CTcpClientChannel(unsigned int id, CString IpAddr, int Port, CString PhoneNum);
	virtual ~CTcpClientChannel();

public:
	virtual int ConnectTo(u_int64 termAddr);

	virtual int Disonnect(u_int64 termAddr);

	virtual int Write(char *buf, int len, u_int64 termAddr);

	void SetClose();

	void SetIpAndPort();

	CTcpClientHandler *m_pHandler;

private:
	unsigned int mPort;
	CString m_IpAddr;
	int m_Id;
};


