#pragma once 

#include "..\StdAfx.h"

class CChannelReceiver
{
public:
	CChannelReceiver();

	virtual void OnReceive(int state, char *buf, int len, u_int64 strTermAddr) = 0;
};