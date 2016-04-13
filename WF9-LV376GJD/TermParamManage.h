#ifndef _TermParamManage_H
#define _TermParamManage_H

#include "jblib.h"
#include "Common.h"
#include "Global.h"
#include "FormSelecte.h"
#include "DataConvert.h"
#include "InputBoxGroupPro.h"
#include "ShowData.h"
#include "DBFOperate.h"
#include "DataPro.h"
#include "InputParamPro.h"

Text Text_SearchMeteRes;

void TermOperateSetParam(HWND Sender);
void TermOperateReadParam(HWND Sender);
void SetTermParam(HWND Sender);
void ReadTermParam(HWND Sender);


void ShowSearchMeterRes(Stru_Param *param, Stru_ParamItem *pItem)
{	
	int i = 0;
	int j = 0;
	int count = 0;
	int mpNo = 0;
	char show[NUM_400] = {0};
	char temp[NUM_100] = {0};
	
	sprintf(temp, "%s", pItem->Param_Value);
	StrToHexStr(temp, pItem->Param_Item_Clen, 1, 0, 0);

  if (fileexits(TEMPPARAMTXT) != FSNULL)
  {
    filedelete(TEMPPARAMTXT);
  }
  
  for (i = 0; i < pItem->Param_Item_Clen; i++)
  {
   	memset(show, 0, sizeof(show));
  	for (j = 0; j < 8; j++)
  	{
  		mpNo = i * 8 + j + 1;  		
  		if (((temp[i] >> j) & 0x01) == 1)
  		{
  			sprintf(show+strlen(show), " 测量点%03d： 成功\r\n", mpNo);
  			count++; 			
  		}
  		else
  		{
  			sprintf(show+strlen(show), " 测量点%03d： 失败\r\n", mpNo);
  		}
  	}
  	WritDubugInfo(TEMPPARAMTXT, show, strlen(show), 0xff);
  }
  
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "搜索成功表数 %d", count);
  ShowData(temp, TEMPPARAMTXT);
}


int ShowTermParam(Stru_Param *param, Stru_ParamItem *pItem)
{
	int i = 0;
	char show[1024] = {0};
	char temp[128] = {0};
	
	if (param->Param_ID != pItem->Param_ID)
	{
		return;
	}
	
	//集中器搜表结果
	if (param->Fn == FN_9 && param->AFN == AFN_698CONF)
	{
		ShowSearchMeterRes(param, pItem);
		return;
	}
	
	if (param->Param_Num == 6 && param->Fn == 1 && param->AFN == AFN_698READ)
	{
		memset((pItem+5)->Param_Value, 0, strlen((pItem+5)->Param_Value));
		sprintf((pItem+5)->Param_Value, "%s", (pItem+3)->Param_Value);
		
		U16 tmpint = StrToInt((pItem+2)->Param_Value, DEC);
		memset((pItem+2)->Param_Value, 0, strlen((pItem+2)->Param_Value));
		memset((pItem+3)->Param_Value, 0, strlen((pItem+3)->Param_Value));
		sprintf((pItem+2)->Param_Value, "%d", tmpint & 0x0fff);
		sprintf((pItem+3)->Param_Value, "%x", (tmpint >> 12) &0x03);	 	
	}
	
 	memset(show, 0, sizeof(show));
  for (i = 0; i < param->Param_Num; i++)
  {
  	strcat(show, (pItem+i)->Param_Item_Name);
  	strcat(show, ":");
  	if ((pItem+i)->Param_Item_Mask == 'e')
  	{
  		memset(temp, 0, sizeof(temp));
  		FormatEnumData(pItem+i, temp);
  		strcat(show, temp);
  	}
  	else if ((pItem+i)->Param_Item_Mask == 's')
  	{
  		memset(temp, 0, sizeof(temp));
  		FormatBSData(param, pItem+i, temp);
  		strcat(show, temp);
  	}
  	else if ((pItem+i)->Param_Item_Mask == 't')
  	{
  		memset(temp, 0, sizeof(temp));
  		sprintf(temp, "%s", (pItem+i)->Param_Value);
  		StrToHexStr(temp, strlen(temp) / 2, 0, 0, 0);
			FormatTimeStr((pItem+i)->Param_Item_Value, temp, (pItem+i)->Param_Item_Clen);
			strcat(show, temp);  		
  	}
  	else
  	{
	  	strcat(show, (pItem+i)->Param_Value);
	  }
		strcat(show, "\r\n"); 	
  }
  
  if (fileexits(TEMPPARAMTXT) != FSNULL)
  {
    filedelete(TEMPPARAMTXT);
  }
  
  WritDubugInfo(TEMPPARAMTXT, show, strlen(show), 0xff);
  
  ShowData(STR_RES_READ, TEMPPARAMTXT);
  
}

