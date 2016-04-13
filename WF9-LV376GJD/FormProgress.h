#ifndef	_FormProgress_H
#define _FormProgress_H

#include "jblib.h"
//#include "M_CatScreen.c" 
Window Form_Progress;
ProgressBar ProgressBar_Proc;

//void func(HWND Obj,PMsg Msg);
void ShowFormProgress(int max, PPROC func)
{
  CreateProgressBar(&ProgressBar_Proc, 6, 6, 136, 15);
  ProgressBar_Proc.min = 0;  
  ProgressBar_Proc.max = max;  
  ProgressBar_Proc.oldvalue = 0;  
  ProgressBar_Proc.value = 0;  
  ProgressBar_Proc.proc = func; 
  
  if (max <= 0 || func == NULL)
  {
    return;
  }
  //SetApplicationMsgProc(ApplicitionMsg); 
  CreateWindow(&Form_Progress, "ÇëÉÔµÈ...", 0, 0, 148, 42);
  AddToWindow(&Form_Progress, &ProgressBar_Proc);
  ShowWindow(&Form_Progress, SS_SHOWCENTER);
  //SetApplicationMsgProc(NULL);
}

void CloseFormProgress()
{
  CloseWindow(&Form_Progress, ID_OK);
}

void ProgressStep()
{
  int value;
  
  ProgressBar_Proc.value++;
  value = ProgressBar_Proc.value * 100 / ProgressBar_Proc.max;
  if (value == ProgressBar_Proc.oldvalue)
  {
    return;
  }
  ProgressBar_Proc.oldvalue = value;
  Refresh(&ProgressBar_Proc);
}
#endif
