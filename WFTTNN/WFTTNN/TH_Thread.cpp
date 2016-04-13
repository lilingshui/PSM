// TH_Thread.cpp: implementation of the TH_Event class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TH_Thread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TH_Event::TH_Event()
{
    m_hEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
}

TH_Event::~TH_Event()
{
    ::CloseHandle(m_hEvent);
}

void TH_Event::Wait()
{
	::WaitForSingleObject(m_hEvent,INFINITE);
}

void TH_Event::Signal()
{
    ::SetEvent(m_hEvent);
}

void TH_Event::Unsignal()
{
	::ResetEvent(m_hEvent);
}

//===========================================================

TH_Mutex::TH_Mutex()
{
	m_hMutex=::CreateMutex(NULL,FALSE,NULL);
}

TH_Mutex::~TH_Mutex()
{
	::CloseHandle(m_hMutex);
}

void TH_Mutex::Lock()
{
	::WaitForSingleObject(m_hMutex,INFINITE);
}

void TH_Mutex::Unlock()
{
	::ReleaseMutex(m_hMutex);
}

//=====================================================================
TH_AutoLock::TH_AutoLock(TH_Mutex *pMutex)
{
	m_pMutex=pMutex;
	m_pMutex->Lock();
}

TH_AutoLock::~TH_AutoLock()
{
	m_pMutex->Unlock();
}

//=====================================================================

DWORD WINAPI ThreadFun(LPVOID pParam)
{
	TH_Thread *th=(TH_Thread*)pParam;
	th->Execute();
	return 0;
}

TH_Thread::TH_Thread()
{
	m_hThread=NULL;
	m_dwThreadID=0;
}

TH_Thread::~TH_Thread()
{

}

void TH_Thread::CreateThread()
{
	m_bStopThread=false;
	m_hThread=::CreateThread(NULL,0,ThreadFun,this,0,&m_dwThreadID);
}

void TH_Thread::TerminateThread(bool bForce)
{
	if (bForce)
		::TerminateThread(m_hThread,0);
	m_bStopThread=true;
    ::WaitForSingleObject(m_hThread,INFINITE);
	m_hThread=NULL;
	m_dwThreadID=0;
}

bool TH_Thread::IsRun()
{
	return (!m_bStopThread);
}

void TH_Thread::StopThread()
{
	m_bStopThread=true;
}