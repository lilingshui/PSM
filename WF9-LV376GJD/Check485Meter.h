#ifndef	_Check485Meter_H
#define _Check485Meter_H

#include "jblib.h"
#include "Common.h"
#include "FormProgress.h"
#include "InputBoxPro.h"
#include "DataConvert.h"
#include "DLT645.h"
#include "AttributeMeter.h"

unsigned char *Check485MeterTerminal;
ListBox ListBox_Check485Meter;

Text Check485MeterText_Term, Check485MeterText_MeterNo, 
  Check485MeterText_MeterAddr, Check485MeterText_Rate, 
  Check485MeterText_Prot, Check485MeterText_Result,
  Check485MeterText_Data;

void Check485MeterLoad();
void Check485MeterDoLoad(HWND Obj,PMsg Msg);
void ListBox_Check485Meter_OnMsg(HWND Obj, PMsg Msg);
void Check485MeterView(HWND Sender);
void Check485MeterGoto(HWND Sender);
void Check485MeterCheck(HWND Sender);
void Check485MeterResult(HWND Sender);
void Form_Check485MeterResult_OnKeyPress(HWND Obj, PMsg Msg);
void Form_Check485MeterResult_Refresh(HWND Obj);
void Check485MeterReset(HWND Sender);
void Check485MeterHelp(HWND Sender);

void Check485Meter(unsigned char *terminal)
{
  Check485MeterTerminal = terminal;
  Window Form_Check485Meter;
  Button Button_Goto, Button_Check, Button_Result, Button_Reset, Button_Help;
  
  Check485MeterLoad();

  CreateListBox(&ListBox_Check485Meter, ListBox_MeterItems, 
    MeterCount, 3, 3, 154, 102);
  ListBox_Check485Meter.OnMsg = ListBox_Check485Meter_OnMsg;  
  ListBox_Check485Meter.OnClick = Check485MeterView;
  CreateButton(&Button_Goto, "跳转1", 3, 108, 38, 20);
  Button_Goto.OnClick = Check485MeterGoto;
  CreateButton(&Button_Check, "检测2", 42, 108, 38, 20);
  Button_Check.OnClick = Check485MeterCheck;
  CreateButton(&Button_Result, "结果3", 81, 108, 38, 20);
  Button_Result.OnClick = Check485MeterResult;
  CreateButton(&Button_Reset, "重置4", 120, 108, 38, 20);
  Button_Reset.OnClick = Check485MeterReset;

  CreateWindow(&Form_Check485Meter, "", 0, 0, 160, 160);
  sprintf(Form_Check485Meter.text, "%s单个电表检测", Check485MeterTerminal);
  AddToWindow(&Form_Check485Meter, &ListBox_Check485Meter);
  AddToWindow(&Form_Check485Meter, &Button_Goto);
  AddToWindow(&Form_Check485Meter, &Button_Check);
  AddToWindow(&Form_Check485Meter, &Button_Result);
  AddToWindow(&Form_Check485Meter, &Button_Reset);
    
  ShowWindow(&Form_Check485Meter, SS_SHOWMAX);
  
  char filename[MAX_FILENAME_LEN] = {0};
  GetRecdFileName(CurrMPAttrib.Term, filename);
  copy(METE_RECD_GW, filename);	 
}

void Check485MeterLoad()
{
  int count;
  use(METE_RECD_GW);
  count = libsumr();
  
  MeterCount = 0;
  memset(ListBox_MeterItems, 0, sizeof(ListBox_MeterItems));
  ShowFormProgress(count, Check485MeterDoLoad);
}

void Check485MeterDoLoad(HWND Obj,PMsg Msg)
{
  unsigned char meter[MAX_ASSETNO_DIGITS] = {};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {}; 
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
    memset(meter, 0, sizeof(meter));
    strcpy(meter, temp);

    libset(r, FD_CKRES);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    strcat(meter, "-");
    strcat(meter, temp);
    
    strcpy(ListBox_MeterItems[MeterCount], meter);
    CurrMPAttrib.MPLocate[MeterCount] = r;
    MeterCount++;      
    ProgressStep();
  }
  CloseFormProgress();
  use("");
}

