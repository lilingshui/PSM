#include "data_parse_dlt.h"
#include "ptcl_module_dlt.h"
#include <time.h>

parse_read_data::parse_read_data(void)
{
}

parse_read_data::~parse_read_data(void)
{
}

int parse_read_data::parse_read_data_current(unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	
	int len_parse = 0;
	if (buf == NULL)
		return 0;

	int value_len = 0;
	double db_value = 0;

	while(len - len_parse > 2)
	{
		short int fn = ptcl_packet_dlt::get_data_addr(buf);
		len_parse = len_parse + 2;

		value_len = parse_fn(fn,buf+len_parse,len - len_parse,v_values);
		if (value_len <= 0) 
			return 0;
		
		len_parse += value_len;
	}

	return len_parse;

}


int parse_read_data::parse_fn(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	int fn_len = 0;
	switch (fn)
	{
	case (short int)0x901f:
		fn_len = parse_901f(fn,buf,len,v_values);
		break;
	case (short int)0x902f:
		fn_len = parse_902f(fn,buf,len,v_values);
		break;
	case (short int)0x911f:
		fn_len = parse_911f(fn,buf,len,v_values);
		break;
	case (short int)0x912f:
		fn_len = parse_912f(fn,buf,len,v_values);
		break;
	case (short int)0x913f:
		fn_len = parse_913f(fn,buf,len,v_values);
		break;
	case (short int)0x914f:
		fn_len = parse_914f(fn,buf,len,v_values);
		break;
	case (short int)0x915f:
		fn_len = parse_915f(fn,buf,len,v_values);
		break;
	case (short int)0x916f:
		fn_len = parse_916f(fn,buf,len,v_values);
		break;
	case (short int)0x941f:
		fn_len = parse_941f(fn,buf,len,v_values);
		break;
	case (short int)0x942f:
		fn_len = parse_942f(fn,buf,len,v_values);
		break;
	case (short int)0x951f:
		fn_len = parse_951f(fn,buf,len,v_values);
		break;
	case (short int)0x952f:
		fn_len = parse_952f(fn,buf,len,v_values);
		break;
	case (short int)0x953f:
		fn_len = parse_953f(fn,buf,len,v_values);
		break;
	case (short int)0x954f:
		fn_len = parse_954f(fn,buf,len,v_values);	
		break;
	case (short int)0x955f:
		fn_len = parse_955f(fn,buf,len,v_values);
		break;
	case (short int)0x956f:
		fn_len = parse_956f(fn,buf,len,v_values);
		break;
	case (short int)0x9c1f:
		fn_len = parse_9c1f(fn,buf,len,v_values);	
		break;
	case (short int)0x9c2f:
		fn_len = parse_9c2f(fn,buf,len,v_values);
		break;
	case (short int)0x9c3f:
		fn_len = parse_9c3f(fn,buf,len,v_values);
		break;
	case (short int)0x9c4f:
		fn_len = parse_9c4f(fn,buf,len,v_values);	
		break;
	case (short int)0x9c5f:
		fn_len = parse_9c5f(fn,buf,len,v_values);
		break;
	case (short int)0x9c6f:
		fn_len = parse_9c6f(fn,buf,len,v_values);
		break;
	case (short int)0x9c7f:
		fn_len = parse_9c7f(fn,buf,len,v_values);
		break;
	case (short int)0x9c8f:
		fn_len = parse_9c8f(fn,buf,len,v_values);	
		break;
	case (short int)0xa01f:
		fn_len = parse_a01f(fn,buf,len,v_values);
		break;
	case (short int)0xa02f:
		fn_len = parse_a02f(fn,buf,len,v_values);
		break;
	case (short int)0xa11f:
		fn_len = parse_a11f(fn,buf,len,v_values);
		break;
	case (short int)0xa12f:
		fn_len = parse_a12f(fn,buf,len,v_values);
		break;
	case (short int)0xb01f:
		fn_len = parse_b01f(fn,buf,len,v_values);
		break;
	case (short int)0xb02f:
		fn_len = parse_b02f(fn,buf,len,v_values);
		break;
	case (short int)0xb11f:
		fn_len = parse_b11f(fn,buf,len,v_values);
		break;
	case (short int)0xb12f:
		fn_len = parse_b12f(fn,buf,len,v_values);
		break;
	case (short int)0xa41f:
		fn_len = parse_a41f(fn,buf,len,v_values);
		break;
	case (short int)0xa42f:
		fn_len = parse_a42f(fn,buf,len,v_values);
		break;
	case (short int)0xa51f:
		fn_len = parse_a51f(fn,buf,len,v_values);
		break;
	case (short int)0xa52f:
		fn_len = parse_a52f(fn,buf,len,v_values);
		break;
	case (short int)0xb41f:
		fn_len = parse_b41f(fn,buf,len,v_values);
		break;
	case (short int)0xb42f:
		fn_len = parse_b42f(fn,buf,len,v_values);
		break;
	case (short int)0xb51f:
		fn_len = parse_b51f(fn,buf,len,v_values);
		break;
	case (short int)0xb52f:
		fn_len = parse_b52f(fn,buf,len,v_values);
		break;
	case (short int)0xb61f:
		fn_len = parse_b61f(fn,buf,len,v_values);
		break;
	case (short int)0xb62f:
		fn_len = parse_b62f(fn,buf,len,v_values);
		break;
	case (short int)0xb63f:
		fn_len = parse_b63f(fn,buf,len,v_values);
		break;
	case (short int)0xb64f:
		fn_len = parse_b64f(fn,buf,len,v_values);
		break;
	case (short int)0xb65f:
		fn_len = parse_b65f(fn,buf,len,v_values);
		break;
	}

	return fn_len;


}

