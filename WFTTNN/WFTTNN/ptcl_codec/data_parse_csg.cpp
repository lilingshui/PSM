#include "data_parse_csg.h"
#include "ptcl_module_csg.h"
#include <time.h>



data_parse_csg_realtime::data_parse_csg_realtime(void)
{
}

data_parse_csg_realtime::~data_parse_csg_realtime(void)
{
}

int data_parse_csg_realtime::parse_realtime_data(unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_parse = 0;
	while (len - len_parse > 6)
	{
		ptcl_module_csg::pnfn_data pnfn;
		ptcl_module_csg::parse_pnfn(buf+len_parse,&pnfn);
		len_parse = len_parse + 6;

		int fn_len = 0;

		fn_len = parse_fn_realtime(pnfn,buf+len_parse,len-len_parse,v_values);

		if (fn_len == 0)
			return 0;

		len_parse = len_parse + fn_len;
	}

	return len_parse;
}



int data_parse_csg_realtime::parse_fn_realtime(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int fn_len = 0;
	switch (pnfn.fn)
	{
	case 0x00010000:
	case 0x00010100:
	case 0x00010200:
	case 0x00010300:
	case 0x00010400:
	case 0x00020000:
	case 0x00020100:
	case 0x00020200:
	case 0x00020300:
	case 0x00020400:
	case 0x00030000:
	case 0x00030100:
	case 0x00030200:
    case 0x00030300:
	case 0x00030400:
	case 0x00040000:
    case 0x00040100:
    case 0x00040200:
	case 0x00040300:
	case 0x00040400:
	case 0x00050000:
	case 0x00050100:
	case 0x00050200:
	case 0x00050300:
	case 0x00050400:
	case 0x00060000:
	case 0x00060100:
	case 0x00060200:
	case 0x00060300:
	case 0x00060400:
	case 0x00070000:
	case 0x00070100:
	case 0x00070200:
	case 0x00070300:
	case 0x00070400:
	case 0x00080000:
	case 0x00080100:
	case 0x00080200:
	case 0x00080300:
	case 0x00080400:
		fn_len = parse_cur_4_2(pnfn,buf,len,v_values);
		break;
	case 0x00600000:
	case 0x00600100:
    case 0x00600200:
	case 0x00600300:
	case 0x00600400:
	case 0x00610000:
	case 0x00610100:
    case 0x00610200:
	case 0x00610300:
	case 0x00610400:
		fn_len = parse_cur_5_4(pnfn,buf,len,v_values);
		break;
	case 0x02010100:
	case 0x02010200:
	case 0x02010300:
		buf[0] = buf[0] & 0xF0;  //MODIFY BY WM 20131023 为了与97规约匹配，去掉小数点位
		fn_len = parse_cur_2_1(pnfn,buf,len,v_values);
		break;

	case 0x02020100:
	case 0x02020200:
	case 0x02020300:
		buf[0] = buf[0] & 0xF0;  //MODIFY BY WM 20131023 为了与97规约匹配，只留二位小数点位
		fn_len = parse_cur_3_3(pnfn,buf,len,v_values);
		break;

	case 0x02030000:
	case 0x02030100:
	case 0x02030200:
	case 0x02030300:
		fn_len = parse_cur_3_4(pnfn,buf,len,v_values);
		break;
	case 0x02040000:
	case 0x02040100:
	case 0x02040200:
	case 0x02040300:
		buf[0] = 0; //MODIFY BY WM 20131023 为了与97规约匹配，只留二位小数点位
		fn_len = parse_cur_3_4(pnfn,buf,len,v_values);
		break;

	case 0x02060000:
	case 0x02060100:
	case 0x02060200:
	case 0x02060300:
		fn_len = parse_cur_2_3(pnfn,buf,len,v_values);
		break;
	case 0x01010000:
	case 0x01010100:
	case 0x01010200:
	case 0x01010300:
	case 0x01010400:
	case 0x01020000:
	case 0x01020100:
	case 0x01020200:
	case 0x01020300:
	case 0x01020400:
	case 0x01030000:
	case 0x01030100:
	case 0x01030200:
	case 0x01030300:
	case 0x01030400:
	case 0x01040000:
	case 0x01040100:
	case 0x01040200:
	case 0x01040300:
	case 0x01040400:
		fn_len = parse_cur_demand(pnfn,buf,len,v_values);
		break;
	case 0x04000101:
		fn_len = parse_04000101(pnfn,buf,len,v_values);
		break;
	case 0x04000102:
		fn_len = parse_04000102(pnfn,buf,len,v_values);
		break;
		
	}

	return fn_len;
}


int data_parse_csg_realtime::parse_cur_4_2(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = ptcl_module::bcd_to_bin(buf,4,2);
	v_values.push_back(value);

	return 4;
}

int data_parse_csg_realtime::parse_cur_5_4(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)  //MODIFY BY WM 2016-2-24
{
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = ptcl_module::bcd_to_bin(buf,5,4);
	v_values.push_back(value);

	return 5;
}

int data_parse_csg_realtime::parse_cur_2_1(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = ptcl_module::bcd_to_bin(buf,2,1);
	v_values.push_back(value);

	return 2;
}

int data_parse_csg_realtime::parse_cur_3_3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	if (buf[2] >> 7 == 1 )
	{
		buf[2] = buf[2] & 0x7F;
		value.db_value = 0 - ptcl_module::bcd_to_bin(buf,3,3);
	}
	else
		value.db_value = ptcl_module::bcd_to_bin(buf,3,3);
	v_values.push_back(value);

	return 3;
}

int data_parse_csg_realtime::parse_cur_3_4(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	if (buf[2] >> 7 == 1 )
	{
		buf[2] = buf[2] & 0x7F;
		value.db_value = 0 - ptcl_module::bcd_to_bin(buf,3,4);
	}
	else
		value.db_value = ptcl_module::bcd_to_bin(buf,3,4);
	v_values.push_back(value);

	return 3;
}

