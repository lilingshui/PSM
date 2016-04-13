#include "stdafx.h"
#include "PtclPacket.h"

CPtclPacket::CPtclPacket()
{
	memset(m_bufPacket,0,MAX_PACKET_BUF_SIZE);
	m_packetBufLen = 0;

	memset(m_bufDataArea,0,MAX_DATAAREA_BUF_SIZE);
	m_dataAreaBufLen = 0;
}

CPtclPacket::~CPtclPacket()
{

}


unsigned char *CPtclPacket::getPacketBuf(unsigned char *packetBuf,int &lenPacketBuf)
{
	if (packetBuf==NULL || lenPacketBuf<=0)
	{
		lenPacketBuf = 0;
	}
	else
	{
		if (lenPacketBuf > m_packetBufLen)
			lenPacketBuf = m_packetBufLen;

		memcpy(packetBuf,m_bufPacket,lenPacketBuf);
	}

	return m_bufPacket;
}

void CPtclPacket::setPacketBuf(unsigned char *packetBuf,int lenPacketBuf)
{
	memset(m_bufPacket,0,MAX_PACKET_BUF_SIZE);
	m_packetBufLen = 0;
	if (packetBuf == NULL || lenPacketBuf <= 0)
	{
		return ;
	}

	if (lenPacketBuf > MAX_PACKET_BUF_SIZE)
		lenPacketBuf = MAX_PACKET_BUF_SIZE;

	memcpy(m_bufPacket,packetBuf,lenPacketBuf);
	m_packetBufLen = lenPacketBuf;

}

unsigned char *CPtclPacket::getDataAreaBuf(unsigned char *dataBuf,int &LenDataBuf)
{
	if (dataBuf==NULL || LenDataBuf <= 0)
	{
		LenDataBuf = 0;
	}
	else
	{
		if (LenDataBuf > m_dataAreaBufLen)
			LenDataBuf = m_dataAreaBufLen;

		memcpy(dataBuf,m_bufDataArea,LenDataBuf);
	}

	return m_bufDataArea;
}

void CPtclPacket::setDataAreaBuf(unsigned char *dataBuf,int LenDataBuf)
{
	memset(m_bufDataArea,0,MAX_DATAAREA_BUF_SIZE);
	m_dataAreaBufLen = 0;
	if (dataBuf == NULL || LenDataBuf <= 0)
	{
		return ;
	}

	if (LenDataBuf > MAX_DATAAREA_BUF_SIZE)
		LenDataBuf = MAX_DATAAREA_BUF_SIZE;

	memcpy(m_bufDataArea,dataBuf,LenDataBuf);
	m_dataAreaBufLen = LenDataBuf;
}

int CPtclPacket::getPacketBufLen()
{
	return m_packetBufLen;
}

int CPtclPacket::getDataAreaBufLen()
{
	return m_dataAreaBufLen;
}

unsigned char *CPtclPacket::getPacketBuf()
{
	return m_bufPacket;
}

unsigned char *CPtclPacket::getDataAreaBuf()
{
	return m_bufDataArea;
}
