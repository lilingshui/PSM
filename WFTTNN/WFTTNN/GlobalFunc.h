#pragma once

class GlobalFunc
{
public:
	GlobalFunc(void);
	~GlobalFunc(void);

	static void convert_ChineseToCode(const string srcChinese,string & destCode);

	static void convert_CodeToChinese(const string srcCode,string & destChinese);

	static void convert_Meter_StringToAddr(const string strAddr,unsigned char *addr);//true 正序 false反
	static string convert_Meter_AddrToString(unsigned char *addr,int addrLen);//true 正序 false反

	static int convert_Meter_AddrToInt(const unsigned char *addr,int addrLen);
	
	static unsigned int convert_Terminal_StringToAddr(const char *strAddr,int ptclClassID);
	static string convert_Terminal_AddrToString(unsigned int nlogic,int ptclClassID);

	static void convert_Terminal_StringToPSW(const string strpsw,unsigned char *psw);
	static void convert_Sign(const string strSign,int &nFn,int signType = 0);//0 国电 1广电
	static void convert_PacketBuffer(const string strPacket,unsigned char *bPacket,int &bPacketSIze);
	static void convert_PacketBuffer(const unsigned char *bPacket,const int bPacketSize,string & strPacket);

	static void convert_StringToBuffer(const string str,unsigned char *psw);
};