int data_parse_csg_realtime::parse_cur_2_3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	if (buf[1] >> 7 == 1 )
	{
		buf[1] = buf[1] & 0x7F;
		value.db_value = 0 - ptcl_module::bcd_to_bin(buf,2,3);
	}
	else
		value.db_value = ptcl_module::bcd_to_bin(buf,2,3);
	v_values.push_back(value);

	return 2;
}

int data_parse_csg_realtime::parse_cur_demand(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 8)
		return 0;

	int data_len = 0;

	unsigned int db_time = 0;
	tm time_tm;
	ptcl_module::bcd_to_time(buf+data_len,5,ptcl_module::time_type_mmhhddmmyy,&time_tm);
	db_time = (unsigned int)mktime(&time_tm);
	data_len = data_len + 5;

	double db_value = 0;
	if (buf[data_len + 2] >> 7 == 1 )
	{
		buf[data_len + 2] = buf[data_len + 2] & 0x7F;
		db_value = 0 - ptcl_module::bcd_to_bin(buf + data_len,3,4);
	}
	else
		db_value = ptcl_module::bcd_to_bin(buf + data_len,3,4);
	data_len = data_len + 3;	
	
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = db_value;
	value.db_time = db_time;
	v_values.push_back(value);

	return data_len;
}

int data_parse_csg_realtime::parse_04000101(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 4)
		return 0;

	int data_len = 0;

	data_len = data_len + 1;//星期

	unsigned int db_time = 0;
	tm time_tm;
	ptcl_module::bcd_to_time(buf+data_len,3,ptcl_module::time_type_ddmmyy,&time_tm);
	db_time = (unsigned int)mktime(&time_tm);
	data_len = data_len + 4;
	
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = db_time;
	v_values.push_back(value);

	return data_len;
}

int data_parse_csg_realtime::parse_04000102(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 3)
		return 0;

	int data_len = 0;

	unsigned int db_time = 0;
	tm time_tm;
	ptcl_module::bcd_to_time(buf+data_len,3,ptcl_module::time_type_ssmmhh,&time_tm);
	db_time = (unsigned int)mktime(&time_tm);
	data_len = data_len + 3;
	
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = db_time;
	v_values.push_back(value);

	return data_len;
}




////////////////////////////////////////
int data_parse_csg_history::parse_history_data(unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL)
		return 0;
	
	int len_parse = 0;

	while (len - len_parse > 6 && len_parse < len)
	{
		ptcl_module_csg::pnfn_data pnfn;
	    ptcl_module_csg::parse_pnfn(buf+len_parse,&pnfn);
		len_parse = len_parse + 6;
		int fn_len = 0;
		fn_len = parse_fn_history(pnfn,buf+len_parse+fn_len,len-len_parse,v_values);

		if (fn_len == 0)
			return 0;

		len_parse = len_parse + fn_len;
	}

	return len_parse;
}

