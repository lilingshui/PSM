#ifndef _BatchSendRecord_H
#define _BatchSendRecord_H

#include "jblib.h"
#include "Common.h"
#include "FormProgress.h"
#include "InputBoxPro.h"
#include "FormSelecte.h"
#include "SendMeterRecord.h"
#include "DataConvert.h"
#include "InputBoxGroupPro.h"
#include "SystemConf.h" 
ListBox ListBox_BatchSendRecord;

void BatchSendRecordLoad();
void BatchSendRecordDoLoad(HWND Obj,PMsg Msg);
void ListBox_BatchSendRecord_OnMsg(HWND Obj, PMsg Msg);
void BatchSendRecordView(HWND Sender);
void BatchSendRecordGoto(HWND Sender);
void BatchSendRecordSend(HWND Sender);
void BatchSendRecordDoSend(HWND Obj, PMsg Msg);
void BatchSendRecordReset(HWND Sender);
void BatchSendRecordDoReset(HWND Obj,PMsg Msg);
void BatchSendRecordHelp(HWND Sender);
void InitTermParam(HWND Sender);
void BatchReadRecord(HWND Sender);
void BatchReadRecordDoSend(HWND Obj, PMsg Msg);
void BatchSendRecordInfo(HWND Sender);


void BatchSendRecord(HWND Sender)
{
  Window Form_BatchSendRecord;
  Button Button_Goto, Button_Send, Button_Read, Button_Info, Button_Init;//Button_Help;
  
  if (GetCommParam(&CommParam, 0) == ERROR)
  {
  	MessageBox(ConstConfERRStr[1], STR_CUE,MB_OK);
  	return;
  }
     
  BatchSendRecordLoad();

  CreateListBox(&ListBox_BatchSendRecord, ListBox_TerminalItems, 
    TerminalCount, 3, 3, 110, 124);
  ListBox_BatchSendRecord.OnMsg = ListBox_BatchSendRecord_OnMsg;  
  ListBox_BatchSendRecord.OnClick = BatchSendRecordView;
  CreateButton(&Button_Goto, "添加1", 115, 3, 40, 20);
  Button_Goto.OnClick = BatchSendRecordGoto;
  CreateButton(&Button_Send, "下发2", 115, 29, 40, 20);
  Button_Send.OnClick = BatchSendRecordSend;
  CreateButton(&Button_Read, "查询3", 115, 55, 40, 20);
  Button_Read.OnClick = BatchReadRecord;
  CreateButton(&Button_Init, "清空4", 115, 81, 40, 20);
  Button_Init.OnClick = InitTermParam;
  CreateButton(&Button_Info, "信息5", 115, 107, 40, 20);
  Button_Info.OnClick = BatchSendRecordInfo;

  CreateWindow(&Form_BatchSendRecord, "终端配置档案", 0, 0, 160, 160);
  AddToWindow(&Form_BatchSendRecord, &ListBox_BatchSendRecord);
  AddToWindow(&Form_BatchSendRecord, &Button_Goto);
  AddToWindow(&Form_BatchSendRecord, &Button_Send);
  AddToWindow(&Form_BatchSendRecord, &Button_Read);
  AddToWindow(&Form_BatchSendRecord, &Button_Init);
  AddToWindow(&Form_BatchSendRecord, &Button_Info);
    
  ShowWindow(&Form_BatchSendRecord, SS_SHOWMAX);
}


//终端列表加载
void BatchSendRecordLoad()
{
  int count;
  use(TERM_RECD);
  count = libsumr();
  
  TerminalCount = 0;
  memset(ListBox_TerminalItems, 0, sizeof(ListBox_TerminalItems));
  ShowFormProgress(count, BatchSendRecordDoLoad);
}

