#ifndef	_FormMain_H
#define _FormMain_H

#include "jblib.h"
#include "SystemConf.h"
#include "ArchivesTerminal.h"
#include "Check485Terminal.h"
#include "BatchSendRecord.h"   
#include "TermOperate.h"
//#include "WirelessDebug.c"
#include "ShowData.h"
#include "Register.h"


void ShowForm_Main(void)
{
 
 	if (!Register())
 	{
 		return;
 	}
 
  Window Form_Main;
  GMenu GMenu_Desktop;
  TMenuItem pGMenu_DesktopItems[] = {
    {getSysIcon(4), SystemConf, "系统配置"}, 
    {getSysIcon(2), ArchivesTerminal, "档案录入"}, 
    {getSysIcon(6), Check485Terminal, "485检测"}, 
    {getSysIcon(6), BatchSendRecord, "档案配置"},    
    {getSysIcon(6), TermOperate, "终端调试"},
    //{getSysIcon(18), WirelessDebug, "无线调试"}, 
    {getSysIcon(5), ShowMsg, "报文查看"}, 
  };

	DelFile(DEBEGINFOTXT, MAX_DEBGFILE_LEN);
	
  CreateGMenu(&GMenu_Desktop, pGMenu_DesktopItems,  
    sizeof(pGMenu_DesktopItems) / sizeof(TMenuItem), 0, 3, 160, 130);

  CreateWindow(&Form_Main, MAINFORMNAME, 0, 0, 160, 160);  
  AddToWindow(&Form_Main, &GMenu_Desktop);

  ShowWindow(&Form_Main, SS_SHOWMAX);

  if (fileexits(METE_RECD_RCV_GW) != FSNULL)
  {
    filedelete(METE_RECD_RCV_GW);
  }   
  
  if (fileexits(METE_DATA_RCV_GW) != FSNULL)
  {
  	filedelete(METE_DATA_RCV_GW);
  }
  
  if (fileexits(TEMPPARAMTXT) != FSNULL)
  {
  	filedelete(TEMPPARAMTXT);
  }
}


#endif
