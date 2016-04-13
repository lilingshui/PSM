#ifndef _InputParamPro_H
#define _InputParamPro_H

#include "jblib.h"
#include "Global.h"
#include "DataPro.h"
#include "FormBSDataPro.h"

/******************************************************************************/
/*                         国网体系规约参数输入界面                           */
/******************************************************************************/

#define MAXTEXTNUM NUM_6
#define MAXEDITNUM NUM_6
#define MAXCOMBONUM NUM_4
#define MAXCHECKBOXNUM NUM_4
#define MAX_ENUMPARAM_ITEMS NUM_10
#define MAX_INPUTPARAM_ITEMS NUM_3

//全局变量 参数输入窗口
Window Form_InputParam;
U8 PageNum;
U8 PageIndex;


void InputParam_OK(HWND Sender)
{
	PageIndex = PageNum;
	CloseWindow(&Form_InputParam, ID_OK);
}
void InputParam_Cancel(HWND Sender)
{
	CloseWindow(&Form_InputParam, ID_CANCEL);
}

//参数输入窗体全局消息处理，按下右扫描键翻页
void Form_InputParam_Msg(PMsg Msg)
{
	U32	k;
	
	Msg->Msg = WM_NULL;
	k = getSystemMessage(MSG_MASK_NOSTANDBY,MAX_WAIT_TIME);
		
	if(k & MSG_MASK_KEY)
	{    
		if (CK_SCANR == (U8)k)  // 右扫描键按下
		{
			PageIndex++;
			if (PageIndex == PageNum)
			{
				PageIndex = 0;		
			}
			CloseWindow(&Form_InputParam, ID_OK);
		}
		else                  // 其它按键归还GUI去处理，否则会不能响应键盘   
		{     
			addSystemMessage(k); 
		}
		       	
	}
	else if (k)             // 归还给系统去处理
	{
		addSystemMessage(k);
	}   	
}

//文本框输入，切换输入法
void EditBoxOnMsg(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR && Msg->WParam == VK_FUNCTION)
  {
    ImmNextIme(IME_ID_123 | IME_ID_abc | IME_ID_ABC);
    Refresh(Obj);
  }
}


