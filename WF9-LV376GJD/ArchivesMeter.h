#ifndef _ArchiveMeter_H
#define _ArchiveMeter_H

#include "jblib.h"
#include "Common.h"
#include "FormProgress.h"
#include "FormSelecte.h"
#include "InputBoxPro.h"
#include "AttributeMeter.h"

unsigned char *ArchivesMeterTerminal;
unsigned char *ArchivesMeterCollect; 

ListBox ListBox_ArchivesMeter;

void ArchivesMeterLoad();
void ArchivesMeterDoLoad(HWND Obj,PMsg Msg);
void ListBox_ArchivesMeter_OnMsg(HWND Obj, PMsg Msg);
void ArchivesMeterView(HWND Sender);
void ArchivesMeterGoto(HWND Sender);
void ArchivesMeterAdd(HWND Sender);
void ArchivesMeterDel(HWND Sender);
void ArchivesMeterDoDel(HWND Obj,PMsg Msg);
void ArchivesMeterPort(HWND Sender);
void ArchivesMeterAmend(HWND Sender);
void ArchivesMeterHelp(HWND Sender);
//void WritMeterRecord(char *meter, char *meterno);
//U8 LookUpMeter(unsigned char *meter);

void ArchivesMeter(unsigned char *terminal, unsigned char *collect)
{
  ArchivesMeterTerminal = terminal;
  ArchivesMeterCollect = collect;
  
  Window Form_ArchivesMeter;
  Button Button_Goto, Button_Add, Button_Del, Button_Port, Button_Amend;

  ArchivesMeterLoad();
  
  CreateListBox(&ListBox_ArchivesMeter, ListBox_MeterItems, 
    MeterCount, 3, 3, 115, 124);
  ListBox_ArchivesMeter.OnMsg = ListBox_ArchivesMeter_OnMsg;  
  ListBox_ArchivesMeter.OnClick = ArchivesMeterView;
  CreateButton(&Button_Goto, "跳转1", 119, 3, 38, 20);
  Button_Goto.OnClick = ArchivesMeterGoto;
  CreateButton(&Button_Add, "添加2", 119, 29, 38, 20);
  Button_Add.OnClick = ArchivesMeterAdd;
  CreateButton(&Button_Amend, "修改3", 119, 55, 38, 20);
  Button_Amend.OnClick = ArchivesMeterAmend;
  CreateButton(&Button_Port, "端口4", 119, 81, 38, 20);
  Button_Port.OnClick = ArchivesMeterPort;
  CreateButton(&Button_Del, "删除5", 119, 107, 38, 20);
  Button_Del.OnClick = ArchivesMeterDel;

  CreateWindow(&Form_ArchivesMeter, "", 0, 0, 160, 160);
  sprintf(Form_ArchivesMeter.text, "终端%s电表档案", CurrMPAttrib.Term);
  AddToWindow(&Form_ArchivesMeter, &ListBox_ArchivesMeter);
  AddToWindow(&Form_ArchivesMeter, &Button_Goto);
  AddToWindow(&Form_ArchivesMeter, &Button_Add);
  AddToWindow(&Form_ArchivesMeter, &Button_Amend);
  AddToWindow(&Form_ArchivesMeter, &Button_Port);
  AddToWindow(&Form_ArchivesMeter, &Button_Del);
      
  ShowWindow(&Form_ArchivesMeter, SS_SHOWMAX);
  
  char filename[MAX_FILENAME_LEN] = {0};
  GetRecdFileName(CurrMPAttrib.Term, filename);
  copy(METE_RECD_GW, filename);	 
	
}


//电表档案加载
void ArchivesMeterLoad()
{
  int count;
  use(METE_RECD_GW);
  count = libsumr();
  
  MeterCount = 0;
  //CurrMPAttrib.MPNum = 0;
  memset(ListBox_MeterItems, 0, sizeof(ListBox_MeterItems));
  ShowFormProgress(count, ArchivesMeterDoLoad);
}

//电表档案加载处理
void ArchivesMeterDoLoad(HWND Obj,PMsg Msg)
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
   
    if (CurrMPAttrib.NWType > 1)
    {
      libset(r, FD_COLLADDR);
      memset(temp, 0, sizeof(temp));
      libread(temp);
      if (strcmp(temp, CurrMPAttrib.Collect) != 0)
      {
        ProgressStep();
        continue;     
      }
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
    MeterCount++;      
    
    libset(r, FD_PORT);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    CurrMPAttrib.Port = StrToInt(temp, DEC);
    
    ProgressStep();
  }
  
  CloseFormProgress();
  use("");
}


