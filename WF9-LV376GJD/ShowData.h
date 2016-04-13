#ifndef	_ShowData_H
#define _ShowData_H

#include "jblib.h"
#include "Global.h"
  
#define MAX_SHOW_LINES 10
#define MAX_SHOW_CHARS 20

Window Form_ShowMsg;
Text Text_ShowMsg;
long MsgOffSet;
U8* MsgFileName;

void Form_ShowMsg_OnKeyPress(HWND Obj, PMsg Msg);
long ReadMsgInfo(P_U8 filename, long offSet, P_U8 msgtext, U8 keyValue);

//
void Form_ShowMsg_OnKeyPress(HWND Obj, PMsg Msg)
{
	int flag = 1;
	long oldOffSet = MsgOffSet;
	
  if(Msg->Msg == WM_CHAR)
  { 
  	switch(Msg->WParam)
  	{
  		case VK_LEFT:
  		case VK_RIGHT:
  		case VK_UP:
  		case VK_DOWN:
  		{
				MsgOffSet = ReadMsgInfo(MsgFileName, MsgOffSet, Text_ShowMsg.text, Msg->WParam); 			
				if (MsgOffSet == ERROR)
				{
					flag = ERROR;
					MsgOffSet = oldOffSet;					
				}
				else
				{
					flag = 1;
				}
  			break;
  		}
  		case VK_DEL:
  		{
				flag = 0;
  			break;
  		}
  		case VK_0:
  		{
  			flag = 0xFF;
  			break;
  		}
  		default:
  			break;
  	}
    
  }
  
  if (flag == 1)
  {
  	Refresh(&Text_ShowMsg);
		Refresh(&Form_ShowMsg);
	}
	else if (flag == 0)
	{
		if (strcmp(MsgFileName, DEBEGINFOTXT) == 0)
		{
		  if (MessageBox("确定要清空报文吗?", "", MB_OKCANCEL) == ID_CANCEL)
			{
				return;
			}
		}

		filedelete(MsgFileName);
		CloseWindow(&Form_ShowMsg, ID_CANCEL);		
	}
	else if (flag == 0xFF)
	{
		MessageBox("左右键：前后翻页\n上下键：上下换行\n删除键：清空报文\n按键0：帮助", "帮助", MB_OK); 
	}
		
  return; 
}