/******************************************************************************/
/*                       698.42/376.1协议通用参数输入界面                     */ 
/*	功能: 通用参数输入界面，接收参数输入并组帧                                */
/*	参数: paramindex——全局参数(GlobalParam)菜单索引, buff——输出数据帧         */
/*	返回: 数据帧长度                                                          */
/******************************************************************************/
unsigned char InputCommParam(int paramindex, U8 *buff)
{
	
	Text Label_Names[MAXTEXTNUM];
	Edit Edit_Values[MAXEDITNUM];
	Combo Combo_Values[MAXCOMBONUM];
	TItem items[MAX_ENUMPARAM_ITEMS][MAXCOMBONUM];
  Button Button_OK, Button_Cancel;
  
  U8 temp[NUM_50] = {0};
  int i = 0, j = 0, index = 0; 
  int len = 0, displen = 0;
  int vLen = 0;       //可变长度
  int count = 0;
  U8 speChar = 0; 
  U32 tmpint = 0;
  U8 pageIndex = 0;
  U8 paramItemIndex = 0;
  
  PageNum = (GlobalParam + paramindex)->Param_Num / MAX_INPUTPARAM_ITEMS;
  if (0 != (GlobalParam + paramindex)->Param_Num % MAX_INPUTPARAM_ITEMS)
  {
  	PageNum++;
  } 
  PageIndex = 0;
  pageIndex = 0; 
	
	while(1)
	{ 
		if (pageIndex >= PageNum)
		{
			break;
		}
		CreateWindow(&Form_InputParam, "", 0,0,160,160);
		sprintf(Form_InputParam.text, "%s P%d/%d", 
					 (GlobalParam+paramindex)->Param_Name, pageIndex + 1, PageNum);     
		
		for (i = 0; i < MAX_INPUTPARAM_ITEMS; i++)
		{
			paramItemIndex = pageIndex * MAX_INPUTPARAM_ITEMS + i;
			if (paramItemIndex >= (GlobalParam+paramindex)->Param_Num)
			{
				break;
			}
			CreateText(&Label_Names[i], (GlobalParamItem + paramItemIndex)->Param_Item_Name, 
								2, 2+35*i, 156, 15);
			AddToWindow(&Form_InputParam, &Label_Names[i]);
			
			switch((GlobalParamItem + paramItemIndex)->Param_Item_Mask)
			{
				case 'e':
				{
					memset((unsigned char *)(&items[i]), 0, sizeof(items[i]));
					count = GetComboTItem(GlobalParamItem + paramItemIndex, items[i], &index);
					if (index < 0)
					{
						index = 0;
					}
					CreateCombo(&Combo_Values[i], items[i], count, 2, 17+35*i, 156, 18);
					Combo_Values[i].index = index;
					AddToWindow(&Form_InputParam, &Combo_Values[i]);
					break;
				}					
				default:
					CreateEdit(&Edit_Values[i], (GlobalParamItem + paramItemIndex)->Param_Value, 
										2, 17+35*i, 156, 18);
					Edit_Values[i].OnMsg = EditBoxOnMsg;
					Edit_Values[i].OnClick = InputParam_OK;
					AddToWindow(&Form_InputParam, &Edit_Values[i]);
					break;
			}
		}		
	
	  CreateButton(&Button_Cancel, "取消", 10, 108, 40, 20);
	  Button_Cancel.OnClick = InputParam_Cancel;
	  CreateButton(&Button_OK, "确定", 110, 108, 40, 20);
	  Button_OK.OnClick = InputParam_OK;	
	
	  AddToWindow(&Form_InputParam, &Button_OK);
	  AddToWindow(&Form_InputParam, &Button_Cancel);
  
    SetApplicationMsgProc(Form_InputParam_Msg);
    
	  if (ShowWindow(&Form_InputParam, SS_SHOWMAX) == ID_CANCEL)
	  {
	  	SetApplicationMsgProc(NULL); 
	  	return 0;
	  }	
	  
	  SetApplicationMsgProc(NULL); 
	  for (i = 0; i < MAX_INPUTPARAM_ITEMS; i++)
		{
			paramItemIndex = pageIndex * MAX_INPUTPARAM_ITEMS + i;
			if (paramItemIndex >= (GlobalParam+paramindex)->Param_Num)
			{
				break;
			}
			if ((GlobalParamItem + paramItemIndex)->Param_Item_Mask == 'e')
			{
  			index = GetEnumValue(GlobalParamItem + paramItemIndex, Combo_Values[i].text);
  			sprintf((GlobalParamItem + paramItemIndex)->Param_Value, "%x", index);
				continue;
			}
			
			len = strlen(Edit_Values[i].text);
			switch((GlobalParamItem + paramItemIndex)->Param_Item_Mask)
			{
		  	case 'a':
				  displen = (GlobalParamItem + paramItemIndex)->Param_Item_Clen;
		  		break;
		  	case 'd':
		  		displen = (GlobalParamItem + paramItemIndex)->Param_Item_Clen;
		  		if (displen < 3 && displen > 0)
		  		{
		  			displen = displen * 2 + 1;
		  		}
		  		else if (displen < 5 && displen >= 3)
		  		{
		  			displen = (displen + 1) * 2;
		  		}  	
		  		break;
		  	case 'i':
		  		displen = NUM_15;
		  		break;
		  	case 'l':
		  		displen = (GlobalParamItem + paramItemIndex)->Param_Item_Clen;
		  		if (displen < 3 && displen > 0)
		  		{
		  			displen = displen * 2 + 1;
		  		}
		  		else if (displen < 5 && displen >= 3)
		  		{
		  			displen = (displen + 1) * 2;
		  		}  	
		  		vLen = StrToInt(Edit_Values[i].text, DEC); 	
		  		break;
		  	case 'h':
		  	default:
				  displen = (GlobalParamItem + paramItemIndex)->Param_Item_Clen * 2;
		  		break;  
			}
			if (displen < 1)
			{
				displen = vLen;
			}
			if (len > displen)
		  {
		  	MessageBox("输入字符长度过大!", STR_CUE, MB_OK);
		  	return 0;
		  } 
		  sprintf((GlobalParamItem + paramItemIndex)->Param_Value, "%s", Edit_Values[i].text);
		}
		
		pageIndex = PageIndex;
	}
   
  index = 0;  
	for (i = 0; i < (GlobalParam + paramindex)->Param_Num; i++)
	{
		memset(temp, 0, sizeof(temp)); 
	  sprintf(temp, "%s", (GlobalParamItem + i)->Param_Value); 
	  
		if ((GlobalParamItem + i)->Param_Item_Mask == 'e')
		{
			buff[index++] = (U8)(StrToInt(temp, HEX));
			continue;
		}
			
		len = (GlobalParamItem + i)->Param_Item_Clen;
		if (len < 1)
		{
			len = vLen;
		}   
		
	  switch((GlobalParamItem + i)->Param_Item_Mask)
	  {

	  	case 's':
	  	{
	  		len	= strlen(temp);	  
	  	  len = len / 2 + len % 2;	
			  StrToHexStr(temp, len, 0, 1, 0);
			  memcpy(buff + index, temp, len); 
			  index += (GlobalParamItem + i)->Param_Item_Clen; 	  
	  	  break;
	  	}
	  	case 'h':
	  	default:
	  	{  	
	  		len	= strlen(temp);	  
	  	  len = len / 2 + len % 2;	
			  StrToHexStr(temp, len, 1, 1, 0);
			  memcpy(buff + index, temp, len); 
			  index += (GlobalParamItem + i)->Param_Item_Clen; 	  
	  	  break;
	  	}
	  	case 'a':
	  	{		  
			  //len = (GlobalParamItem + i)->Param_Item_Clen;
				memcpy(buff + index, temp, len);	
				index += len;
	  		break;
	  	}
	  	/*case 'h':
	  	{
	  		len = (GlobalParamItem + i)->Param_Item_Clen;
	  		//StrToHexStr(temp, len, 1, 1, 0);
	  		if (GetFormatStrBuff((GlobalParamItem + i)->Param_Item_Value, temp, len) == NULL)
	  		{
	  			return 0;
	  		}
	  		memcpy(buff, temp, len);
	  		break;
	  	}*/
	  	case 'd':
	  	{
	  		//len = (GlobalParamItem + i)->Param_Item_Clen;
	  		tmpint = StrToInt(temp, DEC);
	  		for (j = 0; j < len; j++)
	  		{
	  			buff[index++] =(unsigned char)(tmpint >> (j * 8));
	  		}
	  		break; 	
	  	}	
	  	case 'i':
	  	{
	  		SeparateIP(temp, buff+index, 0);
	  		index += len;
	  		break;
	  	}
	  	case 'l':
	  	{
	  		//len = (GlobalParamItem + i)->Param_Item_Clen;
	  		tmpint = StrToInt(temp, DEC);
	  		for (j = 0; j < len; j++)
	  		{
	  			buff[index++] =(unsigned char)(tmpint >> (j * 8));
	  		}
	  		vLen = (int)tmpint;
	  		break; 	
	  	}
	  }
	}   
	  
  return index;	
}


