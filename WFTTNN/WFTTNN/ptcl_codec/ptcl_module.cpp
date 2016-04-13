#include "ptcl_module.h"

ptcl_module::ptcl_module(void)
{
	host_id_ = 0;
}

ptcl_module::~ptcl_module(void)
{
}

void ptcl_module::set_host_id(unsigned char id)
{
	host_id_ = id;
}
unsigned char ptcl_module::get_host_id()
{
	return host_id_;
}

unsigned char ptcl_module::get_frame_seq(__int64 addr)
{
	if (addr == 0)
		return 0;

	std::map <__int64,unsigned char>::iterator it = map_seq_.find(addr);
	if (it == map_seq_.end())
	{
		map_seq_[addr] = 0;
		return 0;
	}
	else
	{
		unsigned char seq = map_seq_[addr];
		if (seq == 127)
			seq = 0;
		else
			seq ++;

		map_seq_[addr] = seq;
		return seq;
	}
	
}

void ptcl_module::reset_frame_seq(__int64 addr)
{
	if (addr == 0)
		return ;

	map_seq_[addr] = 0;
}

unsigned char ptcl_module::bin_to_bcd(unsigned char bin)
{
	if (bin > 99)
		return 0;

	return ((bin/10)*16 + bin%10);
}

unsigned char ptcl_module::bcd_to_bin(unsigned char bcd)
{
	unsigned char b1 = bcd/16;
	if (b1 > 9)
		return 0;

	unsigned char b0 = bcd%16;
	if (b0 > 9)
		return 0;

	return (b1*10 + b0);
}


double ptcl_module::bcd_to_bin(unsigned char *buf,int len,int count_radix_point,unsigned char invalid)
{
	if (buf == NULL || len > 5 || count_radix_point > 10)
		return 0;

	if (is_invalid_data(buf,len,invalid) == true)
		return -10;

	unsigned int int_value = 0;
	for (int n=len-1;n>=0;n--)
	{
		unsigned int pow = 1;
		for (int m=0;m<n;m++)
			pow = pow * 100;

		int_value = int_value + (bcd_to_bin(buf[n]) * pow);
	}

	double pow = 1;
	if (count_radix_point >= 0)
	{
		for (int m=0;m<count_radix_point;m++)
			pow = pow * 10;

		return int_value / pow;
	}
	else
	{
		for (int m=0;m<-1 * count_radix_point;m++)
			pow = pow * 10;
		return int_value * pow;
	}

}

int ptcl_module::bcd_to_bin(unsigned char *buf,int len,unsigned char invalid)
{
	if (buf == NULL || len > 4)
		return 0;

	if (is_invalid_data(buf,len,invalid) == true)
		return -10;

	unsigned int int_value = 0;
	for (int n=len-1;n>=0;n--)
	{
		unsigned int pow = 1;
		for (int m=0;m<n;m++)
			pow = pow * 100;

		int_value = int_value + (bcd_to_bin(buf[n]) * pow);
	}

	return int_value;
}

//12345678 -> 0x78 0x56 0x34 0x12
void ptcl_module::bin_to_bcd(int value,unsigned char *buf,int offset)
{
	int n = 0;
	while (value > 0)
	{
		buf[offset+n] = bin_to_bcd(value % 100);		
		value = value / 100;
		n++;
	}
}
	
	//123456789012 -> 0x12 0x90 0x78 0x56 0x34 0x12
	void ptcl_module:: bin_to_bcd(long value,unsigned char *buf,int offset)
	{
		int n = 0;
		while (value > 0)
		{
			buf[offset+n] = bin_to_bcd(value % 100);
			
			value = value / 100;
			n++;
		}
	}
	
	void ptcl_module::bin_to_bcd(double value,int len,int countRadixPoint,unsigned char *buf,int offset)
	{
		int pow = 1;
		for(int i= 0;i<countRadixPoint;i++)
			pow = pow * 10;
			
			
		if(len <= 4)
		{
			int iValue = (int)(value * pow);		
			bin_to_bcd(iValue,buf,offset);
		}
		else if(len > 4 && len <=6)
		{
			long iValue = (long)(value * pow);		
			bin_to_bcd(iValue,buf,offset);						
		}
	}

