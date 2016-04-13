#pragma once

#include "ptcl_packet.h"
#include <string>

class  ptcl_packet_csg : public ptcl_packet
{
public:
	ptcl_packet_csg(void);
	virtual ~ptcl_packet_csg(void);

	virtual void reset();

	//组装报文，必须先set_packet_buffer
	virtual void format_packet();

	//解析报文 必须先set_packet_buffer
	//返回值 >0:解析报文成功 0:报文长度不够 <0:报文格式不对
	virtual int parse_packet();
	
	//返回值 >0:解析报文成功 0:报文长度不够 <0:报文格式不对
	virtual int parse_header();

	//返回值 >0:解析报文成功 0:报文长度不够 <0:报文格式不对
	virtual int parse_header(unsigned char *buffer,int len_buffer);

	//终端逻辑地址
	virtual unsigned __int64 get_device_addr();
	void set_device_addr(unsigned __int64 addr);

	//主站地址
	virtual unsigned char get_host_id();
	void set_host_id(unsigned char id);

	//控制域功能码
	unsigned char get_afn_ctrl();
	void set_afn_ctrl(unsigned char afn);

	//帧计数有效位
	unsigned char get_fcv();
	void set_fcv(unsigned char fcv);

	//帧计数位
	unsigned char get_fcv_acd();
	void set_fcv_acd(unsigned char fcv_acd);

	//启动标志位
	unsigned char get_frm();
	void set_frm(unsigned char frm);

	//传输方向
	unsigned char get_dir();
	void set_dir(unsigned char dir);

	//功能码域
	unsigned char get_ctrl_code_area();
	void set_ctrl_code_area(unsigned char area);
	
	//应用层功能码
	unsigned char get_afn();
	void set_afn(unsigned char afn);

	//帧序号
	virtual int get_frame_seq();
	void set_frame_seq(unsigned char seq);
	
	//请求确认标志位
	unsigned char get_con();
	void set_con(unsigned char con);

	//结束帧标志
	unsigned char get_fin();
	void set_fin(unsigned char fin);

	//首帧标志
	unsigned char get_fri();
	void set_fri(unsigned char fri);

	//帧时间标签有效位
	unsigned char get_tpv();
	void set_tpv(unsigned char tpv);

	//桢序列字段
	unsigned char get_seq_area();
	void set_seq_area(unsigned char area);

	static unsigned __int64 convert_device_addr(std::string addr);
	static std::string convert_device_addr(unsigned __int64);
    static unsigned int convert_fn(std::string fn);
 	static std::string convert_fn(unsigned int);

protected:
	//报文头结构体
#pragma pack(push, 1)
	struct frame_header_struct
	{
		unsigned char frame_start;//0x68
		unsigned short int len_data_area;//长度L 控制域、地址域、用户数据长度的字节总数
		unsigned short int len_area_ex;
		unsigned char data_area_start;//0x68
		union 
		{
			struct
			{
				unsigned char	afn_ctrl : 4 ,//D0--D3控制域功能码
								fcv		 : 1 ,//D4帧计数有效位
								fcb_acd	 : 1 ,//D5帧计数位 或者 请求访问位
								frm		 : 1 ,//D6启动标志位
								dir		 : 1 ;//D7传输方向位
			};
			unsigned char ctrl_code_area;
		};
		unsigned char device_addr[6];//终端逻辑地址
		unsigned char host_id;
		unsigned char afn;
		union 
		{
			struct
			{
				unsigned char    frame_seq	:	 4 ,//D0--D3帧序号
								 con		:	 1 ,//D4请求确认标志位
								 fin		:	 1 ,//D5结束帧标志
								 fri		:	 1 ,//D6首帧标志
								 tpv		:	 1 ;//D7帧时间标签有效位
			};
			unsigned char seq_area;
		};
	};

#pragma pack(pop)

	frame_header_struct *frame_header_;
};
