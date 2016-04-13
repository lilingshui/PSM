// Reactor.cpp: implementation of the Reactor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reactor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Reactor *Reactor::sm_pInstance=NULL;
Reactor::Reactor()
{
	m_nStartPos=0;
	CreateThread();
}

Reactor::~Reactor()
{
	TerminateThread();
}

Reactor *Reactor::GetInstance()
{
    if (sm_pInstance==NULL)
		sm_pInstance=new Reactor();

	return sm_pInstance;
}

void Reactor::Execute()
{
	while (IsRun())
	{
		ProcessConnectEvent();
		fd_set fsRead;
        fd_set fsWrite;
		fd_set fsExcept;
		SetFDSet(fsRead,ET_READ);
		SetFDSet(fsWrite,ET_WRITE);
		SetFDSet(fsExcept,ET_EXCEPT);
		timeval tv;
		tv.tv_sec=0;
		tv.tv_usec=100*1000;
		int nRet=select(0,&fsRead,&fsWrite,&fsExcept,&tv);
		if (nRet>0)
		{
			for (u_int i=0;i<fsRead.fd_count;i++)
			{
				IEventHandler* pHandler=FindHander(fsRead.fd_array[i]);
				if (pHandler)
					pHandler->HandleEvent(ET_READ);
			}
			for (u_int j=0;j<fsWrite.fd_count;j++)
			{
				IEventHandler* pHandler=FindHander(fsWrite.fd_array[j]);
				if (pHandler)
					pHandler->HandleEvent(ET_WRITE);
			}
			for (u_int index=0;index<fsExcept.fd_count;index++)
			{
				IEventHandler* pHandler=FindHander(fsExcept.fd_array[index]);
				if (pHandler)
					pHandler->HandleEvent(ET_EXCEPT);
			}
		}
		else
			Sleep(100);
	}
}

void Reactor::RegisterHandler(IEventHandler *pHandler,DWORD dwEvent)
{
	TH_AutoLock autoLock(&m_mutexHandlerVec);
    HANDLER_INFO hi;
	hi.pHandler=pHandler;
	hi.dwEvent=dwEvent;
	m_eventHandlerVec.push_back(hi);
}

void Reactor::RemoveHandler(IEventHandler *pHandler)
{
	TH_AutoLock autoLock(&m_mutexHandlerVec);
    EVENT_HANDLER_ITER iter=m_eventHandlerVec.begin();
	for (;iter!=m_eventHandlerVec.end();iter++)
	{
		if (iter->pHandler==pHandler)
		{
			m_eventHandlerVec.erase(iter);
			return;
		}
	}
}

void Reactor::SetFDSet(fd_set &fs,DWORD dwEvent)
{
	if (m_eventHandlerVec.size()==0)
		return;
	int nGroupSize=min(m_eventHandlerVec.size(),64);
	TH_AutoLock autoLock(&m_mutexHandlerVec);
	FD_ZERO(&fs);
	switch (dwEvent)
	{
	case ET_READ:
		{
			for (int i=0;i<nGroupSize;i++)
			{
				int nIndex=(m_nStartPos+i)%m_eventHandlerVec.size();
				SOCKET sock=m_eventHandlerVec[nIndex].pHandler->GetSocket();
				if (sock != INVALID_SOCKET && m_eventHandlerVec[nIndex].dwEvent & ET_READ)
					FD_SET(sock,&fs);
			}
			break;
		}
	case ET_WRITE:
		{
			for (int i=0;i<nGroupSize;i++)
			{
				int nIndex=(m_nStartPos+i)%m_eventHandlerVec.size();
				SOCKET sock=m_eventHandlerVec[nIndex].pHandler->GetSocket();
				if (sock != INVALID_SOCKET && m_eventHandlerVec[nIndex].dwEvent & ET_WRITE)
				{
					CServiceHandler *pHandler=(CServiceHandler *)m_eventHandlerVec[nIndex].pHandler;
					if (pHandler->GetConnectState()==CS_CONNECTING)
						FD_SET(sock,&fs);
				}
			}
			break;
		}
	case ET_EXCEPT:
		{
			for (int i=0;i<nGroupSize;i++)
			{
				int nIndex=(m_nStartPos+i)%m_eventHandlerVec.size();
				SOCKET sock=m_eventHandlerVec[nIndex].pHandler->GetSocket();
				if (sock != INVALID_SOCKET && m_eventHandlerVec[nIndex].dwEvent & ET_EXCEPT)
					FD_SET(sock,&fs);
			}
			break;
		}
	}
	m_nStartPos=(m_nStartPos+nGroupSize)%m_eventHandlerVec.size();
}

IEventHandler* Reactor::FindHander(SOCKET sck)
{
	TH_AutoLock autoLock(&m_mutexHandlerVec);
	for (UINT i=0;i<m_eventHandlerVec.size();i++)
	{
		if (m_eventHandlerVec[i].pHandler->GetSocket()==sck)
		{
			return m_eventHandlerVec[i].pHandler;
		}
	}
	return NULL;
}

void Reactor::ProcessConnectEvent()
{
	TH_AutoLock autoLock(&m_mutexHandlerVec);
	for (UINT i=0;i<m_eventHandlerVec.size();i++)
	{
		if (m_eventHandlerVec[i].dwEvent & ET_WRITE)
		{
			CServiceHandler *pHandler=(CServiceHandler *)m_eventHandlerVec[i].pHandler;
			if (pHandler->GetConnectState()==CS_NULL)
				pHandler->HandleEvent(ET_WRITE);
		}
	}
}

