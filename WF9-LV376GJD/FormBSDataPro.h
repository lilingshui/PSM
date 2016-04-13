#ifndef	_FormBSDataPro_H
#define _FormBSDataPro_H

#include "jblib.h"
#include "DataConvert.h"
#include "InputBoxGroupPro.h"

#define MAX_ITEM_LEN 19
#define MAX_ITEMS 32
#define STR_CHECK "√"

Window Form_BSDataPro;
ListBox ListBox_BSDataPro;
TItem ListBox_BSDataProItems[MAX_ITEMS];
Text Label_BSDataPro;
CheckBox CheckBox_BSDataPro;
int Max_TimeItems;

void FormBSDataProOK(HWND Sender);
void FormBSDataProCancel(HWND Sender);
char FormBSDataPro_Load(PItem items, int count, unsigned char* value);
void ListBox_BSDataPro_OnClick(HWND Sender);
void ListBox_BSDataPro_OnMsg(HWND Obj, PMsg Msg);
void CheckBox_BSDataPro_OnClick(HWND Sender);
void FormBSDataProHelp(HWND Sender);


unsigned int FormBSDataPro(P_U8 caption, PItem items, int count, unsigned char* value)
{

  Button Button_OK;
  char checked = 0;
  
  checked = FormBSDataPro_Load(items, count, value);
  
  if (checked < 0)
  {
  	return ID_CANCEL;
  }
  
  CreateListBox(&ListBox_BSDataPro, ListBox_BSDataProItems, count, 3, 3, 154, 102);
  ListBox_BSDataPro.OnMsg = ListBox_BSDataPro_OnMsg;  
  ListBox_BSDataPro.OnClick = ListBox_BSDataPro_OnClick;
  CreateText(&Label_BSDataPro, "", 2, 111, 70, 18);
  sprintf(Label_BSDataPro.text, "%s", value);
  CreateCheckBox(&CheckBox_BSDataPro, "全选2", 74, 108, 40, 20);
  CheckBox_BSDataPro.checked = checked;
  CheckBox_BSDataPro.OnClick = CheckBox_BSDataPro_OnClick;
  CreateButton(&Button_OK, "确定3", 121, 108, 38, 20);
  Button_OK.OnClick = FormBSDataProOK;

  CreateWindow(&Form_BSDataPro, caption, 0, 0, 160, 160);
  AddToWindow(&Form_BSDataPro, &ListBox_BSDataPro);
  AddToWindow(&Form_BSDataPro, &CheckBox_BSDataPro);
  AddToWindow(&Form_BSDataPro, &Button_OK);
  AddToWindow(&Form_BSDataPro, &Label_BSDataPro);
    
  if (ShowWindow(&Form_BSDataPro, SS_SHOWMAX) == ID_CANCEL)
  {
  	return ID_CANCEL;
  }
  
  memcpy(value, Label_BSDataPro.text, strlen(value));
 
  return ID_OK;
}


char FormBSDataPro_Load(PItem items, int count, unsigned char* value)
{
	int i = 0;
	int selCount = 0;
	int len = 0;
	char temp[MAX_ITEMS] = {0};
	char item[MAX_ITEM_LEN] = {0};
	
	if (count < 1)
	{
		return -1;
	}
	
	memset(ListBox_BSDataProItems, 0, sizeof(ListBox_BSDataProItems));
	
	strcpy(temp, value);
	len = strlen(temp) / 2; 
	StrToHexStr(temp, len, 1, '0', 0);
     
	for (i = 0; i < count; i++)
	{
		if (strlen(items+i) >= MAX_ITEM_LEN - 3)
		{
			return -1;
		}
				
		memset(item, 0, sizeof(item));
		strcpy(item, items+i);
	
		if ((temp[i / 8] >> (i % 8)) & 0x01)
		{
			strcat(item, STR_CHECK);
			selCount++;
		}		
				
		strcpy(ListBox_BSDataProItems[i], item);		
	}
	
	if (selCount == count)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}


