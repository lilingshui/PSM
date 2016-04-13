#ifndef	_InputBoxPro_H
#define _InputBoxPro_H

#include "jblib.h"
#include "EditPro.h"

void Form_InputBoxPro_OnKeyPress(HWND Obj, PMsg Msg);
  
unsigned int InputBoxPro(P_U8 caption, P_U8 value, U8 length)
{
  InputBoxProResult = ID_CANCEL;
  Window Form_InputBoxPro;
  Text Text_Lable;
  Edit Edit_Pro;
  //scan_init(MODEXL1031);      //初始化条码扫描模块 
  scan_init(2);

  CreateText(&Text_Lable, "扫描或手动输入", 6, 6, 136, 15);
  CreateEditPro(&Edit_Pro, caption, 6, 21, 136, 20);
  Edit_Pro.length = length;
  Edit_Pro.selstart = length;
  memset(Edit_Pro.text, 0, length);
  strcpy(Edit_Pro.text, value);
  
  CreateWindow(&Form_InputBoxPro, caption, 0, 0, 148, 65);
  Form_InputBoxPro.OnKeyPress = Form_InputBoxPro_OnKeyPress;
  AddToWindow(&Form_InputBoxPro, &Text_Lable);
  AddToWindow(&Form_InputBoxPro, &Edit_Pro);
  ShowWindow(&Form_InputBoxPro, SS_SHOWCENTER);
  
  memcpy(value, Edit_Pro.text, length);
  if (strlen(value) == 0)
  {
//    scan_stop();
    InputBoxProResult = ID_CANCEL;
  }
  scan_stop();
  return InputBoxProResult;
}

unsigned int InputBoxWS(P_U8 mText, P_U8 caption, P_U8 value, U8 length)
{
  InputBoxProResult = ID_CANCEL;
  Window Form_InputBoxPro;
  Text Text_Lable;
  Edit Edit_Pro;

  CreateText(&Text_Lable, mText, 6, 6, 136, 15);
  CreateEditPro(&Edit_Pro, caption, 6, 21, 136, 20);
  Edit_Pro.length = length;
  Edit_Pro.selstart = length;
  memset(Edit_Pro.text, 0, length);
  strcpy(Edit_Pro.text, value);
  
  CreateWindow(&Form_InputBoxPro, caption, 0, 0, 148, 65);
  Form_InputBoxPro.OnKeyPress = Form_InputBoxPro_OnKeyPress;
  AddToWindow(&Form_InputBoxPro, &Text_Lable);
  AddToWindow(&Form_InputBoxPro, &Edit_Pro);
  ShowWindow(&Form_InputBoxPro, SS_SHOWCENTER);
  
  memcpy(value, Edit_Pro.text, length);
  if (strlen(value) == 0)
  {
    InputBoxProResult = ID_CANCEL;
  }
  return InputBoxProResult;
}

void Form_InputBoxPro_OnKeyPress(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR && Msg->WParam == VK_ENTER)
  {
    InputBoxProResult = ID_OK;
//    scan_stop();
    CloseWindow(Obj, 0x01);
  }
}

void Form_InputBoxWS_OnKeyPress(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR && Msg->WParam == VK_ENTER)
  {
    InputBoxProResult = ID_OK;
    CloseWindow(Obj, ID_OK);
  }
}
#endif
