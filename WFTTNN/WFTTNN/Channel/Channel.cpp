#include "Channel.h"

CChannel::CChannel(unsigned int Id, CString IpAddr, int Port, CString PhoneNum)
{
	mId = Id;
}

unsigned int CChannel::GetId() const
{
	return mId;
}