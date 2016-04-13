#ifndef _SystemConf_H
#define _SystemConf_H

#include "jblib.h"
#include "Common.h"
#include "FormSelecte.h"
#include "FormSelCommType.h"
#include "DataConvert.h"
#include "InputBoxPro.h"
#include "FormSeparateAddr.h"
  
  
void DevAddrSeparate(unsigned char type)
{
  unsigned char select = 0;
  unsigned char temp[8] = {0};
  TItem items[] = {"手动输入", "扫描截取", "扫描查询",};
  unsigned char f;
  unsigned char caption[20] = {0};
  StruGetAddrRule sRule;
  
  switch(type)
  {
  	case TERM:
  		f = FD_GETTERM;
  		sprintf(caption, "终端地址获取方式");
  		break;
  	case METER:
  		f = FD_GETMETER;
  		sprintf(caption, "电表地址获取方式");
  		break;
  	case COLLECT:
  		f = FD_GETCOLLECT;
  		sprintf(caption, "采集器地址获取方式");
  		break;
  	default:
  		return;
  }
  
  memset((unsigned char *)(&sRule), 0, sizeof(sRule));
  sRule.type = type;
  
  if (!(ReadGetAddrRule(&sRule)))
  {
  	MessageBox("数据库打开失败!", STR_CUE, MB_OK);
  	return;
  }

	select = sRule.rule;

  if (FormSelecte(caption, &items, sizeof(items)/sizeof(TItem), 
  	  &select, SS_SHOWCENTER) == ID_CANCEL)
  {
    return;
  }
 
  use(SYSCONFDBF); 
  
  libset(0, f); 
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", select);   
  libwrit(temp);  
   
  use("");  
  
  if (select != 1)
  {
  	return;
  }
  
	memset(caption, 0, sizeof(caption));
  switch(type)
  {
  	case TERM:
  		sprintf(caption, "终端地址截取方式");
  		break;
  	case METER:
  		sprintf(caption, "电表地址截取方式");
  		break;
  	case COLLECT:
  		sprintf(caption, "采集器地址截取方式");
  		break;
  	default:
  		return;
  } 
	  
  unsigned int res = 0;
  while(1)
  { 	
  	res = ShowFormAddrSeparate(caption, &sRule); 
  	if (res == ID_CANCEL || res == ID_OK)
  	{
  		break;
  	}
  	else if (res == ID_YES)
  	{
  		if (TestAddrSeparate(sRule.type))
  		{
  			continue;
  		}
  		break;
  	}
  }
    	
}


//设置终端地址获取方式
void TermAddrSeparate(HWND Sender)
{
  DevAddrSeparate(TERM);
}


//设置电表地址获取方式
void MeterAddrSeparate(HWND Sender)
{
	DevAddrSeparate(METER);
}


//设置采集器地址获取方式
void CollectAddrSeparate(HWND Sender)
{
	DevAddrSeparate(COLLECT);
}


//设置掌机与终端通信方式
void SetTermCommType(HWND Sender)
{
	StruCommParam sCommParam;
	if (GetCommParam(&sCommParam, 0) != 0)
	{
		sCommParam.Port = 0x05;
		sCommParam.Parity = 0x01;
		sCommParam.Baud = 1200;
	}
	
  if (FormSelCommType("设置终端通信方式", &sCommParam) == ID_CANCEL)
  {
    return;
  }
	
	FormSelCommTypeSave(&sCommParam, 0);
}


//设置电表通讯校验方式
void SetMeterCheck(HWND Sender)
{
  unsigned char select = 0;
  unsigned char temp[8] = {0};
  TItem items[] = {"偶校验", "无校验", "奇校验",};
  
  if(use(SYSCONFDBF) != 0)
  {
    MessageBox("数据库打开失败!", STR_CUE, MB_OK);
    return;
  }
  
  if(libsumr() == 0)
  {
    libappend();
  }
   
  libset(0, FD_METERCHECK);
  memset(temp, 0, sizeof(temp));
  libread(temp); 
  select = StrToInt(temp, HEX);
  switch(select)
  {
  	case 0x09:
  		select = 0;
  		break;
    case 0x01:
    	select = 1;
      break;
    case 0x0D:
    	select = 2;
      break;
    default:
      select = 0;
      break;  		
  }

  if (FormSelecte("设置电表校验方式", &items, sizeof(items)/sizeof(TItem),
  	  &select, SS_SHOWCENTER) == ID_CANCEL)
  {
  	use("");
    return;
  }
  
  switch (select)
  {
    case 0:
    default:
      select = 0x09;
      break;
    case 1:
      select = 0x01;
      break;
    case 2:
      select = 0x0D;
      break;
  } 
  
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%02x", select);
  libset(0, FD_METERCHECK);
  libwrit(temp);
  
  use(""); 
}


