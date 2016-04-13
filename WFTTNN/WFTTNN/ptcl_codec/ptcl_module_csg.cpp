#include "ptcl_module_csg.h"

#include <time.h>



ptcl_module_csg *ptcl_module_csg::instance_ = NULL;

ptcl_module_csg::ptcl_module_csg(void)
{
}

ptcl_module_csg::~ptcl_module_csg(void)
{
}

ptcl_module_csg *ptcl_module_csg::get_instance()
{
	if (instance_ == NULL)
		instance_ = new ptcl_module_csg();
	return instance_;
}

int ptcl_module_csg::format_pnfn(pnfn_data *pnfn ,unsigned char *buffer)
{
	if (pnfn == NULL || buffer == NULL)
		return 0;

	pnfn_struct *pnfn_s = (pnfn_struct *)buffer;

	int pn = pnfn->pn;
	if (pn == 0)
	{
		pnfn_s->pn[0] = 0;
		pnfn_s->pn[1] = 0;
	}
	else if (pn == 255)
	{
		pnfn_s->pn[0] = 255;
		pnfn_s->pn[1] = 255;
	}
	else
	{
		pnfn_s->pn[0] = 1 << ((pn - 1) % 8);
		pnfn_s->pn[1] = ((pn - 1) / 8) + 1;
	}

	int fn = pnfn->fn;
	unsigned char *p = (unsigned char *)&fn;

	//pnfn_s->fn[3] = *p;
	//pnfn_s->fn[2] = *(p+1);
	//pnfn_s->fn[1] = *(p+2);
	//pnfn_s->fn[0] = *(p+3);

	//转换成高位在前,低位在后
	pnfn_s->fn[0] = *p;
	pnfn_s->fn[1] = *(p+1);
	pnfn_s->fn[2] = *(p+2);
	pnfn_s->fn[3] = *(p+3);

	return 6;
}

int ptcl_module_csg::parse_pnfn(unsigned char *buffer,pnfn_data *pnfn)
{
	if (pnfn == NULL || buffer == NULL)
		return 0;

	pnfn_struct *pnfn_s = (pnfn_struct *)buffer;

	if (pnfn_s->pn[0] == 0 || pnfn_s->pn[1] == 0)
	{
		pnfn->pn = 0;
	}
	else if (pnfn_s->pn[0] == 255 || pnfn_s->pn[1] == 255)
	{
		pnfn->pn = 255;
	}
	else
	{
		pnfn->pn = 8 * (pnfn_s->pn[1] - 1);
		for (int n=0;n<8;n++)
		{
			if (((pnfn_s->pn[0]>>n) & 0x01) == 0x01)
			{
				pnfn->pn = pnfn->pn + n + 1;
			}
		}
	}

	pnfn->fn = (pnfn_s->fn[0] + (pnfn_s->fn[1] << 8 ) + (pnfn_s->fn[2] << 16) + (pnfn_s->fn[3] << 24));

	return 6;
}

int ptcl_module_csg::parse_fn(unsigned char *buffer,int &i_fn)
{
	if (buffer == NULL)
		return 0;
	
	unsigned char fn[4];
	fn[0] = buffer[0];
	fn[1] = buffer[1];
	fn[2] = buffer[2];
	fn[3] = buffer[3];

	int sign = (fn[0] + (fn[1] << 8 ) + (fn[2] << 16) + (fn[3] << 24));
	i_fn = sign;
	
	return 4;
}

int ptcl_module_csg::parse_pn(unsigned char *buffer,int &i_pn)
{
	if (buffer == NULL)
		return 0;

	unsigned char pn[2];
	pn[0] = buffer[0];
	pn[1] = buffer[1];

	int sign = 0;

	if (pn[0] == 0 || pn[1] == 0)
	{
		sign = 0;
	}
	else if (pn[0] == 255 || pn[1] == 255)
	{
		sign = 255;
	}
	else
	{
		sign = 8 * (pn[1] - 1);
		for (int n=0;n<8;n++)
		{
			if (((pn[0]>>n) & 0x01) == 0x01)
			{
				sign = sign + n + 1;
			}
		}
	}

	i_pn = sign;

	return 2;
}

