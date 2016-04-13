// Reactor.h: interface for the Reactor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REACTOR_H__8F04D147_E84A_4DDF_A452_C2753C8289FF__INCLUDED_)
#define AFX_REACTOR_H__8F04D147_E84A_4DDF_A452_C2753C8289FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EventHandler.h"
#include "../TH_Thread.h"

typedef struct _HANDLER_INFO
{
	IEventHandler* pHandler;
    DWORD dwEvent;
}HANDLER_INFO;

typedef vector<HANDLER_INFO> EVENT_HANDLER_VEC;
typedef vector<HANDLER_INFO>::iterator EVENT_HANDLER_ITER;

class Reactor  : public TH_Thread
{
public:
	virtual ~Reactor();
	static Reactor *GetInstance();
    void Execute();
	void RegisterHandler(IEventHandler *pHandler,DWORD dwEvent=ET_READ);
	void RemoveHandler(IEventHandler *pHandler);
private:
	EVENT_HANDLER_VEC m_eventHandlerVec;
	TH_Mutex m_mutexHandlerVec;
	static Reactor *sm_pInstance;
	int m_nStartPos;
	Reactor();
	void SetFDSet(fd_set &fs,DWORD dwEvent);
	IEventHandler* FindHander(SOCKET sck);
	void ProcessConnectEvent();
};

#endif // !defined(AFX_REACTOR_H__8F04D147_E84A_4DDF_A452_C2753C8289FF__INCLUDED_)
