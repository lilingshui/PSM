#ifndef _ArchiveCollect_H
#define _ArchiveCollect_H

#include "jblib.h"
#include "Common.h"
#include "FormProgress.h"
#include "InputBoxPro.h"
#include "ArchivesMeter.h"

unsigned char *TerminalAsset;
ListBox ListBox_ArchivesCollect;

void ArchivesCollectLoad();
void ArchivesCollectDoLoad(HWND Obj,PMsg Msg);
void ListBox_ArchivesCollect_OnMsg(HWND Obj, PMsg Msg);
void ArchivesCollectView(HWND Sender);
void ArchivesCollectGoto(HWND Sender);
void ArchivesCollectAdd(HWND Sender);
void ArchivesCollectDel(HWND Sender);
void ArchivesCollectDoDel(HWND Obj,PMsg Msg);
void ArchivesCollectClear(HWND Sender);
void ArchivesCollectDoClear(HWND Obj,PMsg Msg);
void ArchivesCollectHelp(HWND Sender);
void ArchivesCollectAmend(HWND Sender);



void ArchivesCollect(unsigned char *terminal)
{
  TerminalAsset = terminal;
  Window Form_ArchivesCollect;
  Button Button_Goto, Button_Add, Button_Del, Button_Amend, Button_Help;

  ArchivesCollectLoad();
  
  CreateListBox(&ListBox_ArchivesCollect, ListBox_CollectItems, 
    CollectCount, 3, 3, 115, 124);
  ListBox_ArchivesCollect.OnMsg = ListBox_ArchivesCollect_OnMsg;  
  ListBox_ArchivesCollect.OnClick = ArchivesCollectView;
  CreateButton(&Button_Goto, "跳转1", 119, 3, 38, 20);
  Button_Goto.OnClick = ArchivesCollectGoto;
  CreateButton(&Button_Add, "添加2", 119, 29, 38, 20);
  Button_Add.OnClick = ArchivesCollectAdd;
  CreateButton(&Button_Amend, "修改3", 119, 55, 38, 20);
  Button_Amend.OnClick = ArchivesCollectAmend;  
  Button_Del.OnClick = ArchivesCollectDel;
  CreateButton(&Button_Del, "删除4", 119, 81, 38, 20);
  Button_Help.OnClick = ArchivesCollectHelp;
  CreateButton(&Button_Help, "帮助5", 119, 107, 38, 20);
  

  CreateWindow(&Form_ArchivesCollect, "", 0, 0, 160, 160);
  sprintf(Form_ArchivesCollect.text, "终端%s采集器", CurrMPAttrib.Term);
  AddToWindow(&Form_ArchivesCollect, &ListBox_ArchivesCollect);
  AddToWindow(&Form_ArchivesCollect, &Button_Goto);
  AddToWindow(&Form_ArchivesCollect, &Button_Add);
  AddToWindow(&Form_ArchivesCollect, &Button_Amend);
  AddToWindow(&Form_ArchivesCollect, &Button_Del);
  AddToWindow(&Form_ArchivesCollect, &Button_Help);
    
  ShowWindow(&Form_ArchivesCollect, SS_SHOWMAX);
  
  char filename[MAX_FILENAME_LEN] = {0};
  GetRecdFileName(CurrMPAttrib.Term, filename);
  copy(METE_RECD_GW, filename);	  
}


//采集器档案加载
void ArchivesCollectLoad()
{
  int count;
  use(METE_RECD_GW);
  count = libsumr();
  
  CollectCount = 0;
  memset(ListBox_CollectItems, 0, sizeof(ListBox_CollectItems));
  ShowFormProgress(count, ArchivesCollectDoLoad);
}

