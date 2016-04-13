// TcpClientChannel.cpp : implementation file
//

#include "PSTNClientChannel.h"
#include "ChannelMgr.h"

// CTcpClientChannel

CPSTNClientChannel::CPSTNClientChannel(unsigned int Id, CString IpAddr, int Port, CString PhoneNum)
	:CChannel(Id, IpAddr, Port, PhoneNum)
{
	mPort = Port;
	m_IpAddr = IpAddr;
	mId = Id;
	csPhoneNumber = PhoneNum;
	//m_pHandler = NULL;
	ucState = IDLE;
	AfxSocketInit();
}

CPSTNClientChannel::~CPSTNClientChannel()
{
}

// CTcpClientChannel member functions
int CPSTNClientChannel::ConnectTo(u_int64 termAddr)
{
	//CPSTNClientHandler *pHandler = NULL;
	//if (m_pHandler == NULL)
	//{
	//	pHandler = new CPSTNClientHandler(termAddr,mId);
	//	if(pHandler->Connect(m_IpAddr, mPort) == TRUE)
	//	{
	//		if (Dial(pHandler) == 0)
	//		{
	//			CChannelMgr::getInstance()->OnReceive(2, NULL, 0, termAddr);
	//			m_pHandler = pHandler;
	//			return 0;
	//		}
	//		else
	//			return -1;
	//	}
	//	else
	//	{
	//		return -1;
	//	}
	//}
	//else
	//	return 0;
	if (ucState == IDLE)
	{
		SetIpAndPort();
		Create();
		if (this->Connect(m_IpAddr, mPort) == TRUE)
		{
			if (Dial() == 0)
			{
				ucState = ONLINE;
				CChannelMgr::getInstance()->OnReceive(2, NULL, 0, termAddr);
				return 0;
			}
			else
			{
				Close();
				return -1;
			}
		}
		return -1;
	}
	else if (ucState == ONLINE)
	{
		return 0;
	}

}

int CPSTNClientChannel::Disonnect(u_int64 termAddr)
{
	//if (m_pHandler != NULL)
	//{
	//	CChannelMgr::getInstance()->OnReceive(1, NULL, 0, termAddr);
	//	char buf[32] = {0};
	//	strcat(buf, "+++");
	//	m_pHandler->Send(buf, strlen("+++"));
	//	Sleep(3000);
	//	memset(buf,0,32);
	//	strcat(buf, "ATH0\r\n");
	//	m_pHandler->Send(buf, strlen("ATH0\r\n"));
	//	Sleep(100);
	//	m_pHandler->Close();
	//	m_pHandler = NULL;
	//}
	//else
	//{
	//	return -1;
	//}
	if (ucState == ONLINE)
	{
		CChannelMgr::getInstance()->OnReceive(1, NULL, 0, termAddr);
		ucState = IDLE;
		char buf[32] = {0};
		strcat(buf, "+++");
		Send(buf, strlen("+++"));
		Sleep(3000);
		memset(buf,0,32);
		strcat(buf, "ATH0\r\n");
		Send(buf, strlen("ATH0\r\n"));
		Sleep(100);
		Close();
	}
	return 0;
	
}

