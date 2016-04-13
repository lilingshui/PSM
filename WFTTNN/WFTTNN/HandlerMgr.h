#pragma once

#include "AcceptChannel.h"
#include "ClientChannel.h"
#include "Frame.h"

typedef vector<CAcceptChannel*>   VEC_ACCEPT_CHANNEL;
typedef vector<CClientChannel*>   VEC_CLIENT_CHANNEL;

class CHandlerMgr
{
public:
	VEC_ACCEPT_CHANNEL  m_vAcceptChannel;
	VEC_CLIENT_CHANNEL  m_vClientChannel;
	~CHandlerMgr(void);	
	static CHandlerMgr *GetInstance();
	void TransferToFep(FRAME_BUF fb);
	void TransferToMaster(FRAME_BUF fb);
private:
	CHandlerMgr(void);
	static CHandlerMgr *m_pInstance;
};
