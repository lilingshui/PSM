#include "data_format_csg.h"
#include "ptcl_module_csg.h"
#include "time.h"

data_format_csg *data_format_csg::instance_ = NULL;

data_format_csg::data_format_csg(void)
{
}

data_format_csg::~data_format_csg(void)
{
}

data_format_csg *data_format_csg::get_instance()
{
	if (instance_ == NULL)
		instance_ = new data_format_csg();
	return instance_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
void data_format_csg::format_read_history( ptcl_packet_csg *packet,
		                      ptcl_module_csg::pnfn_data_uint *v_data_uint,int count_data_uint,
							  time_t start_time,time_t end_time,int densy)
{
	int coll_type = densy;//1:1分钟 2:5分钟  3：15分钟 4:30分钟 5:60分钟 6：1日 7:1月

	for(int n=0;n<count_data_uint;n++)
	{

		int lenDataBuf = 0;
		tm *tm_start = localtime(&start_time);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_start->tm_year+1900)/100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_start->tm_year+1900)%100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_mon + 1);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_mday);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_hour);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_min);

		tm *tm_end = localtime(&end_time);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_end->tm_year+1900)/100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_end->tm_year+1900)%100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_mon+1);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_mday);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_hour);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_min);

		v_data_uint[n].data_buffer[lenDataBuf++] = coll_type;

		v_data_uint[n].len_data = lenDataBuf;
	}

	ptcl_module_csg::get_instance()->format_packet_read_history_data(packet,v_data_uint,count_data_uint);

}

void data_format_csg::format_read_event_record( ptcl_packet_csg *packet,
		                      ptcl_module_csg::pnfn_data_uint *v_data_uint,int count_data_uint,
							  time_t start_time,time_t end_time)
{

	for(int n=0;n<count_data_uint;n++)
	{

		int lenDataBuf = 0;
		tm *tm_start = localtime(&start_time);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_start->tm_year+1900)/100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_start->tm_year+1900)%100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_mon + 1);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_mday);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_hour);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_min);

		tm *tm_end = localtime(&end_time);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_end->tm_year+1900)/100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_end->tm_year+1900)%100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_mon+1);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_mday);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_hour);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_min);

		v_data_uint[n].len_data = lenDataBuf;
	}

	ptcl_module_csg::get_instance()->format_packet_read_event_data(packet,v_data_uint,count_data_uint);

}

void data_format_csg::format_read_alarm_data( ptcl_packet_csg *packet,
		                      ptcl_module_csg::pnfn_data_uint *v_data_uint,int count_data_uint,
							  time_t start_time,time_t end_time)
{

	tm *tm_start = localtime(&start_time);
	tm *tm_end = localtime(&end_time);

	for(int n=0;n<count_data_uint;n++)
	{

		int lenDataBuf = 0;
		tm *tm_start = localtime(&start_time);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_start->tm_year+1900)/100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_start->tm_year+1900)%100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_mon + 1);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_mday);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_hour);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_start->tm_min);

		tm *tm_end = localtime(&end_time);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_end->tm_year+1900)/100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd((tm_end->tm_year+1900)%100);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_mon+1);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_mday);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_hour);
		v_data_uint[n].data_buffer[lenDataBuf++] = ptcl_module::bin_to_bcd(tm_end->tm_min);

		v_data_uint[n].len_data = lenDataBuf;
	}

	ptcl_module_csg::get_instance()->format_packet_read_alarm_data(packet,v_data_uint,count_data_uint);

}


