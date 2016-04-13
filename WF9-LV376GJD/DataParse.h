#ifndef _DataParse_H
#define _DataParse_H

#include "jblib.h"
#include "Global.h"
#include "DataConvert.h"
#include "Common.h"
#include "DBFOperate.h"
#include "DataPro.h"

const char *ConstResStr[] = {"无应答!", "操作成功!", "终端否认!", "异常应答!", "解析失败!",};

void SaveMeterRecord(StruMeterRecord * RecvMeterRecord, int count);
int WritValueToDBF(Stru645Data * data, U8 port);


/******************************************************************************/
/*                        698.41/376.1协议检查数据帧有效性                    */ 
/*	功能: 检查698协议数据帧有效性,提取用户数据区数据帧                        */
/*	参数: frame——返回数据帧 rlen——数据长度                                    */
/*	返回: 成功返回用户数据区实际数据长度，异常返回-1                          */
/******************************************************************************/
int Check698Buff(unsigned char *frame, unsigned short rlen)
{
  int i = 0;
  int len = 0;
  unsigned char convbuff[1024] = {0};
  unsigned char checkSum;
  
  len = rlen - MIN698BUFFLEN;
  if(len < 0)
  {
    return ERROR;
  }
    
  for(i = 0; i < len; i++)
  {
    if ((frame[i] == BUFF_START) && (frame[i + 5] == BUFF_START))
    {
      break;
    }
  }
  if (i == len)
  {
  	return ERROR;
  }
  
  len = (frame[i + 2] * 0x100 + frame[i + 1]) / 4;
  
  checkSum = CheckSum(frame, 6 + i, len);
  
  if(checkSum != (*(frame + 6 + i + len)))
  {
    return ERROR;
  } 
  if(*(frame + 6 + i + 1 + len) != BUFF_END)
  {
    return ERROR;
  }
  
  
  
	memcpy(convbuff, frame, rlen);
	memcpy(frame, convbuff + i + 6, len);  
   
  return len;
}


//检查645协议数据帧有效性
int Check645Frame(U8 frame[], U16 rlen)
{
  int i, len = NUM_12;
  unsigned char CheckNum = 0;
  unsigned char tmpbuf[NUM_255] = {0};
  
  if (rlen < len)
  { 
  	return ERROR;
  }
  
  len = rlen - len + 1;	
  for (i = 0; i < len; i++)
  {
    if(frame[i] == BUFF_START && frame[i + NUM_7] == BUFF_START)
      break;
  }
  if (i == len)
  {
    return ERROR;
  }
    
  memcpy(tmpbuf, frame + i, rlen - i);
  
  len = tmpbuf[NUM_9];
  if (tmpbuf[len + NUM_11] != BUFF_END)
  {
    return ERROR;
  }
  
  len += NUM_10;  
  CheckNum = tmpbuf[len];
  if (CheckNum != CheckSum(tmpbuf, 0, len))
  {
    return ERROR;
  }
  
  len += NUM_2;
  memset(frame, 0, rlen);
  memcpy(frame, tmpbuf, len);
  
  return len;
}

//解析终端地址
char *ParseAddr(U8 *addr, char * term)
{
  char temp[10] = {};
  int tmpint = 0;
  U8 addrtype = 0;
  use(SYSCONFDBF);
  if(libsumr() != 0)
  { 
    libset(0,2);
    libread(temp);
    addrtype = StrToInt(temp,10);
  }
  use("");
    
  if(addrtype != 10) 
  {
    addrtype = 16;    
  }  
  
  memset(temp, 0, sizeof(temp));
  HexToStr(temp, addr, 2, 1, 0);
  
  memcpy(term, temp, 4);
  
  memset(temp, 0, sizeof(temp));
  if(addrtype == 10)
  {
    tmpint = addr[2] + addr[3] * 0x100;
    sprintf(temp, "%05d", tmpint);
    memcpy(term + 4, temp, 5);
  }
  else if(addrtype == 16) 
  {
    HexToStr(temp, addr + 2, 2, 1, 0);
    memcpy(term + 4, temp, 4);
  }
  
  return term;     
}

//解析Pn
int ParsePn(U8 pn1, U8 pn2)
{
  int i;
  if((pn1 == 0) && (pn2 == 0))
    return 0;
  if((pn1 != 0) && (pn2 == 0))
    return -1;
    
  for(i = 0; i < 8; i++)
  {
    if ((pn1 >> i) == 1)
      break;
  }
  
  return ((pn2 - 1) * 8 + i + 1);  
}

//解析Fn
U16 ParseFn(U8 fn1, U8 fn2)
{
  int i;
  if(fn1 == 0)
    return 0;
    
  for(i = 0; i < 8; i++)
  {
    if ((fn1 >> i) == 1)
      break;
  }
  
  return (fn2 * 8 + i + 1);  
}


/******************************************************************************/
/*                            电表地址解析函数                                */ 
/*	功能: 解析表地址                                                          */
/*	参数: buff——返回帧, addr——表地址                                          */
/*	返回: 地址                                                                */
/******************************************************************************/
char *ParseMeteAddr(const unsigned char *buff, char *addr)
{
	sprintf(addr, "%02x%02x%02x%02x%02x%02x", 
	       buff[NUM_5], buff[NUM_4], buff[NUM_3], buff[NUM_2], buff[NUM_1], buff[0]);
	       
	return addr;
}


