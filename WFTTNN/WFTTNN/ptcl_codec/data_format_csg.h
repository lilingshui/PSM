#pragma once
#include "ptcl_packet_csg.h"
#include "ptcl_module_csg.h"
#include <string>



class data_format_csg
{
protected:
	data_format_csg(void);
	virtual ~data_format_csg(void);
public:

	static data_format_csg *get_instance();

	//读历史数据
	void format_read_history( ptcl_packet_csg *packet,
		                      ptcl_module_csg::pnfn_data_uint *v_data_uint,int count_data_uint,
							  time_t start_time,time_t end_time,int densy);



	//读事件记录

	void format_read_event_record( ptcl_packet_csg *packet,
		                      ptcl_module_csg::pnfn_data_uint *v_data_uint,int count_data_uint,
							  time_t start_time,time_t end_time);

	//读告警数据
	void format_read_alarm_data( ptcl_packet_csg *packet,
		                      ptcl_module_csg::pnfn_data_uint *v_data_uint,int count_data_uint,
							  time_t start_time,time_t end_time);




protected:
	static data_format_csg *instance_;
	unsigned __int64 hexStr2IntAddr(std::string const & addrStr);
	unsigned int hexStr2IntFn(std::string const & addrStr);

};
