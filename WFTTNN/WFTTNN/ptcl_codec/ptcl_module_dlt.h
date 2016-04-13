#pragma once

#include "ptcl_module.h"
#include "ptcl_packet_dlt.h"

class  ptcl_module_dlt : public ptcl_module
{
protected:
	ptcl_module_dlt(void);
	virtual ~ptcl_module_dlt(void);

public:
	enum afn_code
	{
		AFN_ALL_TIMING			=	0x08,	//广播对时
		AFN_READ_DATA			=	0x01,	//读数据
		AFN_READ_NEXT_DATA		=	0x12,	//读后续数据
		AFN_READ_DEVICE_ADDR	=	0x13,	//读通信地址
		AFN_WRITE_DATA			=	0x14,	//写数据
		AFN_WRITE_DEVICE_ADDR	=	0x15,	//写通信地址
		AFN_FREEZE				=	0x16,	//冻结命令
		AFN_UPDATE_COM_BPS		=	0x17,	//更改通信速率
		AFN_UPDATE_PSW			=	0x18,	//修改密码
		AFN_MAX_DEMAND_ZERO		=	0x19,	//最大需量清零
		AFN_METER_ZERO			=	0x1A,	//电表清零
		AFN_ALARM_ZERO			=	0x1B	//事件清零
	};

	static ptcl_module_dlt *get_instance();

	////读当前数据///////////////////////////////////////////////////////////////////////
	void format_packet_read_data(	ptcl_packet_dlt *packet,unsigned int data_item);
	/*
	packet: 只需要填写部分成员 电表地址
			 并且返回组装好的报文
	data_item:数据标示
	*/

	
	////读曲线数据///////////////////////////////////////////////////////////////////////
	void format_packet_read_curve_data(	ptcl_packet_dlt *packet,unsigned int data_item,time_t data_time);
	/*
	packet: 只需要填写部分成员 电表地址
			 并且返回组装好的报文
	data_item:数据标示
	*/

	////广播校时///////////////////////////////////////////////////////////////////////
	void format_packet_set_time(	ptcl_packet_dlt *packet,time_t time);
	/*
	packet: 只需要填写部分成员 电表地址
			 并且返回组装好的报文
	data_item:数据标示
	*/



	////设置虚拟表运行电压、电流参数//////////////////////////////////////////////////////////
	void format_packet_write_I_U_data( ptcl_packet_dlt *packet,unsigned int data_item,
		                               short ua, short ub, short uc,
									   double ia,double ib,double ic,
									   unsigned char rs485,unsigned char time);

	////设置虚拟表其他事件记录参数//////////////////////////////////////////////////////////
	void format_packet_write_other_parameter( ptcl_packet_dlt *packet,unsigned int data_item,
		                                      int demandCount, int processCount, int openmeterCount, int dlCount,
											  unsigned char rs485);

	////启动虚拟表时钟与通信告警记录模拟//////////////////////////////////////////////////////////
	void format_packet_start_clock_communicate( ptcl_packet_dlt *packet,unsigned int data_item,
		                                        unsigned char rs485,unsigned char time);

	////设置虚拟表停止/启动走字//////////////////////////////////////////////////////////
	void format_packet_set_stop_start( ptcl_packet_dlt *packet,unsigned int data_item,
		                               unsigned char state,
									   unsigned char rs485,unsigned char time);






	
protected:
	static ptcl_module_dlt *instance_;
};