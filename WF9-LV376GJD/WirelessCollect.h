#ifndef _WirelessCollect_H
#define _WirelessCollect_H

#include "jblib.h"
#include "Common.h"
#include "FormProgress.h"
#include "InputBoxPro.h"
#include "WirelessMeter.h"


ListBox ListBox_WirelessCollect;

void WirelessCollectLoad();
void WirelessCollectDoLoad(HWND Obj,PMsg Msg);
void ListBox_WirelessCollect_OnMsg(HWND Obj, PMsg Msg);
void WirelessCollectView(HWND Sender);
void WirelessCollectGoto(HWND Sender);
void WirelessCollectChannel(HWND Sender);
void WirelessCollectDebug(HWND Sender);

void WirelessCollectRead(HWND Sender);

void WirelessCollectHelp(HWND Sender);



void WirelessCollect(unsigned char *terminal)
{

  Window Form_WirelessCollect;
  Button Button_Goto, Button_Channel, Button_Debug, Button_Read, Button_Help;

  WirelessCollectLoad();
  
  CreateListBox(&ListBox_WirelessCollect, ListBox_CollectItems, 
    CollectCount, 3, 3, 115, 124);
  ListBox_WirelessCollect.OnMsg = ListBox_WirelessCollect_OnMsg;  
  ListBox_WirelessCollect.OnClick = WirelessCollectView;
  CreateButton(&Button_Goto, "添加1", 119, 3, 38, 20);
  Button_Goto.OnClick = WirelessCollectGoto;
  CreateButton(&Button_Channel, "频道2", 119, 29, 38, 20);
  Button_Channel.OnClick = WirelessCollectChannel;
  CreateButton(&Button_Debug, "调试3", 119, 55, 38, 20);
  Button_Debug.OnClick = WirelessCollectDebug; 
  CreateButton(&Button_Read, "抄表4", 119, 81, 38, 20);    
  Button_Read.OnClick = WirelessCollectRead; 
  CreateButton(&Button_Help, "帮助5", 119, 107, 38, 20);
  Button_Help.OnClick = WirelessCollectHelp;
  

  CreateWindow(&Form_WirelessCollect, "", 0, 0, 160, 160);
  sprintf(Form_WirelessCollect.text, "终端%s采集器", CurrMPAttrib.Term);
  AddToWindow(&Form_WirelessCollect, &ListBox_WirelessCollect);
  AddToWindow(&Form_WirelessCollect, &Button_Goto);
  AddToWindow(&Form_WirelessCollect, &Button_Channel);  
  AddToWindow(&Form_WirelessCollect, &Button_Debug);
  AddToWindow(&Form_WirelessCollect, &Button_Read);
  AddToWindow(&Form_WirelessCollect, &Button_Help);
    
  ShowWindow(&Form_WirelessCollect, SS_SHOWMAX);
  
  char filename[MAX_FILENAME_LEN] = {0};
  GetRecdFileName(CurrMPAttrib.Term, filename);
  copy(METE_RECD_GW, filename);	  
}


//采集器档案加载
void WirelessCollectLoad()
{
  int count;
  use(METE_RECD_GW);
  count = libsumr();
  
  CollectCount = 0;
  memset(ListBox_CollectItems, 0, sizeof(ListBox_CollectItems));
  ShowFormProgress(count, WirelessCollectDoLoad);
}

//采集器档案加载处理
void WirelessCollectDoLoad(HWND Obj,PMsg Msg)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0}; 
  int i, r;
  
  for (r = 0; r < ProgressBar_Proc.max; r++)
  {
    libset(r, FD_FLAG);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      ProgressStep();
      continue;
    }
       
    libset(r, FD_COLLADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    for (i = 0; i < CollectCount; i++)
    {
      if (strcmp(temp, ListBox_CollectItems[i]) == 0)
      {
        break;
      }
    }
    if (i == CollectCount)
    {
      strcpy(ListBox_CollectItems[CollectCount], temp);
      CollectCount++; 
    }      
    ProgressStep();
   
  }
  	
  CloseFormProgress();
  use("");
}


