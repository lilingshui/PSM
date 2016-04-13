#include "ptcl_packet_dlt.h"
#include "ptcl_module.h"


ptcl_packet_dlt::ptcl_packet_dlt(void)
{
	reset();
}

ptcl_packet_dlt::~ptcl_packet_dlt(void)
{

}

void ptcl_packet_dlt::reset()
{
	frame_header_ = (frame_header_struct *)buf_packet_;
	len_header_ = 10;
	buf_data_area_ = buf_packet_ + len_header_;
	len_packet_ = 0;
	len_data_area_ = 0;
}

void ptcl_packet_dlt::format_packet()
{
	len_packet_ = 0;
	//memset(frame_header_->frame_head, 0xfe, 4);
	//frame_header_->frame_head = 0xfe;
	frame_header_->frame_start = 0x68;
	frame_header_->data_area_start = 0x68;
	frame_header_->len_area = len_data_area_;
	len_packet_ = len_header_ + len_data_area_;

	for(int i=0;i<len_data_area_;i++)
	{
		buf_data_area_[i] += 0x33;
	}

	//buf_packet_[len_packet_++] = get_check_sum(buf_packet_+4, frame_header_->len_area+len_header_-4);
	buf_packet_[len_packet_++] = get_check_sum(buf_packet_, frame_header_->len_area+len_header_);
	buf_packet_[len_packet_++] = 0x16;
}

int ptcl_packet_dlt::parse_packet()
{
	int result = parse_header();
	if (result < 0)
		return result;

	//unsigned char crc = get_check_sum(buf_packet_+4, frame_header_->len_area+len_header_-4);
	unsigned char crc = get_check_sum(buf_packet_ , frame_header_->len_area+len_header_);
	if(crc != buf_packet_[len_header_ + len_data_area_])
		return -1;

	for(int i=0;i<len_data_area_;i++)
	{
		buf_data_area_[i] -= 0x33;
	}

	if(buf_packet_[len_packet_ - 1] != 0x16)
		return -1;

	return len_packet_;
}

int ptcl_packet_dlt::check_packet()
{
	int result = parse_header();
	if (result < 0)
		return result;

	//unsigned char crc = get_check_sum(buf_packet_+4, frame_header_->len_area+len_header_-4);
	unsigned char crc = get_check_sum(buf_packet_ , frame_header_->len_area+len_header_);
	if(crc != buf_packet_[len_header_ + len_data_area_])
		return -1;

	if(buf_packet_[len_packet_ - 1] != 0x16)
		return -1;

	return len_packet_;
}

int ptcl_packet_dlt::parse_header()
{
	if (frame_header_->frame_start != 0x68 || frame_header_->data_area_start != 0x68)
		return -1;

	len_data_area_ = frame_header_->len_area;
	if (len_data_area_ < 0 || len_data_area_ > MAX_PACKET_LENGTH - len_header_ -2)
		return -1;

	len_packet_ = len_header_ + len_data_area_ + 2;

	return len_header_;
}

int ptcl_packet_dlt::parse_header(unsigned char *buffer,int len_buffer)
{
	if(len_buffer < len_header_)
		return 0;

	frame_header_ = (frame_header_struct *)buffer;
	return parse_header();
}

unsigned __int64 ptcl_packet_dlt::get_meter_addr()
{
	__int64 addr = 0;
	memcpy(&addr, frame_header_->device_addr, 6);
	return addr;
}

void ptcl_packet_dlt::set_meter_addr(unsigned __int64 addr)
{
	memcpy(frame_header_->device_addr, &addr, 6);
}

unsigned char ptcl_packet_dlt::get_ctrl_code()
{
	return frame_header_->ctrl_code;
}

void ptcl_packet_dlt::set_ctrl_code(unsigned char afn)
{
	frame_header_->ctrl_code = afn;
}

short int ptcl_packet_dlt::get_data_addr(unsigned char *buf)
{
	short int data_addr = 0;
	memcpy(&data_addr,buf,2);
	return data_addr;
}

unsigned __int64 ptcl_packet_dlt::get_device_addr()
{
	return 0;
}

int ptcl_packet_dlt::get_frame_seq()
{
	return 0;
}

unsigned char ptcl_packet_dlt::get_host_id()
{
	return 0;
}

unsigned int ptcl_packet_dlt::convert_fn(std::string fn)
 {
	   	std::string str;
		str = fn;

		unsigned int short retVal;
		unsigned char * p = reinterpret_cast<unsigned char *>(&retVal);
		char * stopstring;

		*p++ = static_cast<unsigned char>(
			strtol(str.substr(2, 2).c_str(), &stopstring, 16));
		*p++ = static_cast<unsigned char>(
			strtol(str.substr(0, 2).c_str(), &stopstring, 16));

		return retVal;
 }

 std::string ptcl_packet_dlt::convert_fn(unsigned int short fn)
 {
	unsigned char tempBuf[10] = {0};
	memcpy(tempBuf,&fn,2);

	char temp[10] = {0};
	sprintf(temp, "%02X%02X",tempBuf[1], tempBuf[0]);
		
	std::string sFn(temp);

	return sFn;
 }

  unsigned __int64 ptcl_packet_dlt::convert_meter_addr(std::string addr)
 {
	std::string str;
	char temp[32];
	sprintf(temp, "%012s", addr.c_str());
	str = temp;

	unsigned __int64 retVal;
	unsigned char * p = reinterpret_cast<unsigned char *>(&retVal);
	char * stopstring;

	*p++ = static_cast<unsigned char>(
		strtol(str.substr(10, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(8, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(6, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(4, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(2, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(0, 2).c_str(), &stopstring, 16));

	return retVal;
 }

 std::string ptcl_packet_dlt::convert_meter_addr(unsigned __int64 addr)
 {
	unsigned char tempBuf[16] = {0};
	memcpy(tempBuf,&addr,6);

	char temp[16] = {0};
	sprintf(temp, "%02X%02X%02X%02X%02X%02X", tempBuf[5], tempBuf[4], tempBuf[3], tempBuf[2],tempBuf[1],tempBuf[0]);
		
	std::string sAddr(temp);

	return sAddr;
 }