void ListBox_Check485Meter_OnMsg(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_UP:
        if (ListBox_Check485Meter.index == 0)
        {
          ListBox_Check485Meter.index = ListBox_Check485Meter.count - 1;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_DOWN:
        if (ListBox_Check485Meter.index == ListBox_Check485Meter.count - 1)
        {
          ListBox_Check485Meter.index = 0;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_1:
        Check485MeterGoto(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_2:
        Check485MeterCheck(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_3:
        Check485MeterResult(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_4:        
      case VK_DEL:
        Check485MeterReset(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_0:
        Check485MeterHelp(NULL);
        Msg->Result = MSG_OK;
        break; 
    }
  }
}

void Check485MeterView(HWND Sender)
{
  //Check485MeterResult(NULL);
  if (ListBox_Check485Meter.count < 1)
  {
    return;
  }
  	  
  memset(CurrMPAttrib.Meter, 0, sizeof(CurrMPAttrib.Meter));
  memcpy(CurrMPAttrib.Meter, ListBox_Check485Meter.text, 12);
    
  AttributeMeter();
}

void Check485MeterGoto(HWND Sender)
{
  unsigned char meterAddr[MAX_ASSETNO_DIGITS] = {};
  unsigned char meter[MAX_ASSETNO_DIGITS] = {};
  int i;

  if (InputBoxPro("跳转到", meterAddr, sizeof(meterAddr)) == ID_CANCEL)
  {
    return;
  }
//  GetMeteAddress(meterAddr);
  GetAddress(meterAddr, METER);

  for (i = 0; i < MeterCount; i++)
  {
    memset(meter, 0, sizeof(meter));
    strncpy(meter, ListBox_MeterItems[i], 12);
    if (strcmp(meterAddr, meter) == 0)
    {
      ListBox_Check485Meter.index = i;
      Refresh(&ListBox_Check485Meter);
      break;
    }
  }
}

void Check485MeterCheck(HWND Sender)
{
  DLT645INFO di;
  DLT645DATA ddin, ddout;
  unsigned long long dataItem = 0;
  int r, c, i, j;
  unsigned char temp[MAX_ASSETNO_DIGITS] = {};
  unsigned char meter[MAX_ASSETNO_DIGITS] = {};
  unsigned char data[13] = {};

  DLT645_CreateInfo(&di);
  memset(meter, 0, sizeof(meter));
  strncpy(meter, ListBox_Check485Meter.text, 12);
  dataItem = StrToHex(meter, strlen(meter));
  memcpy(di.addr, &dataItem, di.addrlen);

  use(METE_RECD_GW);
  r = CurrMPAttrib.MPLocate[ListBox_Check485Meter.index];
  for (c = FD_BAUD; c <= FD_ENERGY; c++)
  {
    libset(r, c);
    libwrit("");
  } 
  int baudRate[] = {2400, 1200, 9600};
  for (i = 0; i < 3; i++)
  { 
    uartinit(baudRate[i], CommParam.Parity, 0x01);
    Delay(150);
    
    memset(meter, 0, sizeof(meter));
    strncpy(meter, ListBox_Check485Meter.text, 12);
    
    if (i == 0)
    {
      ddin.CCode = 0x11;
      ddin.datalen = 0x04;
      dataItem = 0x00010000;
      memcpy(ddin.databuf, &dataItem, ddin.datalen);
      if (DLT645_RequestWS(&di, &ddin, &ddout) == 0)
      {
        libset(r, FD_BAUD);
        memset(temp, 0, sizeof(temp));
        sprintf(temp, "%d", baudRate[i]);
        libwrit(temp);

        libset(r, FD_PROTO);
        memset(temp, 0, sizeof(temp));
        sprintf(temp, "DL/T 645-2007");
        libwrit(temp);

        libset(r, FD_CKRES);
        memset(temp, 0, sizeof(temp));
        sprintf(temp, "07");
        libwrit(temp);
        strcat(meter, "-07");
        
        memset(data, 0, sizeof(data));
        memset(temp, 0, sizeof(temp));
        memcpy(data, ddout.databuf+4, 4);
        sprintf(temp, "%02x%02x%02x.%02xkWh", data[3], data[2], data[1], data[0]);
        for(j = 0; j < 5; j++)
        {
          if(temp[j] != '0')
            break;
        }
        memset(data, 0, sizeof(data));
        memcpy(data, temp + j, strlen(temp) - j);
          
        libset(r, FD_ENERGY);
        libwrit(data);                    
        break;
      }
      else
      { 
        libset(r, FD_CKRES);
        memset(temp, 0, sizeof(temp));
        sprintf(temp, STR_FAIL);
        libwrit(temp);
        strcat(meter, "-失败");
      }
      
      //memset(ListBox_MeterItems[ListBox_Check485Meter.index], 0, sizeof(TItem));
      //strcpy(ListBox_MeterItems[ListBox_Check485Meter.index], meter);
    }

    else
    {
      ddin.CCode = 0x01;
      ddin.datalen = 0x02;
      dataItem = 0x9010;
      memcpy(ddin.databuf, &dataItem, ddin.datalen);
      if (DLT645_RequestWS(&di, &ddin, &ddout) == 0)
      {
        libset(r, FD_BAUD);
        memset(temp, 0, sizeof(temp));
        sprintf(temp, "%d", baudRate[i]);
        libwrit(temp);

        libset(r, FD_PROTO);
        memset(temp, 0, sizeof(temp));
        sprintf(temp, "DL/T 645-1997");
        libwrit(temp);

        libset(r, FD_CKRES);
        memset(temp, 0, sizeof(temp));
        sprintf(temp, "97");
        libwrit(temp);
        strcat(meter, "-97");
        
        memset(data, 0, sizeof(data));
        memset(temp, 0, sizeof(temp));
        memcpy(data, ddout.databuf+2, 4);
        sprintf(temp, "%02x%02x%02x.%02xkWh", data[3], data[2], data[1], data[0]);        
        for(j = 0; j < 5; j++)
        {
          if(temp[j] != '0')
            break;
        }
        memset(data, 0, sizeof(data));
        memcpy(data, temp + j, strlen(temp) - j);
          
        libset(r, FD_ENERGY);
        libwrit(data);
               
        break;
      }
      else
      {
        libset(r, FD_CKRES);
        memset(temp, 0, sizeof(temp));
        sprintf(temp, STR_FAIL);
        libwrit(temp);
        strcat(meter, "-失败");
      }
      //memset(ListBox_MeterItems[ListBox_Check485Meter.index], 0, sizeof(TItem));
      //strcpy(ListBox_MeterItems[ListBox_Check485Meter.index], meter);
    }
    
    uartinit(0, CommParam.Parity, 0x01);
  }
  use("");
  Beep(100);
  
  memset(ListBox_MeterItems[ListBox_Check485Meter.index], 0, sizeof(TItem));
  strcpy(ListBox_MeterItems[ListBox_Check485Meter.index], meter);
  
  Check485MeterResult(NULL);    
}

void Check485MeterResult(HWND Sender)
{
  int r;
  Window Form_Check485MeterResult;

  CreateText(&Check485MeterText_Term, "", 3, 3, 154, 15);
  CreateText(&Check485MeterText_MeterAddr, "", 3, 21, 154, 15);
 
  CreateText(&Check485MeterText_Data, "", 3, 52, 154, 15);  
  CreateText(&Check485MeterText_Rate, "", 3, 70, 154, 15);
  CreateText(&Check485MeterText_Prot, "", 3, 88, 154, 15);
  CreateText(&Check485MeterText_Result, "", 3, 106, 154, 15);

  CreateWindow(&Form_Check485MeterResult, "485检测结果", 0, 0, 160, 160);
  Form_Check485MeterResult.OnKeyPress = Form_Check485MeterResult_OnKeyPress;

  unsigned char temp[MAX_ASSETNO_DIGITS] = {};

  sprintf(Form_Check485MeterResult.text, "485检测结果[%d/%d]", 
    ListBox_Check485Meter.index + 1, MeterCount);

  use(METE_RECD_GW);
  r = CurrMPAttrib.MPLocate[ListBox_Check485Meter.index];
  
//  libset(r, 2);
//  memset(temp, 0, sizeof(temp));
//  libread(temp);
  sprintf(Check485MeterText_Term.text, "终端地址：%s", Check485MeterTerminal);

  libset(r, FD_METERADDR);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_MeterAddr.text, "表地址：%s", temp);

  libset(r, FD_BAUD);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_Rate.text, "通讯速率：%s", temp);

  libset(r, FD_PROTO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_Prot.text, "规约：%s", temp);

  libset(r, FD_CKRES);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_Result.text, "检测结果：%s", temp);
  
  libset(r, FD_ENERGY);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_Data.text, "正向有功：%s", temp);

  use("");

  AddToWindow(&Form_Check485MeterResult, &Check485MeterText_Term);
  AddToWindow(&Form_Check485MeterResult, &Check485MeterText_MeterAddr);
  AddToWindow(&Form_Check485MeterResult, &Check485MeterText_Rate);
  AddToWindow(&Form_Check485MeterResult, &Check485MeterText_Prot);
  AddToWindow(&Form_Check485MeterResult, &Check485MeterText_Result);
  AddToWindow(&Form_Check485MeterResult, &Check485MeterText_Data);
  
  ShowWindow(&Form_Check485MeterResult, SS_SHOWMAX);
}