/******************************************************************************/
/*                               获取参数索引函数                             */ 
/*	功能: 获取参数索引                                                    		*/
/*	参数: recvdata——返回数据                                            			*/
/*	返回: 成功返回参数索引值，异常返回-1                                  		*/
/******************************************************************************/
int GetParamIndex(Stru698Data *recvdata)
{
	int i = 0;
	
	for (i = 0; i < MAX_PARAMITEM_NUM; i++)
	{
		if((GlobalParam+i)->Flag ==1 &&
                (GlobalParam+i)->AFN == recvdata->AFN 
                && (GlobalParam+i)->Fn == recvdata->Fn)
		{
			return i;	
		}
	}
		
	for (i = 0; i < MAX_PARAMITEM_NUM; i++)
	{
		
		if ((GlobalParam+i)->AFN == recvdata->AFN 
                && (GlobalParam+i)->Fn == recvdata->Fn
                )
		{
			break;
		}
	}
	
	if (i == MAX_PARAMITEM_NUM)
	{
		return ERROR;
	}
	
	return i;
}


/******************************************************************************/
/*                        698.41/376.1协议解析终端电表档案                    */ 
/*	功能: 解析终端电表档案并保存至数据库                                  		*/
/*	参数: Recv698Data——返回数据                                         			*/
/*	返回: 成功返回1                                                       		*/
/******************************************************************************/
U8 ParseAFN0AF10(Stru698Data* Recv698Data)
{
  int i, j; 
  int count = 0;
  int number = 0;
  int index = 0;
  U16 mpNo = 0;
  char term[10] = {0};
  char temp[15] = {0};

  StruMeterRecord RecvMeterRecord;
  
  number = Recv698Data->Userbuf[index++];
  number += Recv698Data->Userbuf[index] * 0x100;

  if (number < 1)
  {
    return 1;
  }
  index++;
    
  ParseAddr(Recv698Data->Saddr, term);
  memcpy(RecvMeterRecord.Term, term, 9);
  
  use(METE_RECD_RCV_GW); 
  count = libsumr();
    
  for (i = 0; i < number; i++)
  {
    RecvMeterRecord.DevNo = Recv698Data->Userbuf[index+1] * 0x100 
    											+ Recv698Data->Userbuf[index];
    index += 2;
    
    RecvMeterRecord.MeterNo = Recv698Data->Userbuf[index+1] * 0x100 
    												+ Recv698Data->Userbuf[index];
    index += 2;
       
    RecvMeterRecord.BaudRate = (U8)((Recv698Data->Userbuf[index]) >> 5);
    RecvMeterRecord.Port = (Recv698Data->Userbuf[index]) & 0x1F;    
    index++;
    
    RecvMeterRecord.ProtoType = Recv698Data->Userbuf[index++];
    
    memset(temp, 0, sizeof(temp));
    memcpy(temp, Recv698Data->Userbuf + index, 6);
    memset(RecvMeterRecord.MeterAddr, 0, sizeof(RecvMeterRecord.MeterAddr));
    HexToStr(RecvMeterRecord.MeterAddr, temp, 6, 1, 0);
    index += 6;
    index += 6;
    
    RecvMeterRecord.Rates = Recv698Data->Userbuf[index++];
    index++;
    
    memset(temp, 0, sizeof(temp));
    memcpy(temp, Recv698Data->Userbuf + index, 6);   
    memset(RecvMeterRecord.ColAddr, 0, sizeof(RecvMeterRecord.ColAddr));
    HexToStr(RecvMeterRecord.ColAddr, temp, 6, 1, 0);
    index += 6;
    
    RecvMeterRecord.Sort = Recv698Data->Userbuf[index++];
    
    if (RecvMeterRecord.MeterNo == 0)
    {
    	continue;
    }
      
    SaveMeterRecord(&RecvMeterRecord, count);   
  }
  
  use("");
  return 1;  
} 
//保存电表档案至数据库
void SaveMeterRecord(StruMeterRecord * RecvMeterRecord, int count)
{
  int j;
  char temp[23] = {0};
 
  for(j = 0; j < count; j++)
  {
    libset(j, 0);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
      continue;

    libset(j, 7);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if ((temp[0] == '\0') || (RecvMeterRecord->DevNo == StrToInt(temp, 10)))
      break;      
  }
          
  if (j == count)
  {
    libappend();
    j = libgetr();
  }
     
  libset(j, 2);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%s", RecvMeterRecord->Term); 
  libwrit(temp);
    
  libset(j, 4);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%s", RecvMeterRecord->MeterAddr); 
  libwrit(temp);
  
  libset(j, 6);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%s", RecvMeterRecord->ColAddr); 
  libwrit(temp);
  
  libset(j, 7);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", RecvMeterRecord->DevNo); 
  libwrit(temp);
  
  libset(j, 8);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", RecvMeterRecord->MeterNo); 
  libwrit(temp);
  
  libset(j, 9);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", RecvMeterRecord->Port); 
  libwrit(temp);
  
  libset(j, 10);
  memset(temp, 0, sizeof(temp));
  switch(RecvMeterRecord->BaudRate)
  {
    case 3:
      sprintf(temp, "2400");
      break;
    case 2:
      sprintf(temp, "1200");
      break;
    case 1:
      sprintf(temp, "600");
      break;
    case 4:
      sprintf(temp, "4800");
      break;
    case 5:
      sprintf(temp, "7200");
      break;
    case 6:
      sprintf(temp, "9600");
      break;
    case 7:
      sprintf(temp, "19200");
      break; 
    case 0:
    default:
      sprintf(temp, "0");
      break;                       
  }
  libwrit(temp);
  
  libset(j, 11);
  memset(temp, 0, sizeof(temp));
  switch(RecvMeterRecord->ProtoType)
  {
    case 0x1E:
      sprintf(temp, "DL/T 645-2007");
      break;
    case 0x01:
      sprintf(temp, "DL/T 645-1997");      
      break;
    case 0:
      sprintf(temp, "0");
      break;
    case 0x02:
      sprintf(temp, "交采协议");
      break;
    case 0x1F:
      sprintf(temp, "载波协议");
      break;                  
  }
  libwrit(temp);
  
  libset(j, 14);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", RecvMeterRecord->Rates); 
  libwrit(temp);
  
  libset(j, 15);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%02x", RecvMeterRecord->Sort); 
  libwrit(temp);

}


