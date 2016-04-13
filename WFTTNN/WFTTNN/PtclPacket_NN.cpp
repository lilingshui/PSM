#include "stdafx.h"
#include "PtclPacket_NN.h"
//#include "PtclCodec.h"

CPtclPacket_NN::CPtclPacket_NN()
	: CPtclPacket()
{
	memset((void *)&m_frameHeader,0,sizeof(FrameHeaderStruct));
}

CPtclPacket_NN::~CPtclPacket_NN()
{

}

void CPtclPacket_NN::formatPacket()
{
	m_packetBufLen = 0;
	
	m_frameHeader.frameStart = START_FLAG_FRAME;
	m_frameHeader.dataAreaStart = START_FLAG_FRAME;
	m_frameHeader.lenDataArea = m_dataAreaBufLen + 10;
	m_frameHeader.lenDataArea_ = m_frameHeader.lenDataArea;

	int nHeaderBufSize = sizeof (FrameHeaderStruct);
	memcpy(m_bufPacket,(void *)&m_frameHeader,nHeaderBufSize);
	m_packetBufLen = nHeaderBufSize;
	
	if (m_dataAreaBufLen>0)
	{
		memcpy((void *)(m_bufPacket+nHeaderBufSize),(void *)m_bufDataArea,m_dataAreaBufLen);
		m_packetBufLen = m_packetBufLen + m_dataAreaBufLen;
	}

	m_bufPacket[m_packetBufLen++] = getCheckSum(m_bufPacket+6,m_frameHeader.lenDataArea);
	m_bufPacket[m_packetBufLen++] = END_FLAG_FRAME;
}

int CPtclPacket_NN::parsePacket()
{
	if (m_packetBufLen <= 0)
		return 0;

	int nHeadLen = sizeof(FrameHeaderStruct);
	if (m_packetBufLen <= nHeadLen)
	{
		return 0;
	}

	memcpy((void *)&m_frameHeader,m_bufPacket,nHeadLen);

	if (m_frameHeader.frameStart != START_FLAG_FRAME ||
		m_frameHeader.dataAreaStart != START_FLAG_FRAME)
	{
		return -1;
	}

	if (m_frameHeader.lenDataArea != m_frameHeader.lenDataArea_)
	{
		return -1;
	}
	
	int nLenDataArea = m_frameHeader.lenDataArea - 8;
	int nLenPacket = nLenDataArea + nHeadLen;
	
	if (m_packetBufLen < nLenPacket)
		return 0;

	unsigned char nCRC = getCheckSum(m_bufPacket+6,m_frameHeader.lenDataArea);
	if (nCRC != m_bufPacket[nHeadLen + nLenDataArea-2])
		return -1;

	if (m_bufPacket[nHeadLen + nLenDataArea - 1] != END_FLAG_FRAME)
		return -1;

	setDataAreaBuf(m_bufPacket+nHeadLen,nLenDataArea);

	m_packetBufLen = nLenDataArea + nHeadLen;

	return m_packetBufLen;
}

unsigned int CPtclPacket_NN::getDeviceAddr()
{
	unsigned int nAddr = 0;
	memcpy(&nAddr,m_frameHeader.deviceAddr,4);
	return nAddr;
}

void CPtclPacket_NN::setDeviceAddr(unsigned int deviceAddr)
{
	memcpy(m_frameHeader.deviceAddr,&deviceAddr,4);
}

unsigned char CPtclPacket_NN::getHostID()
{
	return m_frameHeader.hostID;
}

void CPtclPacket_NN::setHostID(unsigned char ID)
{
	m_frameHeader.hostID = ID;
}



unsigned char CPtclPacket_NN::getCtrlCodeArea()
{
	return m_frameHeader.ctrlCodeArea;
}

void CPtclPacket_NN::setCtrlCodeArea(unsigned char ctrlCodeArea)
{
	m_frameHeader.ctrlCodeArea = ctrlCodeArea;
}

unsigned char CPtclPacket_NN::getAFN_Ctrl()
{
	return m_frameHeader.AFN_Ctrl;
}

