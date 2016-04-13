#ifndef _TermOperate_H
#define _TermOperate_H

#include "jblib.h"
#include "Common.h"
#include "FormProgress.h"
#include "InputBoxPro.h"
#include "FormSelecte.h"
#include "MeterOperate.h"
#include "TermParamManage.h"
#include "DataConvert.h"
#include "InputBoxGroupPro.h"
#include "Set_Addr_Time.h"
#include "SystemConf.h" 
ListBox ListBox_TermOperate;

unsigned char ReadNum;
unsigned char FailNum;

void TermOperateLoad();
void TermOperateDoLoad(HWND Obj,PMsg Msg);
void ListBox_TermOperate_OnMsg(HWND Obj, PMsg Msg);
void TermOperateView(HWND Sender);
void TermOperateGoto(HWND Sender);
void TermOperateReset(HWND Sender);
void TermOperateDoReset(HWND Obj,PMsg Msg);
void TermOperateHelp(HWND Sender);

void BatchReadData(HWND Sender);
void BatchReadMPData(HWND Sender);

void BatchReadRTDataDoSend(HWND Obj, PMsg Msg);
void BatchReadHisDataDoSend(HWND Obj, PMsg Msg);
void BatchReadDataDoSend(HWND Obj, PMsg Msg);

void TermOperateSetAddr(HWND Sender);

void TermOperate(HWND Sender)
{
  Window Form_TermOperate;
  Button Button_Goto, Button_Send, Button_Read, Button_Data, Button_Reset;

  if (GetCommParam(&CommParam, 0) == ERROR)
  {
  	MessageBox(ConstConfERRStr[1], STR_CUE,MB_OK);
  	return;
  }
     
  TermOperateLoad();

  CreateListBox(&ListBox_TermOperate, ListBox_TerminalItems, 
    TerminalCount, 3, 3, 110, 124);
  ListBox_TermOperate.OnMsg = ListBox_TermOperate_OnMsg;  
  ListBox_TermOperate.OnClick = TermOperateView;
  CreateButton(&Button_Goto, "添加1", 115, 3, 40, 20);
  Button_Goto.OnClick = TermOperateGoto;
  CreateButton(&Button_Send, "设置2", 115, 29, 40, 20);
  Button_Send.OnClick = TermOperateSetParam;
  CreateButton(&Button_Read, "查询3", 115, 55, 40, 20);
  Button_Read.OnClick = TermOperateReadParam;
  CreateButton(&Button_Data, "抄表4", 115, 81, 40, 20);
  Button_Data.OnClick = BatchReadData;
  CreateButton(&Button_Reset, "重置5", 115, 107, 40, 20);
  Button_Reset.OnClick = TermOperateReset;

  CreateWindow(&Form_TermOperate, "终端参数管理及数据查询", 0, 0, 160, 160);
  AddToWindow(&Form_TermOperate, &ListBox_TermOperate);
  AddToWindow(&Form_TermOperate, &Button_Goto);
  AddToWindow(&Form_TermOperate, &Button_Send);
  AddToWindow(&Form_TermOperate, &Button_Read);
  AddToWindow(&Form_TermOperate, &Button_Data);
  AddToWindow(&Form_TermOperate, &Button_Reset);
    
  ShowWindow(&Form_TermOperate, SS_SHOWMAX);
}


//终端列表加载
void TermOperateLoad()
{
  int count;
  use(TERM_RECD);
  count = libsumr();
  
  TerminalCount = 0;
  memset(ListBox_TerminalItems, 0, sizeof(ListBox_TerminalItems));
  ShowFormProgress(count, TermOperateDoLoad);
}

//终端列表加载处理
void TermOperateDoLoad(HWND Obj,PMsg Msg)
{
  int r, i;
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0}; 
  
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
     
    libset(r, FD_TERMADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    /*for (i = 0; i < TerminalCount; i++)
    {
      if (strcmp(temp, ListBox_TerminalItems[i]) == 0)
      {
        break;
      }
    }
    if (i == TerminalCount)
    {
      strcpy(ListBox_TerminalItems[TerminalCount], temp);
      TerminalCount++;
    } */
    strcpy(ListBox_TerminalItems[TerminalCount], temp);
    TerminalCount++;     
    ProgressStep();
  }
  CloseFormProgress();
  use("");
}


