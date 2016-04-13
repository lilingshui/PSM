// TcpClientChannel.cpp : implementation file
//

#include "TcpClientChannel.h"
#include "NetHandlerMgr.h"
#include "ChannelMgr.h"


// CTcpClientChannel

CTcpClientChannel::CTcpClientChannel(unsigned int Id, CString IpAddr, int Port, CString PhoneNum)
	:CChannel(Id, IpAddr, Port, PhoneNum)
{
	m_Id = Id;
	mPort = Port;
	m_IpAddr = IpAddr;
	m_pHandler = NULL;
	
}

CTcpClientChannel::~CTcpClientChannel()
{
}


// CTcpClientChannel member functions
int CTcpClientChannel::ConnectTo(u_int64 termAddr)
{
	
	//pHandler = new CTcpClientHandler();
	CTcpClientHandler *pHandler = NULL;
	//if (CNetHandlerMgr::getInstance()->FindHandler(&pHandler, termAddr) == TRUE)
	//{
	//	return 0;
	//}
	if (m_pHandler == NULL)
	{
		AfxSocketInit();
		pHandler = new CTcpClientHandler(termAddr,m_Id);

		SetIpAndPort();
		if(pHandler->Connect(m_IpAddr, mPort) == TRUE)
		{
			//CNetHandlerMgr::getInstance()->InsertHandler(termAddr, pHandler);
			CChannelMgr::getInstance()->OnReceive(2, NULL, 0, termAddr);
			m_pHandler = pHandler;
			return 0;
		}
		else
		{
			delete pHandler;
			return -1;
		}
	}
	else if (m_pHandler != NULL)
	{
		if (m_pHandler->m_id != termAddr)
		{
			AfxSocketInit();

			m_pHandler->Close();
			pHandler = new CTcpClientHandler(termAddr,m_Id);
			SetIpAndPort();
			if(pHandler->Connect(m_IpAddr, mPort) == TRUE)
			{
				CChannelMgr::getInstance()->OnReceive(2, NULL, 0, termAddr);
				m_pHandler = pHandler;
				return 0;
			}
			else
			{
				delete pHandler;
				return -1;
			}
		}
		else
		{
			return 0;
		}
	}


	
}

int CTcpClientChannel::Disonnect(u_int64 termAddr)
{
	//CTcpClientHandler *pHandler = NULL;

	//if (CNetHandlerMgr::getInstance()->FindHandler(&pHandler, termAddr) == TRUE)
	//{
	//	//pHandler->ShutDown();
	//	pHandler->Close();
	//	CNetHandlerMgr::getInstance()->RemoveTcpClientHandler(termAddr);
	//	delete pHandler;
	//	return 0;
	//}
	if (m_pHandler != NULL)
	{
		CChannelMgr::getInstance()->OnReceive(1, NULL, 0, termAddr);
		m_pHandler->Close();
		m_pHandler = NULL;
	}
	else
	{
		return -1;
	}
}

int CTcpClientChannel::Write(char *buf, int len, u_int64 termAddr)
{
	//CTcpClientHandler  *pHandler = NULL;
	//if (CNetHandlerMgr::getInstance()->FindHandler(&pHandler,termAddr) == TRUE)
	//{
	//	pHandler->Send(buf, len);
	//	return len;
	//}
	//else
	//{
	//	AfxSocketInit();
	//	pHandler = new CTcpClientHandler(termAddr);

	//	if(pHandler->Connect(m_IpAddr, mPort) == TRUE)
	//	{
	//		CNetHandlerMgr::getInstance()->InsertHandler(termAddr, pHandler);
	//		pHandler->Send(buf, len);
	//		return len;
	//	}
	//	else
	//	{
	//		CChannelMgr::getInstance()->OnReceive(CChannelMgr::OFFLINE, NULL,0, termAddr);
	//		return -1;
	//	}
	//}

	CTcpClientHandler *pHandler = NULL;
	//if (CNetHandlerMgr::getInstance()->FindHandler(&pHandler, termAddr) == TRUE)
	//{
	//	return 0;
	//}
	if (m_pHandler == NULL)
	{
		pHandler = new CTcpClientHandler(termAddr,m_Id);

		if(pHandler->Connect(m_IpAddr, mPort) == TRUE)
		{
			//CNetHandlerMgr::getInstance()->InsertHandler(termAddr, pHandler);
			int lenWrite = pHandler->Send(buf, len);
			m_pHandler = pHandler;
			return lenWrite;
		}
		else
		{
			delete pHandler;
			return -1;
		}
	}
	else if (m_pHandler != NULL)
	{
		if (m_pHandler->m_id != termAddr)
		{
			m_pHandler->Close();
			pHandler = new CTcpClientHandler(termAddr, m_Id);
			if(pHandler->Connect(m_IpAddr, mPort) == TRUE)
			{
				int lenWrite = pHandler->Send(buf, len);
				m_pHandler = pHandler;
				return lenWrite;
			}
			else
			{
				delete pHandler;
				return -1;
			}
		}
		else
		{
			int lenWrite = m_pHandler->Send(buf, len);
			return lenWrite;
		}
	}
}


void CTcpClientChannel::SetClose()
{
	m_pHandler = NULL;
}

void CTcpClientChannel::SetIpAndPort()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;

	char strSQL[256] = {0};
	sprintf(strSQL, "SELECT * FROM COMM_ARCH WHERE CH_ID = %d", m_Id);
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return;

	CString strIpAddr = _T("");
	int nPort = -1;
	int nType = -1;
	CString strPhone = _T("");


	while (!record.IsEndEOF())
	{
		_variant_t val;

		record.GetFieldValue("IP_ADDR",val);
		strIpAddr = val.bstrVal;

		record.GetFieldValue("IP_PORT",val);
		nPort = val.intVal;

		record.GetFieldValue("CH_TYPE",val);
		nType = val.intVal;

		record.GetFieldValue("PHONE_NUM",val);
		strPhone = val.bstrVal;

		record.RecordMoveNext();
	}

	m_IpAddr = strIpAddr;
	mPort = nPort;


}