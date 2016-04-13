#include "ptcl_module_dlt.h"
#include "time.h"

ptcl_module_dlt *ptcl_module_dlt::instance_ = NULL;

ptcl_module_dlt::ptcl_module_dlt(void)
{

}

ptcl_module_dlt::~ptcl_module_dlt(void)
{

}

ptcl_module_dlt *ptcl_module_dlt::get_instance()
{
	if (instance_ == NULL)
		instance_ = new ptcl_module_dlt();
	return instance_;
}


void ptcl_module_dlt::format_packet_read_data(	ptcl_packet_dlt *packet,unsigned int data_item)
{
	if (packet == NULL)
		return;

	//控制域
	packet->set_ctrl_code(AFN_READ_DATA);
	
	int len_data = 0;
	unsigned char buf[32] = {0};

	memcpy(buf, &data_item, 2);

	len_data = 2;

	packet->set_data_area_buffer(buf, len_data);

	packet->format_packet();
}

void ptcl_module_dlt::format_packet_read_curve_data(	ptcl_packet_dlt *packet,unsigned int data_item,time_t data_time)
{
	if (packet == NULL)
		return;

	//控制域
	packet->set_ctrl_code(AFN_READ_DATA);
	
	int len_data = 0;
	unsigned char buf[32] = {0};

	memcpy(buf, &data_item, 2);

	len_data = len_data + 2;

	tm *tm_start = localtime(&data_time);
	buf[len_data++] = ptcl_module::bin_to_bcd((tm_start->tm_year+1900)%100);
	buf[len_data++] = ptcl_module::bin_to_bcd(tm_start->tm_mon + 1);
	buf[len_data++] = ptcl_module::bin_to_bcd(tm_start->tm_mday);
	buf[len_data++] = ptcl_module::bin_to_bcd(tm_start->tm_hour);
	buf[len_data++] = ptcl_module::bin_to_bcd(tm_start->tm_min);

//	len_data = len_data + 5;

	packet->set_data_area_buffer(buf, len_data);

	packet->format_packet();

}

void ptcl_module_dlt::format_packet_set_time(	ptcl_packet_dlt *packet,time_t time)
{
	if (packet == NULL)
		return;

	//控制域
	packet->set_ctrl_code(AFN_ALL_TIMING);
	
	int len_data = 0;
	unsigned char buf[32] = {0};

	tm *tm_start = localtime(&time);
	buf[len_data++] = ptcl_module::bin_to_bcd(tm_start->tm_sec);
	buf[len_data++] = ptcl_module::bin_to_bcd(tm_start->tm_min);
	buf[len_data++] = ptcl_module::bin_to_bcd(tm_start->tm_hour);
	buf[len_data++] = ptcl_module::bin_to_bcd(tm_start->tm_mday);
	buf[len_data++] = ptcl_module::bin_to_bcd(tm_start->tm_mon + 1);
	buf[len_data++] = ptcl_module::bin_to_bcd((tm_start->tm_year+1900)%100);

	packet->set_data_area_buffer(buf, len_data);

	packet->format_packet();
}

void ptcl_module_dlt::format_packet_write_I_U_data( ptcl_packet_dlt *packet,unsigned int data_item,
		                                short ua, short ub, short uc,
									   double ia,double ib,double ic,
									   unsigned char rs485,unsigned char time)
{
	if (packet == NULL)
	    return;

	//控制域
	packet->set_ctrl_code(0X04);
	
	int len_data = 0;
	unsigned char buf[32] = {0};

	memcpy(buf, &data_item, 2);
	len_data = len_data + 2;

	//电压
	ptcl_module::bin_to_bcd(ua,buf,len_data);
	len_data = len_data + 2;
	ptcl_module::bin_to_bcd(ub,buf,len_data);
	len_data = len_data + 2;
	ptcl_module::bin_to_bcd(uc,buf,len_data);
	len_data = len_data + 2;

	//电流
	ptcl_module::bin_to_bcd(ia,2,2,buf,len_data);
	len_data = len_data + 2;
	ptcl_module::bin_to_bcd(ib,2,2,buf,len_data);
	len_data = len_data + 2;
	ptcl_module::bin_to_bcd(ic,2,2,buf,len_data);
	len_data = len_data + 2;

	//485通道号
	buf[len_data++] = rs485;

	//持续时间
	buf[len_data++] = ptcl_module::bin_to_bcd(time);

	packet->set_data_area_buffer(buf, len_data);

	packet->format_packet();
}

void ptcl_module_dlt::format_packet_write_other_parameter( ptcl_packet_dlt *packet,unsigned int data_item,
		                                       int demandCount, int processCount, int openmeterCount, int dlCount,
											   unsigned char rs485)
{
	if (packet == NULL)
	    return;

	//控制域
	packet->set_ctrl_code(0X04);
	
	int len_data = 0;
	unsigned char buf[32] = {0};

	memcpy(buf, &data_item, 2);
	len_data = len_data + 2;

	//需量手动清零总次数
	ptcl_module::bin_to_bcd(demandCount,buf,len_data);
	len_data = len_data + 3;

	//编程总次数
	ptcl_module::bin_to_bcd(processCount,buf,len_data);
	len_data = len_data + 3;

	//开表盖总次数
	ptcl_module::bin_to_bcd(openmeterCount,buf,len_data);
	len_data = len_data + 3;

	//电量清零总次数
	ptcl_module::bin_to_bcd(dlCount,buf,len_data);
	len_data = len_data + 3;

	//485通道号
	buf[len_data++] = rs485;

	packet->set_data_area_buffer(buf, len_data);

	packet->format_packet();
}

void ptcl_module_dlt::format_packet_start_clock_communicate( ptcl_packet_dlt *packet,unsigned int data_item,
		                                        unsigned char rs485,unsigned char time)
{
	if (packet == NULL)
	    return;

	//控制域
	packet->set_ctrl_code(0X04);
	
	int len_data = 0;
	unsigned char buf[32] = {0};

	memcpy(buf, &data_item, 2);
	len_data = len_data + 2;

	//485通道号
	buf[len_data++] = rs485;

	//持续时间
	buf[len_data++] = ptcl_module::bin_to_bcd(time);

	packet->set_data_area_buffer(buf, len_data);

	packet->format_packet();
}

void format_packet_set_stop_start( ptcl_packet_dlt *packet,unsigned int data_item,
		                               unsigned char state,
									   unsigned char rs485,unsigned char time)
{
	if (packet == NULL)
	    return;

	//控制域
	packet->set_ctrl_code(0X04);
	
	int len_data = 0;
	unsigned char buf[32] = {0};

	memcpy(buf, &data_item, 2);
	len_data = len_data + 2;

	//状态参数	1	0：停止走字，1：启动走字
	buf[len_data++] = state;

	//485通道号
	buf[len_data++] = rs485;

	//持续时间
	buf[len_data++] = ptcl_module::bin_to_bcd(time);

	packet->set_data_area_buffer(buf, len_data);

	packet->format_packet();
}


