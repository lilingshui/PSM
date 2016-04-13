#ifndef _SendMeterRecord_H
#define _SendMeterRecord_H

#include "jblib.h"
#include "Common.h"
#include "FormProgress.h"
#include "InputBoxPro.h"
#include "DataConvert.h"
#include "DataEncap.h"
#include "DataParse.h"
#include "AttributeMeter.h"


unsigned char *SendMeterRecordTerminal;
ListBox ListBox_SendMeterRecord;
int ReadMeterRecordNum;
int ReadMeterRecordIndex;

Text ReadRecordText_MPNo, ReadRecordText_Baud, ReadRecordText_Port,
  ReadRecordText_Proto, ReadRecordText_MeteAddr, ReadRecordText_ColAddr; 

void SendMeterRecordLoad();
void SendMeterRecordDoLoad(HWND Obj,PMsg Msg);
void ListBox_SendMeterRecord_OnMsg(HWND Obj, PMsg Msg);
void SendMeterRecordView(HWND Sender);
void SendMeterRecordGoto(HWND Sender);
void SendMeterRecordDo(HWND Sender);
void SendMeterRecordReset(HWND Sender);
void SendMeterRecordHelp(HWND Sender);
void ReadMeterRecord(HWND Sender);
void MeterRecordInfoLoad(HWND Sender);
void MeterRecordInfoDoLoad(HWND Obj, PMsg Msg);
void Form_MeterRecordInfo_OnKeyPress(HWND Obj, PMsg Msg);

void ShowReadRecordResult(HWND Sender);
void Form_ReadRecordResult_OnKeyPress(HWND Obj, PMsg Msg);
void Form_ReadRecordResult_Refresh(HWND Obj);


void SendMeterRecord(unsigned char *terminal)
{
  SendMeterRecordTerminal = terminal;

  Window Form_SendMeterRecord;
  Button Button_Goto, Button_Send, Button_Read, Button_Info, Button_Reset;
  
  SendMeterRecordLoad();

  CreateListBox(&ListBox_SendMeterRecord, ListBox_MeterItems, 
    MeterCount, 3, 3, 115, 124);
  ListBox_SendMeterRecord.OnMsg = ListBox_SendMeterRecord_OnMsg;  
  ListBox_SendMeterRecord.OnClick = SendMeterRecordView;
  CreateButton(&Button_Goto, "跳转1", 119, 3, 38, 20);
  Button_Goto.OnClick = SendMeterRecordGoto;
  CreateButton(&Button_Send, "下发2", 119, 29, 38, 20);
  Button_Send.OnClick = SendMeterRecordDo;
  CreateButton(&Button_Read, "查询3", 119, 55, 38, 20);
  Button_Read.OnClick = ReadMeterRecord;  
  CreateButton(&Button_Info, "信息4", 119, 81, 38, 20);
  Button_Info.OnClick = MeterRecordInfoLoad;
  CreateButton(&Button_Reset, "重置5", 119, 107, 38, 20);
  Button_Reset.OnClick = SendMeterRecordReset;

  CreateWindow(&Form_SendMeterRecord, "", 0, 0, 160, 160);
  sprintf(Form_SendMeterRecord.text, "终端%s表档案配置", SendMeterRecordTerminal);
  AddToWindow(&Form_SendMeterRecord, &ListBox_SendMeterRecord);
  AddToWindow(&Form_SendMeterRecord, &Button_Goto);
  AddToWindow(&Form_SendMeterRecord, &Button_Send);
  AddToWindow(&Form_SendMeterRecord, &Button_Read);
  AddToWindow(&Form_SendMeterRecord, &Button_Info);
  AddToWindow(&Form_SendMeterRecord, &Button_Reset);
    
  ShowWindow(&Form_SendMeterRecord, SS_SHOWMAX);
}


//电表档案加载
void SendMeterRecordLoad()
{
  int count;
  use(METE_RECD_GW);
  count = libsumr();
  
  MeterCount = 0;
  memset(ListBox_MeterItems, 0, sizeof(ListBox_MeterItems));
  ShowFormProgress(count, SendMeterRecordDoLoad);
  CurrMPAttrib.MPNum = MeterCount;
}

