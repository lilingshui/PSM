#ifndef _Set_Addr_Time_H
#define _Set_Addr_Time_H

#include "jblib.h"
#include "Common.h"
#include "Global.h"
#include "DataEncap.h"
#include "DataParse.h"
#include "FormSelecte.h"
#include "DataConvert.h"
  
Window Form_SetAddrTime;  
  
int SetTermAddr(unsigned char term[],int index)
{
  int len = 0;
  int result = 0;
  unsigned char buff[NUM_255] = {0};
  unsigned char termaddr[NUM_10] = {0};
  unsigned char Fn[NUM_10] = {0};
  unsigned char afn[NUM_6] = {0}; 
  
  Stru698Data Send698Data;
  
  if (MessageBox("确定要设置地址吗？", 
    STR_CUE, MB_YESNO) == ID_NO)
  {
    return result;
  } 
  
  Send698Data.Ccode = 0x4A;
  Send698Data.AFN = 0x04;
  Send698Data.SEQ = 0xE8;
  Send698Data.Pn = 0;
  if(index ==1)
  {
     Send698Data.Fn = 85;
  }
  else if(index==5)
  {
     Send698Data.Fn = 89;
  }
  
  Send698Data.Ulen = 0; 
  
  memset(Send698Data.Userbuf, 0, sizeof(Send698Data.Userbuf));
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));  
  GetAddr(Send698Data.Saddr, term);
  
  memcpy(Send698Data.Userbuf, Send698Data.Saddr, NUM_4);
  Send698Data.Ulen += NUM_4;
  
  Send698Data.Saddr[2] = 0xFF;
  Send698Data.Saddr[3] = 0xFF;

  len = Build698SendBuff(buff, &Send698Data);
  
  
  SendData(buff, len);
  
  memset(buff, 0, sizeof(buff));
  len = RecvChar(buff, sizeof(buff), NUM_600);
  Beep(100);
  
  result = Check698Buff(buff, len);
  HexToStr(&afn,&buff[NUM_6],1,0,0);
  if (result > NUM_14 && strcmp(afn,"00")==0)
  {
  	
  	HexToStr(&Fn,&buff[NUM_8],4,0,0);
  	if(strcmp(Fn,"00000200")==0)
  	{
  		result = -2;
  	}
  	else if(strcmp(Fn,"00000100")==0)
  	{
  		result = 1;
  	}
  	else
  	{
  		result = -3;
  	}
  }
  
  return result;
  
}


int SetTermAddrWS(unsigned char term[])
{
  int len = 0;
  int i = 0;
  int result = 0;
  unsigned char buff[NUM_100] = {0};
  Stru645Data senddata;
  
  if (MessageBox("确定要设置地址吗？", 
    STR_CUE, MB_YESNO) == ID_NO)
  {
    return result;
  }

	senddata.protype = 1;	
	memset(senddata.address, 0, sizeof(senddata.address));
	sprintf(senddata.address, "F19999999999");
	
  senddata.ctrcode = 4;	
  senddata.dataitm = 0xD322;
	senddata.datalen = 2;
  
  memset(buff, 0, sizeof(buff));  
  GetAddr(buff, term);
  
  memset(senddata.userbuf, 0, sizeof(senddata.userbuf));
  memcpy(senddata.userbuf, buff, NUM_4);
  
  senddata.datalen += NUM_4;
  
  memset(buff, 0, sizeof(buff));
  len = Build645SendBuff(buff, &senddata);
  
  for (i = 0; i < len; i++)
  {
  	if (buff[i] != BUFF_FRONT)
  	{  	
  		break;
  	}
  }
  
  SendData(&buff[i], len - i);
  
  memset(buff, 0, sizeof(buff));
  len = RecvChar(buff, sizeof(buff), NUM_600);
  Beep(100);
  
  result = Check645Frame(buff, len);  
  
  if (result >= NUM_12)
  {
  	if (buff[NUM_8] != 0x84)
  	{
  		result = -1;
  	}
  	else
  	{
  		result = 1;
  	}  	
  }  
  return result;
}


