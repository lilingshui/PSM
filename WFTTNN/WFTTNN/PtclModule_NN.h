#pragma once


#include "PtclModule.h"
#include "PtclPacket_NN.h"
#pragma warning (disable:4251)

class PTCLCODEC_EXPORT CPtclModule_NN : public CPtclModule
{
public:
	CPtclModule_NN(void);
	virtual ~CPtclModule_NN(void);

	

	//应用层功能码定义
	enum AFN_Code
	{
		AFN_ACK						=	0x00, //确认 否认
		AFN_CONNECT_TEST			=	0x02, //链路接口检测
		AFN_SET_PARAM				=	0x04, //写参数
		AFN_ID_PSW					=	0x06, //身份确认及密钥协商
		AFN_READ_PARAM				=	0x0A, //读参数
		AFN_READ_DATA_REALTIME		=	0x0C, //读当前数据
		AFN_READ_DATA_HISTORY		=	0x0D, //读历史数据
		AFN_READ_EVENT_RECORD		=	0x0E, //读事件记录
		AFN_FILE_TRANS				=	0x0F, //文件传输
		AFN_RELAY_STATION_COMMAND	=	0x10, //中继转发
		AFN_READ_DATA_TASK		    =	0x12, //读任务数据
		AFN_READ_DATA_EVENT			=	0x13, //读告警数据
		AFN_CASCADE_COMMAND			=	0x14, //级联命令
		
	};

	

	//数据单元结构
	struct PnFnStruct
	{
		unsigned char Pn[2];
		unsigned char Fn[4];
	};

	//数据单元
	struct FormatPnFnData
	{
		int Pn;
		int Fn;
	};

	//信息体标识
	struct ParsePnFnData
	{
		int vPn[64];
		int nCountPn;
		int vFn[8];
		int nCountFn;
	};

	static int FormatPnFn(CPtclModule_NN::FormatPnFnData *pPnFn ,unsigned char *buffer);
	static int ParsePnFn(unsigned char *buffer,ParsePnFnData *pPnFn);
	static int ParsePn(unsigned char *buffer,int &i_pn);

	//时间标识
	struct TPData
	{		
		unsigned char day;//日
		unsigned char hour;//时
		unsigned char min;//分
		unsigned char sec;//秒
		unsigned char time_out;//超时时间
	};
	//时间标签
	static int FormatTP(TPData *tp,unsigned char *buffer);
	static int ParseTP(unsigned char *buffer,TPData *tp);



	//事件计数器
	struct ECData
	{
		unsigned char importantEC;//重要事件计数器
		unsigned char genericEC;//普通事件计数器
	};

	//事件计数器结构
	struct ECStruct
	{
		unsigned char importantEC;
		unsigned char genericEC;
	};

	//格式化事件计数器
	static int FormatEC(ECData *pEC,unsigned char *pECBuf);
	//解析事件计数器
	static int ParseEC(unsigned char *pECBuf,ECData *pEC);


///////////////////////////////////////////////////////////////////////////////
	static CPtclModule_NN *getInstance();

	//设置调用者类型 //0:主站,采集使用 1:终端使用
	void setHostID(unsigned char hostID);

////////////////////////////////////////////////////////////////////////////////////////////////
	

////确认否认///////////////////////////////////////////////////////////////////////////
	//全部确认 全部否认
	void FormatPacket_Ack_All(	CPtclPacket_NN *pPacket,
								bool confirm_deny,
								TPData *pTP = NULL,
								ECData *pEC = NULL);
	/*
	pPacket: 只需要填写部分成员，终端地址，回应幁序号
			 并且返回组装好的报文
	confirm_deny: 1确认 0否认
	*/

	//部分确认 否认
	struct FormatAckDataUnit
	{
		FormatPnFnData PnFn;
		unsigned char error;
	};
	void FormatPacket_Ack_Part(	CPtclPacket_NN *pPacket,
								unsigned char bAckAFN,
								FormatAckDataUnit *vAck,int countAck,
								TPData *pTP = NULL,
								ECData *pEC = NULL);
	/*pPacket: 只需要填写部分成员，终端地址，回应幁序号
			 并且返回组装好的报文
	bAckAFN: 被确认帧的AFN
	vAck:被确认数据单元集合
	*/