//电表档案加载处理
void SendMeterRecordDoLoad(HWND Obj,PMsg Msg)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0}; 
  int r;
  
  for (r = 0; r < ProgressBar_Proc.max; r++)
  {

    libset(r, 0);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      ProgressStep();
      continue;
    }
    
    libset(r, FD_METERADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (temp[0] == '\0')
    {
    	ProgressStep();
    	continue;
    }
       
    strcpy(ListBox_MeterItems[MeterCount], temp);
    CurrMPAttrib.MPLocate[MeterCount] = r;
    MeterCount++;         
    ProgressStep();
  }
  
  CloseFormProgress();
  use("");
}

//电表列表消息传递事件
void ListBox_SendMeterRecord_OnMsg(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_UP:
        if (ListBox_SendMeterRecord.index == 0)
        {
          ListBox_SendMeterRecord.index = ListBox_SendMeterRecord.count - 1;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_DOWN:
        if (ListBox_SendMeterRecord.index == ListBox_SendMeterRecord.count - 1)
        {
          ListBox_SendMeterRecord.index = 0;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_1:
        SendMeterRecordGoto(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_2:
        SendMeterRecordDo(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_3:
        ReadMeterRecord(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_5:        
      case VK_DEL:
        SendMeterRecordReset(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_4:
        MeterRecordInfoLoad(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_0:
        SendMeterRecordHelp(NULL);
        Msg->Result = MSG_OK;
        break;         
    }
  }
}


//电表列表点击事件
void SendMeterRecordView(HWND Sender)
{

  if (ListBox_SendMeterRecord.count < 1)
  {
    return;
  }
  
  memset(CurrMPAttrib.Meter, 0, sizeof(CurrMPAttrib.Meter));
  sprintf(CurrMPAttrib.Meter, "%s", ListBox_SendMeterRecord.text);
  
  AttributeMeter(); 
}


//跳转命令定位电表
void SendMeterRecordGoto(HWND Sender)
{
  unsigned char meterAddr[MAX_ASSETNO_DIGITS] = {0};
  unsigned char meter[MAX_ASSETNO_DIGITS] = {0};
  int i;

  if (ListBox_SendMeterRecord.count < 1)
  {
    return;
  }
  
  if (InputBoxPro("跳转到", meterAddr, sizeof(meterAddr)) == ID_CANCEL)
  {
    return;
  }
//  GetMeteAddress(meterAddr);
  GetAddress(meterAddr, 2);
  
  for (i = 0; i < MeterCount; i++)
  {
    memset(meter, 0, sizeof(meter));
    strncpy(meter, ListBox_MeterItems[i], 12);
    if (strcmp(meterAddr, meter) == 0)
    {
      ListBox_SendMeterRecord.index = i;
      Refresh(&ListBox_SendMeterRecord);
      break;
    }
  }
}


//下发当前测量点档案
void SendMeterRecordDo(HWND Sender)
{

	int r = 0;
	int count = 0;
	int rlen = 0;
  unsigned short slen = 0; 
  unsigned short mpNo = 0;
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};

  unsigned char sendbuff[MAXSENDBYTES] = {0};
  unsigned char recvbuff[MINRECVBYTES] = {0};

  Stru698Data Send698Data;
  
  if (ListBox_SendMeterRecord.count < 1)
  {
  	return;
  }
   
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, SendMeterRecordTerminal);
  Send698Data.Ccode = C_698SET;
  Send698Data.AFN = AFN_698SET;
  Send698Data.SEQ = INI_698SEQ;
  Send698Data.Pn = 0;
  Send698Data.Fn = FN_RECD;
 
  use(METE_RECD_GW);
  /*count = libsumr();
  for(r = 0; r < count; r++)
  {    
    libset(r, 0);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if(strcmp(temp, STR_DEL) == 0)
    {
      continue;
    }

    libset(r, FD_METERADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);        
    if(strncmp(temp, ListBox_SendMeterRecord.text, 12) == 0)
    {
      break;
    }
  }
  
  if (r >= count)
  {
  	use("");
  	return;  	
  }
  
  libset(r, FD_MPNO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  mpNo = StrToInt(temp, DEC);
  */
  r = CurrMPAttrib.MPLocate[ListBox_SendMeterRecord.index];     
  slen = BuildAFN04F10Buff(sendbuff, mpNo, r);
  use("");

  if(!slen)
  {
    return;
  }

  memset(Send698Data.Userbuf, 0, sizeof(Send698Data.Userbuf));
  memcpy(Send698Data.Userbuf + 2, sendbuff, slen);
  Send698Data.Userbuf[0] = 0x01;
  Send698Data.Userbuf[1] = 0x00;
  Send698Data.Ulen = slen + 2;
  
  memset(sendbuff, 0, sizeof(sendbuff));
  slen = Build698SendBuff(sendbuff, &Send698Data);
 
  uartinit(CommParam.Baud, CommParam.Parity, CommParam.Port);
  Delay(DELAY_OPENPORT);
  
  SendData(sendbuff, slen);
  rlen = RecvChar(recvbuff, sizeof(recvbuff), NUM_600);
  Beep(100);
  
  rlen = Parse698Buff(recvbuff, rlen);
  
  MessageBox(ConstResStr[rlen], STR_CUE, MB_OK); 
  
  uartinit(0, CommParam.Parity, CommParam.Port);
   
}


//抄读当前测量点档案
void ReadMeterRecord(HWND Sender)
{

  int mpNo = 1;
  U16 slen = 0; 
  int rlen = 0;
  unsigned char temp[13] = {0};
  unsigned char sendbuff[MINSENDBYTES] = {0};
  unsigned char recvbuff[MAXRECVBYTES] = {0};
  Stru698Data Send698Data;
  
  if (ListBox_SendMeterRecord.count > 0)
  {
  	sprintf(temp, "%d", ListBox_SendMeterRecord.index + 1);
  }

	if (InputBox("", "请输入测量点号", temp, 4) == ID_CANCEL)
	{
		return;
	}
	
	mpNo = StrToInt(temp, DEC);
	if (mpNo > MAX_MPNO)
	{
		MessageBox("测量点号超过范围!", STR_CUE, MB_OK);
		return;
	}
	
	memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, SendMeterRecordTerminal);
  Send698Data.Ccode = C_698READ;
  Send698Data.AFN = AFN_698READ;
  Send698Data.SEQ = INI_698SEQ;
  Send698Data.Pn = 0;
  Send698Data.Fn = FN_RECD;
  
  ReadMeterRecordIndex = 1; 
  ReadMeterRecordNum = 1;
 
 	Send698Data.Ulen = 0;
  Send698Data.Userbuf[Send698Data.Ulen++] = 0x01;
  Send698Data.Userbuf[Send698Data.Ulen++] = 0x00;  
  Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)mpNo;
  Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)(mpNo >> 8); 
    
  memset(sendbuff, 0, sizeof(sendbuff));
  slen = Build698SendBuff(sendbuff, &Send698Data);
  
  uartinit(CommParam.Baud, CommParam.Parity, CommParam.Port);
  Delay(150);
  
  //copy(METE_RECD_GW, METE_RECD_RCV_GW);
  copy(METE_RECD_NEW_GW, METE_RECD_RCV_GW); 
    
  SendData(sendbuff, slen);
  rlen = RecvChar(recvbuff, sizeof(recvbuff), 800);
  Beep(100);
   
  uartinit(0, CommParam.Parity, CommParam.Port);
  Delay(50);
     
  rlen = Parse698Buff(recvbuff, rlen);
  if (rlen != 1)
  {
    MessageBox(ConstResStr[rlen], STR_CUE, MB_OK);    
    return;
  }       

  GetMPLocate(&CurrMPAttrib, METE_RECD_RCV_GW);   
  //CurrMPAttrib.Flag = ListBox_SendMeterRecord.index;    
    
  ShowReadRecordResult(NULL);
}


