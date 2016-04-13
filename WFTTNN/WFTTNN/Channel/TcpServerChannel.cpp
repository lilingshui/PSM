
#include "TcpServerChannel.h"
#include "NetHandlerMgr.h"


CTcpServerChannel::CTcpServerChannel(unsigned int Id, CString IpAddr, int Port, CString PhoneNum)
	:CChannel(Id, IpAddr, Port, PhoneNum)
{
	mPort = Port;
	StartServer();
}


int CTcpServerChannel::Write(char *buf, int len, u_int64 termAddr)
{
	CTcpServerHandler *pHandler = NULL;
	if (CNetHandlerMgr::getInstance()->FindHandler(&pHandler,termAddr) == TRUE)
	{
		pHandler->Send(buf, len);
		return len;
	}
	else
		return -1;
}

int CTcpServerChannel::StartServer()
{
	if (AfxSocketInit())
	{
		//AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return false;
	}
	
	this->Create(6001);
	this->Listen();

	return 1;
	
}


void CTcpServerChannel::OnAccept(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class

	CTcpServerHandler *pHandler = new CTcpServerHandler();

	if(!Accept(*pHandler))
	{

	}
	else
		delete pHandler;

	__super::OnAccept(nErrorCode);
}


int CTcpServerChannel::ConnectTo(u_int64 termAddr)
{
	return 0;
}

int CTcpServerChannel::Disonnect(u_int64 termAddr)
{
	return 0;
}