/******************************************************************************/
/*                       698.42/376.1协议参数输入界面                         */ 
/*	功能: 接收单一参数采用文本框控件输入，根据数据类型组帧                    */
/*	参数: szParam——参数项内容, buff——输出数据帧                               */
/*	返回: 数据帧长度                                                          */
/******************************************************************************/
unsigned char InputStrParam(Stru_ParamItem *szParam, U8 *buff)
{
	Text Label_Name;
	Edit Edit_Value;
  Button Button_OK, Button_Cancel;
  U8 temp[NUM_50] = {0};
  int i = 0, index = 0; 
  int len = 0, displen = 0;
  U8 speChar = 0; 
  U32 tmpint = 0;
  
	CreateWindow(&Form_InputParam, "", 0,0,160,160);
	sprintf(Form_InputParam.text, "请输入%s", szParam->Param_Item_Name);     
	
	CreateText(&Label_Name, szParam->Param_Item_Name, 6, 19, 148, 15);
	CreateEdit(&Edit_Value, "", 10, 45, 140, 18);
	sprintf(Edit_Value.text, "%s", szParam->Param_Value);
	Edit_Value.OnMsg = EditBoxOnMsg;
	
  CreateButton(&Button_Cancel, "取消", 10, 108, 40, 20);
  Button_Cancel.OnClick = InputParam_Cancel;
  CreateButton(&Button_OK, "确定", 110, 108, 40, 20);
  Button_OK.OnClick = InputParam_OK;	

  AddToWindow(&Form_InputParam, &Label_Name);
  AddToWindow(&Form_InputParam, &Edit_Value);
  AddToWindow(&Form_InputParam, &Button_OK);
  AddToWindow(&Form_InputParam, &Button_Cancel);
  
  if (ShowWindow(&Form_InputParam, SS_SHOWMAX) == ID_CANCEL)
  {
  	return 0;
  }	
  
  len = strlen(Edit_Value.text);
  switch(szParam->Param_Item_Mask)
  {
  	//case 'h':		
  	//  displen = (szParam->Param_Item_Clen + 1) * 2;   	  
  	//  break;
  	case 'a':
		  displen = szParam->Param_Item_Clen;
  		break;
  	case 'd':
  		displen = szParam->Param_Item_Clen;
  		if (displen < 3 && displen > 0)
  		{
  			displen = displen * 2 + 1;
  		}
  		else if (displen < 5 && displen >= 3)
  		{
  			displen = (displen + 1) * 2;
  		}  		
  		break;
  	case 'i':
  		displen = NUM_15;
  		break;
  	case 'h':
  	default:
		  displen = szParam->Param_Item_Clen * 2;
  		break;  	
  }
  
  if (len > displen)
  {
  	MessageBox("输入字符长度过大!", STR_CUE, MB_OK);
  	return 0;
  }    
    
  memset(temp, 0, sizeof(temp)); 
  sprintf(temp, "%s", Edit_Value.text); 
   
  switch(szParam->Param_Item_Mask)
  {
  	case 's':
  	{
  		len = len / 2 + len % 2;	
		  StrToHexStr(temp, len, 0, 1, 0);
		  memcpy(buff, temp, len);  	  
  	  break;
  	}
  	case 'h':
  	default:
  	{  			  
  	  len = len / 2 + len % 2;	
		  StrToHexStr(temp, len, 1, 1, 0);
		  memcpy(buff, temp, len);  	  
  	  break;
  	}
  	case 'a':
  	{		  
		  index = displen - 1;
		  while(i < len)
		  {
		  	buff[index--] = temp[i];
		  	i++;
		  }	
  		break;
  	}
  	/*case 'h':
  	{
  		len = szParam->Param_Item_Clen;
  		//StrToHexStr(temp, len, 1, 1, 0);
  		if (GetFormatStrBuff(szParam->Param_Item_Value, temp, len) == NULL)
  		{
  			return 0;
  		}
  		memcpy(buff, temp, len);
  		break;
  	}*/
  	case 'd':
  	{
  		len = szParam->Param_Item_Clen;
  		tmpint = StrToInt(temp, 10);
  		for (i = 0; i < len; i++)
  		{
  			buff[i] =(unsigned char)(tmpint >> (i * 8));
  		}
  		break; 	
  	}	
  	case 'i':
  	{
  		SeparateIP(temp, buff, 0);
  		break;
  	}
  }
  if (szParam->Param_ID == 0x8016)
  {
  	speChar = buff[0];
  	buff[0] = buff[1];
  	buff[1] = speChar;
  }
  
  return szParam->Param_Item_Clen;   	
}