//采集器档案加载处理
void ArchivesCollectDoLoad(HWND Obj,PMsg Msg)
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
void ListBox_ArchivesCollect_OnMsg(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_UP:
        if (ListBox_ArchivesCollect.index == 0)
        {
          ListBox_ArchivesCollect.index = ListBox_ArchivesCollect.count - 1;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_DOWN:
        if (ListBox_ArchivesCollect.index == ListBox_ArchivesCollect.count - 1)
        {
          ListBox_ArchivesCollect.index = 0;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_1:
        ArchivesCollectGoto(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_2:
        ArchivesCollectAdd(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_DEL:
      case VK_4:
        ArchivesCollectDel(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_3:
        ArchivesCollectAmend(NULL);
        Msg->Result = MSG_OK;
        break;
      case VK_5:
      case VK_0:
        ArchivesCollectHelp(NULL);
        Msg->Result = MSG_OK;
        break; 
    }
  }
}


//采集器列表点击事件
void ArchivesCollectView(HWND Sender)
{
	int r, count;
	unsigned char collect[MAX_ASSETNO_DIGITS] = {0};
	unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
	
  if (ListBox_ArchivesCollect.count < 1)
  {
    return;
  }
  
  memset(CurrMPAttrib.Collect, 0, sizeof(CurrMPAttrib.Collect));
  sprintf(CurrMPAttrib.Collect, "%s", ListBox_ArchivesCollect.text);  
  
  use(METE_RECD_GW);
  count = libsumr();
  for (r = 0; r < count; r++)
  {   
    libset(r, FD_FLAG);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
    	continue;
    }
        
    libset(r, FD_COLLADDR);
    memset(collect, 0, sizeof(collect));
    libread(collect);     
    if (strcmp(collect, ListBox_ArchivesCollect.text) == 0)
    {
    	break;
    }     	
  }  
  libset(r, FD_COLLNO);
  memset(collect, 0, sizeof(collect));
  libread(collect);   
  use("");
  
  ArchivesMeter(TerminalAsset, collect); 
}


//跳转命令查找定位采集器
void ArchivesCollectGoto(HWND Sender)
{
  unsigned char collect[MAX_ASSETNO_DIGITS] = {0};
  int i;
  
  if (ListBox_ArchivesCollect.count < 1)
  {
  	return;
  }

  if (InputBoxPro("跳转到", collect, sizeof(collect)) == ID_CANCEL)
  {
    return;
  }

  GetAddress(collect, 4);

  for (i = 0; i < CollectCount; i++)
  {
    if (strcmp(collect, ListBox_CollectItems[i]) == 0)
    {
      ListBox_ArchivesCollect.index = i;
      Refresh(&ListBox_ArchivesCollect);
      break;
    }
  }
  
}


//采集器添加
void ArchivesCollectAdd(HWND Sender)
{
  unsigned char collectNo[MAX_ASSETNO_DIGITS] = {};
  unsigned char collectAddr[MAX_ASSETNO_DIGITS] = {};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {};
  int r, i;
  
  if (ListBox_ArchivesCollect.count >= MAXCOLLECTCOUNT - 1)
  {
  	MessageBox("采集器数量已达上限!", STR_CUE, MB_OK);
  	return;
  }
  
  while (1)
  {
    memset(collectNo, 0, sizeof(collectNo));
    memset(collectAddr, 0, sizeof(collectAddr));
    if (ListBox_ArchivesCollect.count > 0)
    {
    	sprintf(collectNo, "%s", ListBox_ArchivesCollect.text);
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
      continue; 
    }

		//将采集器档案存入测量点档案数据库 
	  use(METE_RECD_GW);	
	  libappend();
	  r = libgetr();
	  libset(r, FD_TERMNO);
	  libwrit(TerminalAsset);
	  libset(r, FD_TERMADDR);
	  libwrit(CurrMPAttrib.Term);     
	  libset(r, FD_COLLNO);
	  libwrit(collectNo);
	  libset(r, FD_COLLADDR);
	  libwrit(collectAddr);	  
	  use("");

    strcpy(ListBox_CollectItems[CollectCount], collectAddr);
    ListBox_ArchivesCollect.index = CollectCount;
    CollectCount++;
    ListBox_ArchivesCollect.count = CollectCount;
    Refresh(&ListBox_ArchivesCollect);
    
    break;
  }
  
  if (InputBoxProResult == ID_SCANOK)
  {
    ArchivesCollectView(ListBox_ArchivesCollect.handle);
  }
  
}


//采集器删除
void ArchivesCollectDel(HWND Sender)
{
  int count;

  if (ListBox_ArchivesCollect.count < 1)
  {
    return;
  }
  
  if (MessageBox("删除采集器将连带删除采集器下所有电表。\n确定要删除吗？", 
    STR_CUE, MB_YESNO) == ID_NO)
  {
    return;
  }
  
  use(METE_RECD_GW);
  count = libsumr();
  ShowFormProgress(count, ArchivesCollectDoDel);
}

//采集器删除处理
void ArchivesCollectDoDel(HWND Obj,PMsg Msg)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int i, r;
  
  for (r = 0; r < ProgressBar_Proc.max; r++)
  {
    libset(r, FD_COLLADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, ListBox_ArchivesCollect.text) == 0)
    {
	    libset(r, 0);
	    libwrit(STR_DEL);   
    }
    ProgressStep();
  }
  CloseFormProgress();
  use("");
  
  for (i = ListBox_ArchivesCollect.index + 1; i < ListBox_ArchivesCollect.count; i++)
  {
    strcpy(ListBox_CollectItems[i - 1], ListBox_CollectItems[i]);
    if (i == ListBox_ArchivesCollect.count - 1)
    {
      memset(ListBox_CollectItems[i], 0, sizeof(TItem));
    }
  }
  CollectCount--;
  ListBox_ArchivesCollect.count = CollectCount;
  Refresh(&ListBox_ArchivesCollect);  
  
}


