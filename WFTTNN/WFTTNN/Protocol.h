#pragma once

#define MAX_DATAMARK_SIZE 27
#define SEPRATOR ","
#define RTALLCONFIRM	2
#define RTCONFIRM		3

#define RTSUCCESS	1
#define PARSEERROR -1
#define RTALLDENY		-2
#define RTDENY		-3
#define RTDATAMARKERR -4
#define RTTMOUT		-5	//超时

#pragma pack (1)

typedef	struct 								// 帧头结构
{
	unsigned char	_head;								// 68H
    unsigned short int	_len;							// L
	unsigned short int	_lenex;							// L
	unsigned char	_headEx;							// 68H
    unsigned char   _ctrl;							    // 控制字
	unsigned char	_addr[7];							// 地址
	unsigned char   _afn;								// AFN
	unsigned char   _seq;								// SEQ
} FRAME_HEAD,		*LPFRAME_HEAD;

typedef	struct 								
{
	unsigned short int pn;			//信息点标识					
    unsigned long datamark;			//数据标识	
} PNFN,		*LPPNFN;

//返回数据结构
//返回数据头
typedef	struct 
{
	unsigned char   _afn;								// AFN
	unsigned char   _ctrl;							    // 控制字
	unsigned char	_addr[7];							// 地址
	short int size;										//PNFNEX 个数
}UPDATAHEAD,*LPUPDATAHEAD;

typedef	struct 
{
	unsigned short int	pn;			//信息点标识					
    unsigned long		datamark;			//数据标识		
	char				sdata[100];			//数据，字符串
}UPDATA,*LPUPDATA;

typedef	struct 	
{
	unsigned long datamark;			//数据标识
	short int blocknum;				//数据块数
	short int		ilen;			//长度
	short int decimnum;				//小数位数
	short int codestyle;			//码制0-16进制，1-BCD	
}DATAMARKDATA,		*LPDATAMARKDATA;

typedef	struct 	
{
	unsigned char value;
}ONEBYTE,		*LPONEBYTE; //1字节数据


//最大需量及发生时间
typedef	struct 
{
	unsigned char power[3];
	unsigned char tm[5];
}MAXREQTMIN,	*LPMAXREQTMIN; 

//数据时间
typedef	struct 
{
	unsigned char tm[6];
}DATATMIN,	*LPDATATMIN; 


//--------------------645协议头-------------------------
typedef	struct _FRAME645_HEAD								// 帧头结构
{
	unsigned char	_head;								// 68H
	unsigned char	_addr[6];							// 地址
	unsigned char	_headEx;							// 68H
	unsigned char   _ctrl;							    // 控制字
    unsigned char	_len;							// L
	
} FRAME645_HEAD,		*LPFRAME645_HEAD;

#pragma pack ()



LPDATAMARKDATA GetDatamarkdata(unsigned long datamark);
int GetDataMarkValue(LPDATAMARKDATA pDATAMARKDATA,unsigned char *datavalue,char *outstr);
int ParseRetFrame(unsigned char *inbuf,int inlen,unsigned char *outbuf,int *outlen);

int StringtoByte(char *instr,char *outstr,short int *outlen);
int HextoValue(char ch1,char ch2);
int chrtobyte(char ch);
int Convertstr(char *str,int ilen);
int TruncNum(char *instr,int ilen);
int BytetoString(unsigned char *instr,short int inlen,char *outstr);
int GetBCDValue(unsigned char *datavalue,int inlen,int decimnum,char *outstr);
int GetOrderBCDValue(unsigned char *datavalue,int inlen,int decimnum,char *outstr);