#pragma once
#include<vector>
#include<string>
#include"ptcl_module_dlt.h"

class  parse_read_data
{
public:
	struct data_item_value
	{
		data_item_value ()
		{
			data_item_id = 0;
			db_value = 0;
			value_time = 0;
		};

		unsigned short int data_item_id;
		double db_value;
		unsigned int value_time;
	};

public:
	parse_read_data(void);
	~parse_read_data(void);

	//解析数据
	static int parse_read_data_current(unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//解析曲线数据
	static int parse_read_data_curve(unsigned char *buf,int len,std::vector <data_item_value> & v_values);

protected:

	static int parse_fn(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当前正向有功电量
	static int parse_901f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当前反向有功电量
	static int parse_902f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当前正向无功电量
	static int parse_911f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当前反向无功电量
	static int parse_912f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当前一象限无功电量
	static int parse_913f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当前四象限无功电量
	static int parse_914f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当前二象限无功电量
	static int parse_915f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当前三象限无功电量
	static int parse_916f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月正向有功电量
	static int parse_941f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月反向有功电量
	static int parse_942f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月正向无功电量
	static int parse_951f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月反向无功电量
	static int parse_952f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月一象限无功电量
	static int parse_953f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月四象限无功电量
	static int parse_954f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月二象限无功电量
	static int parse_955f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月三象限无功电量
	static int parse_956f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上日正向有功电量
	static int parse_9c1f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上日反向有功电量
	static int parse_9c2f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上日正向无功电量
	static int parse_9c3f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上日反向无功电量
	static int parse_9c4f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上日一象限无功电量
	static int parse_9c5f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上日四象限无功电量
	static int parse_9c6f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上日二象限无功电量
	static int parse_9c7f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上日三象限无功电量
	static int parse_9c8f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//电压数据
	static int parse_b61f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//电流数据
	static int parse_b62f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//有功功率数据
	static int parse_b63f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//无功功率数据
	static int parse_b64f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//功率因数数据
	static int parse_b65f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当月正向有功最大需量数据
	static int parse_a01f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当月反向有功最大需量数据
	static int parse_a02f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当月正向无功最大需量数据
	static int parse_a11f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当月反向无功最大需量数据
	static int parse_a12f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当月正向有功最大需量发生时间数据
	static int parse_b01f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当月反向有功最大需量发生时间数据
	static int parse_b02f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当月正向无功最大需量发生时间数据
	static int parse_b11f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//当月反向无功最大需量发生时间数据
	static int parse_b12f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月正向有功最大需量数据
	static int parse_a41f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月反向有功最大需量数据
	static int parse_a42f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月正向无功最大需量数据
	static int parse_a51f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月反向无功最大需量数据
	static int parse_a52f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月正向有功最大需量发生时间数据
	static int parse_b41f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月反向有功最大需量发生时间数据
	static int parse_b42f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月正向无功最大需量发生时间数据
	static int parse_b51f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);
	//上月反向无功最大需量发生时间数据
	static int parse_b52f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values);

	//////////////////////////曲线数据/////////////////////////////////////////////////////////////////////////////////////////
	static int parse_fn_curve(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time);
	//分时电量曲线数据
	static int parse_9b1f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time);
	//象限无功电量曲线数据
	static int parse_9b5f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time);
	//最大需量曲线数据
	static int parse_aa1f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time);
	//瞬时量曲线数据
	static int parse_bb1f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time);
};