//采集器档案修改
void ArchivesCollectAmend(HWND Sender)
{
  unsigned char collect[MAX_ASSETNO_DIGITS] = {0};
  unsigned char collectno[MAX_ASSETNO_DIGITS] = {0};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int i, r;
  int index;
    
  if (ListBox_ArchivesCollect.count < 1)
  {
    return;
  }
  index = ListBox_ArchivesCollect.index;
 
  while (1)
  {
    memset(collect, 0, sizeof(collect));
    sprintf(collect, "%s", ListBox_ArchivesCollect.text);
    if (InputBoxPro("采集器编号", collect, sizeof(collect)) == ID_CANCEL)
    {
      return;
    }
    strcpy(collectno, collect);
    i = GetAddress(collect, 4);
    if (i)
    {
      MessageBox(ConstConfERRStr[i], STR_CUE, MB_OK);
      return;
    }
    
    for (i = 0; i < CollectCount; i++)
    {
      if (i == ListBox_ArchivesCollect.index)
      {
        continue;
      }
      if (strcmp(collect, ListBox_CollectItems[i]) == 0)
      {
        break;
      }
    }
    if (i < CollectCount && i != ListBox_ArchivesCollect.index)
    {
      MessageBox("采集器已存在", STR_CUE, MB_OK);
      continue; 
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

    	libset(i, FD_COLLADDR);
    	memset(temp, 0, sizeof(temp));
    	libread(temp);
    	if (strcmp(temp, ListBox_ArchivesCollect.text) == 0)
    	{
		    libset(i, FD_COLLNO);
		    libwrit(collectno);
		    libset(i, FD_COLLADDR);
		    libwrit(collect);    		
    	}   	
    }

    use("");

    memset(ListBox_CollectItems[index], 0, sizeof(ListBox_CollectItems[index]));
    strcpy(ListBox_CollectItems[index], collect);
       
    ListBox_ArchivesCollect.index = index;
    Refresh(&ListBox_ArchivesCollect);
    
    break;
  }

}


void ArchivesCollectHelp(HWND Sender)
{
  MessageBox("光标位于采集器列表时\n确认键：电表档案管理\n按键1：跳转\n按键2：添加\n按键3：修改\n按键4/删除键：删除\n按键5：帮助", "帮助", MB_OK); 
}
#endif