//终端列表加载处理
void BatchSendRecordDoLoad(HWND Obj,PMsg Msg)
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
void ListBox_BatchSendRecord_OnMsg(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_UP:
        if (ListBox_BatchSendRecord.index == 0)
        {
          ListBox_BatchSendRecord.index = ListBox_BatchSendRecord.count - 1;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_DOWN:
        if (ListBox_BatchSendRecord.index == ListBox_BatchSendRecord.count - 1)
        {
          ListBox_BatchSendRecord.index = 0;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_1:
        BatchSendRecordGoto(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_2:
        BatchSendRecordSend(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_3:
        BatchReadRecord(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_4:
        InitTermParam(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_5:  
      	BatchSendRecordInfo(NULL);
      	Msg->Result = MSG_OK;
      	break;
      case VK_DEL:
        BatchSendRecordReset(NULL);
        Msg->Result = MSG_OK;
        break;    
      case VK_0:
        BatchSendRecordHelp(NULL);
        Msg->Result = MSG_OK;
        break;
      case VK_FUNCTION:
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
void BatchSendRecordView(HWND Sender)
{
	
	char filename[MAX_FILENAME_LEN] = {0};
	
  if (ListBox_BatchSendRecord.count < 1)
  {
    return;
  }
    
  if (GetRecdFileName(ListBox_BatchSendRecord.text, filename) == NULL)
  {
  	return;
  }  
  copy(filename, METE_RECD_GW);
  
  memset(CurrMPAttrib.Term, 0, sizeof(CurrMPAttrib.Term));
  sprintf(CurrMPAttrib.Term, "%s", ListBox_BatchSendRecord.text);

  SendMeterRecord(ListBox_BatchSendRecord.text);
}


//跳转定位终端
/*void BatchSendRecordGoto(HWND Sender)
{
  unsigned char term[MAX_ASSETNO_DIGITS] = {0};
  int i;

  if (ListBox_BatchSendRecord.count < 1)
  {
    return;
  }
  
  if (InputBoxPro("跳转到", term, sizeof(term)) == ID_CANCEL)
  {
    return;
  }
//  GetTerminalAddress(term);
  GetAddress(term, 0);

  for (i = 0; i < TerminalCount; i++)
  {
    if (strcmp(term, ListBox_TerminalItems[i]) == 0)
    {
      ListBox_BatchSendRecord.index = i;
      Refresh(&ListBox_BatchSendRecord);
      break;
    }
  }
} */

void BatchSendRecordGoto(HWND Sender)
{
  unsigned char term[MAX_ASSETNO_DIGITS] = {0};
  unsigned char termno[MAX_ASSETNO_DIGITS] = {0};
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int i, r;
  
  if (ListBox_BatchSendRecord.count >= MAXTERMINALCOUNT - 1)
  {
  	MessageBox("终端数量已达上限,请进行删除或清空操作!", STR_CUE, MB_OK);
  	return;
  }
 
  while (1)
  {
    memset(term, 0, sizeof(term));
    if (ListBox_BatchSendRecord.count > 0)
    {
    	sprintf(term, "%s", ListBox_BatchSendRecord.text);
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
      ListBox_BatchSendRecord.index = i;
      Refresh(&ListBox_BatchSendRecord);
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
    ListBox_BatchSendRecord.index = TerminalCount;
    TerminalCount++;
    ListBox_BatchSendRecord.count = TerminalCount;
    Refresh(&ListBox_BatchSendRecord);
    
    break;
  }

  /*if (InputBoxProResult == ID_SCANOK)
  {
    ArchivesTerminalView(ListBox_TermOperate.handle);
  }*/
}


//批量下发电表档案
void BatchSendRecordSend(HWND Sender)
{
  int index, step;
  char filename[MAX_FILENAME_LEN] = {0};
  char v_sendnum[13] = {0};
  char v_startnum[13] = {0};
  
  if (ListBox_BatchSendRecord.count < 1)
  {
    return;
  }
  
  memset(CurrMPAttrib.Term, 0, sizeof(CurrMPAttrib.Term));
  sprintf(CurrMPAttrib.Term, "%s", ListBox_BatchSendRecord.text); 
  
  if (GetRecdFileName(ListBox_BatchSendRecord.text, filename) == NULL)
  {
  	MessageBox("未找到该终端的测量点档案文件!", STR_CUE, MB_OK);
  	return;
  }
  
  GetMPLocate(&CurrMPAttrib, filename); 
  if (CurrMPAttrib.MPNum < 1)
  {
  	MessageBox("测量点档案为空!", STR_CUE, MB_OK);
  	return;  	
  }
  copy(filename, METE_RECD_GW);
/*  
  sprintf(v_sendnum, "本次设置数量");
  sprintf(v_startnum, "起始装置序号"); 
  
  if (InputBoxGroupPro("请输入", v_sendnum, v_startnum) == 0)
  {
    return;
  } 

  step = StrToInt(v_sendnum, DEC); 
  index = StrToInt(v_startnum, DEC); 
  if (step < 1 || step > CurrMPAttrib.MPNum)
  {
  	step = CurrMPAttrib.MPNum;
  }
  if (index < 1 || index + step > CurrMPAttrib.MPNum)
  {
  	index = 1;
  }
  CurrMPAttrib.Flag = index;*/
  step = CurrMPAttrib.MPNum;
  
  uartinit(CommParam.Baud, CommParam.Parity, CommParam.Port);
  Delay(DELAY_OPENPORT);
   	       
  ShowFormProgress(step, BatchSendRecordDoSend);    
    
  uartinit(0, CommParam.Parity, CommParam.Port);
}

//批量下发档案处理函数
void BatchSendRecordDoSend(HWND Obj, PMsg Msg)
{
  
  int i = 0;
  int j = 0;
  int r = 0;
  int count = 0;
  int rlen = 0;
	unsigned short slen = 0; 

	unsigned short mpNo = 0;
	unsigned char step = 0;
	unsigned char number = 0;
  unsigned char times = 0;
  unsigned char repeattimes = 3;
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  
  unsigned char sendbuff[1024] = {0};
  unsigned char recvbuff[MINRECVBYTES] = {0};
  Stru698Data Send698Data;
  
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, ListBox_BatchSendRecord.text);
  Send698Data.Ccode = C_698SET;
  Send698Data.AFN = AFN_698SET;
  Send698Data.SEQ = INI_698SEQ;
  Send698Data.Pn = 0;
  Send698Data.Fn = FN_RECD;

  if(CommParam.Port == PORT_IR)
  {
  	step = MAX_SENDRECDNUM_IR;
  }
  else
  {
	  step = GetBatchSendRecordNum();
	}
	
  use(METE_RECD_GW);
  count = libsumr();

  for(i = 0; i < ProgressBar_Proc.max; i += step)
  {
    number = step;
    if (ProgressBar_Proc.max - i < step)
    {
    	number = ProgressBar_Proc.max % step;
    }
    ProgressBar_Proc.value += number;
    ProgressBar_Proc.value--;
    
    memset(Send698Data.Userbuf, 0, sizeof(Send698Data.Userbuf));
    Send698Data.Userbuf[0] = (U8)number;
    Send698Data.Userbuf[1] = 0x00;
    Send698Data.Ulen = 2;
    
    j = 0;
    while(j < number)
    {
      mpNo = (U16)(CurrMPAttrib.Flag);
      //r = mpNo - 1;
      r = CurrMPAttrib.MPLocate[CurrMPAttrib.Flag];
      memset(sendbuff, 0, sizeof(sendbuff));
      slen = BuildAFN04F10Buff(sendbuff, mpNo, r);      
      memcpy(Send698Data.Userbuf + Send698Data.Ulen, sendbuff, slen); 
      Send698Data.Ulen += slen;  
      CurrMPAttrib.Flag++;
      j++;    
    }
    if(Send698Data.SEQ == 0xEF)
    {
      Send698Data.SEQ = 0xE0;
    }
    Send698Data.SEQ++;
    
    memset(sendbuff, 0, sizeof(sendbuff));
    slen = Build698SendBuff(sendbuff, &Send698Data);
    
    while(times < repeattimes)
    {   
      times++;
      SendData(sendbuff, slen);
	    if (CommParam.Baud < BAUD_38400)
	    {
	      rlen = RecvChar(recvbuff, sizeof(recvbuff), 2000);
	    }
	    else
	    {
	      rlen = rbuf(recvbuff, sizeof(recvbuff), 1000);
	    }
      Beep(100);
      
      rlen = Parse698Buff(recvbuff, rlen);
      if (rlen != 1)
      {
      	memset(recvbuff, 0, sizeof(recvbuff));
      	continue;
      }
      else
      {
      	times = 0;
      	break;
      }
    }
    
    if (times != 0)
    {
    	if (rlen != 1)
  		{        			
  			MessageBox(ConstResStr[rlen], STR_CUE, MB_OK); 
  		}
      use("");    
      CloseFormProgress();
      CurrMPAttrib.Flag = i;
    	return;
    }
    else
    {
    	ProgressStep();
    }   

  }
  
  MessageBox(ConstResStr[1], STR_CUE, MB_OK);
  use("");  
  
  CloseFormProgress();
}


//终端信息
void BatchSendRecordInfo(HWND Sender)
{
  int r, count;
  char temp[MAX_ASSETNO_DIGITS] = {0};
  char filename[MAX_FILENAME_LEN] = {0};
  char showInfo[NUM_255] = {0};
  
  if (ListBox_BatchSendRecord.count < 1)
  {
    return;
  }
  
  if (GetRecdFileName(ListBox_BatchSendRecord.text, filename) == NULL)
  {
  	MessageBox("该终端没有添加测量点信息!", STR_CUE, MB_OK);
    return;
  }
  
  GetMPLocate(&CurrMPAttrib, filename);
  if (CurrMPAttrib.MPNum)
  {
  	use(filename);
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
  		
  		libset(r, FD_TERMADDR);
  		memset(temp, 0, sizeof(temp));
  		libread(temp);
  		if (strcmp(temp, ListBox_BatchSendRecord.text) == 0)
  		{
  			libset(r, FD_TERMNO);
  			memset(temp, 0, sizeof(temp));
  			libread(temp);
  			break;
  		}
  	}
  	use("");
  }
  
  sprintf(showInfo, "终端地址：%s\n电表总数：%d\n终端资产号：%s", 
  				ListBox_BatchSendRecord.text, CurrMPAttrib.MPNum, temp);

	MessageBox(showInfo, "终端信息", MB_OK);
	
}

//重置 录入终端资产号
void BatchSendRecordReset(HWND Sender)
{
  int r, count;
  char temp[10] = {};
  char termno[23] = {};
  char show[60] = {};
  
  if (TerminalCount == 0)
  {
    return;
  }
  
  if (InputBoxPro("终端编号", termno, sizeof(termno)) == ID_CANCEL)
  {
    return;
  }
  sprintf(show,"确定要改终端编号吗？\n终端编号: %s", termno);

  if(MessageBox(show, STR_CUE, MB_YESNO) == ID_NO)
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
      continue;

    libset(r, 2);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, ListBox_BatchSendRecord.text) == 0)
    {
      libset(r, 1);
      libwrit(termno); 
    }    
    
  }

}

//重置资产号处理
void BatchSendRecordDoReset(HWND Obj,PMsg Msg)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {};
  int r, c;
  
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

    libset(r, 2);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, ListBox_BatchSendRecord.text) == 0)
    {
      for (c = 5; c < libsumf(); c++)
      {
        libset(r, c);
        libwrit("");
      } 
    }    
    ProgressStep();
  }
  CloseFormProgress();
  use("");
}


