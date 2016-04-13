#ifndef LOGMGR_H_INC
#define LOGMGR_H_INC

#include <fstream>

class CLogMgr {
public:
	static CLogMgr &GetInstanse();
	void LogPacket(const char *type, const unsigned char *buf, int len);

private:
	CLogMgr();
	~CLogMgr();	

	CLogMgr(CLogMgr &) {}

private:
	std::fstream m_logPacket;
	std::string m_strPackeName;
};

#endif