long ReadMsgInfo(P_U8 filename, long offSet, P_U8 msgtext, U8 keyValue)
{
 
  MYFILE *fp;
  
  unsigned char text[MAX_TEXT_LEN] = {};
  unsigned char temp[MAX_TEXT_LEN] = {};
  long len = 0;
  long size = 0;
  int i = 0, j = 0;
  int offset = 0;
  int count = 0;

  fp = fopen(filename, "r");
  
  len = strlen(msgtext);
  memcpy(text, msgtext, len);
  
  size = MAX_SHOW_CHARS * MAX_SHOW_LINES;
  
  if (keyValue == VK_LEFT)
  {
  	offset = offSet - size;
  	if (offset < 0)
  	{
  		offset = 0;
  		size = offSet;
  	}
  	
  	fseek(fp, offset, SEEK_SET);
  	memset(text, 0, sizeof(text));
  	len = fread(text, size, fp);
  	fclose(fp);
  	
  	j = 0;
  	count = 0;
  	for (i = len - 1; i >= 0; i--)
  	{
  		j++;
  		if (i == len - 1)
  		{
  			continue;
  		}

  		if (text[i] == '\n' || j > MAX_SHOW_CHARS)
  		{
  			count++;
  			j = 1;
  		}

			if (count >= MAX_SHOW_LINES)
			{	
				i++;		
				break;
			}
  	}
  	
  	if (i < 0)
  	{
  		i = 0;
  	}
  	
  	if (len > 0)
  	{
  		memset(temp, 0, sizeof(temp));
  		memcpy(temp, text, len);
  		memset(text, 0, sizeof(text));
  		len = len - i;
  		memcpy(text, temp+i, len);  	
  		offset += i;	
  		
  		size = MAX_SHOW_CHARS * MAX_SHOW_LINES;
  		if (i == 0 && len < size)
  		{
  			memcpy(text + len, msgtext, size - len); 
  		} 
  	}
  	else
  	{
  		return ERROR;
  	}

  }
  else if (keyValue == VK_UP)
  {
  	size = MAX_SHOW_CHARS;
  	offset = offSet - size;
  	if (offset < 0)
  	{
  		offset = 0;
  		size = offSet;
  	}
  	
  	fseek(fp, offset, SEEK_SET);
  	memset(text, 0, sizeof(text));
  	len = fread(text, size, fp);
  	fclose(fp);
  	
  	for (i = len - 1; i >= 0; i--)
  	{
			if (text[i] == '\n' && i != len - 1)
			{
				break;
			}
  	} 		
  	i++;
  	
  	if (len > 0)
  	{
  		memset(temp, 0, sizeof(temp));
  		memcpy(temp, text, len);
  		memset(text, 0, sizeof(text));
  		memcpy(text, temp+i, len-i);
  		strcat(text, msgtext);
  		offset += i;
  	}
  	else
  	{
  		return ERROR;
  	}
  	
  }
  else
  {
	  if (len != 0)
	  {
	  	j = 0;
	  	count = MAX_SHOW_LINES;
			for (i = 0; i < len; i++)
			{
				j++;
				if (text[i] == '\n'|| j == MAX_SHOW_CHARS)
				{
					count--;
					j = 0;
				}
		
				if (count <= 0)
				{
					break;
				}
			}
			 		 	
	  }
	  
	  if (keyValue == VK_DOWN)
	  {
	  	j = 0;
  	  for (i=0; i<len; i++)
			{
				j++;
				if (text[i] == '\n'|| j == MAX_SHOW_CHARS)
				{
					i++;
					break;
				}
			}
			offset = offSet + i;
	  }
	  else
	  {
	  	offset = offSet + len;
	  }
	  
	  if (count > 0)
	  {
	  	offset = offSet;
	  }
	  
	  if (fseek(fp, offset, SEEK_SET) != 0)
	  {
	  	fclose(fp);
	  	return ERROR;
	  }
	  
    memset(text, 0, sizeof(text));
	  len = fread(text, size, fp);       
	  fclose(fp);	  
  
	}
	
  j = 0;
  count = 0;  
  len = strlen(text);
	for (i = 0; i < len; i++)
	{
		j++;		
		if (text[i] == '\n'|| j == MAX_SHOW_CHARS)
		{
			count++;
			j = 0;
		}

		if (count >= MAX_SHOW_LINES)
		{
			i++;
			break;
		}
	}

  len = i;  
  for (j = 0; j < len; j++)
  {
  	if (text[j] == '\r')
  	{
  		text[j] = ' ';
  	}
  }
  
  memset(msgtext, 0, strlen(msgtext));
  memcpy(msgtext, text, len);
  
  len = offset;
  return len;
}

int ShowData(P_U8 caption, P_U8 filename)
{
	if (fileexits(filename) == FSNULL)
	{
		return ERROR;
	}
	
	MsgFileName = filename;
	MsgOffSet = 0;
	
  CreateText(&Text_ShowMsg, "", 0, 0, 160, 158);
  memset(Text_ShowMsg.text, 0, sizeof(Text_ShowMsg.text));
  
  MsgOffSet = ReadMsgInfo(MsgFileName, 0, Text_ShowMsg.text, VK_ENTER);
  if (MsgOffSet == ERROR)
  {
		return ERROR;
  }
  
  CreateWindow(&Form_ShowMsg, caption, 0, 0, 160, 160);
  Form_ShowMsg.OnKeyPress = Form_ShowMsg_OnKeyPress;
  
  AddToWindow(&Form_ShowMsg, &Text_ShowMsg);
  ShowWindow(&Form_ShowMsg, SS_SHOWCENTER);
}

void ShowMsg(HWND Sender)
{
	if (ShowData("报文查看器", DEBEGINFOTXT) == ERROR)
	{
		MessageBox("未生成报文", "", MB_OK);
		return;
	}	
}
#endif