int ptcl_module_csg::format_tp(tp_data *tp,unsigned char *buffer)
{
	if (tp == NULL || buffer == NULL)
		return 0;

	buffer[0] = ptcl_module::bin_to_bcd(tp->day);
	buffer[0] = ptcl_module::bin_to_bcd(tp->day);
	buffer[1] = ptcl_module::bin_to_bcd(tp->hour);
	buffer[2] = ptcl_module::bin_to_bcd(tp->min);
	buffer[3] = ptcl_module::bin_to_bcd(tp->sec);
	buffer[4] = tp->time_out;

	return 5;
}

int ptcl_module_csg::parse_tp(unsigned char *buffer,tp_data *tp)
{
	if (tp == NULL || buffer == NULL)
		return 0;

	tp->day = ptcl_module::bcd_to_bin(buffer[0]);
	tp->hour = ptcl_module::bcd_to_bin(buffer[1]);
	tp->min = ptcl_module::bcd_to_bin(buffer[2]);
	tp->sec = ptcl_module::bcd_to_bin(buffer[3]);
	tp->time_out = buffer[4];

	return 5;
}

int ptcl_module_csg::format_tnm(int* v_tn,int count_tn,unsigned char *buffer)
{
	if (buffer == NULL || v_tn == NULL)
		return 0;

	for (int n=0;n<count_tn;n++)
	{
		int value = v_tn[n] % 64;
		int row = value / 8;
		int col = value % 8;

		unsigned char temp = buffer[row];
		temp = temp | (1<<col);
		buffer[row] = temp;
	}

	return 8;
}

int ptcl_module_csg::parse_tnm(unsigned char *buffer,int* v_tn,int &count_tn)
{
	if (buffer == NULL || v_tn == NULL)
		return 0;

	int count = 0;
	for (int n=0;n<8;n++)
	{
		for (int m=0;m<8;m++)
		{
			unsigned char temp = buffer[n] >> m;
			if ((temp & 0x01) == 0x01)
			{
				int value = n*8+m;
				if (count < count_tn)
					v_tn[count++] = value;
			}
		}
	}
	count_tn = count;

	return 8;
}


void ptcl_module_csg::format_packet_confirm_deny_frame( ptcl_packet_csg *packet,
		                               pnfn_data_uint *v_data_uint,
									   int count_data_uint,
									   tp_data *tp)
{
	if(packet == NULL)
		return;

	
	//控制域功能码
	packet->set_afn_ctrl(0x00);	
	packet->set_frm(0);
	packet->set_fcv(0);
	packet->set_fcv_acd(0);
	packet->set_dir(0);

	//主站地址 序列号域
	packet->set_host_id(host_id_);

	//应用层功能码
	packet->set_afn(AFN_ACK);

	packet->set_tpv(0);
	packet->set_fri(1);
	packet->set_fin(1);
	packet->set_con(0);

	//用户数据域
	int len_data = 0;
	unsigned char buffer[1024];
	memset(buffer,0,1024);

	//数据单元
	for (int n=0;n<count_data_uint;n++)
	{
		if (len_data + v_data_uint[n].len_data >= 1024)
			break;

		int len = format_pnfn(&v_data_uint[n].pnfn,buffer + len_data);
		len_data = len_data + len;

		memcpy(buffer + len_data,v_data_uint[n].data_buffer,v_data_uint[n].len_data);
		len_data = len_data + v_data_uint[n].len_data;
	}

	//TP
	if (tp)
	{
		int len = format_tp(tp,buffer + len_data);
		len_data = len_data + len;
	}


	packet->set_data_area_buffer(buffer,len_data);

	packet->format_packet();

}

void ptcl_module_csg::format_packet_confirm_frame(ptcl_packet_csg *packet)
{
	if (packet == NULL)
		return;

	//控制域功能码
	packet->set_afn_ctrl(0x00);	
	packet->set_frm(0);
	packet->set_fcv(0);
	packet->set_fcv_acd(0);
	packet->set_dir(0);

	//主站地址 序列号域
	packet->set_host_id(host_id_);

	//应用层功能码
	packet->set_afn(AFN_ACK);

	packet->set_tpv(0);
	packet->set_fri(1);
	packet->set_fin(1);
	packet->set_con(0);

	//用户数据域
	int len_data = 0;
	unsigned char buffer[8];
	memset(buffer,0,8);

	buffer[0] = 0x00;
	buffer[1] = 0x00;

	buffer[2] = 0x00;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	buffer[5] = 0xE0;

	buffer[6] = 0x00;

	len_data = 7;

	packet->set_data_area_buffer(buffer,len_data);

	packet->format_packet();
}