void TermOperateSetParam(HWND Sender)
{
  Window Form_SetTermParam;
                              
  TMenu SetTermParamMenu;  
  TMenuItem  SetTermParamItems[MAX_PARAMITEM_NUM];
  int i = 0;
  int menuCount = 0;
  
  memset(GlobalParam, 0, sizeof(GlobalParam));
  menuCount = GetParamItemFromDBF(GlobalParam, TYPE_SET, PARAMDBF_GW);
  if (menuCount < 1)
  {
  	MessageBox("菜单加载失败!", "提示", MB_OK);
  	return;
  } 
  
  for (i = 0; i < menuCount; i++)
  {
  	(SetTermParamItems[i]).icon = NULL;
  	(SetTermParamItems[i]).eventproc = NULL;
  	memset((SetTermParamItems[i]).text, 0, sizeof((SetTermParamItems[i]).text));
  	sprintf((SetTermParamItems[i]).text, "%d.%s", i + 1, (GlobalParam+i)->Param_Name);
  }                                       
  
  CreateWindow(&Form_SetTermParam,"终端参数设置",0,0,160,160);
  sprintf(Form_SetTermParam.text, "终端%s参数设置", CurrMPAttrib.Term);
  
  CreateTMenu(&SetTermParamMenu, SetTermParamItems, menuCount,10,10,140,110,FONT_CH_12|FONT_EN_12);  
  SetTermParamMenu.OnClick = SetTermParam;
  SetTermParamMenu.style |= WS_PAGEENABLE;
 
  AddToWindow(&Form_SetTermParam, &SetTermParamMenu);                             
                   
  ShowWindow(&Form_SetTermParam,SS_SHOWMAX);            
}


void TermOperateReadParam(HWND Sender)
{
  Window Form_ReadTermParam;
                              
  TMenu ReadTermParamMenu;  
  TMenuItem  ReadTermParamItems[MAX_PARAMITEM_NUM];
  int i = 0;
  int menuCount = 0;
  
  memset(GlobalParam, 0, sizeof(GlobalParam));
  menuCount = GetParamItemFromDBF(GlobalParam, TYPE_READ, PARAMDBF_GW);
  if (menuCount < 1)
  {
  	MessageBox("菜单加载失败!", "提示", MB_OK);
  	return;
  } 
  
  for (i = 0; i < menuCount; i++)
  {
  	(ReadTermParamItems[i]).icon = NULL;
  	(ReadTermParamItems[i]).eventproc = NULL;
  	memset((ReadTermParamItems[i]).text, 0, sizeof((ReadTermParamItems[i]).text));
  	sprintf((ReadTermParamItems[i]).text, "%d.%s", i + 1, (GlobalParam+i)->Param_Name);  	
  }                                      
  
  CreateWindow(&Form_ReadTermParam,"终端参数查询",0,0,160,160);
  sprintf(Form_ReadTermParam.text, "终端%s参数查询", CurrMPAttrib.Term);
  
  CreateTMenu(&ReadTermParamMenu, ReadTermParamItems, menuCount,10,10,140,110,FONT_CH_12|FONT_EN_12);
  ReadTermParamMenu.style |= WS_PAGEENABLE;
  ReadTermParamMenu.OnClick = ReadTermParam;

  AddToWindow(&Form_ReadTermParam, &ReadTermParamMenu);                             
                  
  ShowWindow(&Form_ReadTermParam,SS_SHOWMAX);       
}