unsigned int ptcl_module::bcd_to_byte(unsigned char *buf,int len,int pos)
{
	if ( buf == NULL )
		return 0;

	unsigned int int_value = 0;
	unsigned char flag = *buf;
	
	int_value = (flag>>pos)&0x01;

	return int_value;
}

bool ptcl_module::is_invalid_data(unsigned char *buffer,int len,unsigned char invalid)
{
	for (int n=0;n<len;n++)
	{
		if (buffer[n] == invalid || buffer[n] == 0xee)
			return true;
	}
	return false;
}

int ptcl_module::bcd_to_time(unsigned char *buf,int len,int type,tm *t)
{
	if (buf == NULL || t == NULL)
		return 0;

	int len_parse = 0;
	if (type == time_type_ssmmhhddmmyy)
	{
		len_parse = 6;
		if (len < len_parse)
			return 0;
		t->tm_sec = bcd_to_bin(buf[0]);
		t->tm_min = bcd_to_bin(buf[1]);
		t->tm_hour = bcd_to_bin(buf[2]);
		t->tm_mday = bcd_to_bin(buf[3]);
		t->tm_mon = bcd_to_bin(buf[4]) - 1;
		t->tm_year = 100 + bcd_to_bin(buf[5]);
	}
	else if (type == time_type_mmhhddmmyy)
	{
		len_parse = 5;
		if (len < len_parse)
			return 0;
		t->tm_sec = 0;
		t->tm_min = bcd_to_bin(buf[0]);
		t->tm_hour = bcd_to_bin(buf[1]);
		t->tm_mday = bcd_to_bin(buf[2]);
		t->tm_mon = bcd_to_bin(buf[3]) - 1;
		t->tm_year = 100 + bcd_to_bin(buf[4]);

		t->tm_year = t->tm_year > 137 ? 70 : t->tm_year;
	}
	else if (type == time_type_ddmmyy)
	{
		len_parse = 3;
		if (len < len_parse)
			return 0;
		t->tm_sec = 0;
		t->tm_min = 0;
		t->tm_hour = 0;
		t->tm_mday = bcd_to_bin(buf[0]);
		t->tm_mon = bcd_to_bin(buf[1]) - 1;
		t->tm_year = 100 + bcd_to_bin(buf[2]);
	}
	else if (type == time_type_mmhhddmm)
	{
		len_parse = 4;
		if (len < len_parse)
			return 0;

		t->tm_sec = 0;
		t->tm_min = bcd_to_bin(buf[0]);
		t->tm_hour = bcd_to_bin(buf[1]);
		t->tm_mday = bcd_to_bin(buf[2]);
		t->tm_mon = bcd_to_bin(buf[3]) - 1;
		t->tm_year = 113;
	}
	else if (type == time_type_hhddmmyy)
	{
		len_parse = 4;
		if (len < len_parse)
			return 0;

		t->tm_sec = 0;
		t->tm_min = 0;
		t->tm_hour = bcd_to_bin(buf[0]);
		t->tm_mday = bcd_to_bin(buf[1]);
		t->tm_mon = bcd_to_bin(buf[2]) - 1;
		t->tm_year = 100 + bcd_to_bin(buf[3]);
	}
	else if (type == time_type_mmyy)
	{
		len_parse = 2;
		if (len < len_parse)
			return 0;

		t->tm_sec = 0;
		t->tm_min = 0;
		t->tm_hour = 0;
		t->tm_mday = 1;
		t->tm_mon = bcd_to_bin(buf[0]) - 1;
		t->tm_year = 100 + bcd_to_bin(buf[1]);
	}
	else if (type == time_type_yymmddhhmmss)
	{
		len_parse = 6;
		if (len < len_parse)
			return 0;

		t->tm_year = 100 + bcd_to_bin(buf[0]);
		t->tm_mon = bcd_to_bin(buf[1]) - 1;
		t->tm_mday = bcd_to_bin(buf[2]);
		t->tm_hour = bcd_to_bin(buf[3]);
		t->tm_min = bcd_to_bin(buf[4]);
		t->tm_sec = bcd_to_bin(buf[5]);
	}
	else if (type == time_type_yymmddhhmm)
	{
		len_parse = 5;
		if (len < len_parse)
			return 0;

		t->tm_year = 100 + bcd_to_bin(buf[0]);
		t->tm_mon = bcd_to_bin(buf[1]) - 1;
		t->tm_mday = bcd_to_bin(buf[2]);
		t->tm_hour = bcd_to_bin(buf[3]);
		t->tm_min = bcd_to_bin(buf[4]);
		t->tm_sec = 0;
	}
	else if (type == time_type_yymmdd)
	{
		len_parse = 3;
		if (len < len_parse)
			return 0;

		t->tm_year = 100 + bcd_to_bin(buf[0]);
		t->tm_mon = bcd_to_bin(buf[1]) - 1;
		t->tm_mday = bcd_to_bin(buf[2]);
		t->tm_hour = 0;
		t->tm_min = 0;
		t->tm_sec = 0;
	}
	else if (type == time_type_mmddhhmm)
	{
		len_parse = 4;
		if (len < len_parse)
			return 0;

		t->tm_year = 113;
		t->tm_mon = bcd_to_bin(buf[0]) - 1;
		t->tm_mday = bcd_to_bin(buf[1]);
		t->tm_hour = bcd_to_bin(buf[2]);
		t->tm_min = bcd_to_bin(buf[3]);
		t->tm_sec = 0;
	}
	else if (type == time_type_mmhhddmm)
	{
		len_parse = 4;
		if (len < len_parse)
			return 0;

		t->tm_year = 113;
		t->tm_mon = bcd_to_bin(buf[3]) - 1;
		t->tm_mday = bcd_to_bin(buf[2]);
		t->tm_hour = bcd_to_bin(buf[1]);
		t->tm_min = bcd_to_bin(buf[0]);
		t->tm_sec = 0;
	}
	else if (type == time_type_hhddmm)
	{
		len_parse = 3;
		if (len < len_parse)
			return 0;

		t->tm_year = 113;
		t->tm_hour = bcd_to_bin(buf[0]);
		t->tm_mday = bcd_to_bin(buf[1]);
		t->tm_mon = bcd_to_bin(buf[2]) - 1;
		t->tm_min = 0;
		t->tm_sec = 0;
	}
	else if (type == time_type_mmhhdd)
	{
		len_parse = 3;
		if(len < len_parse)
			return 0;

		t->tm_year = 113;
		t->tm_mon  = 1;
		t->tm_min  = bcd_to_bin(buf[0]);
		t->tm_hour = bcd_to_bin(buf[1]);
		t->tm_mday = bcd_to_bin(buf[2]);
		t->tm_sec = 0;
	}
	else if(type == time_type_yyyymmddhhmm)
	{
		len_parse = 6;
		if (len < len_parse)
			return 0;

		t->tm_year = (bcd_to_bin(buf[0])*100 + bcd_to_bin(buf[1])) - 1900;
		t->tm_mon = bcd_to_bin(buf[2]) - 1;
		t->tm_mday = bcd_to_bin(buf[3]);
		t->tm_hour = bcd_to_bin(buf[4]);
		t->tm_min = bcd_to_bin(buf[5]);
		t->tm_sec = 0;
	}
	else if (type == time_type_ssmmhh)
	{
		len_parse = 3;
		if(len < len_parse)
			return 0;

		t->tm_year = 113;
		t->tm_mon  = 1;
		t->tm_mday = 1;
		t->tm_hour = bcd_to_bin(buf[2]);
		t->tm_min  = bcd_to_bin(buf[1]);
		t->tm_sec =  bcd_to_bin(buf[0]);
	}

	if (t->tm_mon < 0 || t->tm_mon > 11)
		t->tm_mon = 0;
	if (t->tm_mday < 1 || t->tm_mday > 31)
		t->tm_mday = 1;
	if (t->tm_hour < 0 || t->tm_hour > 23)
		t->tm_hour = 0;
	if (t->tm_min < 0 || t->tm_min > 59)
		t->tm_min = 0;
	if (t->tm_sec < 0 || t->tm_sec > 59)
		t->tm_sec = 0;

	return len_parse;
}