/******************************************************************************/
/*                       698.42/376.1协议参数输入界面                         */ 
/*	功能: 接收单一参数采用列表框控件输入，根据数据类型组帧                    */
/*	参数: szParam——参数项内容, buff——输出数据帧                               */
/*	返回: 数据帧长度                                                          */
/******************************************************************************/
unsigned char InputEnumParam(Stru_ParamItem *szParam, U8 *buff)
{
	Text Label_Name;
	Combo Combo_Value;
	
	Button Button_OK, Button_Cancel;
	int i = 0;
	int index = 0; 
	unsigned char count = 0;

	if (szParam->Param_Item_Mask != 'e')
	{
		return 0;
	}	
	if (strlen(szParam->Param_Item_Value) == 0)	
	{
		return 0;
	}	
	
	CreateWindow(&Form_InputParam, "", 0,0,160,160);
	sprintf(Form_InputParam.text, "请选择%s", szParam->Param_Item_Name); 
		
	TItem items[MAX_ENUMPARAM_ITEMS];
	for (i = 0; i < MAX_ENUMPARAM_ITEMS; i++)
	{
		memset(items[i], 0, sizeof(items[i]));
	}
	count = GetComboTItem(szParam, items, &index);
	if (index < 0)
	{
		index = 0;
	}
		
	CreateText(&Label_Name, szParam->Param_Item_Name, 6, 9, 148, 15);
	
  CreateCombo(&Combo_Value, &items, count, 10, 25, 140, 20);
  Combo_Value.index = index;
  
  CreateButton(&Button_Cancel, "取消", 10, 108, 40, 20);
  Button_Cancel.OnClick = InputParam_Cancel;
  CreateButton(&Button_OK, "确定", 110, 108, 40, 20);
  Button_OK.OnClick = InputParam_OK;
  
  AddToWindow(&Form_InputParam, &Label_Name);
  AddToWindow(&Form_InputParam, &Combo_Value);
  AddToWindow(&Form_InputParam, &Button_OK);
  AddToWindow(&Form_InputParam, &Button_Cancel);
  
  if (ShowWindow(&Form_InputParam, SS_SHOWMAX) == ID_CANCEL)
  {
  	return 0;
  }
  
  memset(szParam->Param_Value, 0, sizeof(szParam->Param_Value));
  index = GetEnumValue(szParam, items[Combo_Value.index]);

  sprintf(szParam->Param_Value, "%x", index);
  
  buff[0] = (unsigned char)index;
  
  return szParam->Param_Item_Clen;
}


/******************************************************************************/
/*                       698.42/376.1协议参数输入界面                         */ 
/*	功能: 接收时间参数输入，根据数据类型组帧                                  */
/*	参数: szParam——参数项内容, buff——输出数据帧                               */
/*	返回: 数据帧长度                                                          */
/******************************************************************************/
/*unsigned char InputTimeParam(Stru_ParamItem *szParam, U8 *buff)
{
	Text Label_Name;
	Edit Edit_Value;
	CheckBox CheckBox_Rule;
  Button Button_OK, Button_Cancel;
  unsigned char temp[NUM_50] = {0};
  int i = 0;
  int len = 0;
  U32 bcdtime = 0;
  U32 bcddate = 0;
  
  if ((szParam->Param_Item[0]).Param_Item_Mask != 't')
  {
  	return 0;
  }
  
	CreateWindow(&Form_InputParam, "", 0,0,160,160);
	sprintf(Form_InputParam.text, "请输入%s", szParam->Param_Name);     
	
	CreateCheckBox(&CheckBox_Rule, "取系统时间", 10, 12, 148, 20);
	CheckBox_Rule.checked = 1;
	
	CreateText(&Label_Name, "", 6, 40, 148, 45);
	sprintf(Label_Name.text, " %s\n 格式:%s", (szParam->Param_Item[0]).Param_Item_Name, (szParam->Param_Item[0]).Param_Item_Value);
	CreateEdit(&Edit_Value, "", 10, 86, 140, 18);
	//sprintf(Edit_Value.text, "%s", (szParam->Param_Item[0]).Param_Value);
	Edit_Value.OnMsg = EditBoxOnMsg;
	
  CreateButton(&Button_Cancel, "取消", 10, 108, 40, 20);
  Button_Cancel.OnClick = InputParam_Cancel;
  CreateButton(&Button_OK, "确定", 110, 108, 40, 20);
  Button_OK.OnClick = InputParam_OK;	

  AddToWindow(&Form_InputParam, &Label_Name);
  AddToWindow(&Form_InputParam, &CheckBox_Rule);
  AddToWindow(&Form_InputParam, &Edit_Value);
  AddToWindow(&Form_InputParam, &Button_OK);
  AddToWindow(&Form_InputParam, &Button_Cancel);
  
  if (ShowWindow(&Form_InputParam, SS_SHOWMAX) == ID_CANCEL)
  {
  	return 0;
  }	
  
  
  if (CheckBox_Rule.checked == 0)
  {
	  len = strlen(Edit_Value.text);
	  
	  if (len != (szParam->Param_Item[0]).Param_Item_Clen * 2)
	  {
	  	MessageBox("输入格式错误!", STR_CUE, MB_OK);
	  	return 0;
	  }    
	    
	  memset(temp, 0, sizeof(temp)); 
	  sprintf(temp, "%s", Edit_Value.text); 
	  
	  len = (szParam->Param_Item[0]).Param_Item_Clen;
	  StrToHexStr(temp, len, 1, 0, 0);
	  memcpy(buff, temp, len); 
  }
  else
  {
  	len = 0;
  	GetTime(&bcdtime);
  	for (i = 0; i < 3; i++)
  	{
  		buff[len++] = (unsigned char)(bcdtime >> (8 * i));
  	}
  	GetDate(&bcddate);
  	for (i = 0; i < 3; i++)
  	{
  		buff[len++] = (unsigned char)(bcddate >> (8 * i));
  	}
  }
     
  return len;   	
}
*/

void InputTimeStr_OK(HWND Sender)
{
	if (strstr(((GUIObject*)Sender)->text, "取消") == NULL)
	{
		CloseWindow((Window*)(((GUIObject*)Sender)->parent), ID_OK);
	}
	else
	{
		CloseWindow((Window*)(((GUIObject*)Sender)->parent), ID_CANCEL);
	}
}

