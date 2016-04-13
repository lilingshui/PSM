#include "ChannelMgr.h"
#include "TcpServerChannel.h"
#include "TcpClientChannel.h"
#include "PSTNClientChannel.h"
#include "..\ADODataBase.h"
#include "..\DBAccess.h"

CChannelMgr::CChannelMgr()
{
}

CChannelMgr::~CChannelMgr()
{
}

CChannelMgr *CChannelMgr::mInstance = NULL;
CChannelMgr *CChannelMgr::getInstance()
{
	if (mInstance == NULL)
		mInstance = new CChannelMgr();
	return mInstance;
}

CChannel *CChannelMgr::getChannel(unsigned int id)
{
	std::map <unsigned int,CChannel*>::iterator it = m_Channel_.find(id);
	if (it != m_Channel_.end())
	{
		return it->second;
	}

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return NULL;

	char strSQL[256] = {0};
	sprintf(strSQL, "SELECT * FROM COMM_ARCH WHERE CH_ID = %d", id);
	CADORecordSet record;
	if (!pConnect->ExecSQL(strSQL,record))
		return NULL;

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



	CChannel *channel = NULL;

	if (nType == 0)
	{
		channel = new CTcpServerChannel(id, strIpAddr, nPort, strPhone);
		m_Channel_[id] = channel;
	}
	else if (nType == 2 || nType == 1)
	{
		channel = new CTcpClientChannel(id, strIpAddr, nPort, strPhone);
		m_Channel_[id] = channel;
	}
	else if (nType == 3)
	{
		channel = new CPSTNClientChannel(id, strIpAddr, nPort, strPhone);
		m_Channel_[id] = channel;
	}
	else if (nType == 4)
	{
		return NULL;
	}

	return channel;
}

void CChannelMgr::Register(CChannelReceiver *pChannelRecv)
{
	if (pChannelRecv)
	{
		v_ChannelRecv_.push_back(pChannelRecv);
	}
}

void CChannelMgr::UnRegister(CChannelReceiver *pChannelRecv)	//界面不会delete
{

}

void CChannelMgr::OnReceive(int state, char *buf, int len, u_int64 strTermAddr)
{
	for (int i=0;i<v_ChannelRecv_.size();i++)
	{
		v_ChannelRecv_.at(i)->OnReceive(state, buf, len, strTermAddr);
	}
}