#include "ptcl_packet_csg.h"
#include "ptcl_module.h"

ptcl_packet_csg::ptcl_packet_csg(void)
{
	reset();
}

ptcl_packet_csg::~ptcl_packet_csg(void)
{
}

void ptcl_packet_csg::reset()
{
	frame_header_ = (frame_header_struct *)buf_packet_;
	len_header_ = 16;
	buf_data_area_ = buf_packet_ + len_header_;
	len_packet_ = 0;
	len_data_area_ = 0;
}

void ptcl_packet_csg::format_packet()
{
	len_packet_ = 0;

	frame_header_->frame_start = 0x68;
	frame_header_->data_area_start = 0x68;
	frame_header_->len_data_area = len_data_area_ + 10;
	frame_header_->len_area_ex = frame_header_->len_data_area;

	len_packet_ = len_header_ + len_data_area_;

	buf_packet_[len_packet_++] =  get_check_sum(buf_packet_ + 6,frame_header_->len_data_area);
	buf_packet_[len_packet_++] = 0x16;
}

int ptcl_packet_csg::parse_packet()
{
	int result = parse_header();
	if (result <= 0)
		return result;

	unsigned char crc = get_check_sum(buf_packet_ + 6,frame_header_->len_data_area);
	if (crc != buf_packet_[len_header_ + len_data_area_])
		return -1;

	if (buf_packet_[len_packet_-1] != 0x16)
		return -1;

	return len_packet_;
}

int ptcl_packet_csg::parse_header()
{
	if (frame_header_->frame_start != 0x68 || frame_header_->data_area_start != 0x68)
		return -1;

	if (frame_header_->len_data_area != frame_header_->len_area_ex)
		return -1;
	
	len_data_area_ = frame_header_->len_data_area - 10;
	if (len_data_area_ < 0 || len_data_area_ > (MAX_PACKET_LENGTH - len_header_ - 2))
		return -1;

	len_packet_ = len_header_ + len_data_area_ + 2;

	return len_header_;
}

int ptcl_packet_csg::parse_header(unsigned char *buffer,int len_buffer)
{
	if (len_buffer < len_header_)
		return 0;

	frame_header_ = (frame_header_struct *)buffer;
	return parse_header();
}

unsigned __int64 ptcl_packet_csg::get_device_addr()
{
	__int64 addr = 0;
	memcpy(&addr, frame_header_->device_addr, 6);
	return addr;
}

void ptcl_packet_csg::set_device_addr(unsigned __int64 addr)
{
	memcpy(frame_header_->device_addr, &addr, 6);
}

unsigned char ptcl_packet_csg::get_host_id()
{
	return frame_header_->host_id;
}

void ptcl_packet_csg::set_host_id(unsigned char id)
{
	frame_header_->host_id = id;
}

unsigned char ptcl_packet_csg::get_afn_ctrl()
{
	return frame_header_->afn_ctrl;
}

void ptcl_packet_csg::set_afn_ctrl(unsigned char afn)
{
	frame_header_->afn_ctrl = afn;
}

unsigned char ptcl_packet_csg::get_fcv()
{
	return frame_header_->fcv;
}

void ptcl_packet_csg::set_fcv(unsigned char fcv)
{
	frame_header_->fcv = fcv;
}

unsigned char ptcl_packet_csg::get_fcv_acd()
{
	return frame_header_->fcb_acd;
}

void ptcl_packet_csg::set_fcv_acd(unsigned char fcb_acd)
{
	frame_header_->fcb_acd = fcb_acd;
}

unsigned char ptcl_packet_csg::get_frm()
{
	return frame_header_->frm;
}

void ptcl_packet_csg::set_frm(unsigned char frm)
{
	frame_header_->frm = frm;
}

unsigned char ptcl_packet_csg::get_dir()
{
	return frame_header_->dir;
}

void ptcl_packet_csg::set_dir(unsigned char dir)
{
	frame_header_->dir = dir;
}

unsigned char ptcl_packet_csg::get_ctrl_code_area()
{
	return frame_header_->ctrl_code_area;
}

void ptcl_packet_csg::set_ctrl_code_area(unsigned char area)
{
	frame_header_->ctrl_code_area = area;
}

unsigned char ptcl_packet_csg::get_afn()
{
	return frame_header_->afn;
}

void ptcl_packet_csg::set_afn(unsigned char afn)
{
	frame_header_->afn = afn;
}

int ptcl_packet_csg::get_frame_seq()
{
	return frame_header_->frame_seq;
}

void ptcl_packet_csg::set_frame_seq(unsigned char seq)
{
	frame_header_->frame_seq = seq;
}

unsigned char ptcl_packet_csg::get_con()
{
	return frame_header_->con;
}

void ptcl_packet_csg::set_con(unsigned char con)
{
	frame_header_->con = con;
}

unsigned char ptcl_packet_csg::get_fin()
{
	return frame_header_->fin;
}

void ptcl_packet_csg::set_fin(unsigned char fin)
{
	frame_header_->fin = fin;
}

unsigned char ptcl_packet_csg::get_fri()
{
	return frame_header_->fri;
}

void ptcl_packet_csg::set_fri(unsigned char fri)
{
	frame_header_->fri = fri;
}

unsigned char ptcl_packet_csg::get_tpv()
{
	return frame_header_->tpv;
}

void ptcl_packet_csg::set_tpv(unsigned char tpv)
{
	frame_header_->tpv = tpv;
}

unsigned char ptcl_packet_csg::get_seq_area()
{
	return frame_header_->seq_area;
}

void ptcl_packet_csg::set_seq_area(unsigned char area)
{
	frame_header_->seq_area = area;
}


 unsigned __int64 ptcl_packet_csg::convert_device_addr(std::string addr)
 {
	std::string str;
	char temp[32];
	sprintf(temp, "%012s", addr.c_str());
	str = temp;

	unsigned __int64 retVal;
	unsigned char * p = reinterpret_cast<unsigned char *>(&retVal);
	char * stopstring;

	*p++ = static_cast<unsigned char>(
		strtol(str.substr(4, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(2, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(0, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(10, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(8, 2).c_str(), &stopstring, 16));
	*p++ = static_cast<unsigned char>(
		strtol(str.substr(6, 2).c_str(), &stopstring, 16));

	return retVal;
 }

 std::string ptcl_packet_csg::convert_device_addr(unsigned __int64 addr)
 {
	unsigned char tempBuf[16] = {0};
	memcpy(tempBuf,&addr,6);

	char temp[16] = {0};
	sprintf(temp, "%02X%02X%02X%02X%02X%02X", tempBuf[2], tempBuf[1], tempBuf[0], tempBuf[5],tempBuf[4],tempBuf[3]);
		
	std::string sAddr(temp);

	return sAddr;
 }

 
 unsigned int ptcl_packet_csg::convert_fn(std::string fn)
 {
	   	std::string str;
		str = fn;

		unsigned int retVal;
		unsigned char * p = reinterpret_cast<unsigned char *>(&retVal);
		char * stopstring;

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

 std::string ptcl_packet_csg::convert_fn(unsigned int fn)
 {
	unsigned char tempBuf[4] = {0};
	memcpy(tempBuf,&fn,4);

	char temp[10] = {0};
	sprintf(temp, "%02X%02X%02X%02X", tempBuf[3], tempBuf[2], tempBuf[1], tempBuf[0]);
		
	std::string sFn(temp);

	return sFn;
 }