char* InputTimeStr(const char *caption, const char *format, char *timeStr)
{
	Window Form_InputTimeStr;
	Text Label_Format;
	Edit Edit_Value;
	Button Button_OK, Button_Cancel;
	
	CreateWindow(&Form_InputTimeStr, "", 0,0,160,160);
	sprintf(Form_InputTimeStr.text, "请输入%s", caption);
	
	CreateText(&Label_Format, "", 6, 19, 148, 15);
	CreateEdit(&Edit_Value, "", 10, 37, 140, 18);
	sprintf(Label_Format.text, "格式:%s", format);
	Edit_Value.OnMsg = EditBoxOnMsg;
	
  CreateButton(&Button_Cancel, "取消", 10, 108, 40, 20);
  Button_Cancel.OnClick = InputParam_Cancel;
  CreateButton(&Button_OK, "确定", 110, 108, 40, 20);
  Button_OK.OnClick = InputTimeStr_OK;	

  AddToWindow(&Form_InputTimeStr, &Label_Format);
  AddToWindow(&Form_InputTimeStr, &Edit_Value);
  AddToWindow(&Form_InputTimeStr, &Button_OK);
  AddToWindow(&Form_InputTimeStr, &Button_Cancel);
  
  if (ShowWindow(&Form_InputTimeStr, SS_SHOWMAX) == ID_CANCEL)
  {
  	return NULL;
  }
  else
  {
  	sprintf(timeStr, "%s", Edit_Value.text);
  	return timeStr;
  }	
}


/******************************************************************************/
/*                   698.42/376.1协议上行通信基本参数输入界面                 */ 
/*	功能: 终端上行通信口通信参数设置 接收心跳周期等参数输入并组帧             */
/*	参数: szParam——参数项内容, buff——输出数据帧                               */
/*	返回: 数据帧长度                                                          */
/******************************************************************************/
unsigned char InputAFN4F1Param(U8 *buff)
{
		
	Text Label_Names[MAXTEXTNUM];
	Edit Edit_Value1, Edit_Value2;
	CheckBox CheckBox_Values[MAXCHECKBOXNUM];
  Button Button_OK, Button_Cancel;
  int i = 0, j = 0, len = 0;
  int delayTimes = 0;
  int tmpint = 0;
  int checkBoxNum = NUM_3;
  
  TItem names[] = {"重发次数","主站确认标志","心跳周期",};
  TItem items[] = {"1类数据自动上报","2类数据自动上报","3类数据自动上报",};

	CreateWindow(&Form_InputParam, "", 0,0,160,160);
	sprintf(Form_InputParam.text, "请输入%s", "通信基本参数");   
	
	delayTimes = StrToInt((GlobalParamItem+2)->Param_Value, HEX);
	CreateText(&Label_Names[i], names[i], 3, 4, 50, 15);
	CreateEdit(&Edit_Value1, "", 55, 3, 99, 15);
	sprintf(Edit_Value1.text, "%d", (delayTimes >> NUM_12) & NUM_3);
	Edit_Value1.length = 1;
	Edit_Value1.OnMsg = EditBoxOnMsg;
	Edit_Value1.OnClick = InputParam_OK;
	AddToWindow(&Form_InputParam, &Label_Names[i]);
	AddToWindow(&Form_InputParam, &Edit_Value1);
	
	i++;
	CreateText(&Label_Names[i], names[i], 3, 23, 148, 15);
	AddToWindow(&Form_InputParam, &Label_Names[i]);
	for (j = 0; j < checkBoxNum; j++)
	{
		CreateCheckBox(&CheckBox_Values[j], items[j], 3, 38+j*16, 148, 15);
		CheckBox_Values[j].checked = 1;
		AddToWindow(&Form_InputParam, &CheckBox_Values[j]);
	}
	
	i++;
	CreateText(&Label_Names[i], names[i], 3, 40+j*16, 50, 15);
	CreateEdit(&Edit_Value2, "", 55, 39+j*16, 99, 15);
	sprintf(Edit_Value2.text, "%s", (GlobalParamItem + 4)->Param_Value);
	Edit_Value2.OnMsg = EditBoxOnMsg;
	Edit_Value2.OnClick = InputParam_OK;
	AddToWindow(&Form_InputParam, &Label_Names[i]);
	AddToWindow(&Form_InputParam, &Edit_Value2);
	
	CreateButton(&Button_Cancel, "取消", 10, 108, 40, 20);
  Button_Cancel.OnClick = InputParam_Cancel;
  CreateButton(&Button_OK, "确定", 110, 108, 40, 20);
  Button_OK.OnClick = InputParam_OK;	
  
  AddToWindow(&Form_InputParam, &Button_OK);
  AddToWindow(&Form_InputParam, &Button_Cancel);
  
  if (ShowWindow(&Form_InputParam, SS_SHOWMAX) == ID_CANCEL)
  {
  	return 0;
  }
  
  tmpint = StrToInt(Edit_Value1.text, DEC);
  if (tmpint > 3)
  {
  	MessageBox("输入数字过大!", STR_CUE, MB_OK);
  	return 0;
  }
  
  i = 0;
  buff[len++] = StrToInt((GlobalParamItem+i)->Param_Value, DEC);
  
  i++;
  buff[len++] = StrToInt((GlobalParamItem+i)->Param_Value, DEC);
  
  i++;
  delayTimes = (delayTimes & 0x0fff) + (tmpint << NUM_12);
  sprintf((GlobalParamItem+i)->Param_Value, "%04x", delayTimes);
  buff[len++] = (U8)delayTimes;
  buff[len++] = (U8)((delayTimes >> NUM_8) & 0x3f); 
  
  i++;
  tmpint = 0;
  for (j = 0; j < checkBoxNum; j++)
  {
  	tmpint += (CheckBox_Values[j].checked * (1 << j));
  }
  buff[len++] = (U8)tmpint;
  sprintf((GlobalParamItem+i)->Param_Value, "%02x", tmpint);
  
  i++;
  buff[len++] = StrToInt(Edit_Value2.text, DEC);
  sprintf((GlobalParamItem+i)->Param_Value, "%s", Edit_Value2.text);
  
  return len;
  		
}