int data_parse_csg_history::parse_fn_history(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int fn_len = 0;
	switch (pnfn.fn)
	{
	case 0x00010000:
	case 0x00010100:
	case 0x00010200:
	case 0x00010300:
	case 0x00010400:
	case 0x00020000:
	case 0x00020100:
	case 0x00020200:
	case 0x00020300:
	case 0x00020400:
	case 0x00030000:
	case 0x00030100:
	case 0x00030200:
    case 0x00030300:
	case 0x00030400:
	case 0x00040000:
    case 0x00040100:
    case 0x00040200:
	case 0x00040300:
	case 0x00040400:
	case 0x00050000:
	case 0x00050100:
	case 0x00050200:
	case 0x00050300:
	case 0x00050400:
	case 0x00060000:
	case 0x00060100:
	case 0x00060200:
	case 0x00060300:
	case 0x00060400:
	case 0x00070000:
	case 0x00070100:
	case 0x00070200:
	case 0x00070300:
	case 0x00070400:
	case 0x00080000:
	case 0x00080100:
	case 0x00080200:
	case 0x00080300:
	case 0x00080400:

	case 0x05060100:
	case 0x05060101:
	case 0x05060102:
	case 0x05060103:
	case 0x05060104:
	case 0x05060200:
	case 0x05060201:
	case 0x05060202:
	case 0x05060203:
	case 0x05060204:
	case 0x05060300:
	case 0x05060301:
	case 0x05060302:
	case 0x05060303:
	case 0x05060304:
	case 0x05060400:
	case 0x05060401:
	case 0x05060402:
	case 0x05060403:
	case 0x05060404:
	case 0x05060500:
	case 0x05060501:
	case 0x05060502:
	case 0x05060503:
	case 0x05060504:
	case 0x05060600:
	case 0x05060601:
	case 0x05060602:
	case 0x05060603:
	case 0x05060604:
	case 0x05060700:
	case 0x05060701:
	case 0x05060702:
	case 0x05060703:
	case 0x05060704:
	case 0x05060800:
	case 0x05060801:
	case 0x05060802:
	case 0x05060803:
	case 0x05060804:

	case 0x00010001:
	case 0x00010101:
	case 0x00010201:
	case 0x00010301:
	case 0x00010401:
	case 0x00020001:
	case 0x00020101:
	case 0x00020201:
	case 0x00020301:
	case 0x00020401:
	case 0x00030001:
	case 0x00030101:
	case 0x00030201:
	case 0x00030301:
	case 0x00030401:
	case 0x00040001:
	case 0x00040101:
	case 0x00040201:
	case 0x00040301:
	case 0x00040401:
	case 0x00050001:
	case 0x00050101:
	case 0x00050201:
	case 0x00050301:
	case 0x00050401:
	case 0x00060001:
	case 0x00060101:
	case 0x00060201:
	case 0x00060301:
	case 0x00060401:
	case 0x00070001:
	case 0x00070101:
	case 0x00070201:
	case 0x00070301:
	case 0x00070401:
	case 0x00080001:
	case 0x00080101:
	case 0x00080201:
	case 0x00080301:
	case 0x00080401:

		fn_len = parse_his_4_2(pnfn,buf,len,v_values);
		break;

	case 0x02010100:
	case 0x02010200:
	case 0x02010300:
		buf[0] = buf[0] & 0xF0;  //MODIFY BY WM 20131023 为了与97规约匹配，去掉小数点位
		fn_len = parse_his_2_1(pnfn,buf,len,v_values);
		break;

	case 0x02020100:
	case 0x02020200:
	case 0x02020300:
		buf[0] = buf[0] & 0xF0;  //MODIFY BY WM 20131023 为了与97规约匹配，只留二位小数点位
		fn_len = parse_his_3_3(pnfn,buf,len,v_values);
		break;

	case 0x02030000:
	case 0x02030100:
	case 0x02030200:
	case 0x02030300:
		fn_len = parse_his_3_4(pnfn,buf,len,v_values);
		break;

	case 0x02040000:
	case 0x02040100:
	case 0x02040200:
	case 0x02040300:
		buf[0] = 0; //MODIFY BY WM 20131023 为了与97规约匹配，只留二位小数点位
		fn_len = parse_his_3_4(pnfn,buf,len,v_values);
		break;

	case 0x02060000:
	case 0x02060100:
	case 0x02060200:
	case 0x02060300:
		fn_len = parse_his_2_3(pnfn,buf,len,v_values);
		break;
	case 0x01010000:
	case 0x01010100:
	case 0x01010200:
	case 0x01010300:
	case 0x01010400:
	case 0x01020000:
	case 0x01020100:
	case 0x01020200:
	case 0x01020300:
	case 0x01020400:
	case 0x01030000:
	case 0x01030100:
	case 0x01030200:
	case 0x01030300:
	case 0x01030400:
	case 0x01040000:
	case 0x01040100:
	case 0x01040200:
	case 0x01040300:
	case 0x01040400:

	case 0x05060900:
	case 0x05060901:
	case 0x05060902:
	case 0x05060903:
	case 0x05060904:
	case 0x05060A00:
	case 0x05060A01:
	case 0x05060A02:
	case 0x05060A03:
	case 0x05060A04:
	case 0x05060B00:
	case 0x05060B01:
	case 0x05060B02:
	case 0x05060B03:
	case 0x05060B04:
	case 0x05060C00:
	case 0x05060C01:
	case 0x05060C02:
	case 0x05060C03:
	case 0x05060C04:

	case 0x01010001:
	case 0x01010101:
	case 0x01010201:
	case 0x01010301:
	case 0x01010401:
	case 0x01020001:
	case 0x01020101:
	case 0x01020201:
	case 0x01020301:
	case 0x01020401:
	case 0x01030001:
	case 0x01030101:
	case 0x01030201:
	case 0x01030301:
	case 0x01030401:
	case 0x01040001:
	case 0x01040101:
	case 0x01040201:
	case 0x01040301:
	case 0x01040401:
	case 0x01050001:
	case 0x01050101:
	case 0x01050201:
	case 0x01050301:
	case 0x01050401:
	case 0x01060001:
	case 0x01060101:
	case 0x01060201:
	case 0x01060301:
	case 0x01060401:
	case 0x01070001:
	case 0x01070101:
	case 0x01070201:
	case 0x01070301:
	case 0x01070401:
	case 0x01080001:
	case 0x01080101:
	case 0x01080201:
	case 0x01080301:
	case 0x01080401:
		fn_len = parse_his_demand(pnfn,buf,len,v_values);
		break;
	}

	return fn_len;
}

int data_parse_csg_history::parse_his_2_3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 7)
		return 0;

	double db_value = 0;
	db_value = ptcl_module::bcd_to_bin(buf,2,3);

	unsigned int value_t = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(buf+2,6,ptcl_module::time_type_yyyymmddhhmm,&value_tm);
	value_t = (unsigned int)mktime(&value_tm);

	
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.value_time = value_t;
	value.db_value = db_value;
	v_values.push_back(value);

	return 8;
}
int data_parse_csg_history::parse_his_3_4(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 8)
		return 0;

	double db_value = 0;
	db_value = ptcl_module::bcd_to_bin(buf,3,4);

	unsigned int value_t = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(buf+3,6,ptcl_module::time_type_yyyymmddhhmm,&value_tm);
	value_t = (unsigned int)mktime(&value_tm);

	
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.value_time = value_t;
	value.db_value = db_value;
	v_values.push_back(value);

	return 9;
}
int data_parse_csg_history::parse_his_3_3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 8)
		return 0;

	double db_value = 0;
	db_value = ptcl_module::bcd_to_bin(buf,3,3);

	unsigned int value_t = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(buf+3,6,ptcl_module::time_type_yyyymmddhhmm,&value_tm);
	value_t = (unsigned int)mktime(&value_tm);

	
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.value_time = value_t;
	value.db_value = db_value;
	v_values.push_back(value);

	return 9;
}
int data_parse_csg_history::parse_his_2_1(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 7)
		return 0;

	double db_value = 0;
	db_value = ptcl_module::bcd_to_bin(buf,2,1);

	unsigned int value_t = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(buf+2,6,ptcl_module::time_type_yyyymmddhhmm,&value_tm);
	value_t = (unsigned int)mktime(&value_tm);

	
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.value_time = value_t;
	value.db_value = db_value;
	v_values.push_back(value);

	return 8;
}

int data_parse_csg_history::parse_his_4_2(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 9)
		return 0;

	double db_value = 0;
	db_value = ptcl_module::bcd_to_bin(buf,4,2);

	unsigned int value_t = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(buf+4,6,ptcl_module::time_type_yyyymmddhhmm,&value_tm);
	value_t = (unsigned int)mktime(&value_tm);

	
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.value_time = value_t;
	value.db_value = db_value;
	v_values.push_back(value);

	return 10;
}