int parse_read_data::parse_901f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9010 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_902f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9020 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_911f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9110 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_912f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9120 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_913f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9130 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_914f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9140 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_915f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9150 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_916f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9160 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_941f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9410 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_942f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9420 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_951f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9510 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_952f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9520 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_953f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9530 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_954f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9540 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_955f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9550 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_956f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9560 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_9c1f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9c10 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}


int parse_read_data::parse_9c2f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9c20 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}


int parse_read_data::parse_9c3f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9c30 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}


int parse_read_data::parse_9c4f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9c40 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}


int parse_read_data::parse_9c5f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9c50 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}


int parse_read_data::parse_9c6f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9c60 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}


int parse_read_data::parse_9c7f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9c70 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}


int parse_read_data::parse_9c8f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 2)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9c80 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}
	return fn_len;
}

int parse_read_data::parse_b61f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 6)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 3;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,2,0);
		data_item_value value;
		value.data_item_id = 0xB610 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 2;
	}

	return fn_len;
}

int parse_read_data::parse_b62f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 6) //MODIFY BY WM 20131022
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 3;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,2,2);
		data_item_value value;
		value.data_item_id = 0xB620 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 2;
	}

	return fn_len;
}

int parse_read_data::parse_b63f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 12)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 4;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xB630 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}
	return fn_len;
}

int parse_read_data::parse_b64f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 8) //MODIFY BY WM 20131022
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 4;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,2,2);
		data_item_value value;
		value.data_item_id = 0xB640 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 2;
	}
	return fn_len;
}


int parse_read_data::parse_b65f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 8)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 4;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,2,3);
		data_item_value value;
		value.data_item_id = 0xB650 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 2;
	}
	return fn_len;
}

int parse_read_data::parse_a01f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 15)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xA010 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}
	return fn_len;
}

int parse_read_data::parse_a02f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 15)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xA020 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}
	return fn_len;
}

int parse_read_data::parse_a11f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 15)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xA110 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}
	return fn_len;
}

int parse_read_data::parse_a12f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 15)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xA120 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}
	return fn_len;
}

int parse_read_data::parse_b01f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 25)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		unsigned int db_time = 0;
		tm time_tm;
		ptcl_module::bcd_to_time(buf+fn_len,4,ptcl_module::time_type_mmhhddmm,&time_tm);
		db_time = (unsigned int)mktime(&time_tm);

		data_item_value value;
		value.data_item_id = 0xB010 + i;
		value.db_value = db_time;
		v_values.push_back(value);

		fn_len = fn_len + 5;
	}
	return fn_len;
}

int parse_read_data::parse_b02f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 25)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		unsigned int db_time = 0;
		tm time_tm;
		ptcl_module::bcd_to_time(buf+fn_len,4,ptcl_module::time_type_mmhhddmm,&time_tm);
		db_time = (unsigned int)mktime(&time_tm);

		data_item_value value;
		value.data_item_id = 0xB020 + i;
		value.db_value = db_time;
		v_values.push_back(value);

		fn_len = fn_len + 5;
	}
	return fn_len;
}

