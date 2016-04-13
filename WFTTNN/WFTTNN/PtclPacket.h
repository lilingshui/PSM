/*
作者：李楠(QQ10568617 linan1003@163.com 2009/03)
类描述：电力通讯规约报文基类，便于规约扩展。报文结构相同的规约对应一个CPtclPacket
		如国电04、05大用户对应CPtclPacket_Official 浙江广东大用户规约对应CPtclPacket_ZJ
		存储报文内容（packet_buffer）和数据域（data_buffer）
		为子类提供统一的纯虚接口
*/
#pragma once

#include "PtclCodec_Global.h"

static const int MAX_PACKET_BUF_SIZE = 2048;
static const int MAX_DATAAREA_BUF_SIZE = 2048;

class PTCLCODEC_EXPORT CPtclPacket  
{
public:
	CPtclPacket();
	virtual ~CPtclPacket();

	//得到报文内容和长度
	unsigned char *getPacketBuf(unsigned char *packetBuf,int &lenPacketBuf);
	//设置报文内容和长度
	void setPacketBuf(unsigned char *packetBuf,int lenPacketBuf);
	
	//得到数据域内容和长度
	unsigned char * getDataAreaBuf(unsigned char *dataBuf,int &LenDataBuf);
	//设置数据域内容和长度
	void setDataAreaBuf(unsigned char *dataBuf,int LenDataBuf);

	//得到报文长度
	int getPacketBufLen();
	//得到数据域长度
	int getDataAreaBufLen();

	//得到报文内容
	unsigned char *getPacketBuf();
	//得到数据域内容
	unsigned char *getDataAreaBuf();

	//设置好packet所有子项以后，组装一个报文
	virtual void formatPacket() = 0;

	//解析一个报文
	//报文长度不够:0 
	//报文格式不对:<0
	//读取报文成功:>0
	virtual int parsePacket() = 0;

	//得到报文序号
	virtual int getFrameSeq() = 0;
	//得到终端逻辑地址
	virtual unsigned int getDeviceAddr() = 0;
	//得到主站ID
	virtual unsigned char getHostID() = 0;

protected:

	unsigned char m_bufPacket[MAX_PACKET_BUF_SIZE];
	unsigned char m_bufDataArea[MAX_DATAAREA_BUF_SIZE];
	int m_packetBufLen;
	int m_dataAreaBufLen;
	
};