void ptcl_module_csg::format_packet_read_realtime_data(	ptcl_packet_csg *packet,
												    pnfn_data *v_pnfn, int pnfn_count,
												    tp_data *tp)
{
	if (packet == NULL || v_pnfn == NULL)
		return ;

	//主站地址 序列号域
	packet->set_host_id(host_id_);

	//控制域功能码
	packet->set_afn_ctrl(0x0B);
	packet->set_frm(1);
	packet->set_fcv(0);
	packet->set_fcv_acd(0);
	packet->set_dir(0);

	//应用层功能码
	packet->set_afn(AFN_READ_CUR_DATA);

	//序列域
	__int64 addr = packet->get_device_addr();
	unsigned char seq = get_frame_seq(addr);
	packet->set_frame_seq(seq);

	if (tp != NULL)
		packet->set_tpv(1);
	else
		packet->set_tpv(0);
	packet->set_fri(1);
	packet->set_fin(1);
	packet->set_con(0);

	//用户数据域
	unsigned char dataBuf[1024];
	int lenDataBuf = 0;
	memset(dataBuf,0,1024);

	//数据单元
	for (int n=0;n<pnfn_count;n++)
	{
		if (lenDataBuf + 6>= 2048)
			break;

		int len = format_pnfn(&v_pnfn[n],dataBuf + lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	//TP
	if (tp != NULL)
	{
		int len = format_tp(tp,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}


	packet->set_data_area_buffer(dataBuf,lenDataBuf);

	packet->format_packet();
}






void ptcl_module_csg::format_packet_read_history_data(	ptcl_packet_csg *packet,
														pnfn_data_uint *v_data_uint,
														int count_data_uint,
														tp_data *tp)
{
	if (packet == NULL)
		return ;

	//控制域功能码
	packet->set_afn_ctrl(0x0B);
	packet->set_frm(1);
	packet->set_fcv(0);
	packet->set_fcv_acd(0);
	packet->set_dir(0);

	//主站地址 
	packet->set_host_id(host_id_);

	//应用层功能码
	packet->set_afn(AFN_READ_DATA_HISTORY);
	

	//序列域
	__int64 addr = packet->get_device_addr();
	unsigned char seq = get_frame_seq(addr);
	packet->set_frame_seq(seq);
	
	if (tp)
		packet->set_tpv(1);
	else
		packet->set_tpv(0);

	packet->set_fri(1);
	packet->set_fin(1);
	packet->set_con(0);

	//用户数据域
	int len_data = 0;
	unsigned char buffer[1024];
	memset(buffer,0,1024);

	//数据单元
	for (int n=0;n<count_data_uint;n++)
	{
		if (len_data + v_data_uint[n].len_data >= 1024)
			break;

		int len = format_pnfn(&v_data_uint[n].pnfn,buffer + len_data);
		len_data = len_data + len;

		memcpy(buffer + len_data,v_data_uint[n].data_buffer,v_data_uint[n].len_data);
		len_data = len_data + v_data_uint[n].len_data;
	}

	//TP
	if (tp)
	{
		int len = format_tp(tp,buffer + len_data);
		len_data = len_data + len;
	}

	packet->set_data_area_buffer(buffer,len_data);

	packet->format_packet();
}

void ptcl_module_csg::format_packet_read_event_data(	ptcl_packet_csg *packet,
														pnfn_data_uint *v_data_uint,
														int count_data_uint,
														tp_data *tp)
{
	if (packet == NULL)
		return ;

	//控制域功能码
	packet->set_afn_ctrl(0x0B);
	packet->set_frm(1);
	packet->set_fcv(0);
	packet->set_fcv_acd(0);
	packet->set_dir(0);

	//主站地址 
	packet->set_host_id(host_id_);

	//应用层功能码
	packet->set_afn(AFN_READ_EVENT_RECORD);
	

	//序列域
	__int64 addr = packet->get_device_addr();
	unsigned char seq = get_frame_seq(addr);
	packet->set_frame_seq(seq);
	
	if (tp)
		packet->set_tpv(1);
	else
		packet->set_tpv(0);

	packet->set_fri(1);
	packet->set_fin(1);
	packet->set_con(0);

	//用户数据域
	int len_data = 0;
	unsigned char buffer[1024];
	memset(buffer,0,1024);

	//数据单元
	for (int n=0;n<count_data_uint;n++)
	{
		if (len_data + v_data_uint[n].len_data >= 1024)
			break;

		int len = format_pnfn(&v_data_uint[n].pnfn,buffer + len_data);
		len_data = len_data + len;

		memcpy(buffer + len_data,v_data_uint[n].data_buffer,v_data_uint[n].len_data);
		len_data = len_data + v_data_uint[n].len_data;
	}

	//TP
	if (tp)
	{
		int len = format_tp(tp,buffer + len_data);
		len_data = len_data + len;
	}

	packet->set_data_area_buffer(buffer,len_data);

	packet->format_packet();
}

void ptcl_module_csg::format_packet_read_alarm_data(	ptcl_packet_csg *packet,
														pnfn_data_uint *v_data_uint,
														int count_data_uint,
														tp_data *tp)
{
	if (packet == NULL)
		return ;

	//控制域功能码
	packet->set_afn_ctrl(0x0B);
	packet->set_frm(1);
	packet->set_fcv(0);
	packet->set_fcv_acd(0);
	packet->set_dir(0);

	//主站地址 
	packet->set_host_id(host_id_);

	//应用层功能码
	packet->set_afn(AFN_READ_DATA_EVENT);
	

	//序列域
	__int64 addr = packet->get_device_addr();
	unsigned char seq = get_frame_seq(addr);
	packet->set_frame_seq(seq);
	
	if (tp)
		packet->set_tpv(1);
	else
		packet->set_tpv(0);

	packet->set_fri(1);
	packet->set_fin(1);
	packet->set_con(0);

	//用户数据域
	int len_data = 0;
	unsigned char buffer[1024];
	memset(buffer,0,1024);

	//数据单元
	for (int n=0;n<count_data_uint;n++)
	{
		if (len_data + v_data_uint[n].len_data >= 1024)
			break;

		int len = format_pnfn(&v_data_uint[n].pnfn,buffer + len_data);
		len_data = len_data + len;

		memcpy(buffer + len_data,v_data_uint[n].data_buffer,v_data_uint[n].len_data);
		len_data = len_data + v_data_uint[n].len_data;
	}

	//TP
	if (tp)
	{
		int len = format_tp(tp,buffer + len_data);
		len_data = len_data + len;
	}

	packet->set_data_area_buffer(buffer,len_data);

	packet->format_packet();
}



void ptcl_module_csg::format_packet_set_param(	ptcl_packet_csg * packet,unsigned char *password,
												ptcl_module_csg::pnfn_data_uint *v_data_unit,
												int count_data_uint,tp_data *tp)
{
	if (packet == NULL || password == NULL)
		return ;

	//控制域功能码
	packet->set_afn_ctrl(0x0A);
	packet->set_frm(1);
	packet->set_fcv(0);
	packet->set_fcv_acd(0);
	packet->set_dir(0);

	//主站地址 序列号域
	packet->set_host_id(host_id_);
	//应用层功能码
	packet->set_afn(AFN_SET_PARAM);
	

	//序列域
	__int64 addr = packet->get_device_addr();
	unsigned char seq = get_frame_seq(addr);
	packet->set_frame_seq(seq);
	
	if (tp)
		packet->set_tpv(1);
	else
		packet->set_tpv(0);

	packet->set_fri(1);
	packet->set_fin(1);
	packet->set_con(1);	

	//用户数据域
	int len_data = 0;
	unsigned char buffer[1024];
	memset(buffer,0,1024);

	//数据单元
	for (int n=0;n<count_data_uint;n++)
	{
		if (len_data + 6 + v_data_unit[n].len_data >= 1024)
			break;

		int len = format_pnfn(&v_data_unit[n].pnfn,buffer + len_data);
		len_data = len_data + len;
		memcpy(buffer + len_data,v_data_unit[n].data_buffer,v_data_unit[n].len_data);
		len_data = len_data + v_data_unit[n].len_data;
	}
	//pss
	if (password)
	{
		unsigned char crc[16] = {0};
		memcpy(buffer+len_data,crc,16);
		len_data = len_data + 16;
	}

	//TP
	if (tp)
	{
		//tp->pfc = seq;
		int len = format_tp(tp,buffer + len_data);
		len_data = len_data + len;
	}

	packet->set_data_area_buffer(buffer,len_data);

	packet->format_packet();
}

void ptcl_module_csg::format_packet_read_parameter(	ptcl_packet_csg *packet,
		                                            pnfn_data *v_pnfn, int pnfn_count,
										            tp_data *tp)
{
	if (packet == NULL || v_pnfn == NULL)
		return ;

	//主站地址 序列号域
	packet->set_host_id(host_id_);

	//控制域功能码
	packet->set_afn_ctrl(0x0B);
	packet->set_frm(1);
	packet->set_fcv(0);
	packet->set_fcv_acd(0);
	packet->set_dir(0);

	//应用层功能码
	packet->set_afn(AFN_READ_PARAM);


	//序列域
	__int64 addr = packet->get_device_addr();
	unsigned char seq = get_frame_seq(addr);
	packet->set_frame_seq(seq);

	if (tp != NULL)
		packet->set_tpv(1);
	else
		packet->set_tpv(0);
	packet->set_fri(1);
	packet->set_fin(1);
	packet->set_con(0);

	//用户数据域
	unsigned char dataBuf[1024];
	int lenDataBuf = 0;
	memset(dataBuf,0,1024);

	//数据单元
	for (int n=0;n<pnfn_count;n++)
	{
		if (lenDataBuf + 6>= 2048)
			break;

		int len = format_pnfn(&v_pnfn[n],dataBuf + lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	//TP
	if (tp != NULL)
	{
		int len = format_tp(tp,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}


	packet->set_data_area_buffer(dataBuf,lenDataBuf);

	packet->format_packet();

}

void ptcl_module_csg::format_packet_transmit(	ptcl_packet_csg * packet,
		                            unsigned char delayType,
									unsigned char port,
									unsigned char btl,
									unsigned char xlfs,
									unsigned char sjw,
									unsigned char tzw,
									unsigned char cssj,
									unsigned char *delayBuf, 
									int lenDelayBuf,
									unsigned char *password,
									tp_data *tp)
	
{
	if (packet == NULL || password == NULL || delayBuf == NULL)
		return ;

	//控制域功能码
	packet->set_afn_ctrl(0x0B);
	packet->set_frm(1);
	packet->set_fcv(0);
	packet->set_fcv_acd(0);
	packet->set_dir(0);

	//主站地址 序列号域
	packet->set_host_id(host_id_);
	//应用层功能码
	packet->set_afn(AFN_RELAY_STATION_COMMAND);
	

	//序列域
	__int64 addr = packet->get_device_addr();
	unsigned char seq = get_frame_seq(addr);
	packet->set_frame_seq(seq);
	
	if (tp)
		packet->set_tpv(1);
	else
		packet->set_tpv(0);

	packet->set_fri(1);
	packet->set_fin(1);
	packet->set_con(1);	

	//用户数据域
	int len_data = 0;
	unsigned char buffer[1024];
	memset(buffer,0,1024);


	//数据单元
	pnfn_data pnfn;
	pnfn.pn = 0;
	pnfn.fn = packet->convert_fn("E3020001");
	len_data = len_data + format_pnfn(&pnfn,buffer + len_data);

	buffer[len_data++] = delayType;
	buffer[len_data++] = port;  //测量点端口号
	buffer[len_data++] = btl; // 通信波特率
	buffer[len_data++] = xlfs; // 校验方式
	buffer[len_data++] = sjw; // 数据位
	buffer[len_data++] = tzw; // 停止位
	buffer[len_data++] = cssj;
	buffer[len_data++] = lenDelayBuf & 0xff;

	memcpy(buffer+len_data,delayBuf,lenDelayBuf);
	len_data	= len_data + lenDelayBuf;

	//pss
	/*if (password)
	{
		unsigned char crc[16] = {0};
		memcpy(buffer+len_data,crc,16);
		len_data = len_data + 16;
	}*/

	//TP
	if (tp)
	{
		//tp->pfc = seq;
		int len = format_tp(tp,buffer + len_data);
		len_data = len_data + len;
	}

	packet->set_data_area_buffer(buffer,len_data);

	packet->format_packet();

}
	