int data_parse_csg_history::parse_his_demand(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 13)
		return 0;

	int data_len = 0;

	unsigned int db_time = 0;
	tm time_tm;
	ptcl_module::bcd_to_time(buf+data_len,5,ptcl_module::time_type_mmhhddmmyy,&time_tm);
	db_time = (unsigned int)mktime(&time_tm);
	data_len = data_len + 5;

	double db_value = 0;
	db_value = ptcl_module::bcd_to_bin(buf+data_len,3,4);
	data_len = data_len + 3;
	
	unsigned int value_t = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(buf+data_len,6,ptcl_module::time_type_yyyymmddhhmm,&value_tm);
	value_t = (unsigned int)mktime(&value_tm);
	data_len = data_len + 6;
	
	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.value_time = value_t;
	value.db_value = db_value;
	value.db_time = db_time;
	v_values.push_back(value);

	return data_len;
}


/////////////////////////////
data_parse_csg_parameter::data_parse_csg_parameter(void)
{

}

data_parse_csg_parameter::~data_parse_csg_parameter(void)
{
}

int data_parse_csg_parameter::parse_parameter(unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_parse = 0;
	while (len - len_parse > 6)
	{
		ptcl_module_csg::pnfn_data pnfn;
		ptcl_module_csg::parse_pnfn(buf+len_parse,&pnfn);
		len_parse = len_parse + 6;

		int fn_len = 0;
		fn_len = parse_fn_parameter(pnfn,buf+len_parse,len-len_parse,v_values);

		if (fn_len == 0)
			return 0;

		len_parse = len_parse + fn_len;
	}

	return len_parse;
}



int data_parse_csg_parameter::parse_fn_parameter(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int fn_len = 0;
	switch (pnfn.fn)
	{
	
	case 0xE0000130:
		fn_len = parse_E0000130(pnfn,buf,len,v_values);
		break;	
	case 0xE0800001:
	case 0xE0800003:
	case 0xE0800004:
		fn_len = parse_NN(pnfn,buf,len,v_values);
		break;
	case 0xE080000A:
		fn_len = parse_E080000A(pnfn,buf,len,v_values);
		break;
	case 0xE0800002:
		fn_len = parse_E0800002(pnfn,buf,len,v_values);
		break;
	case 0xE0000B02:
		fn_len = parse_E0000B02(pnfn,buf,len,v_values);
		break;	
	case 0xE0000B01:
		fn_len = parse_E0000B01(pnfn,buf,len,v_values);
		break;	
	case 0xE080000B:
		fn_len = parse_E080000B(pnfn,buf,len,v_values);
		break;
	case 0xE0800011:
		fn_len = parse_E0800011(pnfn,buf,len,v_values);
		break;
	case 0xE0800012:
		fn_len = parse_E0800012(pnfn,buf,len,v_values);
		break;
	case 0xE0000230:
	case 0xE0000231:
	case 0xE0000232:
	case 0xE0000233:
	case 0xE0000234:
	case 0xE0000235:
	case 0xE0000236:
	case 0xE0000237:
		fn_len = parse_E0000230(pnfn,buf,len,v_values);
		break;
	}

	return fn_len;
}


//终端时钟
int data_parse_csg_parameter::parse_E0000130(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;

	char temp[100] = {0};
	sprintf(temp, "20%02X-%02X-%02X %02X:%02X:%02X", buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string sValue(temp);

		data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);

	return 6;
}

//终端硬件版本号
int data_parse_csg_parameter::parse_E0000B02(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;

	char temp[100] = {0};
	sprintf(temp, "%02X%02X", buf[len_fn+1],buf[len_fn]);		
	std::string sValue(temp);

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);

	return 2;
}

//一字节BIN数据
int data_parse_csg_parameter::parse_NN(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;
	char temp[100] = {0};
	sprintf(temp, "%2d", buf[len_fn]);	

	std::string sValue(temp);

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);


	return 1;
}

int data_parse_csg_parameter::parse_E080000A(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;
	char temp[100] = {0};
	sprintf(temp, "%2d", buf[len_fn]+1);	

	std::string sValue(temp);

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);


	return 1;
}



//电表地址
int data_parse_csg_parameter::parse_E0800002(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;
	char temp[100] = {0};
	sprintf(temp, "%02X%02X%02X%02X%02X%02X", buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string sValue(temp);

		data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);

	return 6;
}

//终端软件版本号
int data_parse_csg_parameter::parse_E0000B01(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;
	char temp[100] = {0};
	sprintf(temp, "%02X%02X-%02X-%02X-%02X-%02X", buf[len_fn+1],buf[len_fn],buf[len_fn+2],buf[len_fn+5],buf[len_fn+4],buf[len_fn+3]);		
	std::string sValue(temp);

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);

	return 6;
}

//测量点端口参数
int data_parse_csg_parameter::parse_E080000B(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;
	char temp[100] = {0};
	sprintf(temp, "%d,%d,%d,%d",buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string sValue(temp);

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);

	return 4;
}

//额定电压(二次侧)
int data_parse_csg_parameter::parse_E0800011(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;

	double db_value = 0;
	db_value = ptcl_module::bcd_to_bin(buf,2,1);

	char temp[100] = {0};
	sprintf(temp, "%.1f", db_value);	

	std::string sValue(temp);

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);


	return 2;
}

//额定电流
int data_parse_csg_parameter::parse_E0800012(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;

	double db_value = 0;
	db_value = ptcl_module::bcd_to_bin(buf,3,3);

	char temp[100] = {0};
	sprintf(temp, "%.3f", db_value);	

	std::string sValue(temp);

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);


	return 3;
}