//批量抄读终端电表档案
void BatchReadRecord(HWND Sender)
{

  char v_sendnum[13] = {0};
  char v_startnum[13] = {0};
  char filename[MAX_FILENAME_LEN] = {0};
  int i, step, startindex;
  
  sprintf(v_sendnum, "本次抄读数量");
  sprintf(v_startnum, "起始装置序号");
  
  if (ListBox_BatchSendRecord.count < 1)
  {
  	return;
  }
  
  if (InputBoxGroupPro("请输入", v_sendnum, v_startnum) == 0)
  {
    return;
  } 
  
  step = StrToInt(v_sendnum, DEC); 
  startindex = StrToInt(v_startnum, DEC);
  
  if (step == 0)
  {
    step = MAXMETERCOUNT;
  }
  if (startindex < 1 || startindex + step > MAXMETERCOUNT)
  {
    startindex = 1;
  }
    
  ReadMeterRecordNum = step;
  
  memset(CurrMPAttrib.Term, 0, sizeof(CurrMPAttrib.Term));
  sprintf(CurrMPAttrib.Term, "%s", ListBox_BatchSendRecord.text); 
   
  CurrMPAttrib.Flag = startindex;
   
  uartinit(CommParam.Baud, CommParam.Parity, CommParam.Port);
  Delay(DELAY_OPENPORT);
  
  copy(METE_RECD_NEW_GW, METE_RECD_RCV_GW); 
  //copy(METE_RECD_GW, METE_RECD_RCV_GW);

	ShowFormProgress(step, BatchReadRecordDoSend);

  uartinit(0, CommParam.Parity, CommParam.Port);
        
  GetMPLocate(&CurrMPAttrib, METE_RECD_RCV_GW);    
  
	if (CurrMPAttrib.MPNum < 1)
	{
		return;
	}

  ReadMeterRecordIndex = 1;
  if (ReadMeterRecordNum > CurrMPAttrib.MPNum)
  {
		ReadMeterRecordNum = CurrMPAttrib.MPNum;
	} 
  //CurrMPAttrib.Flag = i;       
  
  ShowReadRecordResult(NULL);
  
  if(MessageBox("是否覆盖终端档案", STR_CUE, MB_YESNO) == ID_NO)
  {
    return;
  }
  
  GetRecdFileName(ListBox_BatchSendRecord.text, filename);
  //copy(METE_RECD_RCV_GW, METE_RECD_GW);
  copy(METE_RECD_RCV_GW, filename);  
}