/******************************************************************************/
/*                      698.41/376.1协议保存测量点数据至数据库                */ 
/*	功能: 保存测量点数据至数据库                              								*/
/*	参数: sData——返回数据 mpNo——测量点号 fn——字段列 count——数据库记录总数			*/
/*	返回: 返回数据库记录总数                                 									*/
/******************************************************************************/
int SaveMPDataToDBF(char *sData, char *sTime, unsigned char mpNo, unsigned char fn, int count)
{
  int r = 0;
  int rCount = 0;
  char temp[23] = {0};
          
  rCount = count;
  for (r = 0; r < count; r++)
  {  	
  	libset(r, 0);
  	memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      continue;  	
    }
    
    libset(r, FD_MPNO);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (mpNo == StrToInt(temp, DEC))
    {
      break;     	
    }    
  }
  
  if (r >= count)
  {
  	libappend();
  	r = libgetr();
  	
    libset(r, FD_MPNO);
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%d", mpNo);
    libwrit(temp);
  	rCount++;
  }
  
  libset(r, fn);
  libwrit(sData);
  
  libset(r, FD_RESERVE);
  libwrit(sTime);

  return rCount;  
}


/******************************************************************************/
/*                        698.41/376.1协议解析数据                    				*/ 
/*	功能: 解析一类数据F129和二类数据F161并保存数据至数据库                 		*/
/*	参数: Recv698Data——返回数据                                         			*/
/*	返回: 成功返回1 异常返回0                                             		*/
/******************************************************************************/
U8 ParseMPData(Stru698Data* Recv698Data)
{
	
	int index = 0;
  int count = 0;
  //char term[10] = {0};
  char temp[23] = {0};
  char data[23] = {0};
  char stime[23] = {0};
  unsigned char mpNo = 0;
  unsigned char fn = 0;
  unsigned char rates = 0;
   
  //ParseAddr(Recv698Data->Saddr, term);  
    
  fn = Recv698Data->Fn;    
  if (fn == FN_129 && Recv698Data->AFN == AFN_698RTDATA)
  {
  	fn = FD_RTDATA;
  }
  else if (fn == FN_161 && Recv698Data->AFN == AFN_698HISDATA)
  {
  	fn = FD_DAYDATA;
  }
  else
  {
  	return 0;
  }
	
  if (fileexits(METE_DATA_RCV_GW) == FSNULL)
  {
  	copy(METE_RECD_NEW_GW, METE_DATA_RCV_GW);
  }
  use(METE_DATA_RCV_GW); 
  count = libsumr();

  while(index < Recv698Data->Ulen)
  {
  	mpNo = ParsePn(Recv698Data->Userbuf[index], Recv698Data->Userbuf[index+1]);
  	index += 2;
  	  	
  	if (fn == FD_DAYDATA)  										
  	{
  		if (ParseFn(Recv698Data->Userbuf[index], Recv698Data->Userbuf[index+1]) != FN_161)
  		{
  			break;
  		}
  		index += 2;
  		index += 3;   												//日冻结数据时标
  	}     
  	else
  	{
  		if (ParseFn(Recv698Data->Userbuf[index], Recv698Data->Userbuf[index+1]) != FN_129)
  		{
  			break;
  		}
  		index += 2;  		
  	} 
  	
  	memset(temp, 0, sizeof(temp));
  	memset(stime, 0, sizeof(stime));
		memcpy(temp, Recv698Data->Userbuf + index, 5);
		if (temp[0] == 0xee)
		{
			sprintf(stime, "----");
		}
		else
		{
			sprintf(stime, "%02x-%02x-%02x %02x:%02x", temp[4], temp[3], temp[2], temp[1], temp[0]);
		}
		
  	index += 5;															//抄表时间
  	
		rates = Recv698Data->Userbuf[index++];	//费率数
		/*if (!rates)
		{
			continue;
		}*/
		memset(temp, 0, sizeof(temp));
		memset(data, 0, sizeof(data));
		memcpy(temp, Recv698Data->Userbuf + index, 5);
		sprintf(data, "%02x%02x%02x.%02xkWh", temp[4], temp[3], temp[2], temp[1]);
		index += 5;
		index += rates * 5;
		
		count = SaveMPDataToDBF(data, stime, mpNo, fn, count);
  }
  
  use("");
  return 1; 
}


