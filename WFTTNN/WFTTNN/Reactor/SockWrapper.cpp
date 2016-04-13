// SockWrapper.cpp: implementation of the SockWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SockWrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SOCK_InitSocket g_initSocket;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SOCK_NetAddr::SOCK_NetAddr()
{

}

SOCK_NetAddr::SOCK_NetAddr(int nPort,CString strIP)
{
    m_sockAddrIn.sin_family=AF_INET;
	m_sockAddrIn.sin_addr.S_un.S_addr=(strIP=="") ? INADDR_ANY:inet_addr(strIP);
	m_sockAddrIn.sin_port=htons(nPort);
}

SOCK_NetAddr::SOCK_NetAddr(sockaddr_in sockAddrIn)
{
	m_sockAddrIn=sockAddrIn;
}

SOCK_NetAddr::~SOCK_NetAddr()
{

}

sockaddr *SOCK_NetAddr::GetSockAddr()
{
	return (sockaddr*)&m_sockAddrIn;
}

int   SOCK_NetAddr::GetPort()
{
	return ntohs(m_sockAddrIn.sin_port);
}

CString  SOCK_NetAddr::GetIP()
{
	return inet_ntoa(m_sockAddrIn.sin_addr);
}

int SOCK_NetAddr::GetAddrSize()
{
	return sizeof(sockaddr_in);
}

//======================================================================
SOCK_Stream::SOCK_Stream()
{
    m_socket=INVALID_SOCKET;
}

SOCK_Stream::~SOCK_Stream()
{
	if (m_socket != INVALID_SOCKET)
		closesocket(m_socket);
}

int SOCK_Stream::Send(FRAME_BUF fb)
{
    return send(m_socket,(char*)fb.buf,fb.len,0);
}

int SOCK_Stream::Recv(FRAME_BUF &fb)
{
    fb.len=recv(m_socket,(char*)fb.buf,2048,0);
	return fb.len;
}

SOCKET SOCK_Stream::GetSocket()
{
    return m_socket;
}

void SOCK_Stream::SetSocket(SOCKET sck)
{
	m_socket=sck;
}

void SOCK_Stream::CreateClientSocket(SOCK_NetAddr netAddr)
{
	m_socket=::socket(AF_INET,SOCK_STREAM,0);
	DWORD dwValue=1;
	::ioctlsocket(GetSocket(),FIONBIO,&dwValue);
	::connect(GetSocket(),netAddr.GetSockAddr(),netAddr.GetAddrSize());
}

void SOCK_Stream::CloseSocket()
{
	::closesocket(m_socket);
	m_socket=INVALID_SOCKET;
}

SOCK_NetAddr SOCK_Stream::GetPeerAddr()
{
	SOCK_NetAddr addr;
	int len=sizeof(sockaddr_in);
	::getpeername(m_socket,(sockaddr*)addr.GetSockAddr(),&len);
	return addr;
}
//======================================================================
SOCK_Acceptor::SOCK_Acceptor(int nPort)
{
    m_sckListen=socket(AF_INET,SOCK_STREAM,0);
    SOCK_NetAddr netAddr(nPort);
	bind(m_sckListen,netAddr.GetSockAddr(),netAddr.GetAddrSize());
	listen(m_sckListen,5);
}

SOCK_Acceptor::~SOCK_Acceptor()
{
    closesocket(m_sckListen);
}

void SOCK_Acceptor::Accept(SOCK_Stream &sockStream)
{
	SOCKET sck=accept(m_sckListen,NULL,NULL);
	sockStream.SetSocket(sck);
}

SOCKET SOCK_Acceptor::GetSocket()
{
	return m_sckListen;
}