void ListBox_BSDataPro_OnClick(HWND Sender)
{	
	int index = 0;
	int selCount = 0;
	unsigned char value = 0;
	unsigned char temp[MAX_ITEM_LEN] = {0};
	unsigned char item[MAX_ITEM_LEN] = {0};
	
	if (ListBox_BSDataPro.count < 1)
	{
		return;
	}
	
	index = ListBox_BSDataPro.index;
	strcpy(temp, ListBox_BSDataProItems[index]);
		
	if (strstr(temp, STR_CHECK) == NULL)
	{		
		strcpy(item, ListBox_BSDataProItems[index]);
		strcat(item, STR_CHECK);
	}
	else
	{
		memcpy(item, ListBox_BSDataProItems[index], 
						strlen(ListBox_BSDataProItems[index]) - 2);
	}
	
	memset(ListBox_BSDataProItems[index], 0, sizeof(ListBox_BSDataProItems[index]));
	strcpy(ListBox_BSDataProItems[index], item);
	
	memset(temp, 0, sizeof(temp));
	for (index = 0; index < ListBox_BSDataPro.count; index++)
	{
		memset(item, 0, sizeof(item));
		strcpy(item, ListBox_BSDataProItems[index]);
		if (strstr(item, STR_CHECK) != NULL)
		{
			value += (1 << (index % 8));
			selCount++;
		}
		
		if (index % 8 == 7)
		{
			sprintf(temp + strlen(temp), "%02x", value);
			value = 0;
		}
	}
	
	if (index % 8 != 0)
	{
		sprintf(temp + strlen(temp), "%02x", value);
	}
	
	if (selCount == ListBox_BSDataPro.count)
	{
		CheckBox_BSDataPro.checked = 1;
	}
	else
	{
		CheckBox_BSDataPro.checked = 0;
	}
	
	value = strlen(Label_BSDataPro.text);
	if (value > strlen(temp))
	{
		value = value - strlen(temp);
		memcpy(Label_BSDataPro.text + value, temp, strlen(temp));
	}
	else
	{
		sprintf(Label_BSDataPro.text, "%s", temp);
	}
	
	Refresh(&Form_BSDataPro);	
}