int parse_read_data::parse_b11f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 25)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		unsigned int db_time = 0;
		tm time_tm;
		ptcl_module::bcd_to_time(buf+fn_len,4,ptcl_module::time_type_mmhhddmm,&time_tm);
		db_time = (unsigned int)mktime(&time_tm);

		data_item_value value;
		value.data_item_id = 0xB110 + i;
		value.db_value = db_time;
		v_values.push_back(value);

		fn_len = fn_len + 5;
	}
	return fn_len;
}

int parse_read_data::parse_b12f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 25)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		unsigned int db_time = 0;
		tm time_tm;
		ptcl_module::bcd_to_time(buf+fn_len,4,ptcl_module::time_type_mmhhddmm,&time_tm);
		db_time = (unsigned int)mktime(&time_tm);

		data_item_value value;
		value.data_item_id = 0xB120 + i;
		value.db_value = db_time;
		v_values.push_back(value);

		fn_len = fn_len + 5;
	}
	return fn_len;
}


int parse_read_data::parse_a41f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 15)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xA410 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}
	return fn_len;
}

int parse_read_data::parse_a42f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 15)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xA420 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}
	return fn_len;
}

int parse_read_data::parse_a51f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 15)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xA510 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}
	return fn_len;
}

int parse_read_data::parse_a52f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 15)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xA520 + i;
		value.db_value = db_value;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}
	return fn_len;
}

int parse_read_data::parse_b41f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 20)  //MODIFY BY WM 20131022
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		unsigned int db_time = 0;
		tm time_tm;
		ptcl_module::bcd_to_time(buf+fn_len,4,ptcl_module::time_type_mmhhddmm,&time_tm);
		db_time = (unsigned int)mktime(&time_tm);

		data_item_value value;
		value.data_item_id = 0xB410 + i;
		value.db_value = db_time;
		v_values.push_back(value);

		fn_len = fn_len + 4;  //MODIFY BY WM 20131022
	}
	return fn_len;
}

int parse_read_data::parse_b42f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 20)   //MODIFY BY WM 20131022
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		unsigned int db_time = 0;
		tm time_tm;
		ptcl_module::bcd_to_time(buf+fn_len,4,ptcl_module::time_type_mmhhddmm,&time_tm);
		db_time = (unsigned int)mktime(&time_tm);

		data_item_value value;
		value.data_item_id = 0xB420 + i;
		value.db_value = db_time;
		v_values.push_back(value);

		fn_len = fn_len + 4;   //MODIFY BY WM 20131022
	}
	return fn_len;
}


int parse_read_data::parse_b51f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 20)//MODIFY BY WM 20131022
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		unsigned int db_time = 0;
		tm time_tm;
		ptcl_module::bcd_to_time(buf+fn_len,4,ptcl_module::time_type_mmhhddmm,&time_tm);
		db_time = (unsigned int)mktime(&time_tm);

		data_item_value value;
		value.data_item_id = 0xB510 + i;
		value.db_value = db_time;
		v_values.push_back(value);

		fn_len = fn_len + 4;   //MODIFY BY WM 20131022
	}
	return fn_len;
}


int parse_read_data::parse_b52f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{
	if(buf == NULL || len < 20)  //MODIFY BY WM 20131022
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 5;i++)
	{
		unsigned int db_time = 0;
		tm time_tm;
		ptcl_module::bcd_to_time(buf+fn_len,4,ptcl_module::time_type_mmhhddmm,&time_tm);
		db_time = (unsigned int)mktime(&time_tm);

		data_item_value value;
		value.data_item_id = 0xB520 + i;
		value.db_value = db_time;
		v_values.push_back(value);

		fn_len = fn_len + 4;   //MODIFY BY WM 20131022
	}
	return fn_len;
}

