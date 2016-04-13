// TH_Thread.h: interface for the TH_Event class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TH_THREAD_H__61257496_6921_45D6_85B1_E9BA5EF72B87__INCLUDED_)
#define AFX_TH_THREAD_H__61257496_6921_45D6_85B1_E9BA5EF72B87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include <windows.h>

class TH_Event  
{
public:
	TH_Event();
	virtual ~TH_Event();
	void Wait();
	void Signal();
	void Unsignal();
private:
	HANDLE m_hEvent;
};

class TH_Mutex
{
public:
	TH_Mutex();
	virtual ~TH_Mutex();
	void Lock();
	void Unlock();
private:
	HANDLE m_hMutex;
};

class TH_AutoLock
{
public:
	TH_AutoLock(TH_Mutex *pMutex);
	virtual ~TH_AutoLock();
private:
	TH_Mutex *m_pMutex;
};

class TH_Thread
{
public:
	TH_Thread();
	virtual ~TH_Thread();
	virtual void Execute()=0;
protected:
    void CreateThread();
	void TerminateThread(bool bForce=false);
	bool IsRun();
	void StopThread();
private:
	HANDLE m_hThread;
	bool m_bStopThread;
	DWORD m_dwThreadID;
};

#endif // !defined(AFX_TH_THREAD_H__61257496_6921_45D6_85B1_E9BA5EF72B87__INCLUDED_)
