#pragma once
#include<vector>
#include<string>
#include"ptcl_module_csg.h"
#include "data_parse_dlt.h"
class  data_parse_csg_realtime
{
public:
	struct data_item_value
	{
		data_item_value ()
		{
			meter_index = 0;
			data_item_id = 0;
			db_value = 0;
			db_time = 0;
		};

		unsigned int meter_index;
		unsigned int data_item_id;
		double db_value;
		int db_time;//数据项中的发生时间，如最大需量发生时间
	};

public:
	data_parse_csg_realtime(void);
	~data_parse_csg_realtime(void);

	//解析实时数据
	static int parse_realtime_data(unsigned char *buf,int len,std::vector <data_item_value> & v_values);

protected:
	static int parse_fn_realtime(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_cur_4_2(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_cur_5_4(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);//MODIFY BY WM 2016-2-24
	static int parse_cur_2_1(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_cur_3_3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_cur_2_3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_cur_3_4(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_cur_demand(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//电表日期
	static int parse_04000101(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//电表时间
	static int parse_04000102(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
};

class  data_parse_csg_history
{
public:
	struct data_item_value
	{
		data_item_value ()
		{
			meter_index = 0;
			data_item_id = 0;
			value_time = 0;
			db_value = 0;
			db_time = 0;
		};

		unsigned int meter_index;
		unsigned int data_item_id;
		unsigned int value_time;
		double db_value;
		int db_time;  //数据项中的数据发生时间，目前只有需量数据使用
	};

public:
	data_parse_csg_history(void);
	~data_parse_csg_history(void);

	//解析历史数据
	static int parse_history_data(unsigned char *buf,int len,std::vector <data_item_value> & v_values);

protected:

	static int parse_fn_history(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_his_4_2(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_his_2_1(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_his_3_3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_his_2_3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_his_3_4(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_his_demand(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	

};

class  data_parse_csg_parameter
{
public:
	struct data_item_value
	{
		data_item_value ()
		{
			meter_index = 0;
			data_item_id = 0;
			db_value = "";
		};

		unsigned int meter_index;
		unsigned int data_item_id;
		std::string db_value;
	

	};

public:
	data_parse_csg_parameter(void);
	~data_parse_csg_parameter(void);

	//解析参数
	static int parse_parameter(unsigned char *buf,int len,std::vector <data_item_value> & v_values);

protected:

	static int parse_fn_parameter(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//终端时钟
    static int parse_E0000130(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//终端硬件版本号
	static int parse_E0000B02(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//终端软件版本号
	static int parse_E0000B01(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
    //
	static int parse_NN(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//电表通讯端口
	static int parse_E080000A(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//电表地址
	static int parse_E0800002(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//测量点端口参数
	static int parse_E080000B(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);	
	//额定电压
	static int parse_E0800011(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//额定电流
	static int parse_E0800012(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//抄表配置参数
	static int parse_E0000230(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
};

class  data_parse_event_record
{
public:
	struct data_item_value
	{
		data_item_value ()
		{
			meter_index = 0;
			data_item_id = 0;
			db_value = "";
			start_time = 0;
			end_time = 0;
		};

		unsigned int meter_index;
		unsigned int data_item_id;
		std::string db_value;
		unsigned int start_time;
		unsigned int end_time;
	};

public:
	data_parse_event_record(void);
	~data_parse_event_record(void);

	//解析参数
	static int parse_event_record(unsigned char *buf,int len,std::vector <data_item_value> & v_values);

protected:

	static int parse_fn_event_record( ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_record_ERD1(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_record_ERD2(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_record_ERD3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_record_ERD4(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_record_ERD8(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_record_ERD9(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
};

class  data_parse_alarm_data
{
public:
	struct data_item_value
	{
		data_item_value ()
		{
			meter_index = 0;
			data_item_id = 0;
			db_value = "";
			db_time = 0;
		};

		unsigned int meter_index;
		unsigned int data_item_id;
		std::string db_value;
		unsigned int db_time;

	};

public:
	data_parse_alarm_data(void);
	~data_parse_alarm_data(void);

	//解析参数
	static int parse_alarm_data(unsigned char *buf,int len,std::vector <data_item_value> & v_values);

protected:
	static int parse_fn_alarm_data( ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_alarm_ARD1(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_alarm_ARD2(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_alarm_ARD7(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_alarm_ARD11(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	static int parse_alarm_ARD13(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
};

class  parse_transmit
{
public:
	parse_transmit(void);
	~parse_transmit(void);

	//解析数据
	static int parse_transmit_data(unsigned char *buf,int len,std::vector <parse_read_data::data_item_value> & v_values);
};