/******************************************************************************/
/*                   698.42/376.1协议上行通信基本参数输入界面                 */ 
/*	功能: 上行通信工作方式设置并组帧                                          */
/*	参数: paramindex——全局参数菜单索引, buff——输出数据帧                      */
/*	返回: 数据帧长度                                                          */
/******************************************************************************/
unsigned char InputAFN4F8Param(int paramindex, U8 *buff)
{
		
	Text Label_Names[MAXTEXTNUM];
	Edit Edit_Values[MAXEDITNUM];
	Combo Combo_Values[MAXCOMBONUM];
  Button Button_OK, Button_Cancel;
  int i = 0, j = 0, len = 0;
  U32 tmpint = 0;
  
  TItem names[] = {"工作模式", "永久在线重播间隔(s)", "被动激活重播次数", "自动断线时间(min)", "在线时段",};
  TItem items1[] = {"TCP", "UDP",};
  TItem items2[] = {"混合模式", "客户机模式", "服务器模式",};
  TItem items3[] = {"永久在线", "被动激活", "时段在线",};

	CreateWindow(&Form_InputParam, "", 0,0,160,160);
	sprintf(Form_InputParam.text, "%s", (GlobalParam+paramindex)->Param_Name);   
	
	tmpint = StrToInt((GlobalParamItem+i)->Param_Value, HEX);
	CreateText(&Label_Names[i], names[i], 2, 3, 50, 15);
	
	CreateCombo(&Combo_Values[0], items1, sizeof(items1)/sizeof(TItem), 80, 2, 76, 18);
	Combo_Values[0].index = (tmpint >> 7) & 0x01;
	CreateCombo(&Combo_Values[1], items2, sizeof(items2)/sizeof(TItem), 2, 21, 76, 18);
	Combo_Values[1].index = (tmpint >> 4) & 0x03;
	CreateCombo(&Combo_Values[2], items3, sizeof(items3)/sizeof(TItem), 80, 21, 76, 18);
	tmpint = (tmpint & 0x03) - 1;
	if (tmpint < 0 || tmpint > sizeof(items3)/sizeof(TItem))
	{
		tmpint = 0;
	}
	Combo_Values[2].index = tmpint;
	
	AddToWindow(&Form_InputParam, &Label_Names[i]);
	AddToWindow(&Form_InputParam, &Combo_Values[0]);	
	AddToWindow(&Form_InputParam, &Combo_Values[1]);		
	AddToWindow(&Form_InputParam, &Combo_Values[2]);	
		
	i++;
	for (; i < (GlobalParam+paramindex)->Param_Num; i++)
	{
		if (i == (GlobalParam+paramindex)->Param_Num - 1)
		{			
			CreateText(&Label_Names[i], names[i], 2, 28+i*16, 50, 15);
			CreateEdit(&Edit_Values[i], (GlobalParamItem+i)->Param_Value, 55, 27+i*16, 101, 15);
		}
		else
		{		
			CreateText(&Label_Names[i], names[i], 2, 28+i*16, 121, 15);
			CreateEdit(&Edit_Values[i], (GlobalParamItem+i)->Param_Value, 122, 27+i*16, 34, 15);
		}
		Edit_Values[i].OnMsg = EditBoxOnMsg;
		Edit_Values[i].OnClick = InputParam_OK;
		
		AddToWindow(&Form_InputParam, &Label_Names[i]);
		AddToWindow(&Form_InputParam, &Edit_Values[i]);		
	}

  CreateButton(&Button_Cancel, "取消", 10, 108, 40, 20);
  Button_Cancel.OnClick = InputParam_Cancel;
  CreateButton(&Button_OK, "确定", 110, 108, 40, 20);
  Button_OK.OnClick = InputParam_OK;	
  
  AddToWindow(&Form_InputParam, &Button_OK);
  AddToWindow(&Form_InputParam, &Button_Cancel);
	
  if (ShowWindow(&Form_InputParam, SS_SHOWMAX) == ID_CANCEL)
  {
  	return 0;
  }
  
  i = 0;
  len = 0;
  
  tmpint = Combo_Values[0].index * 0x80 + ((Combo_Values[1].index << 4) & 0x30)
  			 + Combo_Values[2].index + 1;  			 
	buff[len++] = (U8)tmpint;
	sprintf((GlobalParamItem+i)->Param_Value, "%02x", tmpint);
	
	i++;	
  tmpint = StrToInt(Edit_Values[i].text, DEC);
  buff[len++] = (U8)tmpint;
  buff[len++] = (U8)(tmpint >> 8);
  sprintf((GlobalParamItem+i)->Param_Value, "%s", Edit_Values[i].text);
  
  i++;  
  tmpint = StrToInt(Edit_Values[i].text, DEC);
  buff[len++] = (U8)tmpint;
  sprintf((GlobalParamItem+i)->Param_Value, "%s", Edit_Values[i].text);
  
  i++; 
  tmpint = StrToInt(Edit_Values[i].text, DEC);
  buff[len++] = (U8)tmpint;
  sprintf((GlobalParamItem+i)->Param_Value, "%s", Edit_Values[i].text);
  
  i++;  
  tmpint = StrToInt(Edit_Values[i].text, HEX);
  buff[len++] = (U8)(tmpint >> 16);
  buff[len++] = (U8)(tmpint >> 8);
  buff[len++] = (U8)tmpint;
  sprintf((GlobalParamItem+i)->Param_Value, "%s", Edit_Values[i].text);
  
  return len;  		
}