int CPSTNClientChannel::Write(char *buf, int len, u_int64 termAddr)
{
	//CPSTNClientHandler *pHandler = NULL;
	////if (CNetHandlerMgr::getInstance()->FindHandler(&pHandler, termAddr) == TRUE)
	////{
	////	return 0;
	////}
	//if (m_pHandler == NULL)
	//{
	//	pHandler = new CPSTNClientHandler(termAddr,mId);

	//	if(pHandler->Connect(m_IpAddr, mPort) == TRUE)
	//	{
	//		//CNetHandlerMgr::getInstance()->InsertHandler(termAddr, pHandler);
	//		if (Dial(pHandler) == 0)
	//		{
	//			CChannelMgr::getInstance()->OnReceive(2, NULL, 0, termAddr);
	//			m_pHandler = pHandler;
	//			int lenWrite = pHandler->Send(buf, len);
	//			return lenWrite;
	//		}
	//		else
	//			return -1;
	//	}
	//	else
	//	{
	//		delete pHandler;
	//		return -1;
	//	}
	//}
	//else if (m_pHandler != NULL)
	//{
	//	if (m_pHandler->m_id != termAddr)
	//	{
	//		//m_pHandler->Close();
	//		//pHandler = new CPSTN(termAddr, m_Id);
	//		//if(pHandler->Connect(m_IpAddr, mPort) == TRUE)
	//		//{
	//		//	int lenWrite = pHandler->Send(buf, len);
	//		//	m_pHandler = pHandler;
	//		//	return lenWrite;
	//		//}
	//		//else
	//		//{
	//		//	delete pHandler;
	//		//	return -1;
	//		//}
	//	}
	//	else
	//	{
	//		int lenWrite = m_pHandler->Send(buf, len);
	//		return lenWrite;
	//	}
	//}
	if (ucState == ONLINE)
	{
		return Send(buf, len);
	}
	else if(ucState == IDLE)
	{
		Create();
		if (this->Connect(m_IpAddr, mPort) == TRUE)
		{
			if (Dial() == 0)
			{
				CChannelMgr::getInstance()->OnReceive(2, NULL, 0, termAddr);
				ucState = ONLINE;
				return Send(buf, len);
			}
			else
			{
				Close();
				return -1;
			}
		}
		return -1;
	}
	return -1;
}

int CPSTNClientChannel::Dial()
{
	char buf[32] = {0};
	strcat(buf, "ATDT");
	strcat(buf, csPhoneNumber.GetBuffer(0));
	strcat(buf, "\r\n");
	this->Send(buf, csPhoneNumber.GetLength() + strlen("ATDT\r\n"));
	time_t ttime = time(NULL);
	char readBuf[1024] = {0};
	while (time(NULL) - ttime < 120)
	{
		int readLen = Receive(readBuf, 1024);
		if (readLen > 0)
		{
			if (JudgeDialResult(readBuf, readLen) >= 0)
				return 0;
			else
				return -1;
		}
	}
	return -1;
}

int CPSTNClientChannel::JudgeDialResult(char *rBuf, unsigned int rLen)
{
		unsigned char buf[512];
		char szResult[512];
		short value = 0;

		memset(szResult,0,sizeof(szResult));
		memset(buf,0,sizeof(buf));

		int idx = 0;
		
		idx += rLen;
		if (idx >= 512)
		{
			idx = 512;
		}
		for(int i=0; i<idx; i++)
		{
			memcpy(buf,rBuf,idx);
			if((buf[i] == '\0') && (i != (idx - 2)))
				buf[i]='\t';
		}

		char *temp = strchr((char*)buf,'\r');

		if(temp)
		{
			int lenTmp = idx - (int)(temp - (char*)buf);
			lenTmp = (lenTmp >= 512 ? 512 : lenTmp);
			memcpy(szResult,temp,lenTmp);
			value = 1;
		}

		_strupr(szResult);

		int state = 0;
		if(strstr(szResult,"CONNEC") != NULL)
		{
			return 0;
		}
		else
			return -1;
}


//void CPSTNClientChannel::SetState(unsigned char state)
//{
//	ucState = state;
//}
//
//unsigned char CPSTNClientChannel::GetState()
//{
//	return ucState;
//}
//
//void CPSTNClientChannel::SetClose()
//{
//	m_pHandler = NULL;
//}

void CPSTNClientChannel::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	ucState = IDLE;
	CChannelMgr::getInstance()->OnReceive(CChannelMgr::OFFLINE, NULL,0, mId);
	Close();
	__super::OnClose(nErrorCode);
}


void CPSTNClientChannel::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	char buf[1024] = {0};
	int len = Receive(buf,1024);

	if (len >= 0)
	{
		CChannelMgr::getInstance()->OnReceive(CChannelMgr::RECEIVING, buf,len, mId);

	}
	__super::OnReceive(nErrorCode);
}

void CPSTNClientChannel::SetIpAndPort()
{
	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return;

	char strSQL[256] = {0};
	sprintf(strSQL, "SELECT * FROM COMM_ARCH WHERE CH_ID = %d", mId);
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
	csPhoneNumber = strPhone;


}