//显示当前测量点信息
void MeterRecordInfoLoad(HWND Sender)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int r, count;
  Window Form_MeterRecordInfo;
  Text Text_Term, Text_TermNo, Text_Count, Text_MeteNo, Text_ColAddr, Text_ColNo;


  if (ListBox_SendMeterRecord.count < 1)
  {
    return;
  }
  
  use(METE_RECD_GW);
  count = libsumr();

  for (r = 0; r < count; r++)
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
    if (strcmp(temp, ListBox_SendMeterRecord.text) == 0)
    {
      break;
    }       
  }

  CreateText(&Text_Term, "", 3, 3, 154, 15);
  sprintf(Text_Term.text, "终端地址：%s", SendMeterRecordTerminal);
  CreateText(&Text_Count, "", 3, 20, 154, 15);
  sprintf(Text_Count.text, "电表总数：%d", MeterCount);
  
  CreateText(&Text_ColAddr, "", 3, 37, 154, 15);
  libset(r, FD_COLLADDR);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Text_ColAddr.text, "采集器：%s", temp);  
  
  CreateText(&Text_MeteNo, "", 3, 54, 154, 22);
  libset(r, FD_METERNO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Text_MeteNo.text, "电表资产号：%s", temp);
  
  CreateText(&Text_TermNo, "", 3, 78, 154, 22);
  libset(r, FD_TERMNO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Text_TermNo.text, "终端资产号：%s", temp);
  
  CreateText(&Text_ColNo, "", 3, 102, 154, 25);
  libset(r, FD_COLLNO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Text_ColNo.text, "采集器资产号：%s", temp);
  use("");

  CreateWindow(&Form_MeterRecordInfo, "", 0, 0, 160, 160);
  sprintf(Form_MeterRecordInfo.text, "电表%s信息", ListBox_SendMeterRecord.text);
  Form_MeterRecordInfo.OnKeyPress = Form_MeterRecordInfo_OnKeyPress;
  AddToWindow(&Form_MeterRecordInfo, &Text_Term);
  AddToWindow(&Form_MeterRecordInfo, &Text_Count);
  AddToWindow(&Form_MeterRecordInfo, &Text_ColAddr);
  
  AddToWindow(&Form_MeterRecordInfo, &Text_MeteNo);
  AddToWindow(&Form_MeterRecordInfo, &Text_TermNo);
  AddToWindow(&Form_MeterRecordInfo, &Text_ColNo);          
   
  if (ShowWindow(&Form_MeterRecordInfo, SS_SHOWMAX) == VK_ENTER)
  {
    return;
  }
  
}
void Form_MeterRecordInfo_OnKeyPress(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_ENTER:
        CloseWindow(Obj, VK_ENTER);
        break;
    }
  }        
}


