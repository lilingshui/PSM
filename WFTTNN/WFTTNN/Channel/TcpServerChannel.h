#pragma once

#include <afxsock.h>
#include "Channel.h"
#include "TcpServerHandler.h"
#include <map>


class CTcpServerChannel:public CChannel, public CSocket
{
public:
	CTcpServerChannel(unsigned int Id, CString IpAddr, int Port, CString PhoneNum);

public:
	virtual int ConnectTo(u_int64 termAddr);

	virtual int Disonnect(u_int64 termAddr);

	virtual int Write(char *buf, int len, u_int64 termAddr);

	int StartServer();



private:
	unsigned int mPort;


public:
	virtual void OnAccept(int nErrorCode);
};