//终端列表消息传递事件
void ListBox_TermOperate_OnMsg(HWND Obj, PMsg Msg)
{
	
  memset(CurrMPAttrib.Term, 0, sizeof(CurrMPAttrib.Term));
  sprintf(CurrMPAttrib.Term, "%s", ListBox_TermOperate.text);
  
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_UP:
        if (ListBox_TermOperate.index == 0)
        {
          ListBox_TermOperate.index = ListBox_TermOperate.count - 1;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_DOWN:
        if (ListBox_TermOperate.index == ListBox_TermOperate.count - 1)
        {
          ListBox_TermOperate.index = 0;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_1:
        TermOperateGoto(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_2:
        TermOperateSetParam(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_3:
        TermOperateReadParam(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_4:
        BatchReadData(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_5:  
      case VK_DEL:
        TermOperateReset(NULL);
        Msg->Result = MSG_OK;
        break;        
      case VK_0:        
        TermOperateHelp(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_FUNCTION:        
        TermOperateSetAddr(NULL);
        Msg->Result = MSG_OK;
        break;
      case VK_DOT:        
        SetTermCommType(NULL);
        if (GetCommParam(&CommParam, 0) == ERROR)
			  {
			  	MessageBox(ConstConfERRStr[1], STR_CUE,MB_OK);
			  	return;
			  }
        Msg->Result = MSG_OK;
        break;        
    }
  }
}


//终端列表点击事件
void TermOperateView(HWND Sender)
{
  
 	char filename[MAX_FILENAME_LEN] = {0};
	
  if (ListBox_TermOperate.count < 1)
  {
    return;
  }
    
  if (GetRecdFileName(ListBox_TermOperate.text, filename) != NULL)
  {
  	copy(filename, METE_RECD_GW);  
  }  
  else
  {
  	copy(METE_RECD_NEW_GW, METE_RECD_GW); 
  }
    
  copy(METE_RECD_GW, METE_DATA_RCV_GW);
   
  memset(CurrMPAttrib.Term, 0, sizeof(CurrMPAttrib.Term));
  sprintf(CurrMPAttrib.Term, "%s", ListBox_TermOperate.text);
   
  MeterOperate(ListBox_TermOperate.text); 
}


//跳转定位终端
/*
void TermOperateGoto(HWND Sender)
{
  unsigned char term[MAX_ASSETNO_DIGITS] = {0};
  int i;

	if (ListBox_TermOperate.count < 1)
	{
		return;
	}
	
  if (InputBoxPro("跳转到", term, sizeof(term)) == ID_CANCEL)
  {
    return;
  }
  GetAddress(term, 0);

  for (i = 0; i < TerminalCount; i++)
  {
    if (strcmp(term, ListBox_TerminalItems[i]) == 0)
    {
      ListBox_TermOperate.index = i;
      Refresh(&ListBox_TermOperate);
      break;
    }
  }
  
}*/
void TermOperateGoto(HWND Sender)
{
  unsigned char term[MAX_ASSETNO_DIGITS] = {0};
  unsigned char termno[MAX_ASSETNO_DIGITS] = {0};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int i, r;
  
  if (ListBox_TermOperate.count >= MAXTERMINALCOUNT - 1)
  {
  	MessageBox("终端数量已达上限,请进行删除或清空操作!", STR_CUE, MB_OK);
  	return;
  }
 
  while (1)
  {
    memset(term, 0, sizeof(term));
    if (ListBox_TermOperate.count > 0)
    {
    	sprintf(term, "%s", ListBox_TermOperate.text);
    }
    
    if (InputBoxPro("终端编号", term, sizeof(term)) == ID_CANCEL)
    {
      return;
    }
    
    strcpy(termno, term);
    
    i = GetAddress(term, 0);
    if (i)
    {
    	MessageBox(ConstConfERRStr[i], STR_CUE, MB_OK);
    	return;
    }       

    for (i = 0; i < TerminalCount; i++)
    {
      if (strcmp(term, ListBox_TerminalItems[i]) == 0)
      {
        break;
      }
    }
    if (i < TerminalCount)
    {
      ListBox_TermOperate.index = i;
      Refresh(&ListBox_TermOperate);
      MessageBox("终端已存在", STR_CUE, MB_OK);
      
      break; 
    }

		//保存终端档案至终端档案数据库    
    use(TERM_RECD);    
    libappend();
    r = libgetr();
    libset(r, FD_TERMNO);
    libwrit(termno);
    libset(r, FD_TERMADDR);
    libwrit(term);
    libset(r, FD_ADDRTYPE);
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%d", CurrMPAttrib.AddrType);
    libwrit(temp);
    use("");
    
    strcpy(ListBox_TerminalItems[TerminalCount], term);
    ListBox_TermOperate.index = TerminalCount;
    TerminalCount++;
    ListBox_TermOperate.count = TerminalCount;
    Refresh(&ListBox_TermOperate);
    
    break;
  }

  /*if (InputBoxProResult == ID_SCANOK)
  {
    ArchivesTerminalView(ListBox_TermOperate.handle);
  }*/
}

void TermOperateSend(HWND Sender)
{

  if (ListBox_TermOperate.index < 0)
  {
    return;
  }

//  SetTermParam(ListBox_TermOperate.text);  
}
void TermOperateRead(HWND Sender)
{

  if (ListBox_TermOperate.index < 0)
  {
    return;
  }

//  ReadTermParam(ListBox_TermOperate.text);  
}


//重置数据抄读结果
void TermOperateReset(HWND Sender)
{
  int count;
  char filename[MAX_FILENAME_LEN] = {0};

  if (ListBox_TermOperate.count < 1)
  {
    return;
  }
  
  if (GetRecdFileName(ListBox_TermOperate.text, filename) == NULL)
  {
  	MessageBox("未找到该终端的测量点档案文件!", STR_CUE, MB_OK);
  	return;
  }
  //copy(filename, METE_RECD_GW);
  
  if (MessageBox("确定要重置该终端数据抄读结果吗？", 
    STR_CUE, MB_YESNO) == ID_NO)
  {
    return;
  }
    
  use(filename);
  count = libsumr();
  ShowFormProgress(count, TermOperateDoReset);
}


void TermOperateDoReset(HWND Obj,PMsg Msg)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {};
  int r;
  
  for (r = 0; r < ProgressBar_Proc.max; r++)
  {
  	libset(r, FD_RTDATA);
  	libwrit("");
  	
  	libset(r, FD_DAYDATA);
  	libwrit("");
  	
		libset(r, FD_RESERVE);
		libwrit("");
  	        
    ProgressStep();
  }
  CloseFormProgress();
  use("");
}


void TermOperateHelp(HWND Sender)
{
  MessageBox("当光标位于终端列表时\n确认键：电表操作\n按键1：跳转\n按键2：设置\n按键3：查询\n按键4：抄表\n按键5：重置", "帮助", MB_OK); 
}


//批量抄读测量点数据
void BatchReadData(HWND Sender)
{
	if (ListBox_TermOperate.count < 1)
  {
    return;
  }
  
  Window Form_ReadData;
	TMenu Menu_ReadData;
	TMenuItem ReadDataItems[] = {
		{NULL, NULL, "1.实时正向有功"},
		{NULL, NULL, "2.日冻结正向有功"},
		{NULL, NULL, "3.透抄正向有功"},
	};
	
	CreateWindow(&Form_ReadData, "", 0,0,160,160);
	sprintf(Form_ReadData.text, "终端%s数据抄读", ListBox_TermOperate.text);
	
	CreateTMenu(&Menu_ReadData, &ReadDataItems,
							sizeof(ReadDataItems)/sizeof(TMenuItem), 10,10,140,110,FONT_CH_12|FONT_EN_12);
	Menu_ReadData.OnClick = BatchReadMPData;
	
	AddToWindow(&Form_ReadData, &Menu_ReadData);
  	
	ShowWindow(&Form_ReadData, SS_SHOWMAX);	  
}


//批量抄读电表数据处理
void BatchReadMPData(HWND Sender)
{

  int index = 0;
  int count = 0;
  int step = 0;
  int startindex = 0; 
  char V_sendnum[13] = {0};
  char V_startnum[13] = {0};
  char showInfo[128] = {0};
  char filename[MAX_FILENAME_LEN] = {0};
  
  index = ((TMenu*)Sender)->index;
    
  if (GetRecdFileName(ListBox_TermOperate.text, filename) == NULL)
  {
		MessageBox("没有当前终端的测量点档案!", STR_CUE, MB_OK);  	
  	return;   
  } 
  else
  {
  	GetMPLocate(&CurrMPAttrib, filename);
	  if (CurrMPAttrib.MPNum < 1)
	  {
	  	MessageBox("当前终端测量点档案为空!", STR_CUE, MB_OK);
	  	return;	  	
	  }
	  copy(filename, METE_DATA_RCV_GW);
  }
  
  memset(CurrMPAttrib.Term, 0, sizeof(CurrMPAttrib.Term));
  sprintf(CurrMPAttrib.Term, "%s", ListBox_TermOperate.text);
        
  sprintf(V_sendnum, "本次抄读数量");
  sprintf(V_startnum, "起始装置序号");
  
  if (InputBoxGroupPro("请输入", V_sendnum, V_startnum) == 0)
  {
    return;
  } 
  
  step = StrToInt(V_sendnum, DEC); 
  startindex = StrToInt(V_startnum, DEC);
  
  if (step < 1 || step > CurrMPAttrib.MPNum)
  {
    step = CurrMPAttrib.MPNum;
  } 
  
  if (startindex < 1 || startindex > MAX_MPNO)
  {
  	startindex = 1;
  }
  if (startindex + step > CurrMPAttrib.MPNum + 1)
  {
  	step = CurrMPAttrib.MPNum - startindex + 1;
  }  
 
  CurrMPAttrib.Flag = startindex - 1;

  uartinit(CommParam.Baud, CommParam.Parity, CommParam.Port);
  Delay(DELAY_OPENPORT);
  
  FailNum = 0;   
  
  memset(GlobalParam, 0, sizeof(GlobalParam)); 
  memset(GlobalParamItem, 0, sizeof(GlobalParamItem));
  
  ReadNum = 0;
  
  switch(index)
  {
  	case 0:
  	{
  		GlobalParam->AFN = AFN_698RTDATA;
  		GlobalParam->Fn = FN_129;
		  if (!GetParamIDFromDBF(GlobalParam, 0, PARAMDBF_GW))
		  {
		  	return;
		  }  		
		  GetParamValueFromDBF(GlobalParam, GlobalParamItem, PARAMDBF_GW);
  		ShowFormProgress(step, BatchReadRTDataDoSend);
  		break;
  	}
  	case 1:
  	{
  		GlobalParam->AFN = AFN_698HISDATA;
  		GlobalParam->Fn = FN_161;  
		  
		  ReadHisDataDate = GetHisDataDate(ReadHisDataDate);
		  if (!ReadHisDataDate)
		  {
		  	return;
		  }
		  
  	  if (!GetParamIDFromDBF(GlobalParam, 0, PARAMDBF_GW))
		  {
		  	return;
		  }  		
		  GetParamValueFromDBF(GlobalParam, GlobalParamItem, PARAMDBF_GW);
  		ShowFormProgress(step, BatchReadHisDataDoSend);
  		break;
  	}
  	case 2:
  		ShowFormProgress(step, BatchReadDataDoSend);
  		break;
  	default:
  		return;
  }

  sprintf(showInfo, "抄读总数量 %d\n起始装置序号 %d\n抄读成功个数 %d\n抄读失败个数 %d", 
  				step, startindex, ReadNum, FailNum);
  MessageBox(showInfo, STR_CUE, MB_OK);
    
  while (FailNum != 0)
  { 
  	if (CurrMPAttrib.Flag < 0)
		{
			break;
		}
	
    if (MessageBox("是否抄读失败项?", STR_CUE, MB_YESNO) == ID_NO)
    {
      break;
    }

    FailNum = 0;
    CurrMPAttrib.Flag = startindex - 1;
    switch(index)
	  {
	  	case 0:
	  		ShowFormProgress(step, BatchReadRTDataDoSend);
	  		break;
	  	case 1:
	  		ShowFormProgress(step, BatchReadHisDataDoSend);
	  		break;
	  	case 2:
	  		ShowFormProgress(step, BatchReadDataDoSend);
	  		break;
	  	default:
	  		return;
	  }
        
    memset(showInfo, 0, sizeof(showInfo));
    sprintf(showInfo, "抄读总数量 %d\n抄读成功个数 %d\n抄读失败个数 %d", step, ReadNum, FailNum);
    MessageBox(showInfo, STR_CUE, MB_OK);
  }
  uartinit(0, CommParam.Parity, CommParam.Port);
    
  if (CurrMPAttrib.Flag == -2)
  {
  	MessageBox(ConstResStr[2], STR_CUE, MB_OK);
  	return;  	
  }   
   
  copy(METE_DATA_RCV_GW, filename);
  GetMPLocate(&CurrMPAttrib, METE_DATA_RCV_GW);
  CurrMPAttrib.Flag = startindex - 1;   
  ShowReadDataResult(index);  
}


//批量抄读测量点实时数据处理Fn129
void BatchReadRTDataDoSend(HWND Obj, PMsg Msg)
{
  int i = 0;
  int j = 0;
  int r = 0;
  int mpNo = 1;
  int rlen = 0;
  unsigned short slen = 0;
  
  unsigned char sendbuff[MINSENDBYTES] = {0};
  unsigned char recvbuff[1024] = {0};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  Stru698Data Send698Data;
  
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, ListBox_TermOperate.text);
  
  Send698Data.Ccode = C_698READ;
  Send698Data.AFN = AFN_698RTDATA;
  Send698Data.SEQ = INI_698SEQ;
  Send698Data.Fn = FN_129;
  Send698Data.Ulen = 0;
    
  for(i = 0; i < ProgressBar_Proc.max; i++)
  {
  	
  	use (METE_DATA_RCV_GW);
  	  	  	
  	r = CurrMPAttrib.MPLocate[CurrMPAttrib.Flag++];
  	libset(r, FD_RTDATA);
  	memset(temp, 0, sizeof(temp));
  	libread(temp);
  	
  	if (strstr(temp, ".") != NULL)
  	{
  		use("");
  		ProgressStep();
  		continue;
  	}
	  	
  	libset(r, FD_DEVNO);
  	memset(temp, 0, sizeof(temp));
  	libread(temp);
  	Send698Data.Pn = StrToInt(temp, DEC); 
  	
  	use("");
             
    if (Send698Data.SEQ == 0xEF)
    {
      Send698Data.SEQ = 0xE0;
    }
    Send698Data.SEQ++;
    
    memset(sendbuff, 0, sizeof(sendbuff));
    slen = Build698SendBuff(sendbuff, &Send698Data);
    
    SendData(sendbuff, slen);

    rlen = RecvData(recvbuff, sizeof(recvbuff) - 1, 2000);
    Beep(100);   
    
    if (rlen < 0)
    {
    	CurrMPAttrib.Flag = -1;
    	CloseFormProgress();
    	return;
    }
    
    rlen = Parse698Buff(recvbuff, rlen);
    if (rlen != 1)
    {
    	FailNum++;
    }
    else
    {
    	ReadNum++;
    }
    
    if (rlen == 2)
    {
    	MessageBox(ConstResStr[rlen], STR_CUE, MB_OK);
    	CurrMPAttrib.Flag = -2;
    	CloseFormProgress();
    	return;
    }
        
    ProgressStep();          
  }
  
  CloseFormProgress();
}


//批量抄读测量点日冻结数据处理Fn161
void BatchReadHisDataDoSend(HWND Obj, PMsg Msg)
{
  int i = 0;
  int j = 0;
  int r = 0;
  int mpNo = 1;
  int rlen = 0;
  unsigned short slen = 0;
  
  unsigned char step = 0;
  unsigned char number = 0;
  unsigned char count = 0;

  unsigned char sendbuff[MINSENDBYTES] = {0};
  unsigned char recvbuff[1024] = {0};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  Stru698Data Send698Data;
  
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, ListBox_TermOperate.text);
  
  Send698Data.Ccode = C_698READ;
  Send698Data.AFN = AFN_698HISDATA;
  Send698Data.SEQ = INI_698SEQ;
  Send698Data.Fn = FN_161;
  Send698Data.Ulen = 0;
  
  step = MAX_READMETERNUM;  
  number = step;
    
  for(i = 0; i < ProgressBar_Proc.max; i += step)
  {

  	if (ProgressBar_Proc.max - i < step)
  	{
  		number = ProgressBar_Proc.max % step;
  	}
  	ProgressBar_Proc.value += number;
  	ProgressBar_Proc.value--;
  	
  	use(METE_DATA_RCV_GW);
  	
  	count = 0;  	
  	Send698Data.Pn = 0;
  	Send698Data.Ulen = 0;
  	 	
  	for (j = 0; j < number; j++)
  	{
	  	r = CurrMPAttrib.MPLocate[CurrMPAttrib.Flag++];
	  	libset(r, FD_DAYDATA);
	  	memset(temp, 0, sizeof(temp));
	  	libread(temp);
	  	
	  	if (strstr(temp, ".") != NULL)
	  	{
	  		continue;
	  	}
	  		  	
	  	libset(r, FD_DEVNO);
	  	memset(temp, 0, sizeof(temp));
	  	libread(temp);
	  	mpNo = StrToInt(temp, DEC);  		  	
	  	count++;
	  	
	  	if (!(Send698Data.Pn))
	  	{
	  		Send698Data.Pn = mpNo;
	  	}
	  	else
	  	{	  			  		
	  		Send698Data.Userbuf[Send698Data.Ulen++] = 1 << ((mpNo - 1) % 8);      
	    	Send698Data.Userbuf[Send698Data.Ulen++] = (mpNo - 1) / 8 + 1;   
		    Send698Data.Userbuf[Send698Data.Ulen++] = 1 << ((Send698Data.Fn - 1) % 8);  
		    Send698Data.Userbuf[Send698Data.Ulen++] = (Send698Data.Fn - 1) / 8;  	    	
    	} 
    	
		  Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)ReadHisDataDate;
	    Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)(ReadHisDataDate >> 8);
	    Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)(ReadHisDataDate >> 16); 	        	   	      	       		
  	}
  	
  	use("");
  	
  	if (!(Send698Data.Pn))
  	{
  		ProgressStep();
  		continue;
  	}  
             
    if (Send698Data.SEQ == 0xEF)
    {
      Send698Data.SEQ = 0xE0;
    }
    Send698Data.SEQ++;
    
    memset(sendbuff, 0, sizeof(sendbuff));
    slen = Build698SendBuff(sendbuff, &Send698Data);
    
    SendData(sendbuff, slen);

    rlen = RecvChar(recvbuff, sizeof(recvbuff) - 1, 1000);
    Beep(100);   
    
    rlen = Parse698Buff(recvbuff, rlen);
    if (rlen != 1)
    {
    	FailNum += count;
    }
    else
    {
    	ReadNum += count;
    }

    ProgressStep();          
  }
  
  CloseFormProgress();
}


