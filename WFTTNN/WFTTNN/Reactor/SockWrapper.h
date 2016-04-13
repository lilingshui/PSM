// SockWrapper.h: interface for the SockWrapper class.
//说明：SocketAPI函数的包装类
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKWRAPPER_H__E7E726E6_E31A_4ADB_9439_18CF80522908__INCLUDED_)
#define AFX_SOCKWRAPPER_H__E7E726E6_E31A_4ADB_9439_18CF80522908__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Winsock2.h>
#pragma comment(lib,"ws2_32.lib")

typedef struct _FRAME_BUF
{
	BYTE buf[2048];
	int  len;
}FRAME_BUF;

class SOCK_InitSocket
{
public:
	SOCK_InitSocket()
	{
		WSADATA ws;
		WSAStartup(MAKEWORD(2,2),&ws);
	}
	virtual ~SOCK_InitSocket()
	{
		WSACleanup();
	}
};

//=============================================

class SOCK_NetAddr  
{
public:
	SOCK_NetAddr();
	SOCK_NetAddr(int nPort,CString strIP="");
	SOCK_NetAddr(sockaddr_in sockAddrIn);
	virtual ~SOCK_NetAddr();
	sockaddr *GetSockAddr();
	int      GetPort();
	CString  GetIP();
	int GetAddrSize();
private:
	sockaddr_in m_sockAddrIn;
};

//======================================================

class SOCK_Stream  
{
public:
	SOCK_Stream();
	virtual ~SOCK_Stream();
    int Send(FRAME_BUF fb);
	int Recv(FRAME_BUF &fb);
	SOCKET GetSocket();
	void SetSocket(SOCKET sck);
	void CreateClientSocket(SOCK_NetAddr netAddr);
	void CloseSocket();
	SOCK_NetAddr GetPeerAddr();
private:
	SOCKET m_socket;
};
//====================================================

class SOCK_Acceptor  
{
public:
	SOCK_Acceptor(int nPort);
	virtual ~SOCK_Acceptor();
	void Accept(SOCK_Stream &sockStream);
	SOCKET GetSocket();
private:
	SOCKET m_sckListen;
};

#endif // !defined(AFX_SOCKWRAPPER_H__E7E726E6_E31A_4ADB_9439_18CF80522908__INCLUDED_)