/******************************************************************************/
/*                       698.41/376.1协议解析透明转发抄表返回帧               */ 
/*	功能: 解析透明转发抄表返回帧并保存电表数据至数据库                    		*/
/*	参数: Recv698Data——返回数据                                         			*/
/*	返回: 成功返回1，异常返回0                                            		*/
/******************************************************************************/
U8 ParseAFN10F01(Stru698Data* Recv698Data)
{
  int res, rlen = 0;
  U8 port = 0;
  char term[10] = {0};
  char temp[15] = {0};
  U8 tmpbuf[100] = {0};
  int index = 0;
  Stru645Data Recv645Data;
  
  if (Recv698Data->AFN != 0x10 || Recv698Data->Fn != 1 || Recv698Data-> Pn != 0)  
  {    
    return 0;
  }

  port = Recv698Data->Userbuf[index++];  
  if (port > 31 || port == 0)
  {
    return 0; 
  }
  
  ParseAddr(Recv698Data->Saddr, term);
  if (strcmp(term, CurrMPAttrib.Term) != 0)
  {
    return 0; 
  }
  
  rlen = Recv698Data->Userbuf[index++];
  rlen += Recv698Data->Userbuf[index] * 0x100;
  if (rlen == 0)
  {
  	return 0;
  }
  index++;
    
  memcpy(tmpbuf, Recv698Data->Userbuf + index, rlen);
  if (Check645Frame(tmpbuf, rlen) < 12)
  {
    return 0;
  }
  
  index = 1;
  memcpy(temp, tmpbuf + index, 6);
  HexToStr(Recv645Data.address, temp, 6, 1, 0); 
  index += 6;
  index++;  
  Recv645Data.ctrcode = tmpbuf[index++];
  Recv645Data.datalen = tmpbuf[index++];
  
  memset(temp, 0, sizeof(temp));
  switch(Recv645Data.ctrcode)
  {
    case 0x81:
      Recv645Data.dataitm = (U8)(tmpbuf[index+1]-DATA645DEAL) * 0x100 + (U8)(tmpbuf[index]-DATA645DEAL); 
      index += 2;
      memcpy(temp, tmpbuf + index, 4);
      break;
    case 0x91:
      Recv645Data.dataitm = ((U8)(tmpbuf[index+3]-DATA645DEAL) << 24) + ((U8)(tmpbuf[index+2]-DATA645DEAL) << 16) 
      										+ ((U8)(tmpbuf[index+1]-DATA645DEAL) << 8) + (U8)(tmpbuf[index]-DATA645DEAL); 
      index += 4;
      memcpy(temp, tmpbuf + index, 4);
      break;
    default:
      return 0;
  }
  
  sprintf(Recv645Data.userbuf, "%02x%02x%02x.%02xkWh", temp[3]-DATA645DEAL, 
  				temp[2]-DATA645DEAL, temp[1]-DATA645DEAL, temp[0]-DATA645DEAL);
  
  res = WritValueToDBF(&Recv645Data, port);
  
  return res;  
}


//保存电表数据至数据库
int WritValueToDBF(Stru645Data * data, U8 port)
{
  int r;
  char temp[23] = {0};
  int count = 0;
 
  use(METE_DATA_RCV_GW); 
  count = libsumr();

  for(r = 0; r < count; r++)
  {
    libset(r, 0);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      continue;
    }

    libset(r, FD_METERADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strncmp(temp, data->address, 12) == 0)
    {
      break;      
    }   	    
  }   
	  
  if (r >= count)
  {
  	use("");
  	return 0;
  }
	   
  libset(r, FD_PORT);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", port);
  libwrit(temp);

  libset(r, FD_RTDATA);
  libwrit(data->userbuf);
  
  use("");  
  return 1;
}


/******************************************************************************/
/*                               获取参数索引函数                             */ 
/*	功能: 获取参数索引                                                        */
/*	参数: compDI——数据项编码                                                  */
/*	返回: 成功返回参数索引值，异常返回-1                                      */
/******************************************************************************/
int Get645ParamIndex(U32 compDI)
{
	int i = 0;
	
	for (i = 0; i < MAX_PARAMITEM_NUM; i++)
	{
		if ((GlobalParam+i)->Param_ID == compDI)
		{
			break;
		}
	}
	
	if (i == MAX_PARAMITEM_NUM)
	{
		return ERROR;
	}
	
	return i;
}


/******************************************************************************/
/*                        浙电体系规约帧解析终端参数函数                      */ 
/*	功能: 解析终端参数                                                        */
/*	参数: recvdata——返回帧信息, paramindex——参数索引                          */
/*	返回: 解析正常返回0，异常返回-1                                           */
/******************************************************************************/
int Parse645Param(Stru645Data* recvdata, int paramindex)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int index = 0;
	int len = 0;
	unsigned int tmpint = 0;
	unsigned char temp[NUM_50] = {0};
	unsigned char conv[NUM_3] = {0};
	
	if (paramindex < 0)
	{
		return ERROR;
	}
			
	if ((GlobalParam+paramindex)->Param_ID != GlobalParamItem->Param_ID)
	{
		memset(GlobalParamItem, 0, sizeof(GlobalParamItem));
		GetParamValueFromDBF(GlobalParam+paramindex, GlobalParamItem, PARAMDBF);
	}
	
	//for (i = 0; i < (GlobalParam+paramindex)->Param_Num; i++)
	for (i = (GlobalParam+paramindex)->Param_Num - 1; i >= 0; i--)
	{
		len = (GlobalParamItem + i)->Param_Item_Clen;
		switch((GlobalParamItem + i)->Param_Item_Mask)
		{
			case 's':
				memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->userbuf[index]);
					strcat(temp, conv);
					index++;
				}
				ReverseStr(temp, 0);
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);				
				break;
			case 'h':	
				memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->userbuf[index]);
					strcat(temp, conv);
					index++;
				}

				ReverseStr(temp, 0);
			  FormatString((GlobalParamItem+i)->Param_Item_Value, temp);	
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
				break;
			case 'd':
				tmpint = 0;
				for (k = 0; k < len; k++)
				{
					tmpint += ((recvdata->userbuf[index]) << (8 * k)); 
					index++;
				}
				sprintf((GlobalParamItem+i)->Param_Value, "%d", tmpint);
				break;
			case 'a':
				k = 0;
				if (len == 0)
				{
					len = recvdata->datalen - DI_BYTES;
				}
				while(k < len)
				{
					if (recvdata->userbuf[index] != 0)
					{
						break;
					}
					index++;
					k++;
				}
				memset(temp, 0, sizeof(temp));
				len -= k;
				memcpy(temp, recvdata->userbuf+index, len);
				index += len;
				k = 0;
				while(k < len)
				{
					(GlobalParamItem+i)->Param_Value[k] = temp[len - k - 1];
					k++;
				}
				break;
			case 'e':
				sprintf((GlobalParamItem+i)->Param_Value, "%x", recvdata->userbuf[index]);
				index++;
				break;
