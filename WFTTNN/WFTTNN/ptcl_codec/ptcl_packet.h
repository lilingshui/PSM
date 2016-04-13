#pragma once


class  ptcl_packet  
{
public:
	ptcl_packet(void);
	virtual ~ptcl_packet(void);

	virtual void reset() = 0;

	//组装报文，必须先set_packet_buffer
	virtual void format_packet() = 0;

	//解析报文 必须先set_packet_buffer
	//返回值 >0:解析报文成功 0:报文长度不够 <0:报文格式不对
	virtual int parse_packet() = 0;
	
	//返回值 >0:解析报文成功 0:报文长度不够 <0:报文格式不对
	virtual int parse_header() = 0;

	//返回值 >0:解析报文成功 0:报文长度不够 <0:报文格式不对
	virtual int parse_header(unsigned char *buffer,int len_buffer) = 0;

	//返回终端逻辑地址 按照报文顺序
	virtual unsigned __int64 get_device_addr() = 0;

	//返回报文序列号
	virtual int get_frame_seq() = 0;

	//返回主站ID
	virtual unsigned char get_host_id() = 0;

	//设置报文buffer
	void set_packet_buffer(unsigned char *buffer,int len);

	//设置用户域数据buffer
	void set_data_area_buffer(unsigned char *buffer,int len);

	//得到报文buffer
	unsigned char *get_packet_buffer();

	//得到报文长度
	int get_packet_length();

	//得到用户域数据buffer
	unsigned char *get_data_area_buffer();

	//得到用户域数据长度
	int get_data_area_length();

	//返回报文头长度
	int get_header_len();

protected:
	enum { MAX_PACKET_LENGTH = 1536 };

	unsigned char get_check_sum(unsigned char *buffer,int len);

	unsigned char buf_packet_[MAX_PACKET_LENGTH];
	unsigned char *buf_data_area_;
	int len_packet_;
	int len_data_area_;
	int len_header_;
	
};