//采集器列表消息传递事件
void ListBox_WirelessCollect_OnMsg(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_UP:
        if (ListBox_WirelessCollect.index == 0)
        {
          ListBox_WirelessCollect.index = ListBox_WirelessCollect.count - 1;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_DOWN:
        if (ListBox_WirelessCollect.index == ListBox_WirelessCollect.count - 1)
        {
          ListBox_WirelessCollect.index = 0;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_1:
        WirelessCollectGoto(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_2:
        WirelessCollectChannel(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_4:
        WirelessCollectRead(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_3:
        WirelessCollectDebug(NULL);
        Msg->Result = MSG_OK;
        break;
      case VK_5:
      case VK_0:
        WirelessCollectHelp(NULL);
        Msg->Result = MSG_OK;
        break; 
    }
  }
}


//采集器列表点击事件
void WirelessCollectView(HWND Sender)
{
	int r, count;
	unsigned char collect[MAX_ASSETNO_DIGITS] = {0};
	unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
	
  if (ListBox_WirelessCollect.count < 1)
  {
    return;
  }
  
  memset(CurrMPAttrib.Collect, 0, sizeof(CurrMPAttrib.Collect));
  sprintf(CurrMPAttrib.Collect, "%s", ListBox_WirelessCollect.text);  
    
  WirelessMeter();
}


//跳转定位或添加采集器
void WirelessCollectGoto(HWND Sender)
{
  unsigned char collectNo[MAX_ASSETNO_DIGITS] = {};
  unsigned char collectAddr[MAX_ASSETNO_DIGITS] = {};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {};
  int r, i;
  
  if (ListBox_WirelessCollect.count >= MAXCOLLECTCOUNT - 1)
  {
  	MessageBox("采集器数量已达上限!", STR_CUE, MB_OK);
  	return;
  }
  
  while (1)
  {
    memset(collectNo, 0, sizeof(collectNo));
    memset(collectAddr, 0, sizeof(collectAddr));
    if (ListBox_WirelessCollect.count > 0)
    {
    	sprintf(collectNo, "%s", ListBox_WirelessCollect.text);
    }
    if (InputBoxPro("采集器编号", collectNo, sizeof(collectNo)) == ID_CANCEL)
    {
      return;
    }
    strcpy(collectAddr, collectNo);    

    i = GetAddress(collectAddr, 4);    
    if(i)
    {
      MessageBox(ConstConfERRStr[i], STR_CUE, MB_OK);
      return;
    }
                            
    for (i = 0; i < CollectCount; i++)
    {
      if (strcmp(collectAddr, ListBox_CollectItems[i]) == 0)
      {
        break;
      }
    }
    if (i < CollectCount)
    {
      MessageBox("采集器已存在", STR_CUE, MB_OK);
      ListBox_WirelessCollect.index = i;
      Refresh(&ListBox_WirelessCollect);
      break; 
    }

		//将采集器档案存入测量点档案数据库 
	  use(METE_RECD_GW);	
	  libappend();
	  r = libgetr();
	  libset(r, FD_TERMADDR);
	  libwrit(CurrMPAttrib.Term);     
	  libset(r, FD_COLLNO);
	  libwrit(collectNo);
	  libset(r, FD_COLLADDR);
	  libwrit(collectAddr);	  
	  use("");

    strcpy(ListBox_CollectItems[CollectCount], collectAddr);
    ListBox_WirelessCollect.index = CollectCount;
    CollectCount++;
    ListBox_WirelessCollect.count = CollectCount;
    Refresh(&ListBox_WirelessCollect);
    
    break;
  }
    
}


//查询设置小无线频道频段
void WirelessCollectChannel(HWND Sender)
{
	ChannelManage(ListBox_WirelessCollect.text, 4);
}


//无线采集器调试
void WirelessCollectDebug(HWND Sender)
{
	WirelessDebugTools(ListBox_WirelessCollect.text, 4);
}


//直接抄表
void WirelessCollectRead(HWND Sender)
{
	char address[13] = {0};
	
	if (InputBox("电表地址", "请输入", address, 12) == ID_CANCEL)
  {
  	return;
  }

	FormatStr(address, 12, '0', 0); 

	WirelessReadMeter(address, ListBox_WirelessCollect.text, 0);
}


void WirelessCollectHelp(HWND Sender)
{
  MessageBox("光标位于采集器列表时\n确认键：电表档案\n按键1：添加\n按键2：频道\n按键3：调试\n按键4：抄表\n按键5：帮助", "帮助", MB_OK); 
}
#endif