//			case 't':
//				memset(temp, 0, sizeof(temp));		
//				for (k = 0; k < len; k++)
//				{
//					temp[k] = recvdata->userbuf[index + len - k - 1];
//				}
//				index += len;
//
//				FormatTimeStr((GlobalParamItem+i)->Param_Item_Value, temp, len);
//				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
//				break;				
			default:
				memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->userbuf[index]);
					strcat(temp, conv);
					index++;
				}
				ReverseStr(temp, 0);
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);							
				break;
		}

	}
	

	return 0;		
}


/******************************************************************************/
/*                        浙电体系规约帧解析测量点参数函数                    */ 
/*	功能: 解析测量点参数及数据                                                */
/*	参数: recvdata——返回帧信息, paramindex——参数索引值                        */
/*	返回: 解析正常返回0，异常返回-1                                           */
/******************************************************************************/
int Parse645Data(Stru645Data* recvdata, int paramindex)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int index = 0;
	int len = 0;
	unsigned int tmpint = 0;
	unsigned char temp[NUM_100] = {0};
	unsigned char conv[NUM_3] = {0};

	if (paramindex < 0)
	{
		return ERROR;
	}
			
	if ((GlobalParam+paramindex)->Param_ID != GlobalParamItem->Param_ID)
	{
		memset(GlobalParamItem, 0, sizeof(GlobalParamItem));
		GetParamValueFromDBF(GlobalParam+paramindex, GlobalParamItem, PARAMDBF);
	}

	for (i = 0; i < (GlobalParam+paramindex)->Param_Num; i++)
	{
		len = (GlobalParamItem + i)->Param_Item_Clen;
		switch((GlobalParamItem + i)->Param_Item_Mask)
		{
			case 's':
				memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->userbuf[index]);
					strcat(temp, conv);
					index++;
				}
				ReverseStr(temp, 0);
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);				
				break;
			case 'h':	
				memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->userbuf[index]);
					strcat(temp, conv);
					index++;
				}
				ReverseStr(temp, 0);
				FormatString((GlobalParamItem+i)->Param_Item_Value, temp);
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
				break;
			case 'd':
				tmpint = 0;
				for (k = 0; k < len; k++)
				{
					tmpint += ((recvdata->userbuf[index]) << (NUM_8 * k)); 
					index++;
				}
				sprintf((GlobalParamItem+i)->Param_Value, "%d", tmpint);
				break;
			case 'a':
				k = 0;
				while(k < len)
				{
					if (recvdata->userbuf[index] != 0)
					{
						break;
					}
					index++;
					k++;
				}
				memset(temp, 0, sizeof(temp));
				len -= k;
				memcpy(temp, recvdata->userbuf+index, len);
				index += len;
				k = 0;
				if (recvdata->dataitm == 0x04A00101)
				{
					while(k < len)
					{
						(GlobalParamItem+i)->Param_Value[k] = temp[k];
						k++;
					}
				}
				else
				{
					while(k < len)
					{
						(GlobalParamItem+i)->Param_Value[k] = temp[len - k - 1];
						k++;
					}
				}
				break;
			case 'e':
				sprintf((GlobalParamItem+i)->Param_Value, "%x", recvdata->userbuf[index]);
				index++;
				break;
//			case 't':		
//				memset(temp, 0, sizeof(temp));		
//				for (k = 0; k < len; k++)
//				{
//					temp[k] = recvdata->userbuf[index + len - k - 1];
//				}
//				index += len;
//
//				FormatTimeStr((GlobalParamItem+i)->Param_Item_Value, temp, len);
//				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
//				break;				
			default:
				memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->userbuf[index]);
					strcat(temp, conv);
					index++;
				}
				ReverseStr(temp, 0);
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);				
				break;
		}
		
	}

	return 0;		
}


