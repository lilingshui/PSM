#ifndef	_EditPro_H
#define _EditPro_H

#include "jblib.h"

#define ID_SCANOK 0x11

int InputBoxProResult;
void EditProInput(HWND Obj, PMsg Msg);


HWND CreateEditPro(PEdit AEdit, P_U8 caption, U8 left, U8 top, U8 width, U8 height)
{
  HWND hwnd;
  hwnd = CreateEdit(AEdit, caption, left, top, width, height);
  AEdit->selstart = 0;
  AEdit->showstart = 0;
  AEdit->ime = IME_ID_ALL;
  AEdit->OnMsg = EditProInput;
  return hwnd;
}

void EditProInput(HWND Obj, PMsg Msg)
{
  struct DECODE_DATA pd;

  if (Msg->Msg == WM_KEYDOWN && Msg->WParam == VK_SCANL)
  {
    if (scan_data(&pd, 0) == 0)
    {
      strcpy(((PEdit)Obj)->text, pd.data);
      ((PEdit)Obj)->selstart = 0;
      ((PEdit)Obj)->sellen = strlen(pd.data);
      ((PEdit)Obj)->showstart = 0;
      Refresh(Obj);
      Beep(100);
      InputBoxProResult = ID_SCANOK;
      CloseWindow(((Edit*)Obj)->parent, ID_OK);
    }
    Msg->Result = MSG_OK;
  }
  if (Msg->Msg == WM_CHAR && Msg->WParam == VK_FUNCTION)
  {
    ImmNextIme(IME_ID_123 | IME_ID_abc | IME_ID_ABC);//| IME_ID_PY);
    Refresh(Obj);
  }
}
#endif