//批量抄读档案处理
void BatchReadRecordDoSend(HWND Obj, PMsg Msg)
{
  int i = 0;
  int j = 0;
  int r = 0;
  int count = 0;
  int mpNo = 1;
  int rlen = 0;
  unsigned short slen = 0;
  
  unsigned char step = 0;
  unsigned char number = 0;
  unsigned char times = 0;
  unsigned char repeattimes = 3;
  
  unsigned char sendbuff[MINSENDBYTES] = {0};
  unsigned char recvbuff[1024] = {0};
  Stru698Data Send698Data;
  
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, ListBox_BatchSendRecord.text);
  Send698Data.Ccode = C_698READ;
  Send698Data.AFN = AFN_698READ;
  Send698Data.SEQ = INI_698SEQ;
  Send698Data.Pn = 0;
  Send698Data.Fn = FN_RECD;
  
  if(CommParam.Port == PORT_IR)
  {
  	step = MAX_SENDRECDNUM_IR;
  }
  else
  {
	  step = MAX_SENDRECDNUM_485;
	}
  number = step;
  
	for(i = 0; i < ProgressBar_Proc.max; i += step)
	{
    number = step;
    if (ProgressBar_Proc.max - i < step)
    {
    	number = ProgressBar_Proc.max % step;
    }
    ProgressBar_Proc.value += number;
    ProgressBar_Proc.value--;
    
		Send698Data.Userbuf[0] = (U8)number;
		Send698Data.Userbuf[1] = 0x00;
		Send698Data.Ulen = 2;
		
		for (j = 0; j < number; j++)
		{
			Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)(CurrMPAttrib.Flag);      
			Send698Data.Userbuf[Send698Data.Ulen++] = (unsigned char)(CurrMPAttrib.Flag >> 8); 
			CurrMPAttrib.Flag++;   
		}
		
		if(Send698Data.SEQ == 0xEF)
		{
			Send698Data.SEQ = 0xE0;
		}
		Send698Data.SEQ++;
    
    memset(sendbuff, 0, sizeof(sendbuff));
    slen = Build698SendBuff(sendbuff, &Send698Data);
    
    while(times < repeattimes)
    {   
      SendData(sendbuff, slen);
      times++;

      rlen = RecvChar(recvbuff, sizeof(recvbuff), NUM_800);
      Beep(100);
      
      rlen = Parse698Buff(recvbuff, rlen);
      if (rlen != 1)
      {
      	if (rlen == 2)
      	{
      		times = 2;
      		break;
      	}
      	memset(recvbuff, 0, sizeof(recvbuff));
      	continue;
      }
      else
      {
      	times = 0;
      	break;
      }
    }

    if (times != 0)
    {
    	MessageBox(ConstResStr[rlen], STR_CUE, MB_OK); 
      CloseFormProgress();
      CurrMPAttrib.Flag = -1;
      
    	return;
    }      
 
    ProgressStep();   

  }
  
  CloseFormProgress();  
  return;
}


//初始化终端参数（AFN01复位命令F4）
void InitTermParam(HWND Sender)
{
  U16 slen = 0, rlen = 0;
  unsigned char sendbuff[MINSENDBYTES] = {0};
  unsigned char recvbuff[MINRECVBYTES] = {0};
  Stru698Data Send698Data;
  
  if(MessageBox("确定要初始化终端档案吗?", STR_CUE, MB_OKCANCEL) == ID_CANCEL)
  {
    return;
  }
   
  memset(Send698Data.Saddr, 0, sizeof(Send698Data.Saddr));
  GetAddr(Send698Data.Saddr, ListBox_BatchSendRecord.text);
  Send698Data.Ccode = 0x41;
  Send698Data.AFN = 0x01;
  Send698Data.SEQ = 0xE9;
  Send698Data.Pn = 0;
  Send698Data.Fn = 4;
  
  Send698Data.Ulen = 0; 
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


void BatchSendRecordHelp(HWND Sender)
{
  MessageBox("当光标位于终端列表时\n确认键：单个档案配置\n按键1：跳转\n按键2：下发\n按键3：查询\n按键4：清空\n按键5/删除键：重置", "帮助", MB_OK); 
}
#endif