/******************************************************************************/
/*                  DL/T645协议 国网600S采集器规约帧解析函数                  */ 
/*	功能: 解析返回帧                                                          */
/*	参数: recvbuff——返回帧, rlen——返回帧长度                                  */
/*	返回: 解析正常返回0，异常返回其它值                                       */
/******************************************************************************/
int Parse645Buff(unsigned char recvbuff[], unsigned short rlen)
{
	int i = 0;
	int result = 1;
	int len = 0;
	unsigned short index = 1;
	Stru645Data recvdata;
	
	if (!rlen)
	{
		return ERR_NULL;
	}
	
	len = Check645Frame(recvbuff, rlen);
	if (len == ERROR)
	{
		return ERR_FAIL;
	}
	  
	memset(recvdata.address, 0, sizeof(recvdata.address));
	ParseMeteAddr(recvbuff+index, recvdata.address);
	index += METEADDR_LEN;
	
	index++;
	recvdata.ctrcode = recvbuff[index++];	
	recvdata.datalen = recvbuff[index++];
	
	switch(recvdata.ctrcode)
	{
		case C_READ_OK:
		{
			if (recvdata.datalen == 0)
			{
				return 0;
			}
			result = DI_BYTES;
			break;
		}
		case C_READ97_OK:
		{
			result = DI_BYTES;
			break;
		}
		case C_READ07_OK:
		{
			result = DI07_BYTES;
			break;
		}
		case C_SET07_OK:
		{
			result = DI07_BYTES;
			break;
		}
		case C_SET07_ERR:
		case C_READ_ERR:
		case C_READ97_ERR:
			return ERR_DENY;
		case C_READ07_ERR:
			result = 0xff;
			break;
		default:
			result = 0;
			break;
	}
	
	if (result == DI07_BYTES || result == 0xff)
	{
		recvdata.dataitm = recvbuff[index++] - DATA645DEAL;
		recvdata.dataitm += ((U8)(recvbuff[index++] - DATA645DEAL) << 8);	
		recvdata.dataitm += ((U8)(recvbuff[index++] - DATA645DEAL) << 16);	
		recvdata.dataitm += ((U8)(recvbuff[index++] - DATA645DEAL) << 24);			
	}
	else if (result == DI_BYTES)
	{
		recvdata.dataitm = recvbuff[index++] - DATA645DEAL;
		recvdata.dataitm += ((U8)(recvbuff[index++] - DATA645DEAL) * 0x100);	
	}
	else
	{
		recvdata.dataitm = recvdata.ctrcode + 0x80;
		recvdata.dataitm += ((recvdata.ctrcode) * 0x100);	
	}
	
	if (result == 0xff)
	{
		if (recvdata.dataitm == DI_LOGIN_QH && recvdata.ctrcode == C_READ07_ERR)
		{
			return ERR_LOGIN;								
		}
		return ERR_DENY;
	}
	
	memset(recvdata.userbuf, 0, sizeof(recvdata.userbuf));
	memcpy(recvdata.userbuf, recvbuff+index, recvdata.datalen - result);
	
	for (i = 0; i < recvdata.datalen - result; i++)
	{
		recvdata.userbuf[i] -= DATA645DEAL;
	}
	
	result = Get645ParamIndex(recvdata.dataitm);
	if (result == ERROR)
	{
		return ERR_PARSE;
	}
	
	/*if (recvdata.dataitm == DI_METERECORD_JS)
	{
		result = ParseJSMeteRecord(&recvdata);
		return result;
	}
	if (recvdata.dataitm == DI_PARAMEVENT_QH)
	{
		result = ParseQHParamEvent(&recvdata, result);
		return result;
	}
	if (recvdata.dataitm == DI_READFAIL_QH)
	{
		result = ParseQHEvent(&recvdata, result);
		return result;
	}*/
	
	if (recvdata.ctrcode == C_READ_OK)
	{
		result = Parse645Param(&recvdata, result);
	}
	else
	{
		result = Parse645Data(&recvdata, result);
	}
	 
	if (0 == result)
	{
		return 0;
	}
	else
	{
		return ERR_PARSE;
	}
	
}


/******************************************************************************/
/*                          698.41/376.1协议参数解析函数                      */ 
/*	功能: 解析补齐number类型参数块           																	*/
/*	参数: paramindex——参数索引 pItemIndex——参数子项索引  number——参数块个数		*/
/*	返回: 成功返回1，异常返回0                                           		  */
/******************************************************************************/
U8 FillParamItem(int paramindex, int pItemIndex, int number)
{
	int i = 0;
	int j = 0;
	unsigned char index = 0;
	unsigned char count = 0;
	
	if (number < 1)
	{
		(GlobalParam+paramindex)->Param_Num = pItemIndex;
		return 0;
	}
	
	count = (GlobalParam+paramindex)->Param_Num - pItemIndex;
	index = (GlobalParam+paramindex)->Param_Num;

	for (i = 1; i < number; i++)
	{
		for (j = 0; j < count; j++)
		{	
			memcpy(&(GlobalParamItem[index]), &(GlobalParamItem[pItemIndex+j]), sizeof(Stru_ParamItem));
			index++;
		}
	}
	
	(GlobalParam+paramindex)->Param_Num = index;
	return 1;	
}


/******************************************************************************/
/*                          698.41/376.1协议参数解析函数                      */ 
/*	功能: 获取参数内容并保存至全局数组GlobalParamItem                     		*/
/*	参数: recvdata——返回数据  paramindex——参数索引                    				*/
/*	返回: 成功返回1，异常返回-1                                           		*/
/******************************************************************************/
int Parse698Param(Stru698Data *recvdata, int paramindex)
{
	int i = 0;
	int k = 0;
	int index = 0;
	int len = 0;
	int vLen = 0;       //可变长度
	int flag = 0;
	unsigned int tmpint = 0;
	unsigned char temp[NUM_200] = {0};
	unsigned char conv[NUM_3] = {0};
		
	if ((GlobalParam+paramindex)->Param_ID != GlobalParamItem->Param_ID)
	{
		memset(GlobalParamItem, 0, sizeof(GlobalParamItem));
		GetParamValueFromDBF(GlobalParam+paramindex, GlobalParamItem, PARAMDBF_GW);
	}
	
	for (i = 0; i < (GlobalParam+paramindex)->Param_Num; i++)
	{
		len = (GlobalParamItem + i)->Param_Item_Clen;

		switch((GlobalParamItem + i)->Param_Item_Mask)
	  {
	  	case 'h':
	  	{
	  		memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->Userbuf[index]);
					strcat(temp, conv);
					index++;
				}
		  	ReverseStr(temp, 0);		
			  FormatString((GlobalParamItem+i)->Param_Item_Value, temp);	
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
	  		break;
	  	}
	  	case '5':
	  	{
	  		memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->Userbuf[index]);
					strcat(temp, conv);
					index++;
				}
		  	ReverseStr(temp, 0);		
			  FormatString((GlobalParamItem+i)->Param_Item_Value, temp);					
				if (temp[0] != '-' && temp[0] >= '8')
		  	{
		  		temp[0] = ((temp[0] - 0x30) & 0x07) + 0x30;
		  		sprintf((GlobalParamItem+i)->Param_Value, "-%s", temp);
		  	}
		  	else
		  	{
			  	sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
			  }
	  		break;
	  	}	  	
	  	case 'd':
	  	case 'l':
	  	{
	  		tmpint = 0;
				for (k = 0; k < len; k++)
				{
					tmpint += ((recvdata->Userbuf[index]) << (8 * k)); 
					index++;
				}
				vLen = (int)tmpint;
				sprintf((GlobalParamItem+i)->Param_Value, "%d", tmpint);
	  		break;
	  	}
	  	case 'e':
	  	{
	  		sprintf((GlobalParamItem+i)->Param_Value, "%x", recvdata->Userbuf[index]);
				index++;
	  		break;
	  	}
	  	case 'a':
	  	{
	  		k = 0;
				if (len == 0)
				{
					len = vLen;
					vLen = 0;
				}
				while(k < len)
				{
					if (recvdata->Userbuf[index] != 0)
					{
						break;
					}
					index++;
					k++;
				}
				memset(temp, 0, sizeof(temp));
				len -= k;
				memcpy(temp, recvdata->Userbuf+index, len);
				index += len;
				k = 0;
				while(k < len)
				{
					//(GlobalParamItem+i)->Param_Value[k] = temp[len - k - 1];
					(GlobalParamItem+i)->Param_Value[k] = temp[k];
					k++;
				}
				break;
	  	}