void Form_Check485MeterResult_OnKeyPress(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_LEFT: 
      case VK_UP:
        if (ListBox_Check485Meter.index == 0)
        {
          ListBox_Check485Meter.index = ListBox_Check485Meter.count - 1;
        }
        else
        {
          ListBox_Check485Meter.index--;
        }
        Msg->Result = MSG_OK;
        break; 
      case VK_RIGHT:
      case VK_DOWN:
        if (ListBox_Check485Meter.index == ListBox_Check485Meter.count - 1)
        {
          ListBox_Check485Meter.index = 0;
        }
        else
        {
          ListBox_Check485Meter.index++;
        }
        Msg->Result = MSG_OK;
        break; 
    }
    if (Msg->Result == MSG_OK)
    {
      Form_Check485MeterResult_Refresh(Obj);
      Refresh(Obj);
    }                
  }   
}

void Form_Check485MeterResult_Refresh(HWND Obj)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {};
  int r;
  
  sprintf(((Window*)Obj)->text, "485检测结果[%d/%d]", 
    ListBox_Check485Meter.index + 1, MeterCount);

  use(METE_RECD_GW);
  r = CurrMPAttrib.MPLocate[ListBox_Check485Meter.index];
  
  //libset(r, 2);
  //memset(temp, 0, sizeof(temp));
  //libread(temp);
  sprintf(Check485MeterText_Term.text, "终端地址：%s", Check485MeterTerminal);

  libset(r, FD_METERADDR);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_MeterAddr.text, "表地址：%s", temp);

  libset(r, FD_BAUD);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_Rate.text, "通讯速率：%s", temp);

  libset(r, FD_PROTO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_Prot.text, "规约：%s", temp);

  libset(r, FD_CKRES);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_Result.text, "检测结果：%s", temp);
  
  libset(r, FD_ENERGY);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sprintf(Check485MeterText_Data.text, "正向有功：%s", temp);

  use("");
}

void Check485MeterReset(HWND Sender)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {};
  int r, c;
  
  if (ListBox_Check485Meter.count < 1)
  {
    return;
  }
  
  if (MessageBox("确定要重置该电表的检测结果吗？", 
    STR_CUE, MB_YESNO) == ID_NO)
  {
    return;
  }
  
  use(METE_RECD_GW);
  r = CurrMPAttrib.MPLocate[ListBox_Check485Meter.index];
  for (c = FD_BAUD; c <= FD_ENERGY; c++)
  {
    libset(r, c);
    libwrit("");
  } 
  use("");
  memset(temp, 0, sizeof(temp));
  strcpy(temp, ListBox_Check485Meter.text);
  memset(ListBox_MeterItems[ListBox_Check485Meter.index], 0, sizeof(TItem));
  strncpy(ListBox_MeterItems[ListBox_Check485Meter.index], temp, 13);
  Refresh(&ListBox_Check485Meter);
}

void Check485MeterHelp(HWND Sender)
{
  MessageBox("当光标位于电表列表时\n确认键：配置电表属性\n按键1：跳转\n按键2：检测\n按键3：结果\n按键4/删除键：重置", "帮助", MB_OK); 
}
#endif