void InputAFN4F33ParamItem(HWND Sender)
{
	int i = 0;
	int index = 0;
	int count = 0;
	unsigned char value[200] = {0};
	TItem items[MAX_ITEMS] = {""};
	TItem names[] = {"抄表控制字", "抄表日日期", "抄表日时间", 
  	"抄表间隔", "校时时间", "抄表时段",};
  	
  count = sizeof(names)/sizeof(TItem);
 	for (i = 0; i < count; i++)
 	{
 		if (strstr(((GUIObject*)Sender)->text, names[i]) != NULL)
 		{
 			break;
 		}
// 		count = ((GUIObject*)Sender)->top;
// 		if (count > 18 * i && count < 18 * (i + 1))
// 		{
// 			break;
// 		}  		
 	}
 	
 	if (i >= count)
 	{
 		return;		
 	}
 	
 	i += 2;
 	switch((GlobalParamItem+i)->Param_Item_Mask)
 	{
 		case 'B':
 		{
			count = GetComboTItem(GlobalParamItem+i, items, &index);
			if (count < 1)
			{
				return;
			}
			sprintf(value, "%s", (GlobalParamItem+i)->Param_Value);
			FormBSDataPro((GlobalParamItem+i)->Param_Item_Name, items, count, value); 			
 			break;
 		}
 		case 't':
 		{
	 		sprintf(value, "%s", (GlobalParamItem+i)->Param_Value);
	 		InputTimeStr((GlobalParamItem+i)->Param_Item_Name, 
	 								(GlobalParamItem+i)->Param_Item_Value, value); 			
 			break;
 		}
 		case 'l':
	 	{
	 		count = StrToInt((GlobalParamItem+i)->Param_Value, DEC);
	 		memset(value, 0, sizeof(value));
	 		sprintf(value, "%s", (GlobalParamItem+i+1)->Param_Value);
	 		if (FormTimeDataPro((GlobalParamItem+i+1)->Param_Item_Name, 
	 				&count, 24, value) == ID_CANCEL)
	 		{
	 			return;
	 		}
	 		sprintf((GlobalParamItem+i)->Param_Value, "%d", count);
	 		sprintf((GlobalParamItem+i+1)->Param_Value, "%s", value);
	 		memset(value, 0, sizeof(value));
	 		sprintf(value, "%d", count);
	 		break;
	 	}
 		default:
 			return;
 	} 	
 	
	sprintf(((Edit*)(((Button*)Sender)->next))->text, "%s", value);

	Refresh(&Form_InputParam);
}