///曲线数据/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int parse_read_data::parse_read_data_curve(unsigned char *buf,int len,std::vector <data_item_value> & v_values)
{	
	int len_parse = 0;
	if (buf == NULL)
		return 0;

	int value_len = 0;
	double db_value = 0;
	
	bool first = true;
	while(len - len_parse > 2)
	{
		short int fn = ptcl_packet_dlt::get_data_addr(buf);
		len_parse = len_parse + 2;

		unsigned int value_t = 0;  //数据时标
		if(first == true)
		{
			first = false;

			tm value_tm;
			ptcl_module::bcd_to_time(buf+len_parse,5,ptcl_module::time_type_yymmddhhmm,&value_tm);
			value_t = (unsigned int)mktime(&value_tm);	

			len_parse = len_parse + 5;
		}
		
		value_len = parse_fn_curve(fn,buf+len_parse,len - len_parse,v_values,value_t); //MODIFY BY WM 20131024
		if (value_len <= 0) 
			return 0;
		
		len_parse += value_len;
	}

	return len_parse;
}

int parse_read_data::parse_fn_curve(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time)
{
	int fn_len = 0;
	switch (fn)
	{
	case (short int)0x9b1f:
		fn_len = parse_9b1f(fn,buf,len,v_values,value_time);
		break;
	case (short int)0x9b5f:
		fn_len = parse_9b5f(fn,buf,len,v_values,value_time);
		break;
	case (short int)0xaa1f:
		fn_len = parse_aa1f(fn,buf,len,v_values,value_time);
		break;
	case (short int)0xbb1f:
		fn_len = parse_bb1f(fn,buf,len,v_values,value_time);
		break;
	}
	return fn_len;
}

//分时电量曲线数据
int parse_read_data::parse_9b1f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time)
{
	if(buf == NULL || len < 80)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 20;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9b10 + i;
		value.db_value = db_value;
		value.value_time = value_time;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}

	return fn_len;
}
//象限无功电量曲线数据
int parse_read_data::parse_9b5f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time)
{
	if(buf == NULL || len < 80)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 20;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,2);
		data_item_value value;
		value.data_item_id = 0x9b50 + i;
		value.db_value = db_value;
		value.value_time = value_time;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}

	return fn_len;
}
//最大需量曲线数据
int parse_read_data::parse_aa1f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time)
{
	if(buf == NULL || len < 160)
		return 0;

	int fn_len = 0;
	for (int i = 0;i < 20;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,4,4);
		data_item_value value;
		value.data_item_id = 0xaa10 + i;
		value.db_value = db_value;
		value.value_time = value_time;
		v_values.push_back(value);
		fn_len = fn_len + 4;

		unsigned int db_time = 0;
		tm time_tm;
		ptcl_module::bcd_to_time(buf+fn_len,4,ptcl_module::time_type_mmhhddmm,&time_tm);
		db_time = (unsigned int)mktime(&time_tm);

		value.data_item_id = 0xba10 + i;
		value.db_value = db_time;
		v_values.push_back(value);
		fn_len = fn_len + 4;
	}

	return fn_len;
}
//瞬时量曲线数据
int parse_read_data::parse_bb1f(short int fn,unsigned char *buf,int len,std::vector <data_item_value> & v_values,unsigned int value_time)
{
	if(buf == NULL || len < 40)
		return 0;

	int fn_len = 0;

	//电压
	for (int i = 0;i < 3;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,2,0);
		data_item_value value;
		value.data_item_id = 0xBB10 + i;
		value.db_value = db_value;
		value.value_time = value_time;
		v_values.push_back(value);
		fn_len = fn_len + 2;
	}

	//电流
	for (int i = 0;i < 3;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,2,2);
		data_item_value value;
		value.data_item_id = 0xBB13 + i;
		value.db_value = db_value;
		value.value_time = value_time;
		v_values.push_back(value);
		fn_len = fn_len + 2;
	}

	//有功功率
	for (int i = 0;i < 4;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,3,4);
		data_item_value value;
		value.data_item_id = 0xBB16 + i;
		value.db_value = db_value;
		value.value_time = value_time;
		v_values.push_back(value);
		fn_len = fn_len + 3;
	}

	//无功功率
	for (int i = 0;i < 4;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,2,2);
		data_item_value value;
		value.data_item_id = 0xBB1A + i;
		value.db_value = db_value;
		value.value_time = value_time;
		v_values.push_back(value);
		fn_len = fn_len + 2;
	}

	//功率因数
	for (int i = 0;i < 4;i++)
	{
		double db_value = ptcl_module::bcd_to_bin(buf+fn_len,2,3);
		data_item_value value;
		value.data_item_id = 0xBB1E + i;
		value.db_value = db_value;
		value.value_time = value_time;
		v_values.push_back(value);
		fn_len = fn_len + 2;
	}

	return fn_len;
}






















