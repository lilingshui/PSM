#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "SOCKWrapper.h"

enum
{
	ET_READ=1,
	ET_WRITE=2,
	ET_EXCEPT=4
};

class IEventHandler  
{
public:
	IEventHandler(){}
	virtual ~IEventHandler(){}
	virtual SOCKET GetSocket()=0;
    virtual void   HandleEvent(DWORD et)=0;
};

//==========================================================
enum 
{
	CS_NULL,
	CS_CONNECTING,
	CS_CONNECTED
};
class CServiceHandler : public IEventHandler
{
public:
	CServiceHandler(){}
	virtual ~CServiceHandler(){}
	virtual SOCKET GetSocket(){return m_sockStream.GetSocket();}
	SOCK_Stream &GetStream(){return m_sockStream;}
	virtual int GetConnectState(){return CS_NULL;}
protected:
	SOCK_Stream m_sockStream;
};

#endif //EVENTHANDLER_H