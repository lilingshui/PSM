#pragma once
#include<map>

#include "ptcl_module.h"
#include "ptcl_packet_csg.h"


class  ptcl_module_csg : public ptcl_module
{
protected:
	ptcl_module_csg(void);
	virtual ~ptcl_module_csg(void);
public:
	//应用层功能码定义
	enum AFN_Code
	{
		AFN_ACK						=	0x00, //确认 否认
		AFN_CONNECT_TEST			=	0x02, //链路接口检测
		AFN_SET_PARAM				=	0x04, //写参数
		AFN_ID_PSW					=	0x06, //身份确认及密钥协商
		AFN_READ_PARAM				=	0x0A, //读参数
		AFN_READ_CUR_DATA		    =	0x0C, //读当前数据
		AFN_READ_DATA_HISTORY		=	0x0D, //读历史数据
		AFN_READ_EVENT_RECORD		=	0x0E, //读事件记录
		AFN_FILE_TRANS				=	0x0F, //文件传输
		AFN_RELAY_STATION_COMMAND	=	0x10, //中继转发
		AFN_READ_DATA_TASK		    =	0x12, //读任务数据
		AFN_READ_DATA_EVENT			=	0x13, //读告警数据
		AFN_CASCADE_COMMAND			=	0x14, //级联命令
		
	};

	//数据单元
	struct pnfn_data
	{
		int pn;
		int fn;
	};

	//数据单元结构
	struct pnfn_struct
	{
		unsigned char pn[2];
		unsigned char fn[4];
	};

	//格式化信息点和数据标识
	static int format_pnfn(pnfn_data *pnfn ,unsigned char *buffer);
	static int parse_pnfn(unsigned char *buffer,pnfn_data *pnfn);
	static int parse_pn(unsigned char *buffer,int &i_pn);
	static int parse_fn(unsigned char *buffer,int &i_fn);




	//时间标识
	struct tp_data
	{		
		unsigned char day;//日
		unsigned char hour;//时
		unsigned char min;//分
		unsigned char sec;//秒

		unsigned char time_out;//超时时间
	};
	//时间标签
	static int format_tp(tp_data *tp,unsigned char *buffer);
	static int parse_tp(unsigned char *buffer,tp_data *tp);


	//格式化计量点信息
	static int format_tnm(int* v_tn,int count_tn,unsigned char *buffer);
	//解析计量点信息
	static int parse_tnm(unsigned char *buffer,int* v_tn,int &count_tn);

	//历史数据、事件记录、确认/否认数据单元
	struct pnfn_data_uint
	{
		pnfn_data pnfn;
		unsigned char data_buffer[1024];
		int len_data;
	};



	static ptcl_module_csg *get_instance();



	////确认、否认//////////////////////////////////////////////////////////////////////////
	void format_packet_confirm_deny_frame( ptcl_packet_csg *packet,
		                              pnfn_data_uint *v_data_uint,
									  int count_data_uint,
									  tp_data *tp = NULL);

	///全部确认//////////////////////////////////////////////////////////////////////////////
	void format_packet_confirm_frame(ptcl_packet_csg *packet);



	////读当前数据//////////////////////////////////////////////////////////////////////////
	void format_packet_read_realtime_data(	ptcl_packet_csg *packet,
		                                pnfn_data *v_pnfn, int pnfn_count,
										tp_data *tp = NULL);
	/*
	packet: 只需要填写部分成员，终端地址,并且返回组装好的报文
	v_pnfn:信息点标识及数据标识集合
	*/


	////读历史数据//////////////////////////////////////////////////////////////////////////
	void format_packet_read_history_data(	ptcl_packet_csg *packet,
											pnfn_data_uint *v_data_uint,
											int count_data_uint,
											tp_data *tp = NULL);
	/*
	pPacket: 只需要填写部分成员终端地址
			 并且返回组装好的报文
	v_data_uint:数据单元集合(data_buffer数据开始及结束时间)
	*/

	////读事件记录/////////////////////////////////////////////////////////////////////////
	void format_packet_read_event_data(	ptcl_packet_csg *packet,
											pnfn_data_uint *v_data_uint,
											int count_data_uint,
											tp_data *tp = NULL);

	////读告警数据/////////////////////////////////////////////////////////////////////////
	void format_packet_read_alarm_data(	ptcl_packet_csg *packet,
											pnfn_data_uint *v_data_uint,
											int count_data_uint,
											tp_data *tp = NULL);


	////写参数////////////////////////////////////////////////////////////////////////////
	void format_packet_set_param(	ptcl_packet_csg * packet,
									unsigned char *password,
									pnfn_data_uint *v_data_unit,
									int count_data_unit,tp_data *tp = NULL);

	////读参数///////////////////////////////////////////////////////////////////////////
	void format_packet_read_parameter(	ptcl_packet_csg *packet,
		                                pnfn_data *v_pnfn, int pnfn_count,
										tp_data *tp = NULL);

	////中继转发////////////////////////////////////////////////////////////////////////////
	void format_packet_transmit(	ptcl_packet_csg * packet,
		                            unsigned char delayType,
									unsigned char port,
									unsigned char btl,
									unsigned char xlfs,
									unsigned char sjw,
									unsigned char tzw,
									unsigned char cssj,
									unsigned char *delayBuf, 
									int lenDelayBuf,
									unsigned char *password,
									tp_data *tp = NULL);
	


protected:
	static ptcl_module_csg *instance_;

	std::map <unsigned long,unsigned char> m_mapSeq;

	unsigned char getFrameSeq(unsigned int nAddr);

};
