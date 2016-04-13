#include "StdAfx.h"
#include "LogMgr.h"
#include <string>
using namespace std;

static const char *GetNowTime()
{
	time_t tNow;
	struct tm *pNow;	
	tNow = time(NULL);
	pNow = localtime(&tNow);

	static char sTime[24] = {0};
	sprintf(sTime, "%d-%02d-%02d %02d:%02d:%02d", pNow->tm_year + 1900, pNow->tm_mon + 1, 
		pNow->tm_mday, pNow->tm_hour, pNow->tm_min, pNow->tm_sec);

	return sTime;
}

static const char *GetPacketFileName()
{	
	time_t tNow;
	struct tm *pNow;	
	tNow = time(NULL);
	pNow = localtime(&tNow);

	static char fName[256] = {0};
	sprintf(fName, ".\\log\\packet[%d-%d-%d]", pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday);
	
	return fName;
}

CLogMgr::CLogMgr()
{
	m_strPackeName = GetPacketFileName();
	m_logPacket.open(m_strPackeName, ios::app);
}

CLogMgr::~CLogMgr()
{
	if (m_logPacket.is_open())
		m_logPacket.close();
}

CLogMgr &CLogMgr::GetInstanse()
{
	static CLogMgr instance;
	return instance;
}

void CLogMgr::LogPacket(const char *type, const unsigned char *buf, int len)
{
	string temp = GetPacketFileName();
	if (temp.compare(m_strPackeName) != 0)
	{
		m_strPackeName = temp;
		m_logPacket.close();
		m_logPacket.open(m_strPackeName, ios::app);
	}

	m_logPacket << GetNowTime() << " " << type << ": ";

	string packet;
	char tempBuf[8] = {0};
	for (int i = 0; i < len; i++)
	{	
		sprintf(tempBuf, "%02X ", buf[i]);
		packet.append(tempBuf);
	}

	m_logPacket << packet << endl;
}