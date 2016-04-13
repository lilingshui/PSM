#pragma once

#include "PtclPacket.h"

static const int START_FLAG_FRAME = 0x68;
static const int END_FLAG_FRAME = 0x16;

class PTCLCODEC_EXPORT CPtclPacket_NN : public CPtclPacket
{
public:
	CPtclPacket_NN();
	virtual ~CPtclPacket_NN();

	//设置用户区域 
	//注意 此处用户数据域指从 序列域 到 校验和 之间的数据

	//设置好packet所有属性，生成报文放在m_bufPacket中
	virtual void formatPacket();
	/*
	长度域自动计算，需要填写控制码域，主站ID，终端地址，APN，序列域
	*/


	//报文长度不够:0 
	//报文格式不对:<0
	//读取报文成功:报文长度
	virtual int parsePacket();


	//终端地址
	virtual unsigned int getDeviceAddr();
	void setDeviceAddr(unsigned int deviceAddr);

	/////////////////////////////////////////////////////////
	//主站地址
	virtual unsigned char getHostID();
	void setHostID(unsigned char ID);

	///////////////////////////////////////////////////////////
	//控制码域
	unsigned char getCtrlCodeArea();
	void setCtrlCodeArea(unsigned char ctrlCodeArea);
	//控制码域
	unsigned char getAFN_Ctrl();
	void setAFN_Ctrl(unsigned char AFN_Ctrl);
	//帧计数有效位
	unsigned char getFCV();
	void setFCV(unsigned char FCV);
	//帧计数位 或者 请求访问位
	unsigned char getFCB_ACD();
	void setFCB_ACD(unsigned char FCB_ACD);
	//启动标志位
	unsigned char getPRM();
	void setPRM(unsigned char PRM);
	//传输方向位
	unsigned char getDIR();
	void setDIR(unsigned char DIR);

	//应用层功能码
	unsigned char getAFN();
	void setAFN(unsigned char AFN);

	//序列域
	unsigned char getSeqArea();
	void setSeqArea(unsigned char seqArea);
	//帧序号
	virtual int getFrameSeq();
	void setFrameSeq(unsigned char seq);
	//请求确认标志位
	unsigned char getCON();
	void setCON(unsigned char CON);
	//结束帧标志
	unsigned char getFIN();
	void setFIN(unsigned char FIN);
	//首帧标志
	unsigned char getFRI();
	void setFRI(unsigned char FRI);
	//帧时间标签有效位
	unsigned char getTPV();
	void setTPV(unsigned char TPV);

	int getAUXLen();
protected:
	
#pragma pack(push, 1)
//报文头结构体
	struct FrameHeaderStruct
	{
		unsigned char frameStart;//0x68
		unsigned short int lenDataArea;//长度L 控制域、地址域、用户数据长度的字节总数
		unsigned short int lenDataArea_;
		unsigned char dataAreaStart;//0x68
		union 
		{
			struct
			{
				unsigned char	AFN_Ctrl : 4 ,//D0--D3控制域功能码
                                FCV		 : 1 ,//D4帧计数有效位
                                FCB_ACD	 : 1 ,//D5帧计数位 或者 请求访问位
                                PRM		 : 1 ,//D6启动标志位
                                DIR		 : 1 ;//D7传输方向位
			};
			unsigned char ctrlCodeArea;
		};
		unsigned char deviceAddr[6];//终端逻辑地址
		unsigned char hostID;
		unsigned char AFN;
		union 
		{
			struct
			{
				unsigned char    frameSeq	:	 4 ,//D0--D3帧序号
                                 CON		:	 1 ,//D4请求确认标志位
                                 FIN		:	 1 ,//D5结束帧标志
                                 FRI		:	 1 ,//D6首帧标志
                                 TPV		:	 1 ;//D7帧时间标签有效位
			};
			unsigned char seqArea;
		};
	};




//事件计数器结构
	struct ECStruct
	{
		unsigned char importantEC;
		unsigned char genericEC;
	};

//时间标识结构
	struct TPStruct
	{
		unsigned char sec;
		unsigned char min;
		unsigned char hour;
		unsigned char day;
		unsigned char timeOut;
	};


#pragma pack(pop)

	FrameHeaderStruct m_frameHeader;

	unsigned char getCheckSum(unsigned char *dateBuf,int dateBufLen);
};


