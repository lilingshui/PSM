#include "StdAfx.h"
#include "GlobalFunc.h"
//#include <Global_Defines.h>

GlobalFunc::GlobalFunc(void)
{
}

GlobalFunc::~GlobalFunc(void)
{
}

void GlobalFunc::convert_ChineseToCode(const string srcChinese,string & destCode)
{
	/*
	const char *p = srcChinese.c_str();
	int len = (int)strlen(p);
	char buf[8];
	destCode = "";
	for (int n=0;n<len;n++)
	{
		sprintf(buf,"%02X",(unsigned char)p[n]);
		destCode = destCode + string(buf);
	}
	*/
}

void GlobalFunc::convert_CodeToChinese(const string srcCode,string & destChinese)
{
	/*
	char buf[1024*20];
	memset(buf,0,1024*20);

	const char *p = srcCode.c_str();
	int len = (int)strlen(p);

	len = len/2;

	for (int n=0;n<len;n++)
	{
		sscanf((char *)(p+n*2),"%02X",&buf[n]);
	}

	destChinese = string(buf);
	*/
}

unsigned int GlobalFunc::convert_Terminal_StringToAddr(const char *strAddr,int ptclClassID)
{
	if (strAddr == NULL)
		return 0;

	unsigned int nlogic = 0;
	sscanf(strAddr,"%08X",&nlogic);

	/*
	if (ptclClassID == Ptcl_Class_719)
	{
		return nlogic;
	}
	*/

	unsigned char *p = (unsigned char *)&nlogic;
	std::reverse(p,p+4);

	if ( ptclClassID == 7 )//Ptcl_Class_ZJ
	{
		p = (unsigned char *)&nlogic;
		unsigned char b = p[2];
		p[2] = p[3];
		p[3] = b;

		return nlogic;
	}
	/*
	else if (ptclClassID == Ptcl_Class_Official)//Ptcl_Class_ZJ
	{
		p = (unsigned char *)&nlogic;
		unsigned char b = p[2];
		p[2] = p[3];
		p[3] = b;

		b = p[0];
		p[0] = p[1];
		p[1] = b;

		return nlogic;
	}
	*/

	return nlogic;
}

string GlobalFunc::convert_Terminal_AddrToString(unsigned int nlogic,int ptclClassID)
{
	unsigned char *p = (unsigned char *)&nlogic;
	/*
	if (ptclClassID == Ptcl_Class_ZJ || Ptcl_Class_EQ)//Ptcl_Class_ZJ
	{
		unsigned char b = p[2];
		p[2] = p[3];
		p[3] = b;
	}
	else if (ptclClassID == Ptcl_Class_Official)//Ptcl_Class_ZJ
	{
		unsigned char b = p[2];
		p[2] = p[3];
		p[3] = b;

		b = p[0];
		p[0] = p[1];
		p[1] = b;
	}
	*/

	p = (unsigned char *)&nlogic;
	std::reverse(p,p+4);

	char strAddr[20];
	memset(strAddr,0,20);
	sprintf(strAddr,"%08X",nlogic);
	
	return string(strupr(strAddr));
}

int GlobalFunc::convert_Meter_AddrToInt(const unsigned char *addr,int addrLen)
{
	int mod;
	if (addrLen % 2 != 0)
	{
		mod = addrLen + 1;
	}
	else
	{
		mod = addrLen;
	}
	
	int by = 1;
	int nMeterAddr = 0;
	char bBuff[4];
	for (int i = 0; i < mod/2; i++)
	{
		memset(bBuff, 0, 2);
		sprintf(bBuff, "%2X", (unsigned char)addr[i]);
		nMeterAddr = nMeterAddr + atoi(bBuff) * by;
		by = by * 100; 
	}
	return nMeterAddr;
}