//批量透抄电表数据处理
void BatchReadDataDoSend(HWND Obj, PMsg Msg)
{
  int i = 0;
  int r = 0;
  int rlen = 0;
  unsigned short slen = 0;
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  unsigned char sendbuff[MINSENDBYTES] = {0};
  unsigned char recvbuff[MAXRECVBYTES] = {0};
  Stru698Data Send698Data;
  
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, ListBox_TermOperate.text);
  
  Send698Data.Ccode = C_698REBOOT;
  Send698Data.AFN = AFN_698FWDCMD;
  Send698Data.SEQ = INI_698SEQ;
  Send698Data.Pn = 0;
  Send698Data.Fn = 1;
              
  for(i = 0; i < ProgressBar_Proc.max; i++)
  {
		
		use(METE_DATA_RCV_GW);
		r = CurrMPAttrib.MPLocate[CurrMPAttrib.Flag++];
		
		libset(r, FD_RESERVE);
		libwrit("");
		
		libset(r, FD_RTDATA);
		memset(temp, 0, sizeof(temp));
		libread(temp);
		
  	if (strstr(temp, ".") != NULL)
  	{
  		use("");
  		ProgressStep();
  		continue;
  	}
  	
    memset(sendbuff, 0, sizeof(sendbuff));
    slen = BuildAFN10F01Buff(sendbuff, r);
  	
  	use("");
  	
    Send698Data.Ulen = 0;
    memset(Send698Data.Userbuf, 0, sizeof(Send698Data.Userbuf));
    memcpy(Send698Data.Userbuf, sendbuff, slen);
    Send698Data.Ulen += slen;
                
    if(Send698Data.SEQ == 0xEF)
    {
      Send698Data.SEQ = 0xE0;
    }
    Send698Data.SEQ++;
    
    memset(sendbuff, 0, sizeof(sendbuff));
    slen = Build698SendBuff(sendbuff, &Send698Data);
    
    SendData(sendbuff, slen);
    
    rlen = RecvData(recvbuff, sizeof(recvbuff) - 1, NUM_1000);
    Beep(100);
  
    if (rlen < 0)
    {
    	CurrMPAttrib.Flag = -1;
    	CloseFormProgress();
    	return;
    }
      
    rlen = Parse698Buff(recvbuff, rlen);
    
    if (rlen != 1)
    {
    	FailNum++;
    }
    else
    {
    	ReadNum++;
    }
    
    ProgressStep();    
  }
  
  CloseFormProgress();
}