//	  	case 't':
//	  	{
//	  		memset(temp, 0, sizeof(temp));		
//				for (k = 0; k < len; k++)
//				{
//					temp[k] = recvdata->Userbuf[index + len - k - 1];
//				}
//				index += len;
//
//				FormatTimeStr((GlobalParamItem+i)->Param_Item_Value, temp, len);
//				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
//	  		break;
//	  	}
	  	case 'n':
	  	{
	  		tmpint = 0;
				for (k = 0; k < len; k++)
				{
					tmpint += ((recvdata->Userbuf[index]) << (8 * k)); 
					index++;
				}
				sprintf((GlobalParamItem+i)->Param_Value, "%d", tmpint);
				if (!FillParamItem(paramindex, i + 1, tmpint))
				{
					return 1;
				}										
	  		break;
	  	}
	  	case 'i':
	  	{		
	  		if (len != NUM_4)
	  		{
	  			return ERROR;
	  		}
	  		memset(temp, 0, sizeof(temp));
				memcpy(temp, recvdata->Userbuf+index, len);
				index += len;
				sprintf((GlobalParamItem+i)->Param_Value, "%d.%d.%d.%d", 
				        temp[0], temp[1], temp[2], temp[3]);		
				break;		
	  	}
	  	case 's':
	  	{
	  		if ((i) && ((GlobalParamItem + i - 1)->Param_Item_Mask == 'l'))
				{
					len = len * vLen;
					vLen = 0;
					memset((GlobalParamItem+i)->Param_Value, 0, 
									strlen((GlobalParamItem+i)->Param_Value));
				}
	  		memset(temp, 0, sizeof(temp));	  					
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->Userbuf[index]);
					strcat(temp, conv);
					index++;
				}
		  	//ReverseStr(temp, 0);				  	
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);				
	  		break;
	  	}
	  	default:
	  	{
	  		memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->Userbuf[index]);
					strcat(temp, conv);
					index++;
				}
		  	ReverseStr(temp, 0);				  	
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);				
	  		break;
	  	}	  	
	  }
	}

	return 1;	
}


/******************************************************************************/
/*                          698.41/376.1协议测量点数据解析函数                */ 
/*	功能: 获取参数内容并保存至全局数组GlobalParamItem                     		*/
/*	参数: recvdata——返回数据  paramindex——参数索引                    				*/
/*	返回: 成功返回1，异常返回-1                                           		*/
/******************************************************************************/
int Parse698Data(Stru698Data *recvdata, int paramindex)
{
	int i = 0;
	int k = 0;
	int index = 0;
	int len = 0;
	int vLen = 0;       //可变长度
	int flag = 0;
	unsigned int tmpint = 0;
	unsigned char temp[NUM_50] = {0};
	unsigned char conv[NUM_3] = {0};
		
	if ((GlobalParam+paramindex)->Param_ID != GlobalParamItem->Param_ID)
	{
		memset(GlobalParamItem, 0, sizeof(GlobalParamItem));
		GetParamValueFromDBF(GlobalParam+paramindex, GlobalParamItem, PARAMDBF_GW);
	}
		
	if ((recvdata->AFN == AFN_698RTDATA && recvdata->Fn == FN_129)
		||(recvdata->AFN == AFN_698HISDATA && recvdata->Fn == FN_161))
	{
		ParseMPData(recvdata);		
	}
		
	index += 4;
	for (i = 0; i < (GlobalParam+paramindex)->Param_Num; i++)
	{
		len = (GlobalParamItem + i)->Param_Item_Clen;
		if (len > recvdata->Ulen - index)
		{
			break;
		}

		switch((GlobalParamItem + i)->Param_Item_Mask)
	  {
	  	case 'h':
	  	{
	  		memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->Userbuf[index]);
					strcat(temp, conv);
					index++;
				}
		  	ReverseStr(temp, 0);		
			  FormatString((GlobalParamItem+i)->Param_Item_Value, temp);	
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
	  		break;
	  	}
	  	case '5':
	  	{
	  		memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->Userbuf[index]);
					strcat(temp, conv);
					index++;
				}
		  	ReverseStr(temp, 0);		
			  FormatString((GlobalParamItem+i)->Param_Item_Value, temp);					
				if (temp[0] != '-' && temp[0] >= '8')
		  	{
		  		temp[0] = ((temp[0] - 0x30) & 0x07) + 0x30;
		  		sprintf((GlobalParamItem+i)->Param_Value, "-%s", temp);
		  	}
		  	else
		  	{
			  	sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
			  }
	  		break;
	  	}	  	
	  	case 'd':
	  	case 'l':
	  	{
	  		tmpint = 0;
				for (k = 0; k < len; k++)
				{
					tmpint += ((recvdata->Userbuf[index]) << (8 * k)); 
					index++;
				}
				vLen = (int)tmpint;
				sprintf((GlobalParamItem+i)->Param_Value, "%d", tmpint);
	  		break;
	  	}
	  	case 'e':
	  	{
	  		sprintf((GlobalParamItem+i)->Param_Value, "%x", recvdata->Userbuf[index]);
				index++;
	  		break;
	  	}
	  	case 'a':
	  	{
	  		k = 0;
				if (len == 0)
				{
					len = vLen;
				}
				while(k < len)
				{
					if (recvdata->Userbuf[index] != 0)
					{
						break;
					}
					index++;
					k++;
				}
				memset(temp, 0, sizeof(temp));
				len -= k;
				memcpy(temp, recvdata->Userbuf+index, len);
				index += len;
				k = 0;
				while(k < len)
				{
					//(GlobalParamItem+i)->Param_Value[k] = temp[len - k - 1];
					(GlobalParamItem+i)->Param_Value[k] = temp[k];
					k++;
				}
				break;
	  	}
