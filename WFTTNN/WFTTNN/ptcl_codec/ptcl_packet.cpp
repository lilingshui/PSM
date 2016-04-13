#include "ptcl_packet.h"
#include <string>


ptcl_packet::ptcl_packet(void)
{
	memset(buf_packet_,0,MAX_PACKET_LENGTH);
	buf_data_area_ = NULL;
	len_packet_ = 0;
	len_data_area_ = 0;
	len_header_ = 0;
}

ptcl_packet::~ptcl_packet(void)
{
}

void ptcl_packet::set_packet_buffer(unsigned char *buffer,int len)
{
	int start_flag_len = 0;
	for(int i=0;i<len;i++)
	{
		if(buffer[i] == 0x68)
			break;
		start_flag_len ++;
	}

	if (buffer == NULL || len < 0 || len > MAX_PACKET_LENGTH)
		return ;

	memcpy(buf_packet_,buffer + start_flag_len,len - start_flag_len);
	len_packet_ = len;
}

void ptcl_packet::set_data_area_buffer(unsigned char *buffer,int len)
{
	if (buffer == NULL || len < 0 || len > (MAX_PACKET_LENGTH-len_header_))
		return ;

	memcpy(buf_data_area_,buffer,len);
	len_data_area_ = len;
}

unsigned char *ptcl_packet::get_packet_buffer()
{
	return buf_packet_;
}

unsigned char *ptcl_packet::get_data_area_buffer()
{
	return buf_data_area_;
}

int ptcl_packet::get_packet_length()
{
	return len_packet_;
}

int ptcl_packet::get_data_area_length()
{
	return len_data_area_;
}

int ptcl_packet::get_header_len()
{
	return len_header_;
}

unsigned char ptcl_packet::get_check_sum(unsigned char *buffer,int len)
{
	if (buffer == NULL || len <= 0)
		return 0;

	unsigned char sum = 0;
	for(int n=0;n<len;n++)
		sum = sum + buffer[n];

	return sum;
}
