#include "stdafx.h"
#include "PtclModule_NN.h"
#include "PtclCodec.h"

CPtclModule_NN *CPtclModule_NN::m_pInstance = NULL;
CPtclModule_NN::CPtclModule_NN(void)
{
	m_hostID = 18;
}

CPtclModule_NN::~CPtclModule_NN(void)
{
}

CPtclModule_NN *CPtclModule_NN::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CPtclModule_NN();
	}
	return m_pInstance;
}

void CPtclModule_NN::setHostID(unsigned char hostID)
{
	m_hostID = hostID;
}

int CPtclModule_NN::FormatPnFn(CPtclModule_NN::FormatPnFnData *pPnFn,unsigned char *buffer)
{
	if (pPnFn == NULL || buffer == NULL)
		return 0;

	PnFnStruct *pnfn_s = (PnFnStruct *)buffer;

	int pn = pPnFn->Pn;
	if (pn == 0)
	{
		pnfn_s->Pn[0] = 0;
		pnfn_s->Pn[1] = 0;
	}
	else if (pn == 255)
	{
		pnfn_s->Pn[0] = 255;
		pnfn_s->Pn[1] = 255;
	}
	else
	{
		pnfn_s->Pn[0] = 1 << ((pn - 1) % 8);
		pnfn_s->Pn[1] = ((pn - 1) / 8) + 1;
	}

	int fn = pPnFn->Fn;
	unsigned char *p = (unsigned char *)&fn;

	//pnfn_s->fn[3] = *p;
	//pnfn_s->fn[2] = *(p+1);
	//pnfn_s->fn[1] = *(p+2);
	//pnfn_s->fn[0] = *(p+3);

	//转换成高位在前,低位在后
	pnfn_s->Fn[0] = *p;
	pnfn_s->Fn[1] = *(p+1);
	pnfn_s->Fn[2] = *(p+2);
	pnfn_s->Fn[3] = *(p+3);

	return 6;
}

int CPtclModule_NN::ParsePnFn(unsigned char *buffer,ParsePnFnData *pPnFn)
{
	if (pPnFn == NULL || buffer == NULL)
		return 0;

	PnFnStruct *pnfn_s = (PnFnStruct *)buffer;

	if (pnfn_s->Pn[0] == 0 || pnfn_s->Pn[1] == 0)
	{
		pPnFn->vPn[0] = 0;
	}
	else if (pnfn_s->Pn[0] == 255 || pnfn_s->Pn[1] == 255)
	{
		pPnFn->vPn[0] = 255;
	}
	else
	{
		pPnFn->vPn[0] = 8 * (pnfn_s->Pn[1] - 1);
		for (int n=0;n<8;n++)
		{
			if (((pnfn_s->Pn[0]>>n) & 0x01) == 0x01)
			{
				pPnFn->vPn[0] = pPnFn->vPn[0] + n + 1;
			}
		}
	}

	pPnFn->vFn[0] = (pnfn_s->Fn[0] + (pnfn_s->Fn[1] << 8 ) + (pnfn_s->Fn[2] << 16) + (pnfn_s->Fn[3] << 24));

	return 6;
}

int CPtclModule_NN::ParsePn(unsigned char *buffer,int &i_pn)
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

int CPtclModule_NN::FormatTP(TPData *tp,unsigned char *buffer)
{
	if (tp == NULL || buffer == NULL)
		return 0;

	buffer[0] = CPtclCodec::binToBcd(tp->day);
	buffer[1] = CPtclCodec::binToBcd(tp->hour);
	buffer[2] = CPtclCodec::binToBcd(tp->min);
	buffer[3] = CPtclCodec::binToBcd(tp->sec);
	buffer[4] = tp->time_out;

	return 5;
}

int CPtclModule_NN::ParseTP(unsigned char *buffer,TPData *tp)
{
	if (tp == NULL || buffer == NULL)
		return 0;

	tp->day = CPtclCodec::bcdToBin(buffer[0]);
	tp->hour = CPtclCodec::bcdToBin(buffer[1]);
	tp->min = CPtclCodec::bcdToBin(buffer[2]);
	tp->sec = CPtclCodec::bcdToBin(buffer[3]);
	tp->time_out = buffer[4];

	return 5;
}