void SetTermParam(HWND Sender)
{
	
	int i = 0;
  int slen = 0;
  int rlen = 0;
  int paramindex = 0;

  char show[NUM_255] = {0};
  unsigned char sendbuff[MAXSENDBYTES] = {0};
  unsigned char recvbuff[MINRECVBYTES] = {0};

  Stru698Data Send698Data;
  
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, CurrMPAttrib.Term);
  
  paramindex = ((TMenu *)Sender)->index;
  
 (GlobalParam+paramindex)->Flag = 1;
  Send698Data.AFN = (GlobalParam+paramindex)->AFN;
  Send698Data.SEQ = INI_698SEQ;
  Send698Data.Pn = (GlobalParam+paramindex)->Pn;
  Send698Data.Fn = (GlobalParam+paramindex)->Fn;
  Send698Data.Ulen = 0; 
  if (Send698Data.AFN == AFN_698READ)
  {
  	Send698Data.AFN = AFN_698SET;
  	Send698Data.Ccode = C_698SET;
	}
	else
	{
		Send698Data.Ccode = C_698REBOOT;
	}
	  
  memset(GlobalParamItem, 0, sizeof(GlobalParamItem));
  if (0 == GetParamValueFromDBF(GlobalParam+paramindex, GlobalParamItem, PARAMDBF_GW))
  {
  	memset(show, 0, sizeof(show));
		sprintf(show, "确定要执行%s操作吗?", (GlobalParam+paramindex)->Param_Name);
		if (MessageBox(show, STR_CUE, MB_OKCANCEL) == ID_CANCEL)
		{
			return;
		}
  }
  else
  {
  	memset(sendbuff, 0, sizeof(sendbuff));
  	slen = InputParam(paramindex, sendbuff);  
  	if (slen == 0)
  	{
  		return;
  	}
  	Send698Data.Ulen = slen;
  	memcpy(Send698Data.Userbuf, sendbuff, slen);
  	
  	SetParamValueToDBF(GlobalParam+paramindex, GlobalParamItem, PARAMDBF_GW);
  }
    
  memset(sendbuff, 0, sizeof(sendbuff));
  slen = Build698SendBuff(sendbuff, &Send698Data);
  
  uartinit(CommParam.Baud, CommParam.Parity, CommParam.Port);
  Delay(DELAY_OPENPORT);
  
  SendData(sendbuff, slen);
  
  if (CommParam.Baud < BAUD_38400)
  {     
    rlen = RecvChar(recvbuff, sizeof(recvbuff), NUM_600);
  }
  else
  {
    rlen = rbuf(recvbuff, sizeof(recvbuff), NUM_600);
  }
  
  Beep(100);  
  uartinit(0, CommParam.Parity, CommParam.Port); 

  rlen = Parse698Buff(recvbuff, rlen);
  (GlobalParam+paramindex)->Flag = 0;
  MessageBox(ConstResStr[rlen], STR_CUE, MB_OK);        
}