	struct ParseAckDataUnit
	{
		ParsePnFnData PnFn;
		unsigned char error;
	};
	//解析确认否认报文
	int ParsePacket_Ack(unsigned char *pDataAreaBuf,int lenDataArea,
						int &Fn,int &AFN,CPtclModule_NN::ParseAckDataUnit *vAck,int &countAck);
	/*pDataAreaBuf 数据域报文  lenDataArea 数据域报文长度
	  FN 数据单元Fn 全部确认 2全部否认  3部分否认
	  vAck 部分否认时候的否认数据单元数组

	*/

////复位命令///////////////////////////////////////////////////////////////////////////
	void FormatPacket_Reset(	CPtclPacket_NN *pPacket,
								int Fn,
								unsigned char *pPSW,
								TPData *pTP = NULL);
	/*
	pPacket: 只需要填写部分成员，终端地址
			 并且返回组装好的报文
	Fn: 1:硬件初始化 2:数据区初始化 3:参数及其全体数据区初始化
	pPSW:终端访问密码
	*/

////心跳 登陆 注销///////////////////////////////////////////////////////////////////////////
	void FormatPacket_ConnectTest(	CPtclPacket_NN *pPacket,int Fn);
	/*
	pPacket: 只需要填写部分成员，终端地址
			 主站->服务器 主站ID为通讯ID 终端地址第3字节填写主站ID 
			 并且返回组装好的报文
	Fn:1:登陆 2:注销 3:心跳
	*/

////设置参数///////////////////////////////////////////////////////////////////////////
	struct DataUint
	{
		FormatPnFnData PnFn;
		unsigned char pDataBuf[1024];
		int lenDataBuf;
	};
	void FormatPacket_SetParam(	CPtclPacket_NN *pPacket,
								DataUint* pDataUint,int nCountDataUnit,
								unsigned char *pPSW,
								TPData *pTP = NULL);
	/*
	pPacket: 只需要填写部分成员，终端地址
			 并且返回组装好的报文
	pDataUint:参数数据单元集合
	*/

////控制命令///////////////////////////////////////////////////////////////////////////
	void FormatPacket_Command(	CPtclPacket_NN *pPacket,
								CPtclModule_NN::DataUint* pDataUint,int nCountDataUnit,
								unsigned char *pPSW,
								TPData *pTP = NULL);
	/*
	pPacket: 只需要填写部分成员，终端地址
			 并且返回组装好的报文
	pDataUint:控制命令数据单元集合
	*/

////查询参数///////////////////////////////////////////////////////////////////////////
	void FormatPacket_ReadParam(	CPtclPacket_NN *pPacket,
									FormatPnFnData *pPnFn,int nCountPnFn,
									TPData *pTP = NULL);

	//查询参数带数据单元的
	void FormatPacket_ReadParam_DataUnit( CPtclPacket_NN *pPacket,
										  DataUint* pDataUint,int nCountDataUnit,
										  TPData *pTP = NULL);

	/*
	pPacket: 只需要填写部分成员，终端地址
			 并且返回组装好的报文
	vPnFn:查询参数信息点集合
	*/

////读取1，2，3类数据///////////////////////////////////////////////////////////////////////////
	void FormatPacket_ReadData_RealTime(	CPtclPacket_NN *pPacket,
		                                    FormatPnFnData *pPnFn,int nCountPnFn,
		                                    TPData *pTP = NULL);
	/*
	pPacket: 只需要填写部分成员，终端地址
			 并且返回组装好的报文
	vPnFn:信息点集合
	*/

	void FormatPacket_ReadData_History(	CPtclPacket_NN *pPacket,
										DataUint* pDataUint,int nCountDataUnit,
										TPData *pTP = NULL);
	/*
	pPacket: 只需要填写部分成员 主站ID，终端地址
			 并且返回组装好的报文
	vHistory:数据单元集合
	*/

	void FormatPacket_ReadData_Event(	CPtclPacket_NN *pPacket,
										int Fn,unsigned char startID,unsigned endID,
										TPData *pTP = NULL);
	/*
	pPacket: 只需要填写部分成员，终端地址
			 并且返回组装好的报文
	Fn:1:重要事件 2:普通事件
	startID:事件起始标识
	endID:事件结束标识
	*/

////数据转发///////////////////////////////////////////////////////////////////////////
	void FormatPacket_Transmit(	CPtclPacket_NN *pPacket,
									unsigned char *pDelayBuf,int lenDelayBuf,
									unsigned char *pPSW,
									TPData *pTP = NULL);
	/*
	pPacket: 只需要填写部分成员，终端地址
			 并且返回组装好的报文
	vPnFn:查询参数信息点集合
	*/

	void CRC16( unsigned char *data ,int data_len ,int start_id ,int crc_key , unsigned char *crc_code);
	void GetCRC(unsigned char *ADDR, int frametype, int teamno, unsigned char *buf, unsigned char hostID, int CRCKey , unsigned char *CRC);
	int  GetPW(CPtclPacket_NN *pPacket, unsigned char *UserDataBuf, unsigned char *pPSW, unsigned char *CRC, int isEx = 0);
private:
	static CPtclModule_NN *m_pInstance;
protected:
	unsigned char m_hostID;
	map <unsigned long,unsigned char> m_mapSeq;

	unsigned char getFrameSeq(unsigned int nAddr);
	
	

};