void CPtclPacket_NN::setAFN_Ctrl(unsigned char AFN_Ctrl)
{
	m_frameHeader.AFN_Ctrl = AFN_Ctrl;
}

unsigned char CPtclPacket_NN::getFCV()
{
	return m_frameHeader.FCV;
}

void CPtclPacket_NN::setFCV(unsigned char FCV)
{
	m_frameHeader.FCV = FCV;
}

unsigned char CPtclPacket_NN::getFCB_ACD()
{
	return m_frameHeader.FCB_ACD;
}

void CPtclPacket_NN::setFCB_ACD(unsigned char FCB_ACD)
{
	m_frameHeader.FCB_ACD = FCB_ACD;
}

unsigned char CPtclPacket_NN::getPRM()
{
	return m_frameHeader.PRM;
}

void CPtclPacket_NN::setPRM(unsigned char PRM)
{
	m_frameHeader.PRM = PRM;
}

unsigned char CPtclPacket_NN::getDIR()
{
	return m_frameHeader.DIR;
}

void CPtclPacket_NN::setDIR(unsigned char DIR)
{
	if (DIR >= 1)
		m_frameHeader.DIR = 1;
	else
		m_frameHeader.DIR = 0;
}

unsigned char CPtclPacket_NN::getAFN()
{
	return m_frameHeader.AFN;
}

void CPtclPacket_NN::setAFN(unsigned char afn)
{
	m_frameHeader.AFN = afn;
}


unsigned char CPtclPacket_NN::getSeqArea()
{
	return m_frameHeader.seqArea;
}

void CPtclPacket_NN::setSeqArea(unsigned char seqArea)
{
	m_frameHeader.seqArea = seqArea;
}

int CPtclPacket_NN::getFrameSeq()
{
	return m_frameHeader.frameSeq;
}

void CPtclPacket_NN::setFrameSeq(unsigned char seq)
{
	m_frameHeader.frameSeq = seq;
}

unsigned char CPtclPacket_NN::getCON()
{
	return m_frameHeader.CON;
}

void CPtclPacket_NN::setCON(unsigned char CON)
{
	m_frameHeader.CON = CON;
}

unsigned char CPtclPacket_NN::getFIN()
{
	return m_frameHeader.FIN;
}

void CPtclPacket_NN::setFIN(unsigned char FIN)
{
	m_frameHeader.FIN = FIN;
}

unsigned char CPtclPacket_NN::getFRI()
{
	return m_frameHeader.FRI;
}

void CPtclPacket_NN::setFRI(unsigned char FRI)
{
	m_frameHeader.FRI = FRI;
}

unsigned char CPtclPacket_NN::getTPV()
{
	return m_frameHeader.TPV;
}

void CPtclPacket_NN::setTPV(unsigned char TPV)
{
	m_frameHeader.TPV = TPV;
}

int CPtclPacket_NN::getAUXLen()
{
	int lenAUX = 0;
	//计算附加信息长度
	unsigned char FCB_ACD = m_frameHeader.FCB_ACD;
	unsigned char DIR = m_frameHeader.DIR;
	unsigned char AFN = m_frameHeader.AFN;
	unsigned char TPV = m_frameHeader.TPV;
	//if (FCB_ACD == 1 && DIR == 1) //上行报文 而且 要求访问
	//{
	//	lenAUX = lenAUX + sizeof(ECStruct);
	//}
	if (DIR == 0 && (AFN == 0x01 || AFN == 0x09 )) //下行报文 而且 要求密码
	{
		lenAUX = lenAUX + 16;
	}
	if (TPV)//时间标识
	{
		lenAUX = lenAUX + sizeof(TPStruct);
	}
	return lenAUX;
}


unsigned char CPtclPacket_NN::getCheckSum(unsigned char *dateBuf,int dateBufLen)
{
	if (dateBuf == NULL)
		return 0;

	unsigned char bCheckSun = 0;

	for(int i = 0; i < dateBufLen; i++)
		bCheckSun= bCheckSun + dateBuf[i];

	return bCheckSun;
}