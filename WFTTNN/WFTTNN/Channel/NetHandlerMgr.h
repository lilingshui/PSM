#pragma once

#include "TcpServerHandler.h"
#include "TcpClientHandler.h"
#include "PSTNClientHandler.h"
#include <map>

class  CNetHandlerMgr
{
protected:
	 CNetHandlerMgr();
	~ CNetHandlerMgr();

public:
	static CNetHandlerMgr *getInstance();

	BOOL FindHandler(CTcpServerHandler **Handler, u_int64 id);

	void InsertHandler(u_int64 Id, CTcpServerHandler *Handler);

	void RemoveTcpServerHandler(u_int64 Id);

	BOOL FindHandler(CTcpClientHandler **Handler, u_int64 id);

	void InsertHandler(u_int64 Id, CTcpClientHandler *Handler);

	void RemoveTcpClientHandler(u_int64 Id);

	BOOL FindHandler(CPSTNClientHandler **Handler, u_int64 id);

	void InsertHandler(u_int64 Id, CPSTNClientHandler *Handler);

	void RemovePPPClientHandler(u_int64 Id);

protected:
	static CNetHandlerMgr *mInstance;


private:
	std::map<u_int64, CTcpServerHandler *> m_TcpServerHandlers;

	std::map<u_int64, CTcpClientHandler *> m_TcpClinetHandlers;

	std::map<u_int64, CPSTNClientHandler *> m_PPPClinetHandlers;
};