//测量点参数查询
void ReadTermParam(HWND Sender)
{
	int i = 0;
  int rlen = 0;
  int slen = 0;
  int paramindex = 0;

  unsigned char temp[NUM_10] = {0};
  unsigned char sendbuff[MINSENDBYTES] = {0};
  unsigned char recvbuff[MAXRECVBYTES] = {0};

  Stru698Data Send698Data;
  
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, CurrMPAttrib.Term);
  
  paramindex = ((TMenu *)Sender)->index;
  (GlobalParam+paramindex)->Flag = 1;
  Send698Data.AFN = (GlobalParam+paramindex)->AFN;
  Send698Data.SEQ = INI_698SEQ;
  Send698Data.Pn = (GlobalParam+paramindex)->Pn;
  Send698Data.Fn = (GlobalParam+paramindex)->Fn;
  Send698Data.Ulen = 0; 
 
  Send698Data.Ccode = C_698READ;
  memset(GlobalParamItem, 0, sizeof(GlobalParamItem));
  GetParamValueFromDBF(GlobalParam+paramindex, GlobalParamItem, PARAMDBF_GW);
  
  if (Send698Data.Fn == FN_85 && Send698Data.AFN == AFN_698READ)
  {
	  Send698Data.Saddr[2] = 0xFF;
	  Send698Data.Saddr[3] = 0xFF;
  }
  
  if (Send698Data.Pn == 1)
  {
  	ImmNextIme(IME_ID_123);
	  if (InputBox("测量点", "请输入测量点号", temp, 4) == ID_CANCEL)
	  {
	    return;
	  }
	  Send698Data.Pn = StrToInt(temp, DEC);
	  if (Send698Data.Pn < 1 || Send698Data.Pn > MAX_MPNO)
	  {
	  	Send698Data.Pn = 1;
	  }
  }
  
  if (Send698Data.Fn == FN_33 && Send698Data.AFN == AFN_698READ)
  {
  	ImmNextIme(IME_ID_123);
  	strcpy(temp, "31");
	  if (InputBox("范围[1, 31]", "请输入端口", temp, 2) == ID_CANCEL)
	  {
	    return;
	  }
	  
	  Send698Data.Userbuf[Send698Data.Ulen++] = 1;
	  Send698Data.Userbuf[Send698Data.Ulen] = StrToInt(temp, DEC);
	  
	  if (Send698Data.Userbuf[Send698Data.Ulen] < 1 
	  	|| Send698Data.Userbuf[Send698Data.Ulen] > 31)
	  {
	  	Send698Data.Userbuf[Send698Data.Ulen] = 31;
	  }
	  
	  Send698Data.Ulen++; 	  
  }  
  
  //二类数据
  if (Send698Data.AFN == AFN_698HISDATA)
  {
  	//unsigned char v_date[NUM_7] = {0};
  	unsigned int readDate = 0;
  	
	  /*if (InputBox("格式:YYMMDD", "请输入冻结日期", v_date, 6) == ID_CANCEL)
	  {
	  	return;
	  }
	  readDate = StrToInt(v_date, HEX);
	  if (0 == readDate)
	  {
	  	readDate = GetHisDataDate(readDate);
	  }	*/
	  readDate = GetHisDataDate(readDate);
	  if (!readDate)
	  {
	  	return;
	  }
	  
	  Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)readDate;
    Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)(readDate >> 8);
    Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)(readDate >> 16); 
  }
  
  slen = Build698SendBuff(sendbuff, &Send698Data);
  
  uartinit(CommParam.Baud, CommParam.Parity, CommParam.Port);
  Delay(DELAY_OPENPORT);
  
  SendData(sendbuff, slen);
  
  rlen = RecvChar(recvbuff, sizeof(recvbuff), 1000);
  
  Beep(100);
  
  uartinit(0, CommParam.Parity, CommParam.Port);
     
  rlen = Parse698Buff(recvbuff, rlen);
  if (rlen != 1)
  {
    MessageBox(ConstResStr[rlen], STR_CUE, MB_OK);      
    return;  
  } 
        
  ShowTermParam(GlobalParam + paramindex, GlobalParamItem);
  
  if ((GlobalParam + paramindex)->Type == TYPE_READSET)
  {
  	if (MessageBox("是否同步保存参数?", STR_CUE, MB_YESNO) == ID_NO)
  	{
  		return;
  	}
  	SetParamValueToDBF(GlobalParam + paramindex, GlobalParamItem, PARAMDBF_GW);
  }
  (GlobalParam+paramindex)->Flag = 0;

}
#endif