//	  	case 't':
//	  	{
//	  		memset(temp, 0, sizeof(temp));		
//				for (k = 0; k < len; k++)
//				{
//					temp[k] = recvdata->Userbuf[index + len - k - 1];
//				}
//				index += len;
//
//				FormatTimeStr((GlobalParamItem+i)->Param_Item_Value, temp, len);
//				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
//	  		break;
//	  	}
	  	case 'n':
	  	{
	  		tmpint = 0;
				for (k = 0; k < len; k++)
				{
					tmpint += ((recvdata->Userbuf[index]) << (8 * k)); 
					index++;
				}
				sprintf((GlobalParamItem+i)->Param_Value, "%d", tmpint);
				break;	
	  	}
	  	case 'i':
	  	{		
	  		if (len != NUM_4)
	  		{
	  			return ERROR;
	  		}
	  		memset(temp, 0, sizeof(temp));
				memcpy(temp, recvdata->Userbuf+index, len);
				index += len;
				sprintf((GlobalParamItem+i)->Param_Value, "%d.%d.%d.%d", 
				        temp[0], temp[1], temp[2], temp[3]);		
				break;		
	  	}
	  	case 's':
	  	{
	  		memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->Userbuf[index]);
					strcat(temp, conv);
					index++;
				}
		  	ReverseStr(temp, 0);		
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
	  		break;
	  	}
	  	default:
	  	{
	  		memset(temp, 0, sizeof(temp));			
				for (k = 0; k < len; k++)
				{
					memset(conv, 0, sizeof(conv));
					sprintf(conv, "%02x", recvdata->Userbuf[index]);
					strcat(temp, conv);
					index++;
				}
		  	ReverseStr(temp, 0);		
				sprintf((GlobalParamItem+i)->Param_Value, "%s", temp);
	  		break;
	  	}
	  }
	}
		
	return 1;	
}


/******************************************************************************/
/*                         698.41/376.1协议帧解析函数                         */ 
/*	功能: 解析返回帧                                                      		*/
/*	参数: recvbuff——返回数据帧  len——返回数据帧长度                   				*/
/*	返回: 成功返回1，异常返回错误编码                                     		*/
/******************************************************************************/
int Parse698Buff(U8 recvbuff[], U16 len)
{

  int result = 0;
  int index = 0;
  int paramindex = 0;  
  Stru698Data Recv698Data; 
  
  if (!len)
  {
  	return 0;
  }
  
  result = Check698Buff(recvbuff, len);
  if (result < 1)
  {
  	return 3;
  }
  
  index = 1;
  memcpy(Recv698Data.Saddr, recvbuff + index, ADDRBYTES);
  index += ADDRBYTES;
  Recv698Data.AFN = recvbuff[index++];
  Recv698Data.SEQ = recvbuff[index++];
  Recv698Data.Pn = ParsePn(recvbuff[index], recvbuff[index+1]);
  index += PNBYTES;
  Recv698Data.Fn = ParseFn(recvbuff[index], recvbuff[index+1]);
  index += FNBYTES;

  if (Recv698Data.AFN == 0)
  {
    return Recv698Data.Fn;  
  }
    
  if (Recv698Data.Pn != 0)
  {
  	index = index - PNBYTES - PNBYTES;
  }
  
  memset(Recv698Data.Userbuf, 0, sizeof(Recv698Data.Userbuf));
  Recv698Data.Ulen = len - index - CSBYTES - ENDBYTES;
  memcpy(Recv698Data.Userbuf, recvbuff + index, Recv698Data.Ulen);
  
  //AFN = 0x0A, Fn = 10 电表档案解析
  if (Recv698Data.AFN == AFN_698READ && Recv698Data.Fn == FN_RECD)
  {
    result = ParseAFN0AF10(&Recv698Data);
    return result;
  }//AFN = 0x10, Fn = 1 透抄电表命令解析
  else if (Recv698Data.AFN == AFN_698FWDCMD && Recv698Data.Fn == 1)
  {
  	result = ParseAFN10F01(&Recv698Data);
  	return result;  	
  }
    
  paramindex = GetParamIndex(&Recv698Data);

  if (paramindex == ERROR)
  {
  	return 0;
  }
    
  if (Recv698Data.Pn == 0)
  {
  	result = Parse698Param(&Recv698Data, paramindex);
  }
  else
  {
	  result = Parse698Data(&Recv698Data, paramindex);
	}

  return result; 
}
#endif