//重置 录入电表资产号
void SendMeterRecordReset(HWND Sender)
{
  unsigned char temp[23] = {};
  unsigned char meterno[23] = {};
  char show[60] = {};
  int r, c;
  
  if (MeterCount == 0)
  {
    return;
  }
  if (InputBoxPro("电表编号", meterno, sizeof(meterno)) == ID_CANCEL)
  {
    return;
  }
  sprintf(show,"确定要改电表编号吗？\n电表编号: %s", meterno);
  if (MessageBox(show, STR_CUE, MB_YESNO) == ID_NO)
  {
    return;
  }
  
  use(METE_RECD_GW);
  c = libsumr();

  for (r = 0; r < c; r++)
  {
    libset(r, 0);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
      continue;
        
    libset(r, 4);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strncmp(temp, ListBox_SendMeterRecord.text, 12) == 0)
    {
      libset(r, 3);
      libwrit(meterno); 
      break;
    } 
  } 
  use("");
  return;
}


//电表档案抄读结果显示
void ShowReadRecordResult(HWND Sender)
{
  int r;
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  Window Form_ReadRecordResult;

  CreateText(&ReadRecordText_MPNo, "", 3, 3, 154, 15);
  CreateText(&ReadRecordText_Baud, "", 3, 21, 154, 15); 
  CreateText(&ReadRecordText_Port, "", 3, 39, 154, 15);  
  CreateText(&ReadRecordText_Proto, "", 3, 57, 154, 15);
  CreateText(&ReadRecordText_MeteAddr, "", 3, 88, 154, 15);
  CreateText(&ReadRecordText_ColAddr, "", 3, 106, 154, 15);

  CreateWindow(&Form_ReadRecordResult, STR_RES_READ, 0, 0, 160, 160);
  Form_ReadRecordResult.OnKeyPress = Form_ReadRecordResult_OnKeyPress;

  sprintf(Form_ReadRecordResult.text, "抄读结果[%d/%d]", 
    ReadMeterRecordIndex, ReadMeterRecordNum);

  use(METE_RECD_RCV_GW);
//  r = ReadRecordLocate[ReadMeterRecordIndex];    
  r = CurrMPAttrib.MPLocate[CurrMPAttrib.Flag];

  libset(r, FD_MPNO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_MPNo.text, "测量点：%s", temp);

  libset(r, FD_BAUD);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_Baud.text, "波特率：%s", temp);

  libset(r, FD_PORT);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_Port.text, "端 口：%s", temp);
  
  libset(r, FD_PROTO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_Proto.text, "协 议：%s", temp);
  
  libset(r, FD_METERADDR);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_MeteAddr.text, "表地址：%s", temp);
  
  libset(r, FD_COLLADDR);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_ColAddr.text, "采集器：%s", temp);  

  use("");

  AddToWindow(&Form_ReadRecordResult, &ReadRecordText_MPNo);
  AddToWindow(&Form_ReadRecordResult, &ReadRecordText_Baud);
  AddToWindow(&Form_ReadRecordResult, &ReadRecordText_Port);
  AddToWindow(&Form_ReadRecordResult, &ReadRecordText_Proto);
  AddToWindow(&Form_ReadRecordResult, &ReadRecordText_MeteAddr);
  AddToWindow(&Form_ReadRecordResult, &ReadRecordText_ColAddr);
  
  ShowWindow(&Form_ReadRecordResult, SS_SHOWMAX);
}