void GlobalFunc::convert_Meter_StringToAddr(const string strAddr,unsigned char *addr)
{

	const char *p = strAddr.c_str();
	int len = (int)strlen(p);
	if (len > 12)
		return ;

	char temp[20];
	memset(temp,0,20);
	int n = 0;
	for (n=len+1;n<=12;n++)
		strcat(temp,"0");
	
	char meterAddr[20];
	memset(meterAddr,0,20);
	strcat(meterAddr,temp);
	strcat(meterAddr,p);

	for (int j=0;j<6;j++)
	{
		unsigned int b = 0;
		sscanf(meterAddr+n*2,"%02X",&b);
	
		addr[6-1-j] = (unsigned char )b;
	}
}


string GlobalFunc::convert_Meter_AddrToString(unsigned char *addr,int addrLen)
{
	char strAddr[20];
	memset(strAddr,0,20);
	if (addrLen > 6)
		return string(strAddr);

	for (int n=0;n<addrLen;n++)
	{
		char temp[8];
		sprintf(temp,"%02X",addr[addrLen-1-n]);
		strcat(strAddr,temp);
	}

	return string(strAddr);
}



void GlobalFunc::convert_Terminal_StringToPSW(const string strpsw,unsigned char *psw)
{
	string strpassword = strpsw;
	int strlen = strpassword.length();
	
	if (strlen > 8)
		return ;

	if (strlen < 4)
	{
		string strZero = "";
		for(int i=0;i<4-strlen;i++)
			strZero += "0";
		strpassword = strZero+strpassword; 

		strlen = 4;
	}

	const char *p = strpassword.c_str();
	strlen = strlen/2;
	for (int n=0;n<strlen;n++)
	{
		sscanf((char *)(p+n*2),"%02X",&psw[n]);
	}
}


void GlobalFunc::convert_Sign(const string strSign,int &nFn,int signType)
{
	nFn = 0;
	const char *p = strSign.c_str();
	int nLen = (int)strlen(p);
	if (signType == 0)
	{
		if (nLen < 2)
			return ;

		sscanf((char *)(p+(nLen-2)),"%02X",&nFn);
	}
	else if (signType == 1)
	{
		if (nLen < 4)
			return ;
		int n1;
		char temp[4];
		memset(temp,0,4);
		strncpy(temp,p+(nLen-4),2);
		sscanf((char *)temp,"%02X",&n1);

		int n0;
		memset(temp,0,4);
		strncpy(temp,p+(nLen-2),2);
		sscanf((char *)temp,"%02X",&n0);

		nFn = n1*256 + n0;
	}
	else if (signType == 50)
	{
		if (nLen < 8)
			return ;

		char temp[20];
		memset(temp,0,20);
		strncpy(temp,p,8);

		sscanf(temp,"%08X",&nFn);
	}
}

void GlobalFunc::convert_PacketBuffer(const string strPacket,unsigned char *bPacket,int &bPacketSIze)
{
	/*
	bPacketSIze = 0;

	string s = strPacket;
	int len = (int)s.length();

	char b = s.at(len-1);
	if (b != ' ')
	{
		s = s + " ";
		len++;
	}
	
	len = len/3;

	const char *p = s.c_str();
	for (int n=0;n<len;n++)
	{
		sscanf((char *)(p+n*3),"%02X",&bPacket[n]);
		bPacketSIze++;
	}
	*/
}

void GlobalFunc::convert_PacketBuffer(const unsigned char *bPacket,const int bPacketSize,string & strPacket)
{
	char buf[8];
	for (int n=0;n<bPacketSize;n++)
	{
		sprintf(buf,"%02X ",(unsigned char)bPacket[n]);
		//strPacket = strPacket + string(buf);
	}
}

void GlobalFunc::convert_StringToBuffer(const string str,unsigned char *psw)
{
	string strpassword = str;
	int strlen = strpassword.length();

	const char *p = strpassword.c_str();
	strlen = strlen/2;
	for (int n=0;n<strlen;n++)
	{
		sscanf((char *)(p+n*2),"%02X",&psw[n]);
	}
}