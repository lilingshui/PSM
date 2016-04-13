#pragma once

#include <afxsock.h>

class CChannel
{
public:
	CChannel(unsigned int Id, CString IpAddr, int Port, CString PhoneNum);

	virtual int ConnectTo(u_int64 termAddr) = 0;  //return 0:success -1:failed
	virtual int Disonnect(u_int64 termAddr) = 0;  //return 0:success -1:failed
	virtual int Write(char *buf,int len, u_int64 termAddr) = 0;	 //return >0:success write len -1:failed

	unsigned int GetId() const;

private:
	unsigned int mId;
};