//抄表配置参数
int data_parse_csg_parameter::parse_E0000230(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_fn = 0;
	char temp[8] = {0};
	sprintf(temp, "%d", buf[len_fn]);
	std::string sValue(temp);

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = sValue;
	v_values.push_back(value);

	len_fn += 5;
	// 信息点标识
	int nums = buf[len_fn];
	len_fn += 1;
	len_fn += (nums == 0xFF) ? 2 : (2*nums);
	// 数据标识
	nums = buf[len_fn];
	len_fn += 1;
	len_fn += (nums == 0xFF) ? 4 : (4*nums);
	
	return len_fn;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
data_parse_event_record::data_parse_event_record(void)
{

}

data_parse_event_record::~data_parse_event_record(void)
{
}

int data_parse_event_record::parse_event_record(unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_parse = 0;
	while (len - len_parse > 6)
	{
		ptcl_module_csg::pnfn_data pnfn;
		ptcl_module_csg::parse_pnfn(buf+len_parse,&pnfn);
		len_parse = len_parse + 6;

		int fn_len = 0;
		fn_len = parse_fn_event_record(pnfn,buf+len_parse,len-len_parse,v_values);

		if (fn_len == 0)
			return 0;

		len_parse = len_parse + fn_len;
	}

	return len_parse;
}

int data_parse_event_record::parse_fn_event_record(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int fn_len = 0;
	switch (pnfn.fn)
	{
	case 0xE2010001:
		fn_len = parse_record_ERD1(pnfn,buf,len,v_values);
		break;
	case 0xE2010002:
	case 0xE2010003:
		fn_len = parse_record_ERD2(pnfn,buf,len,v_values);
		break;
	case 0xE2010004:
	case 0xE2010005:
    case 0xE2010006:
	case 0xE2010007:
	case 0xE2010008:
    case 0xE2010009:
	case 0xE201000B:
	case 0xE201000C:
    case 0xE201000D:
    case 0xE201000F:
    case 0xE2010011:
	case 0xE2010012:
    case 0xE2010015:
		fn_len = parse_record_ERD3(pnfn,buf,len,v_values);
		break;	
	case 0xE201000A:
		fn_len = parse_record_ERD4(pnfn,buf,len,v_values);
		break;
	case 0xE2010014:
		fn_len = parse_record_ERD8(pnfn,buf,len,v_values);
		break;
	case 0xE2010010:
		fn_len = parse_record_ERD9(pnfn,buf,len,v_values);
		break;
	}

	return fn_len;
}

int data_parse_event_record::parse_record_ERD1(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 8)
		return 0;

	int len_fn = 0;
	//编程时间（YYMMDDhhmmss）
	char temp[100] = {0};
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string pro_time(temp);
    len_fn = len_fn + 6;
	
	//通信通道 
	int type = buf[len_fn++];
	std::string td = "";
	switch(type)
	{
	case 0x02:
		td = "GPRS/CDMA";
		break;
	case 0x03:
		td = "PSTN";
		break;
	case 0x04:
		td = "Ethernet";
		break;
	case 0x06:
		td = "RS232/RS485";
		break;
	case 0x07:
		td = "CSD";
		break;
	case 0x08:
		td = "Radio";
		break;
	case 0xFF:
		td = "无效";
		break;
	}

	//编程数据标识组数（BIN码，1字节）
	int n = buf[len_fn];
	sprintf(temp, "%d",buf[len_fn++]);		
	std::string count(temp);

	//编程内容
	int len_bcnr = len - len_fn;
	std::string bcnr = "";
	for(int i = 0; i <= len_bcnr; i ++)
	{
		sprintf(temp, "%02X",buf[len_fn+i]);		
		std::string s(temp);
		bcnr = bcnr + s;
	}
	

	std::string value_all = "编程时间:" + pro_time + " 通信通道:" + td
		                     +" 编程数据标识组数:" + count
							 +" 编程内容:" + bcnr;

	unsigned int startTime = 0; //ADD BY WM  20131020用于自动测试事件中只有发生时间事件的判断
	tm value_tm_start;
	ptcl_module::bcd_to_time(buf,6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm_start);
	startTime = (unsigned int)mktime(&value_tm_start);//END ADD

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = value_all;
	value.start_time = startTime; //ADD BY WM  20131020
	value.end_time = startTime;   //ADD BY WM  20131020
	v_values.push_back(value);

	return len_fn;
}

int data_parse_event_record::parse_record_ERD2(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 6)
		return 0;

	int len_fn = 0;
	//最大需量清零时间
	char temp[100] = {0};
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string ql_time(temp);
    len_fn = len_fn + 6;

	std::string value_all = "最大需量清零时间:" + ql_time;

	unsigned int startTime = 0; //ADD BY WM  20131020用于自动测试事件中只有发生时间事件的判断
	tm value_tm_start;
	ptcl_module::bcd_to_time(buf,6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm_start);
	startTime = (unsigned int)mktime(&value_tm_start);//END ADD

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = value_all;
	value.start_time = startTime; //ADD BY WM  20131020
	value.end_time = startTime;   //ADD BY WM  20131020
	v_values.push_back(value);

	return len_fn;
}


int data_parse_event_record::parse_record_ERD3(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 28)
		return 0;

	int len_fn = 0;
	//开始时间
	char temp[100] = {0};
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string start_time(temp);

	unsigned int startTime = 0;
	tm value_tm_start;
	ptcl_module::bcd_to_time(buf,6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm_start);
	startTime = (unsigned int)mktime(&value_tm_start);

    len_fn = len_fn + 6;


	//结束时间
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string end_time(temp);

	unsigned int endTime = 0;
	tm value_tm_end;
	ptcl_module::bcd_to_time(buf+len_fn,6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm_end);
	endTime = (unsigned int)mktime(&value_tm_end);

    len_fn = len_fn + 6;

	//正向有功总电能量（DI3DI2DI1DI0=00010000H）增量
	double db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string db_value_pp(temp);
    len_fn = len_fn + 4;

	//反向有功总电能量（DI3DI2DI1DI0=00020000H）增量
    db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string db_value_np(temp);
    len_fn = len_fn + 4;

	//组合无功1总电能量（DI3DI2DI1DI0=00030000H）增量
	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string db_value_np1(temp);
    len_fn = len_fn + 4;

	//组合无功2总电能量（DI3DI2DI1DI0=00040000H）增量
	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string db_value_np2(temp);
    len_fn = len_fn + 4;

	std::string value_all = "开始时间:" + start_time + " 结束时间:" + end_time
		                     +" 正向有功总电能量增量:" + db_value_pp
							 +" 反向有功总电能量增量:" + db_value_np
							 +" 组合无功1总电能量增量:" + db_value_np1
							 +" 组合无功2总电能量增量:" + db_value_np2;

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = value_all;
	value.start_time = startTime;
	value.end_time = endTime;
	v_values.push_back(value);

	return len_fn;
}


