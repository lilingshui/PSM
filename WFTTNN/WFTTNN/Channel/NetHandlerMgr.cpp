#include "NetHandlerMgr.h"

CNetHandlerMgr *CNetHandlerMgr::mInstance = NULL;
CNetHandlerMgr *CNetHandlerMgr::getInstance()
{
	if (mInstance == NULL)
		mInstance = new CNetHandlerMgr();
	return mInstance;
}
CNetHandlerMgr::CNetHandlerMgr()
{
}

CNetHandlerMgr::~ CNetHandlerMgr()
{
}

BOOL CNetHandlerMgr::FindHandler(CTcpServerHandler **Handler, u_int64 id)
{
	std::map<u_int64, CTcpServerHandler *>::iterator it = m_TcpServerHandlers.find(id);
	if (it != m_TcpServerHandlers.end())
	{
		*Handler = it->second;
		return TRUE;
	}
	return FALSE;
}

void CNetHandlerMgr::InsertHandler(u_int64 Id, CTcpServerHandler *Handler)
{
	std::map<u_int64, CTcpServerHandler *>::iterator it = m_TcpServerHandlers.find(Id);
	if (it != m_TcpServerHandlers.end())
		return ;

	m_TcpServerHandlers[Id] = Handler;
}

void CNetHandlerMgr::RemoveTcpServerHandler(u_int64 Id)
{
	std::map<u_int64, CTcpServerHandler *>::iterator it = m_TcpServerHandlers.find(Id);
	if (it != m_TcpServerHandlers.end())
	{
		m_TcpServerHandlers.erase(Id);
	}
	return;
}

BOOL CNetHandlerMgr::FindHandler(CTcpClientHandler **Handler, u_int64 id)
{
	std::map<u_int64, CTcpClientHandler *>::iterator it = m_TcpClinetHandlers.find(id);
	if (it != m_TcpClinetHandlers.end())
	{
		*Handler = it->second;
		return TRUE;
	}
	return FALSE;
}

void CNetHandlerMgr::InsertHandler(u_int64 Id, CTcpClientHandler *Handler)
{
	std::map<u_int64, CTcpClientHandler *>::iterator it = m_TcpClinetHandlers.find(Id);
	if (it != m_TcpClinetHandlers.end())
		return ;

	m_TcpClinetHandlers[Id] = Handler;
}

void CNetHandlerMgr::RemoveTcpClientHandler(u_int64 Id)
{
	std::map<u_int64, CTcpClientHandler *>::iterator it = m_TcpClinetHandlers.find(Id);
	if (it != m_TcpClinetHandlers.end())
	{
		m_TcpClinetHandlers.erase(Id);
	}
	return;
}

BOOL CNetHandlerMgr::FindHandler(CPSTNClientHandler **Handler, u_int64 id)
{
	std::map<u_int64, CPSTNClientHandler *>::iterator it = m_PPPClinetHandlers.find(id);
	if (it != m_PPPClinetHandlers.end())
	{
		*Handler = it->second;
		return TRUE;
	}
	return FALSE;
}

void CNetHandlerMgr::InsertHandler(u_int64 Id, CPSTNClientHandler *Handler)
{
	std::map<u_int64, CPSTNClientHandler *>::iterator it = m_PPPClinetHandlers.find(Id);
	if (it != m_PPPClinetHandlers.end())
		return ;

	m_PPPClinetHandlers[Id] = Handler;
}

void CNetHandlerMgr::RemovePPPClientHandler(u_int64 Id)
{
	std::map<u_int64, CPSTNClientHandler *>::iterator it = m_PPPClinetHandlers.find(Id);
	if (it != m_PPPClinetHandlers.end())
	{
		m_PPPClinetHandlers.erase(Id);
	}
	return;
}