int ptcl_module::time_to_bcd(int type,tm *t,unsigned char *buf)
{
	if (buf == NULL || t == NULL)
		return 0;

	int len_format = 0;
	if (type == time_type_ssmmhhddmmyy)
	{
		len_format = 6;

		buf[0] = bin_to_bcd(t->tm_sec);
		buf[1] = bin_to_bcd(t->tm_min);
		buf[2] = bin_to_bcd(t->tm_hour);
		buf[3] = bin_to_bcd(t->tm_mday);
		buf[4] = bin_to_bcd(t->tm_mon+1);
		buf[5] = bin_to_bcd(t->tm_year%100);

	}
	else if (type == time_type_mmhhddmmyy)
	{
		len_format = 5;
		buf[0] = bin_to_bcd(t->tm_min);
		buf[1] = bin_to_bcd(t->tm_hour);
		buf[2] = bin_to_bcd(t->tm_mday);
		buf[3] = bin_to_bcd(t->tm_mon+1);
		buf[4] = bin_to_bcd(t->tm_year%100);

	}
	else if (type == time_type_ddmmyy)
	{
		len_format = 3;
		buf[0] = bin_to_bcd(t->tm_mday);
		buf[1] = bin_to_bcd(t->tm_mon+1);
		buf[2] = bin_to_bcd(t->tm_year%100);
	}
	else if (type == time_type_mmhhddmm)
	{
		len_format = 4;
		buf[0] = bin_to_bcd(t->tm_min);
		buf[1] = bin_to_bcd(t->tm_hour);
		buf[2] = bin_to_bcd(t->tm_mday);
		buf[3] = bin_to_bcd(t->tm_mon+1);

	}
	else if (type == time_type_hhddmmyy)
	{
		len_format = 4;
		buf[0] = bin_to_bcd(t->tm_hour);
		buf[1] = bin_to_bcd(t->tm_mday);
		buf[2] = bin_to_bcd(t->tm_mon+1);
		buf[3] = bin_to_bcd(t->tm_year%100);
	}
	else if (type == time_type_mmyy)
	{
		len_format = 2;
		buf[0] = bin_to_bcd(t->tm_mon+1);
		buf[1] = bin_to_bcd(t->tm_year%100);
	}
	else if (type == time_type_ssmmhhddwwmmyy)
	{
		len_format = 6;
		buf[0] = bin_to_bcd(t->tm_sec);
		buf[1] = bin_to_bcd(t->tm_min);
		buf[2] = bin_to_bcd(t->tm_hour);
		buf[3] = bin_to_bcd(t->tm_mday);
		int week = t->tm_wday;
		if (week == 0)
		 week = 7;
		buf[4] = ((t->tm_mon+1)%10)+(((t->tm_mon+1)/10)<<4)+((bin_to_bcd(t->tm_wday))<<5);
		buf[5] = bin_to_bcd(t->tm_year%100);
	}
	else if (type == time_type_yymmddhhmmss)
	{
		len_format = 6;

		buf[0] = bin_to_bcd(t->tm_year%100);
		buf[1] = bin_to_bcd(t->tm_mon+1);
		buf[2] = bin_to_bcd(t->tm_mday);
		buf[3] = bin_to_bcd(t->tm_hour);
		buf[4] = bin_to_bcd(t->tm_min);
		buf[5] = bin_to_bcd(t->tm_sec);
	}
	else if (type == time_type_yymmddhhmm)
	{
		len_format = 5;

		buf[0] = bin_to_bcd(t->tm_year%100);
		buf[1] = bin_to_bcd(t->tm_mon+1);
		buf[2] = bin_to_bcd(t->tm_mday);
		buf[3] = bin_to_bcd(t->tm_hour);
		buf[4] = bin_to_bcd(t->tm_min);
	}
	else if (type == time_type_yymmdd)
	{
		len_format = 3;

		buf[0] = bin_to_bcd(t->tm_year%100);
		buf[1] = bin_to_bcd(t->tm_mon+1);
		buf[2] = bin_to_bcd(t->tm_mday);

	}
	else if (type == time_type_mmddhhmm)
	{
		len_format = 4;

		buf[0] = bin_to_bcd(t->tm_mon+1);
		buf[1] = bin_to_bcd(t->tm_mday);
		buf[2] = bin_to_bcd(t->tm_hour);
		buf[3] = bin_to_bcd(t->tm_min);
	}

	else if (type == time_type_mmddhhmmss)
	{
		len_format = 5;

		buf[0] = bin_to_bcd(t->tm_mon+1);
		buf[1] = bin_to_bcd(t->tm_mday);
		buf[2] = bin_to_bcd(t->tm_hour);
		buf[3] = bin_to_bcd(t->tm_min);
		buf[4] = bin_to_bcd(t->tm_sec);
	}

	return len_format;
}