void Form_ReadRecordResult_OnKeyPress(HWND Obj, PMsg Msg)
{
	if (ReadMeterRecordNum <= 1)
	{
		return;
	}
		
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_LEFT: 
      case VK_UP:  
      {   
        if ((ReadMeterRecordIndex == 1) || (CurrMPAttrib.Flag == 0))
        { 
        	return;
        }
        else 
        {
        	ReadMeterRecordIndex--;
        	CurrMPAttrib.Flag--;    
        }
        Msg->Result = MSG_OK;
        break; 
      }
      case VK_RIGHT:
      case VK_DOWN:
      {
        if ((ReadMeterRecordIndex == ReadMeterRecordNum) 
        	|| (CurrMPAttrib.Flag == CurrMPAttrib.MPNum - 1)) 
        {
        	return;
        }
        else
        {
        	ReadMeterRecordIndex++;
        	CurrMPAttrib.Flag++;
        }	      	     	
        Msg->Result = MSG_OK;
        break; 
      }
      default:
       	break;
    }
    if (Msg->Result == MSG_OK)
    {
      Form_ReadRecordResult_Refresh(Obj);
      Refresh(Obj);
    }                
  }   
}

//档案显示翻页更新
void Form_ReadRecordResult_Refresh(HWND Obj)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int r;
  
  sprintf(((Window*)Obj)->text, "抄读结果[%d/%d]", 
    ReadMeterRecordIndex, ReadMeterRecordNum);

  use(METE_RECD_RCV_GW);
//  r = ReadRecordLocate[ReadMeterRecordIndex];  
  r = CurrMPAttrib.MPLocate[CurrMPAttrib.Flag];    
  
  libset(r, FD_MPNO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_MPNo.text, "测量点：%s", temp);

  libset(r, FD_BAUD);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_Baud.text, "波特率：%s", temp);

  libset(r, FD_PORT);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_Port.text, "端 口：%s", temp);
  
  libset(r, FD_PROTO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_Proto.text, "协 议：%s", temp);
  
  libset(r, FD_METERADDR);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_MeteAddr.text, "表地址：%s", temp);
  
  libset(r, FD_COLLADDR);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(ReadRecordText_ColAddr.text, "采集器：%s", temp);  

  use("");
}


void SendMeterRecordHelp(HWND Sender)
{
  MessageBox("当光标位于电表列表时\n确认键：配置电表属性\n按键1：跳转\n按键2：下发\n按键3：查询\n按键4：信息\n按键5/删除键：重置", "帮助", MB_OK); 
}
#endif
