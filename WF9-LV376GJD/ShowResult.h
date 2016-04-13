#ifndef	_ShowResult_H
#define _ShowResult_H

#include "jblib.h"

#define MAXSHOWRESULTNUM 200
  
Window Form_ShowResult;
Text Text_ShowResult;

void ShowResult(P_U8 Caption, P_U8 showtext, PPROC func)
{
  CreateText(&Text_ShowResult, "", 0, 0, 160, 156);
  Text_ShowResult.proc = func;
  
  memset(Text_ShowResult.text, 0, sizeof(Text_ShowResult.text));
  memcpy(Text_ShowResult.text, showtext, strlen(showtext));
 
  if (func == NULL)
  {
    return;
  }
  
  CreateWindow(&Form_ShowResult, Caption, 0, 0, 160, 160);
  AddToWindow(&Form_ShowResult, &Text_ShowResult);
  ShowWindow(&Form_ShowResult, SS_SHOWCENTER);
}

void CloseFormResult()
{
  CloseWindow(&Form_ShowResult, ID_OK);
}


void RefleshShowResult(P_U8 refleshtext)
{
	unsigned char count = 0;
	count = strlen(refleshtext);
	if (count > MAXSHOWRESULTNUM)
	{
		count = MAXSHOWRESULTNUM;
	}

  memset(Text_ShowResult.text, 0, sizeof(Text_ShowResult.text));
  memcpy(Text_ShowResult.text, refleshtext, count); 
//  Refresh(&Text_ShowResult);
  Refresh(&Form_ShowResult);
}
#endif
