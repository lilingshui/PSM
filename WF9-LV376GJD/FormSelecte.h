#ifndef	_FormSelecte_H
#define _FormSelecte_H

#include "jblib.h"

Window Form_Selecte;
int FormSelecteResult;
void FormSelecteOK(HWND Sender);
void FormSelecteCancel(HWND Sender);

unsigned int FormSelecte(P_U8 caption, PItem items, int count, P_U8 index, U8 style)
{
  Combo ComboBox;
  Button Button_OK, Button_Cancel;
  FormSelecteResult = ID_CANCEL;

  CreateCombo(&ComboBox, items, count, 6, 6, 136, 20);
  ComboBox.index = *index;
  CreateButton(&Button_Cancel, "取消", 17, 32, 40, 20);
  Button_Cancel.OnClick = FormSelecteCancel;
  CreateButton(&Button_OK, "确定", 91, 32, 40, 20);
  Button_OK.OnClick = FormSelecteOK;
  
  CreateWindow(&Form_Selecte, caption, 6, 16, 148, 76);
  AddToWindow(&Form_Selecte, &ComboBox);
  AddToWindow(&Form_Selecte, &Button_Cancel);
  AddToWindow(&Form_Selecte, &Button_OK);
  ShowWindow(&Form_Selecte, style);
  
  *index = ComboBox.index;
  return FormSelecteResult;
}

void FormSelecteOK(HWND Sender)
{
  FormSelecteResult = ID_OK;
  CloseWindow(&Form_Selecte, ID_OK);
}

void FormSelecteCancel(HWND Sender)
{
  FormSelecteResult = ID_CANCEL;
  CloseWindow(&Form_Selecte, ID_CANCEL);
}
#endif
