
#include "TcpServerHandler.h"
#include "ChannelMgr.h"
#include "NetHandlerMgr.h"
#include "..\ptcl_codec\ptcl_packet_csg.h"
#include "..\ptcl_codec\ptcl_module_csg.h"

CTcpServerHandler::CTcpServerHandler()
{	
	memset(m_buffer,0,1024);
	mId = 0;
}


CTcpServerHandler::~CTcpServerHandler(void)
{
}


void CTcpServerHandler::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	int len = this->Receive(m_buffer,1024);
	ptcl_packet_csg Packet;
	if (Packet.parse_header() < 0)
	{
		return ;
	}

	u_int64 nId = Packet.get_device_addr();

	if (Packet.get_afn() == 0x02)
	{
		char *PnFn = m_buffer + Packet.get_header_len();
		unsigned int *Fn = (unsigned int *)(PnFn + 2);

		switch (*Fn)
		{
		case 0xe0001000:
			{
			}
		}
	}

	CString strTermAddr;
	strTermAddr.Format("%ll", nId);

	CNetHandlerMgr::getInstance()->InsertHandler(mId, this);
	CChannelMgr::getInstance()->OnReceive(2, m_buffer,len,0);
	CSocket::OnReceive(nErrorCode);
}


void CTcpServerHandler::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	CNetHandlerMgr::getInstance()->RemoveTcpServerHandler(mId);
	this->Close();
	CSocket::OnClose(nErrorCode);
}