int data_parse_event_record::parse_record_ERD4(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 12)
		return 0;

	int len_fn = 0;
	//停电时刻
	char temp[100] = {0};
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string td_time(temp);
    len_fn = len_fn + 6;

	//复电时刻
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string fd_time(temp);
    len_fn = len_fn + 6;

	std::string value_all = "停电时刻:" + td_time + " 复电时刻:" + fd_time;

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = value_all;
	v_values.push_back(value);

	return len_fn;
}

int data_parse_event_record::parse_record_ERD8(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 10)
		return 0;

	int len_fn = 0;
	//参数变更发生时间
	char temp[100] = {0};
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string td_time(temp);
    len_fn = len_fn + 6;

	unsigned int startTime = 0; //ADD BY WM  20131020用于自动测试事件中只有发生时间事件的判断
	tm value_tm_start;
	ptcl_module::bcd_to_time(buf,6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm_start);
	startTime = (unsigned int)mktime(&value_tm_start);//END ADD

	std::string di_all = "";
	while(len - len_fn >= 4)
	{
	  //变更参数1标识编码（DI3DI2DI1DI0）
	  char temp[100] = {0};
	  sprintf(temp, "%02X%02X%02X%02X",buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	  std::string di(temp);
      len_fn = len_fn + 4;
	  di_all = di_all + " 变更参数标识编码:" +  di;
	}

	std::string value_all = "参数变更发生时间:" + td_time + di_all;

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = value_all;
	value.start_time = startTime; //ADD BY WM
	value.end_time = startTime;   //ADD BY WM
	v_values.push_back(value);

	return len_fn;
}

int data_parse_event_record::parse_record_ERD9(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 14)
		return 0;

	int len_fn = 0;
	//校时发生时间
	char temp[100] = {0};
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string td_time(temp);
    len_fn = len_fn + 6;

	//正向有功总电能（DI3DI2DI1DI0=00010000H）
	double db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%f",db_value);		
	std::string db_value_pp(temp);
    len_fn = len_fn + 4;

	//反向有功总电能（DI3DI2DI1DI0=00020000H）
    db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%f",db_value);		
	std::string db_value_np(temp);
    len_fn = len_fn + 4;

	
	std::string value_all = "校时发生时间:" + td_time +
		                    " 正向有功总电能:" + db_value_pp +
							" 反向有功总电能:" + db_value_np;

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = value_all;
	v_values.push_back(value);

	return len_fn;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
data_parse_alarm_data::data_parse_alarm_data(void)
{

}

data_parse_alarm_data::~data_parse_alarm_data(void)
{
}

int data_parse_alarm_data::parse_alarm_data(unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int len_parse = 0;
	while (len - len_parse > 6)
	{
		ptcl_module_csg::pnfn_data pnfn;
		ptcl_module_csg::parse_pnfn(buf+len_parse,&pnfn);
		len_parse = len_parse + 6;

		int fn_len = 0;
		fn_len = parse_fn_alarm_data(pnfn,buf+len_parse,len-len_parse,v_values);

		if (fn_len == 0)
			return 0;

		len_parse = len_parse + fn_len;
	}

	return len_parse;
}

int data_parse_alarm_data::parse_fn_alarm_data(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int fn_len = 0;
	switch (pnfn.fn)
	{
	
	case 0xE200002F: //电能表通讯失败
	case 0xE200003F: //电能表校时失败
		fn_len = parse_alarm_ARD1(pnfn,buf,len,v_values);
		break;	
	case 0xE2000001: //计量装置门开闭
	case 0xE2000002: //电流反极性
	case 0xE2000003: //电压逆相序
	case 0xE2000004: //电流反极性
	case 0xE2000005:
	case 0xE2000006:
	case 0xE2000007:
	case 0xE2000008:
	case 0xE2000009:
	case 0xE200000A:
	case 0xE200000B:
	case 0xE200000C:
	case 0xE200000D:
	case 0xE200000E:
	case 0xE200000F:
	case 0xE2000010:
	case 0xE2000011:
	case 0xE2000012:
	case 0xE2000013:
	case 0xE2000014:
	case 0xE2000015:
	case 0xE2000016:
	case 0xE2000017:
	case 0xE2000018:
	case 0xE2000019:
	case 0xE200001A:
	case 0xE200001B:
	case 0xE200001C:
	case 0xE200001D:
	case 0xE200001E:
	case 0xE200001F:
	case 0xE2000033: //终端掉电
   	case 0xE2000034: //终端上电
		fn_len = parse_alarm_ARD2(pnfn,buf,len,v_values);
		break;
   	case 0xE2000031: 
		fn_len = parse_alarm_ARD7(pnfn,buf,len,v_values);
		break;
	  case 0xE200004D: 
	  case 0xE200004E:  //表盖开启告警
		fn_len = parse_alarm_ARD11(pnfn,buf,len,v_values);
		break;
	case 0xE200003E: //电能表时钟异常
		fn_len = parse_alarm_ARD13(pnfn,buf,len,v_values);
		break;	
	}

	return fn_len;
}

std::string get_alarm_state(int alarm_state)
{
	switch(alarm_state)
	{
	case 0x01:
		return "发生";
		break;
	case 0x00:
		return "恢复";
		break;
	case 0xFF:
		return "无效";
		break;
	}
}