//电表列表消息传递事件
void ListBox_ArchivesMeter_OnMsg(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_UP:
        if (ListBox_ArchivesMeter.index == 0)
        {
          ListBox_ArchivesMeter.index = ListBox_ArchivesMeter.count - 1;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_DOWN:
        if (ListBox_ArchivesMeter.index == ListBox_ArchivesMeter.count - 1)
        {
          ListBox_ArchivesMeter.index = 0;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_1:
        ArchivesMeterGoto(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_2:
        ArchivesMeterAdd(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_3:
        ArchivesMeterAmend(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_5:         
      case VK_DEL:
        ArchivesMeterDel(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_4:
        ArchivesMeterPort(NULL);
        Msg->Result = MSG_OK;
        break;
      case VK_0:
        ArchivesMeterHelp(NULL);
        Msg->Result = MSG_OK;
        break;         
    }
  }
}


//电表列表点击事件
void ArchivesMeterView(HWND Sender)
{
  if (ListBox_ArchivesMeter.count < 1)
  {
    return;
  }

  memset(CurrMPAttrib.Meter, 0, sizeof(CurrMPAttrib.Meter));
  sprintf(CurrMPAttrib.Meter, "%s", ListBox_ArchivesMeter.text);

  AttributeMeter();  
}


//跳转命令定位电表
void ArchivesMeterGoto(HWND Sender)
{
  
  int i;
	unsigned char meter[MAX_ASSETNO_DIGITS] = {};
	
  if (ListBox_ArchivesMeter.count < 1)
  {
    return;
  }
  
  if (InputBoxPro("跳转到", meter, sizeof(meter)) == ID_CANCEL)
  {
    return;
  }

//  GetMeteAddress(meter);
  GetAddress(meter, 2);

  for (i = 0; i < MeterCount; i++)
  {
    if (strcmp(meter, ListBox_MeterItems[i]) == 0)
    {
      ListBox_ArchivesMeter.index = i;
      Refresh(&ListBox_ArchivesMeter);
      break;
    }
  }
}


//电表档案添加
void ArchivesMeterAdd(HWND Sender)
{
  unsigned char meterNo[MAX_ASSETNO_DIGITS] = {0};
  unsigned char meterAddr[MAX_ASSETNO_DIGITS] = {0};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int r, i;
  
  if (ListBox_ArchivesMeter.count >= MAXMETERCOUNT - 1)
  {
  	MessageBox("电表数量已达上限!", STR_CUE, MB_OK);
  	return;
  }
  
  while (1)
  {
    memset(meterNo, 0, sizeof(meterNo));
    memset(meterAddr, 0, sizeof(meterAddr));
    if (ListBox_ArchivesMeter.count > 0)
    {
    	sprintf(meterNo, "%s", ListBox_ArchivesMeter.text);
    }
    if (InputBoxPro("电表编号", meterNo, sizeof(meterNo) - 1) == ID_CANCEL)
    {
      return;
    }
    strcpy(meterAddr, meterNo);    
    
    i = GetAddress(meterAddr, 2);
    
    if (i)
    {
      MessageBox(ConstConfERRStr[i], STR_CUE, MB_OK);
      return;
    }
                            
    /*for (i = 0; i < MeterCount; i++)
    {
      if (strcmp(meterAddr, ListBox_MeterItems[i]) == 0)
      {
        break;
      }
    }
    if (i < MeterCount)
    {
      MessageBox("电表已存在", STR_CUE, MB_OK);
      continue; 
    }*/
    
    if (LookUpMeter(meterAddr, METE_RECD_GW))
    {
      MessageBox("电表已存在", STR_CUE, MB_OK);
      continue;   		
    }

    WritMeterRecord(meterAddr, meterNo,ArchivesMeterTerminal,ArchivesMeterCollect);

    strcpy(ListBox_MeterItems[MeterCount], meterAddr);
    ListBox_ArchivesMeter.index = MeterCount;
    MeterCount++;
    ListBox_ArchivesMeter.count = MeterCount;
    Refresh(&ListBox_ArchivesMeter);

    if (InputBoxProResult == ID_OK)
    {
      break;
    }
  }
}


//电表档案删除
void ArchivesMeterDel(HWND Sender)
{
  int count;

  if (ListBox_ArchivesMeter.count < 1)
  {
    return;
  }
    
  if (MessageBox("确定要删除该电表吗？", 
    STR_CUE, MB_YESNO) == ID_NO)
  {
    return;
  }
  
  use(METE_RECD_GW);
  count = libsumr();
  ShowFormProgress(count, ArchivesMeterDoDel);
}

//电表档案删除处理
void ArchivesMeterDoDel(HWND Obj,PMsg Msg)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int r, i;
  
  for (r = ProgressBar_Proc.max - 1; r >= 0; r--)
  {
  	
  	memset(temp, 0, sizeof(temp));
  	libset(r, 0);
  	libread(temp);
  	if (strcmp(temp, STR_DEL) == 0)
  	{
  		ProgressStep();
  		continue;
  	}

		memset(temp, 0, sizeof(temp));
    libset(r, FD_METERADDR);
    libread(temp);
    if (strcmp(temp, ListBox_ArchivesMeter.text) == 0)
    {
      break;
    }
    ProgressStep();
  }
  
  CloseFormProgress();
  
  if (1 == ListBox_ArchivesMeter.count)
  {
  	libset(r, FD_METERADDR);
  	libwrit("");
  }
  else
  {
  	libset(r, 0);
  	libwrit(STR_DEL);
  }
  use("");

  for (i = ListBox_ArchivesMeter.index + 1; i < ListBox_ArchivesMeter.count; i++)
  {
    strcpy(ListBox_MeterItems[i - 1], ListBox_MeterItems[i]);
    if (i == ListBox_ArchivesMeter.count - 1)
    {
      memset(ListBox_MeterItems[i], 0, sizeof(TItem));
    }
  }
  MeterCount--;
  ListBox_ArchivesMeter.count = MeterCount;
  Refresh(&ListBox_ArchivesMeter);
}


//抄表端口选择
void ArchivesMeterPort(HWND Sender)
{
  int count = 0;
  U8 port = 0;
  TItem portItems[] = {"端口1", "端口2", "端口3", "端口4", "端口31",};
  
  count = sizeof(portItems)/sizeof(TItem);
  port = CurrMPAttrib.Port;
  if (port == 31)
  {
  	port = count - 1;
  }
  else if (port > 0 && port < count)
  {
  	port--;
  }
  else
  {
  	port = 0;
  }
  
  if (FormSelecte("请选择端口号", &portItems, count, &port, SS_SHOWDEFAULT) == ID_CANCEL)
  {
    return;
  }
  
  port++;
  if (port == count - 1)
  {
  	port = 31;
  }
  
  CurrMPAttrib.Port = port;
}


//电表档案写入数据库
/*void WritMeterRecord(char *meter, char *meterno)
{
  char temp[MAX_ASSETNO_DIGITS] = {0};
  int i, j, r;
  int mpNo = 1;
  unsigned char appendFlag = 1;
  
  use(METE_RECD_GW);
  r = libsumr();
  
  for(i = 0; i < r; i++)
  {
    memset(temp, 0, sizeof(temp));
    libset(i, 0);
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      continue;
    }  
       
    memset(temp, 0, sizeof(temp));
    libset(i, FD_COLLADDR);
    libread(temp);
    if (strcmp(CurrMPAttrib.Collect, temp) != 0)
    {
      continue;
    }   
    
    memset(temp, 0, sizeof(temp));
    libset(i, FD_METERADDR);
    libread(temp);   
    if (temp[0] = '\0')
    {
    	appendFlag = 0;
    	break;
    }      
  }
  
  if (appendFlag)
  { 
	  libappend();
	  r = libgetr();  
	}
	else
	{
		r = i;
	} 
	
	if (0 == r)
	{
		libset(r, FD_TERMNO);
	  libwrit(ArchivesMeterTerminal);
	  libset(r, FD_TERMADDR);
	  libwrit(CurrMPAttrib.Term);  
	}
	 
  libset(r, FD_COLLNO);
  libwrit(ArchivesMeterCollect);
  libset(r, FD_COLLADDR);
  libwrit(CurrMPAttrib.Collect);  
  libset(r, FD_METERNO);
  libwrit(meterno);
  libset(r, FD_METERADDR);
  libwrit(meter);
   
  if (CurrMPAttrib.NWType > 1)
  {
  	mpNo = r + 2;  	
  }
  else
  {
	  mpNo = r + 1;
	}
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", mpNo);
  libset(r, FD_DEVNO);
  libwrit(temp);
  libset(r, FD_MPNO);
  libwrit(temp);
  
  libset(r, FD_PORT);
  memset(temp, 0, sizeof(temp));
  if (CurrMPAttrib.Port == 0)
  {
	  switch(CurrMPAttrib.NWType)
	  { 
	    case 0:
	    default:
	    	CurrMPAttrib.Port = 1;
	      break;
	    case 1:
	    case 3:
	    	CurrMPAttrib.Port = 31;
	    	break;
	    case 2:
	      CurrMPAttrib.Port = 2;
	      break;
	  }
	}
	sprintf(temp, "%d", CurrMPAttrib.Port);
	libwrit(temp);	
  
  libset(r, FD_RATES);
  libwrit("4");  
  libset(r, FD_SORT);
  libwrit("51");
  
  use("");
} */


//电表档案修改
void ArchivesMeterAmend(HWND Sender)
{
  unsigned char meterAddr[MAX_ASSETNO_DIGITS] = {0};
  unsigned char meterNo[MAX_ASSETNO_DIGITS] = {0};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int i, r;
  int index;
   
  if (ListBox_ArchivesMeter.count < 1)
  {
    return;
  }
  index = ListBox_ArchivesMeter.index;
   
  while (1)
  {
    memset(meterAddr, 0, sizeof(meterAddr));
    sprintf(meterAddr, "%s", ListBox_ArchivesMeter.text);
    if (InputBoxPro("电表编号", meterAddr, sizeof(meterAddr)) == ID_CANCEL)
    {
      return;
    }
    strcpy(meterNo, meterAddr);
    i = GetAddress(meterAddr, 2);
    if (i)
    {
      MessageBox(ConstConfERRStr[i], STR_CUE, MB_OK);
      return;
    }
    
    if (LookUpMeter(meterAddr, METE_RECD_GW))
    {
      MessageBox("电表已存在", STR_CUE, MB_OK);
      return;   		
    }       

    use(METE_RECD_GW);
    r = libsumr();
    for(i = 0; i < r; i++)
    {
      libset(i, 0);
      memset(temp, 0, sizeof(temp));
      libread(temp);
      if (strcmp(temp, STR_DEL) == 0)
      {
        continue;
      }
       
      libset(i, FD_METERADDR);
      memset(temp, 0, sizeof(temp));
      libread(temp);
      if (strcmp(temp, ListBox_ArchivesMeter.text) == 0)
      {
        break;  
      }     
    }
    libset(i, FD_METERNO);
    libwrit(meterNo);
    libset(i, FD_METERADDR);
    libwrit(meterAddr);  

    use("");

    memset(ListBox_MeterItems[index], 0, sizeof(ListBox_MeterItems[index]));
    strcpy(ListBox_MeterItems[index], meterAddr);
       
    ListBox_ArchivesMeter.index = index;
    Refresh(&ListBox_ArchivesMeter);
    
    break;
  }
}

//数据库查找电表
/*
U8 LookUpMeter(unsigned char *meter)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int i, j, r;
  
  use(METE_RECD_GW);
  r = libsumr();
  
  for(i = 0; i < r; i++)
  {
    memset(temp, 0, sizeof(temp));
    libset(i, 0);
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      continue;
    }  
             
    memset(temp, 0, sizeof(temp));
    libset(i, FD_METERADDR);
    libread(temp);
    if(strncmp(meter, temp, NUM_12) == 0)
    {
    	break;     
    }    
  }
  use("");
  
  if(i != r)
  {
     return 1;
  }
    
  return 0;
}*/


void ArchivesMeterHelp(HWND Sender)
{
  MessageBox("当光标位于电表列表时\n确认键：配置电表属性\n按键1：跳转\n按键2：添加\n按键3：修改\n按键4：清空\n按键5/删除键：删除", "帮助", MB_OK); 
}
#endif