int SetTermTime(unsigned char term[])
{
  int len = 0;
  int i = 0;
  int result = 0;
  unsigned char buff[NUM_255] = {0};
  Stru698Data Send698Data;
  U32 tmpTime;
  U32 tmpDate;
  U8 tmpWeek;
  
  if (MessageBox("确定要广播对时吗？", 
    STR_CUE, MB_YESNO) == ID_NO)
  {
    return result;
  }
    
  Send698Data.Ccode = 0x41;
  Send698Data.AFN = 0x05;
  Send698Data.SEQ = 0xE8;
  Send698Data.Pn = 0;
  Send698Data.Fn = 31;
  Send698Data.Ulen = 0; 
  
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));  
  GetAddr(Send698Data.Saddr, term);
  
  GetTime(&tmpTime);
  GetDate(&tmpDate);
  memset(Send698Data.Userbuf, 0, sizeof(Send698Data.Userbuf));
  for (i = 0; i < 3; i++)
  {
  	Send698Data.Userbuf[Send698Data.Ulen++] = (U8)(tmpTime >> 8 * i);
  }
  Send698Data.Userbuf[Send698Data.Ulen++] = (U8)(tmpDate);
  tmpWeek = (U8)(tmpDate >> 24);
  if (tmpWeek == 0)
  {
  	tmpWeek = 7;
  }
  tmpWeek = (U8)(tmpWeek << 5);
  
  Send698Data.Userbuf[Send698Data.Ulen++] = (U8)((U8)(tmpDate >> 8) + tmpWeek);
  Send698Data.Userbuf[Send698Data.Ulen++] = (U8)(tmpDate >> 16);

  
  Send698Data.Saddr[2] = 0xFF;
  Send698Data.Saddr[3] = 0xFF;

  len = Build698SendBuff(buff, &Send698Data);
  
  SendData(buff, len);
  
  memset(buff, 0, sizeof(buff));
  len = RecvChar(buff, sizeof(buff), NUM_600);
  Beep(100);
  
	result = Check698Buff(buff, len);  
	
	if (result > 14)
  {
  	result = 1;  	
  }
  return result;
}


int SetTermTimeWS(unsigned char term[])
{
  int len = 0;
  int i = 0;
  int result = 0;
  unsigned char buff[NUM_100] = {0};
  Stru645Data senddata;
  U32 tmpTime;
  U32 tmpDate;
  
  if (MessageBox("确定要强制对时吗？", 
    STR_CUE, MB_YESNO) == ID_NO)
  {
    return result;
  }
  
	senddata.protype = 1;	
	memset(senddata.address, 0, sizeof(senddata.address));
	sprintf(senddata.address, "F19999999999");
	
  senddata.ctrcode = 4;	
  senddata.dataitm = 0xD323;
	senddata.datalen = 0;
  
  memset(buff, 0, sizeof(buff));  
  GetAddr(buff, term);
  

  GetTime(&tmpTime);
  GetDate(&tmpDate);
  memset(senddata.userbuf, 0, sizeof(senddata.userbuf));
  for (i = 0; i < 3; i++)
  {
  	senddata.userbuf[senddata.datalen++] = (U8)(tmpTime >> 8 * i);
  }
  for (i = 0; i < 3; i++)
  {
  	senddata.userbuf[senddata.datalen++] = (U8)(tmpDate >> 8 * i);
  }
  senddata.datalen += 2;
  
  memset(buff, 0, sizeof(buff));
  len = Build645SendBuff(buff, &senddata);
  for (i = 0; i < len; i++)
  {
  	if (buff[i] != BUFF_FRONT)
  	{
  		break;
  	}
  }
  
  SendData(&buff[i], len - i);
  
  memset(buff, 0, sizeof(buff));
  len = RecvChar(buff, sizeof(buff), NUM_600);
  Beep(100);
  
  result = Check645Frame(buff, len); 

  if (result >= NUM_12)
  {
  	if (buff[NUM_8] != 0x84)
  	{
  		result = -1;
  	}
  	else
  	{
  		result = 1;
  	}
  }
  return result;
      
}


void Menu_SetAddrTime_OnClick(HWND Sender)
{
	int index = 0;
	
	index = ((PTMenu)Sender)->index;
	CurrMPAttrib.Flag = index + 1;
		
	CloseWindow(&Form_SetAddrTime, ID_OK);				
}


int SetAddrTime()
{
                          
  TMenu Menu_SetAddrTime;  
              
   TMenuItem  itemsSetAddrTime[]={
    {NULL,NULL,"1设置地址(09)"},
    {NULL,NULL,"2强制设置"},
    {NULL,NULL,"3广播对时"},
    {NULL,NULL,"4强制对时"},
    {NULL,NULL,"5设置地址(12)"},
                                                          
    };              
  
  CreateWindow(&Form_SetAddrTime,"",0,0,95,77);
  Form_SetAddrTime.style &= ~WS_CAPTION;
  
  CreateTMenu(&Menu_SetAddrTime, itemsSetAddrTime,
  sizeof(itemsSetAddrTime)/sizeof(TMenuItem),0,0,94,76,FONT_CH_12|FONT_EN_12);
  Menu_SetAddrTime.OnClick = Menu_SetAddrTime_OnClick;
 
  AddToWindow(&Form_SetAddrTime, &Menu_SetAddrTime);                             
                  
  CurrMPAttrib.Flag = 0;
  
  if (ShowWindow(&Form_SetAddrTime, SS_SHOWCENTER) == ID_CANCEL)
  {
  	return ID_CANCEL;
  } 
  
  return CurrMPAttrib.Flag;
  
}

#endif