int data_parse_alarm_data::parse_alarm_ARD1(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 7)
		return 0;

	int len_fn = 0;

	//告警状态
	char temp[100] = {0};

	std::string alarm_state = get_alarm_state(buf[len_fn]);
	len_fn++;

	//告警发生时间
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string alarm_time(temp);

	unsigned int db_time = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(&buf[1],6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm);//MODIFY BY WM 20131021
	db_time = (unsigned int)mktime(&value_tm);

	std::string db_value = "告警状态:" + alarm_state + " 告警时间" + alarm_time;

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = db_value;
	value.db_time = db_time;
	v_values.push_back(value);

	return 7;	
}

int data_parse_alarm_data::parse_alarm_ARD2(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 60)
		return 0;

	int len_fn = 0;

	//告警状态
	char temp[100] = {0};
	std::string alarm_state = get_alarm_state(buf[len_fn]);
	len_fn++;
	//告警发生时间
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string alarm_time(temp);

	unsigned int db_time = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(&buf[1],6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm);//MODIFY BY WM 20131021
	db_time = (unsigned int)mktime(&value_tm);

	len_fn = len_fn + 6;

	//发生时数据
	std::string value_all = " 发生时数据:";

	double db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value(temp);
	value_all = value_all + " 正向有功总电能:" + s_value;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value1(temp);
	value_all = value_all + " 反向有功总电能:" + s_value1;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value2(temp);
	value_all = value_all + " 组合无功1总电能:" + s_value2;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value3(temp);
	value_all = value_all + " 组合无功2总电能:" + s_value3;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,2,1);
	sprintf(temp, "%.1f",db_value);		
	std::string s_value4(temp);
	value_all = value_all + " A相电压:" + s_value4;
    len_fn = len_fn + 2;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,2,1);
	sprintf(temp, "%.1f",db_value);		
	std::string s_value5(temp);
	value_all = value_all + " B相电压:" + s_value5;
    len_fn = len_fn + 2;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,2,1);
	sprintf(temp, "%.1f",db_value);		
	std::string s_value6(temp);
	value_all = value_all + " C相电压:" + s_value6;
    len_fn = len_fn + 2;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,3);
	sprintf(temp, "%.3f",db_value);		
	std::string s_value7(temp);
	value_all = value_all + " A相电流:" + s_value7;
    len_fn = len_fn + 3;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,3);
	sprintf(temp, "%.3f",db_value);		
	std::string s_value8(temp);
	value_all = value_all + " B相电流:" + s_value8;
    len_fn = len_fn + 3;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,3);
	sprintf(temp, "%.3f",db_value);		
	std::string s_value9(temp);
	value_all = value_all + " C相电流:" + s_value9;
    len_fn = len_fn + 3;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value10(temp);
	value_all = value_all + " 总有功功率:" + s_value10;
    len_fn = len_fn + 3;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value11(temp);
	value_all = value_all + " A相有功功率:" + s_value11;
    len_fn = len_fn + 3;


	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value12(temp);
	value_all = value_all + " B相有功功率:" + s_value12;
    len_fn = len_fn + 3;


	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value13(temp);
	value_all = value_all + " C相有功功率:" + s_value13;
    len_fn = len_fn + 3;


	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value14(temp);
	value_all = value_all + " 总无功功率:" + s_value14;
    len_fn = len_fn + 3;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value15(temp);
	value_all = value_all + " A相无功功率:" + s_value15;
    len_fn = len_fn + 3;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value16(temp);
	value_all = value_all + " B相无功功率:" + s_value16;
    len_fn = len_fn + 3;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value17(temp);
	value_all = value_all + " C相无功功率:" + s_value17;
    len_fn = len_fn + 3;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,2,3);
	sprintf(temp, "%.3f",db_value);		
	std::string s_value18(temp);
	value_all = value_all + " 总功率因数:" + s_value18;
    len_fn = len_fn + 2;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,2,3);
	sprintf(temp, "%.3f",db_value);		
	std::string s_value19(temp);
	value_all = value_all + " A相功率因数:" + s_value19;
    len_fn = len_fn + 2;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,2,3);
	sprintf(temp, "%.3f",db_value);		
	std::string s_value20(temp);
	value_all = value_all + " B相功率因数:" + s_value20;
    len_fn = len_fn + 2;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,2,3);
	sprintf(temp, "%.3f",db_value);		
	std::string s_value21(temp);
	value_all = value_all + " C相功率因数:" + s_value21;
    len_fn = len_fn + 2;

	std::string s1_value = "告警状态:" + alarm_state + "告警时间" + alarm_time + value_all;

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = s1_value;
	value.db_time = db_time;
	v_values.push_back(value);

	return len_fn;	
}

