#pragma once
#include<map>


class  ptcl_module
{
public:

	public:
	ptcl_module(void);
	virtual ~ptcl_module(void);

	void set_host_id(unsigned char id);
	unsigned char get_host_id();

	//45 --> 0x45
	static unsigned char bin_to_bcd(unsigned char bin);

	//0x45 --> 45
	static unsigned char bcd_to_bin(unsigned char bcd);

	//报文0x12 0x34 0x56 0x78 (NNNNNN.NN) --> 785634.12
	//len最大为4 count_radix_point最大为8
	static double bcd_to_bin(unsigned char *buf,int len,int count_radix_point,unsigned char invalid = 0xff);

	//报文0x12 0x34 0x56 0x78 (NNNNNNNN) --> 78563412
	//len最大为4
	static int bcd_to_bin(unsigned char *buf,int len,unsigned char invalid = 0xff);

	static void bin_to_bcd(int value,unsigned char *buf,int offset);
	static void bin_to_bcd(long value,unsigned char *buf,int offset);
	static void bin_to_bcd(double value,int len,int countRadixPoint,unsigned char *buf,int offset);

	//len可以为0
	static unsigned int bcd_to_byte(unsigned char *buf,int len,int pos = 0);

	enum time_type
	{
		//反序 [0]秒 [1]分 [2]时 [3]日 [4]月 [5]年
		time_type_ssmmhhddmmyy  = 1,
		time_type_mmhhddmmyy    = 2,
		time_type_ddmmyy		= 3,
		time_type_mmhhddmm      = 4,
		time_type_hhddmmyy      = 5,
		time_type_mmyy          = 6,
		time_type_hhddmm		= 7,
		time_type_ssmmhhddwwmmyy = 8,
		time_type_mmhhdd        = 9, 
		time_type_ssmmhh        = 10, 
		
		//正序 [0]年 [1]月 [2]日 [3]时 [4]分 [5]秒
		time_type_yymmddhhmmss	= 21,
		time_type_yymmddhhmm	= 22,
		time_type_yymmdd		= 23,
		time_type_mmddhhmm		= 24,
		time_type_mmddhhmmss    = 25,
		time_type_yyyymmddhhmm  = 26,
	};
	static int bcd_to_time(unsigned char *buf,int len,int type,tm *t);
	static int time_to_bcd(int type,tm *t,unsigned char *buf);

	static bool is_invalid_data(unsigned char *buffer,int len,unsigned char invalid = 0xff);

	static bool check_bcd(unsigned char *buffer, int len, unsigned char point = 0);


protected:
	unsigned char host_id_;

	std::map <__int64,unsigned char> map_seq_;

	virtual unsigned char get_frame_seq(__int64 addr);
	virtual void reset_frame_seq(__int64 addr);
};