//按功能键 设置终端地址&时间
void TermOperateSetAddr(HWND Sender)
{
  int res = 0;
  unsigned char term[MAX_ASSETNO_DIGITS] = {0};
  
  if (ListBox_TermOperate.index < 0)
  {
    return;
  }    
 
  sprintf(term, "%s", ListBox_TermOperate.text);      

  res = SetAddrTime();
  if (res == ID_CANCEL)
  {
  	return;
  }
  
  uartinit(CommParam.Baud, CommParam.Parity, CommParam.Port);
  Delay(150);
  switch(res)
  {
  	case 1:
  		res = SetTermAddr(term,res);
  		break;
  	case 2:
  		res = SetTermAddrWS(term);
  		break;
  	case 3:
  		res = SetTermTime(term);
  		break;
  	case 4:
  		res = SetTermTimeWS(term);
  		break;
  	case 5:
  		res = SetTermAddr(term,res);
  		break;
  	default:
  		break;
  }

	
  uartinit(0, CommParam.Parity, CommParam.Port);  
  
  if (res == 0)
  {
  	return;
  }
  
  if (res == -2)
  {
  	MessageBox(SEND_DENY, STR_CUE, MB_OK);   
  }
  else if (res == 1)
  {
  	MessageBox(SEND_OK, STR_CUE, MB_OK);
  }
  else if(res == -1)
  {
  	MessageBox(SEND_NULL, STR_CUE, MB_OK);
  }
  else if( res == -3)
  {
  	MessageBox(SEND_FAIL, STR_CUE, MB_OK);
  }
  else
  {
  	MessageBox("未知错误", STR_CUE, MB_OK);
  }
    
}
#endif