int data_parse_alarm_data::parse_alarm_ARD7(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 44)
	     return 0;

	int len_fn = 0;

	//告警状态
	char temp[100] = {0};
	std::string alarm_state = get_alarm_state(buf[len_fn]);
	len_fn++;
	//告警发生时间
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string alarm_time(temp);

	unsigned int db_time = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(buf,6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm);
	db_time = (unsigned int)mktime(&value_tm);

	len_fn = len_fn + 6;

	//发生时数据
	std::string value_all = " 发生时数据:";

	double db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value(temp);
	value_all = value_all + " 正向有功总电能:" + s_value;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value1(temp);
	value_all = value_all + " 反向有功总电能:" + s_value1;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value2(temp);
	value_all = value_all + " 组合无功1总电能:" + s_value2;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value3(temp);
	value_all = value_all + " 组合无功2总电能:" + s_value3;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value4(temp);
	value_all = value_all + " 正向有功总最大需量及发生时间:" + s_value4;
    len_fn = len_fn + 3;

	sprintf(temp, "%02X-%02X-%02X %02X:%02X",buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string s_value5(temp);
	value_all = value_all  + "," + s_value5;
	len_fn = len_fn + 5;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value6(temp);
	value_all = value_all + " 反向有功总最大需量及发生时间:" + s_value6;
    len_fn = len_fn + 3;

	sprintf(temp, "%02X-%02X-%02X %02X:%02X",buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string s_value7(temp);
	value_all = value_all  + "," + s_value7;
	len_fn = len_fn + 5;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value8(temp);
	value_all = value_all + " 组合无功1总最大需量及发生时间:" + s_value8;
    len_fn = len_fn + 3;

	sprintf(temp, "%02X-%02X-%02X %02X:%02X",buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string s_value9(temp);
	value_all = value_all  + "," + s_value9;
	len_fn = len_fn + 5;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,3,4);
	sprintf(temp, "%.4f",db_value);		
	std::string s_value10(temp);
	value_all = value_all + " 组合无功2总最大需量及发生时间:" + s_value10;
    len_fn = len_fn + 3;

	sprintf(temp, "%02X-%02X-%02X %02X:%02X",buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string s_value11(temp);
	value_all = value_all  + "," + s_value11;
	len_fn = len_fn + 5;
	std::string s1_value = "告警状态:" + alarm_state + "告警时间" + alarm_time + value_all;

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = s1_value;
	value.db_time = db_time;
	v_values.push_back(value);

	return len_fn;	
}

int data_parse_alarm_data::parse_alarm_ARD11(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 56)
	     return 0;

	int len_fn = 0;

	//告警状态
	char temp[100] = {0};
	std::string alarm_state = get_alarm_state(buf[len_fn]);
	len_fn++;
	//告警发生时间
	sprintf(temp, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string alarm_time(temp);

	unsigned int db_time = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(buf,6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm);
	db_time = (unsigned int)mktime(&value_tm);

	len_fn = len_fn + 6;

	//发生时数据
	std::string value_all = " 发生前数据:";

	double db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value(temp);
	value_all = value_all + " 正向有功总电能:" + s_value;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value1(temp);
	value_all = value_all + " 反向有功总电能:" + s_value1;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value2(temp);
	value_all = value_all + " 第一象限无功总电能:" + s_value2;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value3(temp);
	value_all = value_all + " 第二象限无功总电能:" + s_value3;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value4(temp);
	value_all = value_all + " 第三象限无功总电能:" + s_value4;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value5(temp);
	value_all = value_all + " 第四象限无功总电能:" + s_value5;
    len_fn = len_fn + 4;

	 value_all = value_all + " 发生后数据:";

    db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value6(temp);
	value_all = value_all + " 正向有功总电能:" + s_value6;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value7(temp);
	value_all = value_all + " 反向有功总电能:" + s_value7;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value8(temp);
	value_all = value_all + " 第一象限无功总电能:" + s_value8;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value9(temp);
	value_all = value_all + " 第二象限无功总电能:" + s_value9;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value10(temp);
	value_all = value_all + " 第三象限无功总电能:" + s_value10;
    len_fn = len_fn + 4;

	db_value = ptcl_module::bcd_to_bin(buf+len_fn,4,2);
	sprintf(temp, "%.2f",db_value);		
	std::string s_value11(temp);
	value_all = value_all + " 第四象限无功总电能:" + s_value11;
    len_fn = len_fn + 4;

	std::string s1_value = "告警状态:" + alarm_state + "告警时间" + alarm_time + value_all;

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = s1_value;
	value.db_time = db_time;
	v_values.push_back(value);

	return len_fn;	
}



int data_parse_alarm_data::parse_alarm_ARD13(ptcl_module_csg::pnfn_data pnfn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 19)
	     return 0;

	int len_fn = 0;

	//告警状态
	char temp[100] = {0};
	std::string alarm_state = get_alarm_state(buf[len_fn]);
	len_fn++;

	//告警发生时间
	char temp6[100] = {0};
	sprintf(temp6, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string alarm_time(temp6);

	unsigned int db_time = 0;
	tm value_tm;
	ptcl_module::bcd_to_time(&buf[1],6,ptcl_module::time_type_ssmmhhddmmyy,&value_tm);//MODIFY BY WM 20131021
	db_time = (unsigned int)mktime(&value_tm);

	len_fn = len_fn + 6;

	//发生时数据
	//电表日期
	//电表时间
	sprintf(temp6, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn+6],buf[len_fn+5],buf[len_fn+4]);		
	std::string meter_time(temp6);
	len_fn = len_fn + 7;

	//终端时钟
	sprintf(temp6, "%02X-%02X-%02X %02X:%02X:%02X",buf[len_fn+5],buf[len_fn+4],buf[len_fn+3],buf[len_fn+2],buf[len_fn+1],buf[len_fn]);		
	std::string term_time(temp6);
	len_fn = len_fn + 6;

	std::string db_value = "告警状态:" + alarm_state + " 告警时间" + alarm_time + " 电表时钟:" + meter_time + " 终端时钟" + term_time;

	data_item_value value;
	value.meter_index = pnfn.pn;
	value.data_item_id = pnfn.fn;
	value.db_value = db_value;
	value.db_time = db_time;
	v_values.push_back(value);

	return len_fn;	
}

parse_transmit::parse_transmit(void)
{
}

parse_transmit::~parse_transmit(void)
{
}

int parse_transmit::parse_transmit_data(unsigned char *buf,int len,std::vector <parse_read_data::data_item_value> & v_values)
{
	int len_parse = 0;

	ptcl_module_csg::pnfn_data pnfn;
	ptcl_module_csg::parse_pnfn(buf+len_parse,&pnfn);
	len_parse = len_parse + 6;

	len_parse ++; //中继类型

	int lenDltPacket = buf[len_parse];
	len_parse ++;

	ptcl_packet_dlt packet;
	packet.set_packet_buffer(buf+len_parse,lenDltPacket);
	packet.parse_packet();
	int fn_len = 0;
	fn_len = parse_read_data::parse_read_data_current(packet.get_data_area_buffer(),packet.get_data_area_length(),v_values);

	if (fn_len == 0)
		return 0;

	len_parse = len_parse + fn_len;

	return len_parse;
}