int CPtclModule_NN::FormatEC(ECData *pEC,unsigned char *pECBuf)
{
	if (pEC == NULL || pECBuf == NULL)
		return 0;

	ECStruct EC;
	memset((void *)&EC,0,sizeof(ECStruct));

	EC.genericEC = pEC->genericEC;
	EC.importantEC = pEC->importantEC;

	memcpy(pECBuf,(void *)&EC,sizeof(ECStruct));

	return sizeof(ECStruct);
}

int CPtclModule_NN::ParseEC(unsigned char *pECBuf,ECData *pEC)
{
	if (pEC == NULL || pECBuf == NULL)
		return 0;

	ECStruct *pECStruct=(ECStruct *)pECBuf;

	pEC->importantEC = pECStruct->importantEC;
	pEC->genericEC = pECStruct->genericEC;

	return sizeof(ECStruct);
}
void CPtclModule_NN::FormatPacket_Ack_All(	CPtclPacket_NN *pPacket,
													bool confirm_deny,
													TPData *pTP,
													ECData *pEC)
{
	if (pPacket == NULL)
		return ;

	//主站ID
	pPacket->setHostID(m_hostID);

	//控制域功能码
	pPacket->setPRM(0);
	pPacket->setAFN_Ctrl(0);
	pPacket->setFCV(0);
	pPacket->setFCB_ACD(0);
	pPacket->setDIR(0);
	

	//应用层功能码
	pPacket->setAFN(AFN_ACK);

	//序列域
	if (pTP != NULL)
		pPacket->setTPV(1);
	else
		pPacket->setTPV(0);
	pPacket->setFRI(1);
	pPacket->setFIN(1);
	pPacket->setCON(0);

	//用户数据域
	int lenDataBuf = 0;
	unsigned char dataBuf[20];
	memset(dataBuf,0,20);
	//数据单元
	FormatPnFnData PnFn;
	PnFn.Pn = 0;
	if (confirm_deny)
		PnFn.Fn = 1;
	else
		PnFn.Fn = 2;

	int len = FormatPnFn(&PnFn,dataBuf);
	lenDataBuf = len;

	//EC
	if (pEC != NULL)
	{
		len = FormatEC(pEC,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}
	//TP
	if (pTP != NULL)
	{
		len = FormatTP(pTP,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	pPacket->setDataAreaBuf(dataBuf,lenDataBuf);

	pPacket->formatPacket();
}


void CPtclModule_NN::FormatPacket_Ack_Part(	CPtclPacket_NN *pPacket,
													unsigned char bAckAFN,
													FormatAckDataUnit *vAck,int countAck,
													TPData *pTP,
													ECData *pEC)
{
	if (pPacket == NULL)
		return ;

	//主站ID
	pPacket->setHostID(m_hostID);

	//控制域功能码
	pPacket->setPRM(0);
	pPacket->setAFN_Ctrl(0);
	pPacket->setFCV(0);
	pPacket->setFCB_ACD(0);
	pPacket->setDIR(0);

	//应用层功能码
	pPacket->setAFN(AFN_ACK);

	//序列域
	if (pTP != NULL)
		pPacket->setTPV(1);
	else
		pPacket->setTPV(0);
	pPacket->setFRI(1);
	pPacket->setFIN(1);
	pPacket->setCON(0);

	//用户数据域
	int lenDataBuf = 0;
	unsigned char dataBuf[1024];
	memset(dataBuf,0,1024);
	//数据单元
	FormatPnFnData PnFn;
	PnFn.Pn = 0;
	PnFn.Fn = 3;

	int len = FormatPnFn(&PnFn,dataBuf);
	lenDataBuf = len;

	dataBuf[lenDataBuf++] = bAckAFN;

	for (int n=0;n<countAck;n++)
	{
		len = FormatPnFn(&(vAck[n].PnFn),dataBuf+lenDataBuf);
		lenDataBuf =lenDataBuf + len;
		dataBuf[lenDataBuf++] = vAck[n].error;
	}

	//EC
	if (pEC != NULL)
	{
		len = FormatEC(pEC,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}
	//TP
	if (pTP != NULL)
	{
		len = FormatTP(pTP,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	pPacket->setDataAreaBuf(dataBuf,lenDataBuf);

	pPacket->formatPacket();
}

int CPtclModule_NN::ParsePacket_Ack(unsigned char *pDataAreaBuf,int lenDataArea,
											int &Fn,int &AFN,ParseAckDataUnit *vAck,int &countAck)
{
	int nCount = countAck;
	countAck = 0;
	int lenParse = 0;
	if (pDataAreaBuf == NULL || lenDataArea <4)
		return 0;
	ParsePnFnData PnFn;
	lenParse = ParsePnFn(pDataAreaBuf,&PnFn);
	Fn = PnFn.vFn[0];
	if (Fn == 1 || Fn == 2)
		return lenParse;
	if (Fn == 3 && vAck != NULL)
	{
		AFN = pDataAreaBuf[lenParse++];
		
		while (lenParse <lenDataArea)
		{
			if (countAck >= nCount)
				return lenParse;

			int temp = ParsePnFn(pDataAreaBuf + lenParse,&(vAck[countAck].PnFn));
			lenParse = lenParse + temp;
			vAck[countAck].error = pDataAreaBuf[lenParse++];
			countAck++;
		}
	}
	return lenParse;
}



void CPtclModule_NN::FormatPacket_SetParam(CPtclPacket_NN *pPacket,
													DataUint* pDataUint,int nCountDataUnit,
													unsigned char *pPSW,
													TPData *pTP)
{
	if (pPacket == NULL)
		return ;

	//主站ID
	pPacket->setHostID(m_hostID);

	//控制域功能码
	pPacket->setAFN_Ctrl(0x0A);
	pPacket->setPRM(1);
	pPacket->setFCV(0);
	pPacket->setFCB_ACD(0);
	pPacket->setDIR(0);

	//应用层功能码
	pPacket->setAFN(AFN_SET_PARAM);

	//序列域
	unsigned char seq = getFrameSeq(pPacket->getDeviceAddr());

	pPacket->setFrameSeq(seq);
	if (pTP != NULL)
		pPacket->setTPV(1);
	else
		pPacket->setTPV(0);
	pPacket->setFRI(1);
	pPacket->setFIN(1);
	pPacket->setCON(1);

	//用户数据域
	int lenDataBuf = 0;
	unsigned char dataBuf[2048];
	memset(dataBuf,0,2048);

	//数据单元
	for (int n=0;n<nCountDataUnit;n++)
	{
		if (lenDataBuf + 4 + pDataUint[n].lenDataBuf >= 2048)
			break;

		int len = FormatPnFn(&pDataUint[n].PnFn,dataBuf + lenDataBuf);
		lenDataBuf = lenDataBuf + len;
		memcpy(dataBuf + lenDataBuf,pDataUint[n].pDataBuf,pDataUint[n].lenDataBuf);
		lenDataBuf = lenDataBuf + pDataUint[n].lenDataBuf;
	}

	//PSW
	if (pPSW != NULL)
	{
		unsigned char CRC[16] = {0};
		int nPswLen = GetPW(pPacket, dataBuf, pPSW, CRC,1);
		memcpy(dataBuf+lenDataBuf,CRC,nPswLen);
		lenDataBuf = lenDataBuf + nPswLen;
	}
	
	//TP
	if (pTP != NULL)
	{
		int len = FormatTP(pTP,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	pPacket->setDataAreaBuf(dataBuf,lenDataBuf);

	pPacket->formatPacket();
}



void CPtclModule_NN::FormatPacket_ReadParam(	CPtclPacket_NN *pPacket,
													FormatPnFnData *pPnFn,int nCountPnFn,
													TPData *pTP)
{
	if (pPacket == NULL)
		return ;

	//主站ID
	pPacket->setHostID(m_hostID);

	//控制域功能码
	pPacket->setAFN_Ctrl(0x0B);//待证实
	pPacket->setPRM(1);
	pPacket->setFCV(0);
	pPacket->setFCB_ACD(0);
	pPacket->setDIR(0);

	//应用层功能码
	pPacket->setAFN(AFN_READ_PARAM);

	//序列域
	unsigned char seq = getFrameSeq(pPacket->getDeviceAddr());
	pPacket->setFrameSeq(seq);
	if (pTP != NULL)
		pPacket->setTPV(1);
	else
		pPacket->setTPV(0);
	pPacket->setFRI(1);
	pPacket->setFIN(1);
	pPacket->setCON(0);

	//用户数据域
	int lenDataBuf = 0;
	unsigned char dataBuf[2048];
	memset(dataBuf,0,2048);

	//数据单元
	for (int n=0;n<nCountPnFn;n++)
	{
		if (lenDataBuf + 4>= 2048)
			break;

		int len = FormatPnFn(pPnFn+n,dataBuf + lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	//TP
	if (pTP != NULL)
	{
		int len = FormatTP(pTP,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	pPacket->setDataAreaBuf(dataBuf,lenDataBuf);

	pPacket->formatPacket();
}



void CPtclModule_NN::FormatPacket_ReadParam_DataUnit(CPtclPacket_NN *pPacket,
															  DataUint* pDataUint,
															  int nCountDataUnit,
															  TPData *pTP)
{
	if (pPacket == NULL)
		return ;

	//主站ID
	pPacket->setHostID(m_hostID);

	//控制域功能码
	pPacket->setAFN_Ctrl(0x0B);//待证实
	pPacket->setPRM(1);
	pPacket->setFCV(0);
	pPacket->setFCB_ACD(0);
	pPacket->setDIR(0);

	//应用层功能码
	pPacket->setAFN(AFN_READ_PARAM);

	//序列域
	unsigned char seq = getFrameSeq(pPacket->getDeviceAddr());
	pPacket->setFrameSeq(seq);
	if (pTP != NULL)
		pPacket->setTPV(1);
	else
		pPacket->setTPV(0);
	pPacket->setFRI(1);
	pPacket->setFIN(1);
	pPacket->setCON(0);

	//用户数据域
	int lenDataBuf = 0;
	unsigned char dataBuf[2048];
	memset(dataBuf,0,2048);

	//数据单元
	for (int n=0;n<nCountDataUnit;n++)
	{
		if (lenDataBuf + 4>= 2048)
			break;

		int len = FormatPnFn(&pDataUint[n].PnFn, dataBuf + lenDataBuf);
		lenDataBuf = lenDataBuf + len;
		
		if (pDataUint[n].lenDataBuf > 0)
			memcpy(dataBuf+lenDataBuf, pDataUint[n].pDataBuf, pDataUint[n].lenDataBuf);

		lenDataBuf = lenDataBuf + pDataUint[n].lenDataBuf;
	}

	//TP
	if (pTP != NULL)
	{
		int len = FormatTP(pTP,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	pPacket->setDataAreaBuf(dataBuf,lenDataBuf);

	pPacket->formatPacket();
}




void CPtclModule_NN::FormatPacket_ReadData_RealTime(	CPtclPacket_NN *pPacket,
															FormatPnFnData *pPnFn,int nCountPnFn,
															TPData *pTP)
{
	if (pPacket == NULL)
		return ;

	//主站ID
	pPacket->setHostID(m_hostID);

	//控制域功能码
	pPacket->setAFN_Ctrl(0x0B);
	pPacket->setPRM(1);
	pPacket->setFCV(0);
	pPacket->setFCB_ACD(0);
	pPacket->setDIR(0);

	//应用层功能码
	pPacket->setAFN(AFN_READ_DATA_REALTIME);

	//序列域
	unsigned char seq = getFrameSeq(pPacket->getDeviceAddr());
	pPacket->setFrameSeq(seq);
	if (pTP != NULL)
		pPacket->setTPV(1);
	else
		pPacket->setTPV(0);
	pPacket->setFRI(1);
	pPacket->setFIN(1);
	pPacket->setCON(0);

	//用户数据域
	int lenDataBuf = 0;
	unsigned char dataBuf[2048];
	memset(dataBuf,0,2048);

	//数据单元
	for (int n=0;n<nCountPnFn;n++)
	{
		if (lenDataBuf + 4>= 2048)
			break;

		int len = FormatPnFn(pPnFn+n,dataBuf + lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	//TP
	if (pTP != NULL)
	{
		int len = FormatTP(pTP,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	pPacket->setDataAreaBuf(dataBuf,lenDataBuf);

	pPacket->formatPacket();
}

void CPtclModule_NN::FormatPacket_ReadData_History(	CPtclPacket_NN *pPacket,
															DataUint* pDataUint,int nCountDataUnit,
															TPData *pTP)
{
	if (pPacket == NULL)
		return ;

	//主站ID
	pPacket->setHostID(m_hostID);

	//控制域功能码
	pPacket->setAFN_Ctrl(0x0B);//待证实
	pPacket->setPRM(1);
	pPacket->setFCV(0);
	pPacket->setFCB_ACD(0);
	pPacket->setDIR(0);

	//应用层功能码
	pPacket->setAFN(AFN_READ_DATA_HISTORY);

	//序列域
	unsigned char seq = getFrameSeq(pPacket->getDeviceAddr());
	pPacket->setFrameSeq(seq);
	if (pTP != NULL)
		pPacket->setTPV(1);
	else
		pPacket->setTPV(0);
	pPacket->setFRI(1);
	pPacket->setFIN(1);
	pPacket->setCON(0);

	//用户数据域
	int lenDataBuf = 0;
	unsigned char dataBuf[2048];
	memset(dataBuf,0,2048);

	//数据单元
	for (int n=0;n<nCountDataUnit;n++)
	{
		if (lenDataBuf + 4 + pDataUint[n].lenDataBuf >= 2048)
			break;

		int len = FormatPnFn(&pDataUint[n].PnFn,dataBuf + lenDataBuf);
		lenDataBuf = lenDataBuf + len;
		memcpy(dataBuf + lenDataBuf,pDataUint[n].pDataBuf,pDataUint[n].lenDataBuf);
		lenDataBuf = lenDataBuf + pDataUint[n].lenDataBuf;
	}

	//TP
	if (pTP != NULL)
	{
		int len = FormatTP(pTP,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	pPacket->setDataAreaBuf(dataBuf,lenDataBuf);

	pPacket->formatPacket();
}

void CPtclModule_NN::FormatPacket_ReadData_Event(	CPtclPacket_NN *pPacket,
														int Fn,unsigned char startID,unsigned endID,
														TPData *pTP)
{
	if (pPacket == NULL)
		return ;

	//主站ID
	pPacket->setHostID(m_hostID);

	//控制域功能码
	pPacket->setAFN_Ctrl(0x0B);//待证实
	pPacket->setPRM(1);
	pPacket->setFCV(0);
	pPacket->setFCB_ACD(0);
	pPacket->setDIR(0);

	//应用层功能码
	pPacket->setAFN(AFN_READ_DATA_EVENT);

	//序列域
	unsigned char seq = getFrameSeq(pPacket->getDeviceAddr());
	pPacket->setFrameSeq(seq);
	if (pTP != NULL)
		pPacket->setTPV(1);
	else
		pPacket->setTPV(0);
	pPacket->setFRI(1);
	pPacket->setFIN(1);
	pPacket->setCON(0);

	//用户数据域
	int lenDataBuf = 0;
	unsigned char dataBuf[20];
	memset(dataBuf,0,20);

	//数据单元
	FormatPnFnData PnFn;
	PnFn.Pn = 0;
	PnFn.Fn = Fn;
	int len = FormatPnFn(&PnFn,dataBuf);
	lenDataBuf = len;

	dataBuf[lenDataBuf++] = startID;
	dataBuf[lenDataBuf++] = endID;

	//TP
	if (pTP != NULL)
	{

		len = FormatTP(pTP,dataBuf+lenDataBuf);
		lenDataBuf = lenDataBuf + len;
	}

	pPacket->setDataAreaBuf(dataBuf,lenDataBuf);

	pPacket->formatPacket();
}


unsigned char CPtclModule_NN::getFrameSeq(unsigned int nAddr)
{
	unsigned char seq = 0;
	map <unsigned long,unsigned char>::iterator it = m_mapSeq.find(nAddr);
	if (it == m_mapSeq.end())
	{
		m_mapSeq[nAddr] = seq;
	}
	else
	{
		seq = m_mapSeq[nAddr];
		if (seq == 15)
			seq = 0;
		else
			seq ++;

		m_mapSeq[nAddr] = seq;

	}
	return seq;
}


//密码算法
void CPtclModule_NN::CRC16( unsigned char *data ,int data_len ,int start_id ,int crc_key , unsigned char *crc_code)
{
	unsigned char CL,CH,SaveHi,SaveLo;
	unsigned char CRC16Lo = 0;
	unsigned char CRC16Hi = 0;

	CL = crc_key % 256;
    CH = crc_key / 256;
	for(int i = start_id ;i<(data_len + start_id);i++)
	{
		 CRC16Lo = CRC16Lo ^ data[i]; //每一个数据与CRC寄存器进行异或
		 for(int j = 0 ;j< 8 ;j++)
		 {
            SaveHi = CRC16Hi;
            SaveLo = CRC16Lo;
            CRC16Hi = CRC16Hi / 2;           //高位右移一位
            CRC16Lo = CRC16Lo / 2;           //低位右移一位
            if((SaveHi & 0x01) == 0x01)		//如果高位字节最后一位为1
                CRC16Lo = CRC16Lo | 0x80;    //则低位字节右移后前面补1
											 //否则自动补0
            if((SaveLo & 0x01) == 0x01)		//如果LSB为1，则与多项式码进行异或
			{
                CRC16Hi = CRC16Hi ^ CH;
                CRC16Lo = CRC16Lo ^ CL;
			}
		 }
	}
	crc_code[0] = CRC16Lo;              //CRC低位
    crc_code[1] = CRC16Hi;              //CRC高位
}


void CPtclModule_NN::GetCRC(unsigned char *ADDR, int frametype, int teamno, unsigned char *buf, unsigned char hostID, int CRCKey ,unsigned char *CRC)
{
	unsigned char CRCBuf[255] = {0};
	if(frametype==0x04||frametype==0x05)
		CRCBuf[0] = 0x4A; //设置命令与控制命令
	else if(frametype==0x01)
		CRCBuf[0] = 0x41; //复位命令
	else if(frametype==0x10)
		CRCBuf[0] = 0x4B; //数据转发
	else if(frametype==0x0F)
		CRCBuf[0] = 0x4B; //文件传输
	else
		return;

	CRCBuf[1] = ADDR[0];
	CRCBuf[2] = ADDR[1];
	CRCBuf[3] = ADDR[2];
	CRCBuf[4] = ADDR[3];
	
	CRCBuf[5] = (hostID<<1) + teamno;

	memcpy(CRCBuf+6,buf,6);

	CRC16(CRCBuf ,12 ,0 ,CRCKey ,CRC);
}

int CPtclModule_NN::GetPW(CPtclPacket_NN *pPacket, unsigned char *UserDataBuf, unsigned char *pPSW, unsigned char *CRC,int isEx)
{
	if (isEx == 0)
	{
		int crc_key = (int)pPSW[0] * 0x100 + (int)pPSW[1];
		unsigned char CRCDataBuf[12] = {0};
		CRCDataBuf[0] = pPacket->getAFN();		//AFN
		CRCDataBuf[1] = pPacket->getSeqArea();	//SEQAREA
		memcpy(CRCDataBuf+2, UserDataBuf, 4);	//用户数据域取4字节

		unsigned char CRCDeviceAddr[4] = {0};
		unsigned int nAddr = pPacket->getDeviceAddr();
		memcpy(CRCDeviceAddr,&nAddr,4);

		GetCRC(CRCDeviceAddr, pPacket->getAFN(), 0, CRCDataBuf, m_hostID, crc_key, CRC);

		return 2;
	}
	else if (isEx == 1)
	{
		memset(CRC,0,16);
		return 16;
	}
	return 0;
}