/******************************************************************************/
/*                   698.42/376.1协议上行通信基本参数输入界面                 */ 
/*	功能: 上行通信工作方式设置并组帧                                          */
/*	参数: paramindex——全局参数菜单索引, buff——输出数据帧                      */
/*	返回: 数据帧长度                                                          */
/******************************************************************************/
unsigned char InputAFN4F33Param(int paramindex, U8 *buff)
{
		
//	Text Label_Names[MAXTEXTNUM];
	Button Button_Names[MAXTEXTNUM];
	Edit Edit_Values[MAXEDITNUM];
  Button Button_OK, Button_Cancel;
  int i = 0, j = 0, len = 0;
  int index = 0;
  U32 tmpint = 0;
  unsigned char temp[200] = {0};
    
  TItem names[] = {"抄表控制字", "抄表日日期", "抄表日时间", 
  	"抄表间隔", "校时时间", "抄表时段",};
  TItem items1[16] = {""};
  TItem items2[32] = {""};
  TItem items3[24] = {""};

	while(1)
  { 
	  tmpint = StrToInt((GlobalParamItem+1)->Param_Value, DEC);
	  
	  if (tmpint > 31 || tmpint < 1)
	  {
	  	tmpint = 31;
	  } 
 
  	memset(temp, 0, sizeof(temp));
  	sprintf(temp, "%d", tmpint);
	  if (InputBox("范围[1, 31]", "请输入端口", temp, 2) == ID_CANCEL)
	  {
	  	return 0;  	
	  }
	  
	  tmpint = StrToInt(temp, DEC);
	  if (tmpint > 31 || tmpint < 1)
	  {	  	
	  	continue;
	  }	
	  break;  
	}

	CreateWindow(&Form_InputParam, "", 0,0,160,160);
	sprintf(Form_InputParam.text, "%s", (GlobalParam+paramindex)->Param_Name);   

	for (i = 0; i < MAXEDITNUM; i++)
	{
//		CreateText(&Label_Names[i], names[i], 2, 2+35*i, 156, 15);
//		AddToWindow(&Form_InputParam, &Label_Names[i]);
		CreateButton(&Button_Names[i], names[i], 2, 1+18*i, 70, 18);
		AddToWindow(&Form_InputParam, &Button_Names[i]);
		Button_Names[i].OnClick = InputAFN4F33ParamItem;
							
		CreateEdit(&Edit_Values[i], (GlobalParamItem + i + 2)->Param_Value, 
										75, 1+18*i, 83, 18);
		Edit_Values[i].OnMsg = EditBoxOnMsg;		
		Edit_Values[i].OnClick = InputParam_OK;
		AddToWindow(&Form_InputParam, &Edit_Values[i]);
	}

  CreateButton(&Button_Cancel, "取消", 10, 110, 40, 20);
  Button_Cancel.OnClick = InputParam_Cancel;
  CreateButton(&Button_OK, "确定", 110, 110, 40, 20);
  Button_OK.OnClick = InputParam_OK;	
  
  AddToWindow(&Form_InputParam, &Button_OK);
  AddToWindow(&Form_InputParam, &Button_Cancel);
	
  if (ShowWindow(&Form_InputParam, SS_SHOWMAX) == ID_CANCEL)
  {
  	return 0;
  }
  
  i = 0;
  len = 0;
  index = 0;
  
  buff[len++] = 1;
  sprintf(GlobalParamItem->Param_Value, "1");
  
  index++;
//  buff[len++] = 31;
//  sprintf((GlobalParamItem+index)->Param_Value, "31");
	buff[len++] = (unsigned char)tmpint;
	sprintf((GlobalParamItem+index)->Param_Value, "%d", tmpint);
  
  index++;
  tmpint = StrToInt(Edit_Values[i].text, HEX);
  buff[len++] = (unsigned char)tmpint;
  buff[len++] = (unsigned char)(tmpint >> 8);
  sprintf((GlobalParamItem+index)->Param_Value, "%04x", tmpint);
 
  i++;
  index++;
  tmpint = StrToInt(Edit_Values[i].text, HEX);
  buff[len++] = (unsigned char)tmpint;
  buff[len++] = (unsigned char)(tmpint >> 8);
  buff[len++] = (unsigned char)(tmpint >> 16);
  buff[len++] = (unsigned char)(tmpint >> 24);
  sprintf((GlobalParamItem+index)->Param_Value, "%08x", tmpint);  
  
  i++;
  index++;
  tmpint = StrToInt(Edit_Values[i].text, HEX);    
  buff[len++] = (unsigned char)tmpint;
  buff[len++] = (unsigned char)(tmpint >> 8);
  sprintf((GlobalParamItem+index)->Param_Value, "%04x", tmpint); 
  
  i++;
  index++;
  tmpint = StrToInt(Edit_Values[i].text, DEC);  
  buff[len++] = (unsigned char)tmpint;
  sprintf((GlobalParamItem+index)->Param_Value, "%d", tmpint);   
  
  i++;
  index++;
  tmpint = StrToInt(Edit_Values[i].text, HEX);    
  buff[len++] = (unsigned char)tmpint;
  buff[len++] = (unsigned char)(tmpint >> 8);
  buff[len++] = (unsigned char)(tmpint >> 16);
  sprintf((GlobalParamItem+index)->Param_Value, "%06x", tmpint);   
  
  i++;
  index++;
  tmpint = StrToInt(Edit_Values[i].text, DEC);  
  buff[len++] = (unsigned char)tmpint;
  sprintf((GlobalParamItem+index)->Param_Value, "%d", tmpint);       
  
  i++;
  index++;
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%s", (GlobalParamItem+index)->Param_Value);
  tmpint = tmpint * 4;
  StrToHexStr(temp, tmpint, 0, 0, 0);
  memcpy(buff+len, temp, tmpint);
  len += tmpint;
  
  return len;  		
}


/******************************************************************************/
/*                          698.42/376.1协议参数输入界面                      */ 
/*	功能: 698.42/376.1协议参数设置输入界面                                    */
/*	参数: paramindex——全局参数(GlobalParam)菜单索引, buff——输出数据帧         */
/*	返回: 数据帧长度                                                          */
/******************************************************************************/
int InputParam(int paramindex, U8 buff[])
{
	int len = 0;
	PageNum = 0;
	PageIndex = 0;
		
	if ((GlobalParam+paramindex)->Pn == 1)
	{		
		return len;		//可扩展测量点参数
	}
	if ((GlobalParam+paramindex)->Param_Num == 1)
	{
		switch(GlobalParamItem->Param_Item_Mask)
		{
			case 'e':
				len = InputEnumParam(GlobalParamItem, buff);
				break;
			case 't':
				break;
			default:
				len = InputStrParam(GlobalParamItem, buff);
				break;
		}
	}
	else
	{
		if ((GlobalParam+paramindex)->AFN == AFN_698SET 
			 || (GlobalParam+paramindex)->AFN == AFN_698READ)
		{
			switch((GlobalParam+paramindex)->Fn)
			{
				//case 1:  //F1特殊处理
				//	len = InputAFN4F1Param(buff);
				//	break;
				case 8:  //F8特殊处理
					len = InputAFN4F8Param(paramindex, buff);
					break;
				case 10: //F10特殊处理
					break;
				case 33:
					len = InputAFN4F33Param(paramindex, buff);
					break;
				default:
					len = InputCommParam(paramindex, buff);
					break;
			}
		}
		else
		{
			len = InputCommParam(paramindex, buff);
		}
	}
	
	if ((len == 7) && (GlobalParam+paramindex)->Fn == 1) //F1特殊处理
	{
		if ((GlobalParam+paramindex)->AFN == AFN_698SET 
			 || (GlobalParam+paramindex)->AFN == AFN_698READ)
		{
			U16 tmpint = (buff[2] + buff[3] * 0x100) & 0x0fff;
			buff[2] = (U8)tmpint;
			buff[3] = ((buff[4] << 4) + (U8)(tmpint >> 8)) & 0x3f;
			buff[4] = buff[6];
			len--;
		}
	}
	
	return len;
}
#endif