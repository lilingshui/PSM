#pragma once

#include "Channel.h"
//#include "PSTNClientHandler.h"
#include <afxsock.h>
#include <map>
// CTcpClientChannel command target

class CPSTNClientChannel : public CChannel, public CSocket
{
	enum 
	{
		IDLE		=0,
		CONNECT		=1,
		DIALING		=2,
		ONLINE		=3,
	};

public:
	CPSTNClientChannel(unsigned int id, CString IpAddr, int Port, CString PhoneNum);
	virtual ~CPSTNClientChannel();

public:
	virtual int ConnectTo(u_int64 termAddr);

	virtual int Disonnect(u_int64 termAddr);

	virtual int Write(char *buf, int len, u_int64 termAddr);

	int Dial();

	//void SetState(unsigned char state);

	//unsigned char GetState();

	void SetClose();


private:
	//CPSTNClientHandler *m_pHandler;
	unsigned int mPort;
	CString m_IpAddr;
	CString csPhoneNumber;
	unsigned char ucState;
	int mId;

	int JudgeDialResult(char *rBuf, unsigned int rLen);
	void SetIpAndPort();
public:
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};


