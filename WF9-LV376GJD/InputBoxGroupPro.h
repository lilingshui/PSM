#ifndef	_InputBoxGroupPro_H
#define _InputBoxGroupPro_H

#include "jblib.h"
Window Form_InputBoxGroup;
//#include "EditPro.h"

//void Form_InputBoxPro_OnKeyPress(HWND Obj, PMsg Msg);
void InputBoxGroupPro_OK(HWND Sender);
void InputBoxGroupPro_Cancel(HWND Sender);
  
unsigned char InputBoxGroupPro(P_U8 caption, U8 value1[], U8 value2[])
{

  Text Lable1, Lable2;
  Edit Edit1, Edit2;
  Button Button_OK, Button_Cancel;

  CreateText(&Lable1, value1, 6, 9, 75, 15);
  CreateText(&Lable2, value2, 6, 32, 75, 15);
  CreateEdit(&Edit1, "", 80, 6, 53, 20);
  Edit1.length = 4;
  Edit1.ime = IME_ID_123;
  Edit1.OnClick = InputBoxGroupPro_OK;
  CreateEdit(&Edit2, "", 80, 29, 53, 20);
  Edit2.length = 4;
  Edit2.ime = IME_ID_123;
  Edit2.OnClick = InputBoxGroupPro_OK;
  
  CreateButton(&Button_OK, "确定", 85, 53, 40, 20);
  Button_OK.OnClick = InputBoxGroupPro_OK;
  CreateButton(&Button_Cancel, "取消", 16, 53, 40, 20); 
  Button_Cancel.OnClick = InputBoxGroupPro_Cancel;
  
  CreateWindow(&Form_InputBoxGroup, caption, 0, 0, 145, 100);

  AddToWindow(&Form_InputBoxGroup, &Lable1);
  AddToWindow(&Form_InputBoxGroup, &Lable2);  
  AddToWindow(&Form_InputBoxGroup, &Edit1);
  AddToWindow(&Form_InputBoxGroup, &Edit2);    
  AddToWindow(&Form_InputBoxGroup, &Button_OK);
  AddToWindow(&Form_InputBoxGroup, &Button_Cancel);  
  if (ShowWindow(&Form_InputBoxGroup, SS_SHOWCENTER) == ID_OK)
	{
		memset(value1, 0, strlen(value1));
		memset(value2, 0, strlen(value2));
		sprintf(value1, "%s", Edit1.text);
		sprintf(value2, "%s", Edit2.text);
		return 1;
	}
	else
	{		
		return 0;
	}
  
}

void InputBoxGroupPro_OK(HWND Sender)
{
	CloseWindow(&Form_InputBoxGroup, ID_OK);
}
void InputBoxGroupPro_Cancel(HWND Sender)
{
	CloseWindow(&Form_InputBoxGroup, ID_CANCEL);
}
#endif
