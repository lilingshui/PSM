#pragma once

#include "ptcl_packet.h"
#include <string>

class  ptcl_packet_dlt : public ptcl_packet
{
public:
	ptcl_packet_dlt(void);
	virtual ~ptcl_packet_dlt(void);

	virtual void reset();

	//组装报文，必须先set_packet_buffer
	virtual void format_packet();

	//解析报文 必须先set_packet_buffer
	//返回值 >0:解析报文成功 0:报文长度不够 <0:报文格式不对
	virtual int parse_packet();

	// 配合自动测试加的
    int check_packet();

	//返回值 >0:解析报文成功 0:报文长度不够 <0:报文格式不对
	virtual int parse_header();

	//返回值 >0:解析报文成功 0:报文长度不够 <0:报文格式不对
	virtual int parse_header(unsigned char *buffer,int len_buffer);

	//返回终端逻辑地址 按照报文顺序
	virtual unsigned __int64 get_device_addr();

	//返回报文序列号
	virtual int get_frame_seq();

	//返回主站ID
	virtual unsigned char get_host_id();

	//返回终端逻辑地址 按照报文顺序
	unsigned __int64 get_meter_addr();

	//逻辑地址
	void set_meter_addr(unsigned __int64 addr);

	//控制域功能码
	unsigned char get_ctrl_code();
	void set_ctrl_code(unsigned char afn);

	//数据标识
	static short int get_data_addr(unsigned char *buf);

	static unsigned int convert_fn(std::string fn);
 	static std::string convert_fn(unsigned int short fn);
	static std::string convert_meter_addr(unsigned __int64 addr);
	static unsigned __int64 convert_meter_addr(std::string addr);

protected:
	
	//报文头结构体
#pragma pack(push, 1)
	struct frame_header_struct
	{
		//unsigned char frame_head[4];	//前导,用来唤醒
		unsigned char frame_start;//0x68
		unsigned char device_addr[6];
		unsigned char data_area_start;//0x68
		unsigned char ctrl_code;
		unsigned char len_area;
	};

#pragma pack(pop)

	frame_header_struct *frame_header_;

};