//设置终端地址进制
void SelectAddrType(HWND Sender)
{
  unsigned char select = 0;
  unsigned char temp[8] = {0};
  TItem items[] = {"10进制", "16进制",};
  
  if(use(SYSCONFDBF) != 0)
  {
    MessageBox("数据库打开失败!", STR_CUE, MB_OK);
    return;
  }
  
  if(libsumr() == 0)
  {
    libappend();
  }
  
  libset(0, FD_ADDRDEC);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  
  if(StrToInt(temp, DEC) == 16)
  {
  	select = 1;
  }
  
  if (FormSelecte("终端逻辑地址进制", &items, sizeof(items)/sizeof(TItem),
  	  &select, SS_SHOWCENTER) == ID_CANCEL)
  {
  	use("");
    return;
  }
  
  if (select)
  {
  	select = 16;
  }
  else
  {
  	select = 10;
  }
  
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", select);   
  libset(0, FD_ADDRDEC);
  libwrit(temp);
  
  use(""); 
}


//设置低压集抄架构
void SetNetWorkType(HWND Sender)
{
  unsigned char select = 0;
  unsigned char temp[8] = {0};
  TItem items[] = {"双层RS485结构", "双层无线载波结构", "三层RS485结构", "三层无线载波结构",};
  
  if(use(SYSCONFDBF) != 0)
  {
    MessageBox("数据库打开失败!", STR_CUE, MB_OK);
    return;
  }
  
  if(libsumr() == 0)
  {
    libappend();
  }
  
  libset(0, FD_NWTYPE);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  
  select = StrToInt(temp, DEC);

  if (FormSelecte("设置低压集抄架构", &items, sizeof(items)/sizeof(TItem),
  	  &select, SS_SHOWCENTER) == ID_CANCEL)
  {
  	use("");
    return;
  }

  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", select);
  libset(0, FD_NWTYPE);
  libwrit(temp);
  
  use(""); 
}


//设置无线模块类型
void SetWlsModuleType(HWND Sender)
{
  unsigned char select = 0;
  unsigned char temp[8] = {0};
  TItem items[] = {"CC1100", "SI1000",};
    
  if(use(SYSCONFDBF) != 0)
  {
    MessageBox("数据库打开失败!", STR_CUE, MB_OK);
    return;
  }
  
  if(libsumr() == 0)
  {
    libappend();
  }
  
  libset(0, FD_WLSTYPE);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  
  select = StrToInt(temp, DEC);

  if (FormSelecte("设置无线模块类型", &items, sizeof(items)/sizeof(TItem), 
  		&select, SS_SHOWCENTER) == ID_CANCEL)
  {
  	use("");
    return;
  }

  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", select);
  libset(0, FD_WLSTYPE);
  libwrit(temp);
  
  use(""); 
}


//设置批量处理档案个数
void SetBatchSendRecordNum(HWND Sender)
{
  unsigned char select = 0;
  unsigned char temp[8] = {0};
  TItem items[] = {"8", "16", "24", "32"};
    
  if(use(SYSCONFDBF) != 0)
  {
    MessageBox("数据库打开失败!", STR_CUE, MB_OK);
    return;
  }
  
  if(libsumr() == 0)
  {
    libappend();
  }
  
  libset(0, FD_SETRECDNUM);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  
  select = StrToInt(temp, DEC);

  if (FormSelecte("批量处理电表档案个数", &items, sizeof(items)/sizeof(TItem), 
  		&select, SS_SHOWCENTER) == ID_CANCEL)
  {
  	use("");
    return;
  }

  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", select);
  libset(0, FD_SETRECDNUM);
  libwrit(temp);
  
  use(""); 
}


//显示软件版本
void VersionView(HWND Sender)
{   
  if (fileexits(VERSIONMIF) == FSNULL)
	{
		MessageBox("威胜国网低压集抄系统\n   掌机调试工具\n  \nWF9\n  -LV376GJD-2.1\n   2013.2.27", "软件版本", MB_OK); 
	}
	else
	{
		ShowData("软件版本", VERSIONMIF);
	}
}


void SystemConf(HWND Sender)
{
  Window Form_SystemConf;
                              
  TMenu SystemConfMenu;     
                
  TMenuItem  SystemConfItems[]={
    {NULL,SetTermCommType," 1.终端本地通信方式"},
    {NULL,SetMeterCheck," 2.电表检测校验方式"},
    {NULL,SelectAddrType," 3.终端逻辑地址进制"},
    {NULL,TermAddrSeparate," 4.终端地址获取方式"},                
    {NULL,MeterAddrSeparate," 5.电表地址获取方式"},
    {NULL,CollectAddrSeparate," 6.采集地址获取方式"},
    {NULL,SetNetWorkType," 7.低压集抄架构设置"},
    //{NULL,SetWlsModuleType," 8.无线模块类型设置"},
    {NULL,SetBatchSendRecordNum," 8.批量处理档案个数"},
    {NULL,VersionView," 9.软件版本查看"},              
    };                     

  CreateWindow(&Form_SystemConf, "系统配置", 0,0,160,160);
  CreateTMenu(&SystemConfMenu,SystemConfItems,
  sizeof(SystemConfItems)/sizeof(TMenuItem),8,3,144,126,FONT_EN_12|FONT_CH_12);
                               
  AddToWindow(&Form_SystemConf,&SystemConfMenu);
                
  ShowWindow(&Form_SystemConf,SS_SHOWMAX); 
}
#endif