void ListBox_BSDataPro_OnMsg(HWND Obj, PMsg Msg)
{
	if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_UP:
        if (ListBox_BSDataPro.index == 0)
        {
          ListBox_BSDataPro.index = ListBox_BSDataPro.count - 1;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_DOWN:
        if (ListBox_BSDataPro.index == ListBox_BSDataPro.count - 1)
        {
          ListBox_BSDataPro.index = 0;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_2:
      	CheckBox_BSDataPro_OnClick(NULL);
      	Msg->Result = MSG_OK;
      	break;
      case VK_3:
        FormBSDataProOK(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_0:
        FormBSDataProHelp(NULL);
        Msg->Result = MSG_OK;
        break; 
      default:
      	break;        
    }
  }
}


void CheckBox_BSDataPro_OnClick(HWND Sender)
{
	int i = 0;
	unsigned char temp[MAX_ITEMS] = {0};	
	unsigned char item[MAX_ITEM_LEN] = {0};
	
	if (CheckBox_BSDataPro.checked)
	{
		CheckBox_BSDataPro.checked = 0;
		
		for (i = 0; i < ListBox_BSDataPro.count; i++)
		{
			memset(item, 0, sizeof(item));
			strcpy(item, ListBox_BSDataProItems[i]);
			if (strstr(item, STR_CHECK) != NULL)
			{
				memset(temp, 0, sizeof(temp));
				memcpy(temp, ListBox_BSDataProItems[i], 
								strlen(ListBox_BSDataProItems[i]) - 2);
				memset(ListBox_BSDataProItems[i], 0, 
								sizeof(ListBox_BSDataProItems[i]));
				strcpy(ListBox_BSDataProItems[i], temp);				
			}
		}
		
		memset(Label_BSDataPro.text, '0', strlen(Label_BSDataPro.text));		
	}
	else
	{
		CheckBox_BSDataPro.checked = 1;

		for (i = 0; i < ListBox_BSDataPro.count; i++)
		{
			memset(item, 0, sizeof(item));
			strcpy(item, ListBox_BSDataProItems[i]);			
			if (strstr(item, STR_CHECK) == NULL)
			{
				memset(temp, 0, sizeof(temp));
				strcpy(temp, ListBox_BSDataProItems[i]); 
				strcat(temp, STR_CHECK);
				strcpy(ListBox_BSDataProItems[i], temp);				
			}
		}		
		
		memset(Label_BSDataPro.text, 'f', strlen(Label_BSDataPro.text));
	}
	
	
		
	Refresh(&Form_BSDataPro);
}


void FormBSDataProOK(HWND Sender)
{
  CloseWindow(&Form_BSDataPro, ID_OK);
}


void FormBSDataProCancel(HWND Sender)
{
  CloseWindow(&Form_BSDataPro, ID_CANCEL);
}


void FormBSDataProHelp(HWND Sender)
{
	MessageBox("BS位结构参数编辑\n确认键：选择\n按键2：全选\n按键3：确认\n按键0：帮助", "帮助", MB_OK);
}



char FormTimeDataPro_Load(int count, unsigned char* value)
{
	int i = 0;
	int len = 0;
	char temp[MAX_ITEMS] = {0};
	char item[MAX_ITEM_LEN] = {0};
	
	if (count < 1)
	{
		return 0;
	}
	memset(ListBox_BSDataProItems, 0, sizeof(ListBox_BSDataProItems));
	     
  len = strlen(value);
  if (len % 8 != 0)
  {
  	return -1;
  }
  if (count != len / 8)
  {
  	return -1;
  }
  
	for (i = 0; i < count; i++)
	{
		memcpy(temp, value+i*8, 8);
		StrToHexStr(temp, 4, 0, 0, 0);
		sprintf(item, "%02x:%02x-%02x:%02x", temp[1], temp[0], temp[3], temp[2]); 
		
		strcpy(ListBox_BSDataProItems[i], item);		
	}
	
	return 1;			
}


void FormTimeDataProAdd(HWND Sender)
{
	int count = 0;
	unsigned char stime[MAX_ITEM_LEN] = {0};
	unsigned char etime[MAX_ITEM_LEN] = {0};
	unsigned char temp[MAX_ITEM_LEN] = {0};
		
	if (ListBox_BSDataPro.count >= Max_TimeItems)
	{
		MessageBox("已达上限值", "提示", MB_OK);
		return;
	}
	
	while(1)
	{
	  memset(stime, 0, sizeof(stime));
	  memset(etime, 0, sizeof(etime));      
	  sprintf(stime, "起始(hhmm)");
	  sprintf(etime, "结束(hhmm)");
	  
	  if (InputBoxGroupPro("请输入抄表时段", stime, etime) == 0)
	  {
	    return;
	  } 
	  
	  if (strlen(stime) != 4 || strlen(etime) != 4)
  	{
  		MessageBox("格式错误!", "提示", MB_OK);
  		continue;
  	}
  	break;
	}
  
  sprintf(temp, "%c%c:%c%c-%c%c:%c%c", stime[0], stime[1], stime[2], stime[3],
  				etime[0], etime[1], etime[2], etime[3]);
  
  count = ListBox_BSDataPro.count; 		
  strcpy(ListBox_BSDataProItems[count], temp);
  ListBox_BSDataPro.index = count;
  count++;
  ListBox_BSDataPro.count = count;
  
	Refresh(&Form_BSDataPro);
}

void FormTimeDataProAmend(HWND Sender)
{
	int i = 0;
	unsigned char stime[MAX_ITEM_LEN] = {0};
	unsigned char etime[MAX_ITEM_LEN] = {0};
	unsigned char temp[MAX_ITEM_LEN] = {0};
	
	if (ListBox_BSDataPro.count < 1)
	{
		return;
	}
		
	i = ListBox_BSDataPro.index;	
		
	while(1)
	{
	  memset(stime, 0, sizeof(stime));
	  memset(etime, 0, sizeof(etime));      
	  sprintf(stime, "起始(hhmm)");
	  sprintf(etime, "结束(hhmm)");
	  
	  if (InputBoxGroupPro("请输入抄表时段", stime, etime) == 0)
	  {
	    return;
	  } 
	  
	  if (strlen(stime) != 4 || strlen(etime) != 4)
  	{
  		MessageBox("格式错误!", "提示", MB_OK);
  		continue;
  	}
  	break;
	}
  
  sprintf(temp, "%c%c:%c%c-%c%c:%c%c", stime[0], stime[1], stime[2], stime[3],
  				etime[0], etime[1], etime[2], etime[3]);
  		
  strcpy(ListBox_BSDataProItems[i], temp);
  ListBox_BSDataPro.index = i;
  
  Refresh(&Form_BSDataPro);
}

void FormTimeDataProDel(HWND Sender)
{
	int i = 0;
	int count = 0;
	unsigned char temp[MAX_ITEM_LEN] = {0};
	
	if (ListBox_BSDataPro.count < 1)
	{
		return;
	}
	
	if (MessageBox("确定要删除该时段吗?", "提示", MB_OKCANCEL) == ID_CANCEL)
	{
		return;
	}
	
	for (i = ListBox_BSDataPro.index; i < ListBox_BSDataPro.count - 1; i++)
  {
  	strcpy(ListBox_BSDataProItems[i], ListBox_BSDataProItems[i + 1]);  	
  }
  memset(ListBox_BSDataProItems[i], 0, sizeof(TItem));
		
	count = ListBox_BSDataPro.count;
	ListBox_BSDataPro.count = count - 1;;
	
  Refresh(&Form_BSDataPro);
}

void FormTimeDataProSave(HWND Sender)
{
	CloseWindow(&Form_BSDataPro, ID_OK);
}

void FormTimeDataProHelp(HWND Sender)
{
	MessageBox("抄表时段编辑\n确认键：修改\n按键1：添加\n按键2：修改\n按键3/删除键：删除\n按键4：保存\n按键5：帮助", "帮助", MB_OK);
}

void ListBox_TimeDataPro_OnMsg(HWND Obj, PMsg Msg)
{
	if (Msg->Msg == WM_CHAR)
  {
    switch (Msg->WParam)
    {
      case VK_UP:
        if (ListBox_BSDataPro.index == 0)
        {
          ListBox_BSDataPro.index = ListBox_BSDataPro.count - 1;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_DOWN:
        if (ListBox_BSDataPro.index == ListBox_BSDataPro.count - 1)
        {
          ListBox_BSDataPro.index = 0;
          Msg->Result = MSG_OK;
          Refresh(Obj);
        }
        break; 
      case VK_1:
      	FormTimeDataProAdd(NULL);
      	Msg->Result = MSG_OK;
      	break;        
      case VK_2:
      	FormTimeDataProAmend(NULL);
      	Msg->Result = MSG_OK;
      	break;
      case VK_3:
        FormTimeDataProDel(NULL);
        Msg->Result = MSG_OK;
        break; 
      case VK_4:
        FormTimeDataProSave(NULL);
        Msg->Result = MSG_OK;
        break;  
      case VK_5:       
      case VK_0:
        FormTimeDataProHelp(NULL);
        Msg->Result = MSG_OK;
        break; 
      default:
      	break;        
    }
  }
}


unsigned char* FormTimeDataSave(unsigned char* value)
{
	int i = 0;
	int j = 0;
	unsigned char temp[MAX_ITEM_LEN] = {0};
	unsigned char item[MAX_ITEM_LEN] = {0};
		
	for (i = 0; i < ListBox_BSDataPro.count; i++)
	{
		memset(item, 0, sizeof(item));
		memset(temp, 0, sizeof(temp));
		strcpy(item, ListBox_BSDataProItems[i]);
//		for (j = 0; j < strlen(item); j += 3)
//		{
//			memcpy(temp+strlen(temp), item+j, 2);
//		}
		sprintf(temp, "%c%c%c%c%c%c%c%c", item[3], item[4], item[0], item[1],
						item[9], item[10], item[6], item[7]);
		
		memcpy(value+strlen(value), temp, strlen(temp));		
	}
	
	return value;
}

unsigned int FormTimeDataPro(P_U8 caption, int* count, int max, unsigned char* value)
{
  Button Button_Add, Button_Amend, Button_Del, Button_OK, Button_Help;

  if (max < *count)
  {
  	return ID_CANCEL;
  }
    
  if (FormTimeDataPro_Load(*count, value) < 0)
  {
  	return ID_CANCEL;
  }

  Max_TimeItems = max;
 
  CreateListBox(&ListBox_BSDataPro, ListBox_BSDataProItems, *count, 3, 3, 115, 124);
  ListBox_BSDataPro.OnMsg = ListBox_TimeDataPro_OnMsg;  
  ListBox_BSDataPro.OnClick = FormTimeDataProAmend;
  CreateButton(&Button_Add, "添加1", 119, 3, 38, 20);
  Button_Add.OnClick = FormTimeDataProAdd;
  CreateButton(&Button_Amend, "修改2", 119, 29, 38, 20);
  Button_Amend.OnClick = FormTimeDataProAmend;
  CreateButton(&Button_Del, "删除3", 119, 55, 38, 20);
  Button_Del.OnClick = FormTimeDataProDel;  
  CreateButton(&Button_OK, "保存4", 119, 81, 38, 20);
  Button_OK.OnClick = FormTimeDataProSave;
  CreateButton(&Button_Help, "帮助5", 119, 107, 38, 20);
  Button_Help.OnClick = FormTimeDataProHelp;

  CreateWindow(&Form_BSDataPro, caption, 0, 0, 160, 160);
  AddToWindow(&Form_BSDataPro, &ListBox_BSDataPro);
  AddToWindow(&Form_BSDataPro, &Button_Add);
  AddToWindow(&Form_BSDataPro, &Button_Amend);
  AddToWindow(&Form_BSDataPro, &Button_Del);
  AddToWindow(&Form_BSDataPro, &Button_OK);
  AddToWindow(&Form_BSDataPro, &Button_Help);
    
  if (ShowWindow(&Form_BSDataPro, SS_SHOWMAX) == ID_CANCEL)
  {
  	return ID_CANCEL;
  }
  
  *count = ListBox_BSDataPro.count;
  memset(value, 0, strlen(value));
  FormTimeDataSave(value);
 
  return ID_OK;
}
#endif
