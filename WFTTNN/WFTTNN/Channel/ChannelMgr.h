#pragma once

#include "Channel.h"
#include "ChannelReceiver.h"
#include <vector>
#include <map>

class CChannelMgr
{
public:
	enum 
	{
		OFFLINE = 1,
		ONLINE = 2,
		RECEIVING = 3,
	};
protected:
	CChannelMgr();
	~CChannelMgr();

public:
	static CChannelMgr *getInstance();
	
	CChannel *getChannel(unsigned id);

	CChannelReceiver *getChannelRecv(unsigned int id);

	void Register(CChannelReceiver *pChannelRecv);

	void UnRegister(CChannelReceiver *pChannelRecv);

	void OnReceive(int state, char *buf, int len, u_int64 strTermAddr);


protected:
	static CChannelMgr *mInstance;

private:
	std::map <unsigned int,CChannel*> m_Channel_;
	std::vector <CChannelReceiver*> v_ChannelRecv_;
};