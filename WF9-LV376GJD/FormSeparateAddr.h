#ifndef _FormSeparateAddr_H
#define _FormSeparateAddr_H

#include "jblib.h"
#include "Common.h"
#include "DataConvert.h"
#include "InputBoxPro.h"

Window  Form_AddrSeparate;
unsigned char SeparateAddrFlag;


void FormAddrSeparate_Test(HWND Sender)
{
	SeparateAddrFlag = 1;
	CloseWindow(&Form_AddrSeparate, ID_OK); 
}

char TestAddrSeparate(unsigned char type)
{
  char res = 0;
  unsigned char temp[23] = {0}; 

  if (InputBoxPro("资产编号", temp, sizeof(temp) - 1) == ID_CANCEL)
  {
    return 0;
  }
 
  res = GetAddress(temp, type);
  if (res)
  {
  	MessageBox(ConstConfERRStr[res], STR_CUE, MB_OK);
  	return 1;
  }
  
  MessageBox(temp, "截取地址", MB_OK);  
  return 0;
}


void FormAddrSeparate_OK(HWND Sender)
{ 
  CloseWindow(&Form_AddrSeparate, ID_OK);   
}


//设备地址截取规则配置界面
unsigned int ShowFormAddrSeparate(P_U8 caption, StruGetAddrRule *sRule)
{
	
	unsigned char temp[MAXSEPSTRLEN] = {0};
  Text  Text_SP, Text_SL, Text_FS, Text_BS;
  Edit  Edit_SP, Edit_SL, Edit_FStr, Edit_BStr;

  Button Button_OK, Button_Test;
  
  SeparateAddrFlag = 0;
  
  CreateText(&Text_SP, "起始位置", 5,8,50,18);
  CreateText(&Text_SL, "截取长度", 5,28,50,18);
  CreateText(&Text_FS, "前补字符", 5,48,50,18);
  CreateText(&Text_BS, "后补字符", 5,68,50,18);

  CreateEdit(&Edit_SP, "", 58,5,97,18);
  CreateEdit(&Edit_SL, "", 58,25,97,18);
  CreateEdit(&Edit_FStr, "", 58,45,97,18);
  CreateEdit(&Edit_BStr, "", 58,65,97,18);
  
  Edit_SP.ime = IME_ID_123;
  Edit_SL.ime = IME_ID_123;
  Edit_FStr.ime = IME_ID_123 | IME_ID_ABC;
  Edit_BStr.ime = IME_ID_123 | IME_ID_ABC;
  
  Edit_SP.length = 2; 
  Edit_SL.length = 2;
  if (sRule->type)
  {
  	Edit_FStr.length = 12; 
  	Edit_BStr.length = 12;
	}
	else
	{
		Edit_FStr.length = 8; 
		Edit_BStr.length = 8; 
	}
	
	sprintf(Edit_SP.text, "%d", sRule->startPos);
	sprintf(Edit_SL.text, "%d", sRule->sepLen);
	sprintf(Edit_FStr.text, "%s", sRule->frontStr);
	sprintf(Edit_BStr.text, "%s", sRule->backStr);
	
	Edit_FStr.OnMsg = Edit_OnMsg;
	Edit_BStr.OnMsg = Edit_OnMsg;  
  Edit_SP.OnClick = FormAddrSeparate_OK;       
  Edit_SL.OnClick = FormAddrSeparate_OK;      
  Edit_FStr.OnClick = FormAddrSeparate_OK;    
  Edit_BStr.OnClick = FormAddrSeparate_OK;
  
  CreateButton(&Button_Test, "验证", 20, 91, 40, 20);
  Button_Test.OnClick = FormAddrSeparate_Test;
    
  CreateButton(&Button_OK, "保存", 98, 91, 40, 20);
  Button_OK.OnClick = FormAddrSeparate_OK;
  
  CreateWindow(&Form_AddrSeparate, caption, 0,0,160,160);
 
  AddToWindow(&Form_AddrSeparate, &Text_SP);
  AddToWindow(&Form_AddrSeparate, &Text_SL);
  AddToWindow(&Form_AddrSeparate, &Text_FS);
  AddToWindow(&Form_AddrSeparate, &Text_BS);
  AddToWindow(&Form_AddrSeparate, &Edit_SP);
  AddToWindow(&Form_AddrSeparate, &Edit_SL);
  AddToWindow(&Form_AddrSeparate, &Edit_FStr);
  AddToWindow(&Form_AddrSeparate, &Edit_BStr);                                      
  AddToWindow(&Form_AddrSeparate, &Button_Test);
  AddToWindow(&Form_AddrSeparate, &Button_OK);
   
  if (ShowWindow(&Form_AddrSeparate, SS_SHOWMAX) == ID_CANCEL)
  {
  	return ID_CANCEL;
  }
  
  sRule->startPos = StrToInt(Edit_SP.text, DEC);
  sRule->sepLen = StrToInt(Edit_SL.text, DEC);
  strcpy(sRule->frontStr, Edit_FStr.text);
	strcpy(sRule->backStr, Edit_BStr.text); 
 
 	if (SaveGetAddrRule(sRule))
 	{
 		SeparateAddrFlag = 0; 		
 		return ID_CUSTOM;
 	}
 	else
 	{
 		if (SeparateAddrFlag)
 		{
 			return ID_YES;
 		}
 		return ID_OK;
 	}
 	 	
}
#endif
