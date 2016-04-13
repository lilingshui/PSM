#ifndef _AttributeMeter_H
#define _AttributeMeter_H

#include "jblib.h"
#include "Common.h"
#include "Global.h"
#include "DataConvert.h"

Window Form_AttributeMeter;


void AttributeMeterOK(HWND Sender)
{ 
  CloseWindow(&Form_AttributeMeter, ID_OK);
}


void AttributeMeterCancel(HWND Sender)
{
  CloseWindow(&Form_AttributeMeter, ID_CANCEL);
}


//电表属性加载
void AttributeMeterLoad(StruMeterRecord *sMeterRecord)
{   
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int r, i, port = 0; 
  int baud = 0; 
      
  use(METE_RECD_GW);
  memset(temp, 0, sizeof(temp));
  r = libsumr();
  for(i = 0; i < r; i++)
  {
    memset(temp, 0, sizeof(temp));
    libset(i, 0);
    libread(temp);  
    if(strcmp(temp, STR_DEL) == 0)
    {
      continue;
    }
       
    memset(temp, 0, sizeof(temp));
    libset(i, FD_METERADDR);
    libread(temp);
    if (strncmp(CurrMPAttrib.Meter, temp, NUM_12) == 0)
    {
      break;
    }
  }
 
  r = i;
  
  memset(temp, 0, sizeof(temp));
  libset(r, FD_MPNO);
  libread(temp);
  sMeterRecord->MeterNo = StrToInt(temp, DEC);
  
  memset(temp, 0, sizeof(temp));
  libset(r, FD_PROTO);
  libread(temp);
  if (strcmp(temp, "DL/T 645-1997") == 0)
  {
  	sMeterRecord->ProtoType = 1;
  }
  else if (strcmp(temp, "交采协议") == 0)
  {
  	sMeterRecord->ProtoType = 2;
  }
  else
  {
  	sMeterRecord->ProtoType = 0;
  }
  
  memset(temp, 0, sizeof(temp));
  libset(r, FD_PORT);
  libread(temp);
  
  port = StrToInt(temp, DEC);
  switch(port)
  {
    case 1:
    case 2:
    case 3:
    case 4:
      port -= 1;
      break;
    case 31:
      port = 4;
      break;
    default:
      port = 0;
      break;   
  }
  sMeterRecord->Port = port; 
  
  memset(temp, 0, sizeof(temp));
  libset(r, FD_BAUD);
  libread(temp);
  sMeterRecord->BaudRate = StrToInt(temp, DEC);
  
  memset(temp, 0, sizeof(temp));
  libset(r, FD_SORT);
  libread(temp);
  sMeterRecord->Sort = StrToInt(temp, HEX);
  
  use("");
    
}

//电表属性保存
void AttributeMeterSave(StruMeterRecord *sMeterRecord)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int r, i, count;
  int mpNo = 0;
      
  mpNo = sMeterRecord->MeterNo;
  
  use(METE_RECD_GW);
  count = libsumr();
 
  for(i = 0; i < count; i++)
  {
    memset(temp, 0, sizeof(temp));
    libset(i, 0);
    libread(temp);  
    if(strcmp(temp, STR_DEL) == 0)
    {
      continue;
    }
           
    memset(temp, 0, sizeof(temp)); 
    libset(i, FD_METERADDR);
    libread(temp);
    if (strncmp(CurrMPAttrib.Meter, temp, NUM_12) == 0)
    {
      r = i;
      continue; 
    } 
    
    memset(temp, 0, sizeof(temp));
    libset(i, FD_DEVNO);
    libread(temp);
    if(mpNo == StrToInt(temp, DEC))
    {
        MessageBox("测量点号已存在!", "", MB_OK);
        use("");
        return;
    }             
    
  }
 
 	memset(temp, 0, sizeof(temp));
 	sprintf(temp, "%d", sMeterRecord->MeterNo);
  if (mpNo != 0)
  {
    libset(r, FD_DEVNO);
    libwrit(temp);   
  }
  libset(r, FD_MPNO);
  libwrit(temp);
 
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", sMeterRecord->Port); 
  libset(r, FD_PORT);
  libwrit(temp); 
  
  libset(r, FD_PROTO);
  if (sMeterRecord->ProtoType == 1)
  {
    libwrit("DL/T 645-1997");
  }
  else if (sMeterRecord->ProtoType == 2)
  {
  	libwrit("交采协议");
  }
  else
  {
    libwrit("DL/T 645-2007");
  }
  
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", sMeterRecord->BaudRate);
  libset(r, FD_BAUD);
  libwrit(temp);
 
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%02x", sMeterRecord->Sort); 
  libset(r, FD_SORT);
  libwrit(temp);
  
  use("");
}

void AttributeMeter(void)
{ 

  Text  Label_MP, Label_Port, Label_Proto, Label_Baud, Label_Sort;
  Edit  Edit_MP, Edit_Sort;
	Combo Combo_Port, Combo_Proto, Combo_Baud;
  Button Button_OK, Button_Cancel;
  int i = 0, baudCount = 0;
  StruMeterRecord mRecord;
  
  TItem portItems[] = {"端口1", "端口2", "端口3", "端口4", "端口31",};
  TItem protoItems[] = {"DL/T645-2007", "DL/T645-1997", "交采协议"};
  TItem baudItems[] = {"1200", "2400", "9600",};
  
  CreateButton(&Button_OK, "确定", 115, 110, 35, 20);
  Button_OK.OnClick = AttributeMeterOK;
  CreateButton(&Button_Cancel, "取消", 10, 110, 35, 20);
  Button_Cancel.OnClick = AttributeMeterCancel;
  
  AttributeMeterLoad(&mRecord);  
  
  CreateText(&Label_Port,"端 口:",5,5,45,18);
  CreateText(&Label_Proto, "规 约:",5,27,45,18);
  CreateText(&Label_Baud, "波特率:",5,49,45,18);
  CreateText(&Label_MP,"测量点:",5,71,50,18);
  CreateText(&Label_Sort,"大类小类", 5,91,50,18);
  
  CreateEdit(&Edit_MP, "", 58,69,97,18); 
  sprintf(Edit_MP.text, "%d", mRecord.MeterNo);
  CreateEdit(&Edit_Sort, "", 58,89,97,18); 
  sprintf(Edit_Sort.text, "%02x", mRecord.Sort);
  CreateCombo(&Combo_Port, &portItems, sizeof(portItems)/sizeof(TItem),48,3,107,20); 
  Combo_Port.index = mRecord.Port; 
  CreateCombo(&Combo_Proto, &protoItems, sizeof(protoItems)/sizeof(TItem),48,25,107,20);
  Combo_Proto.index = mRecord.ProtoType;  
  baudCount = sizeof(baudItems)/sizeof(TItem);
  CreateCombo(&Combo_Baud, &baudItems, baudCount,48,47,107,20);

  for (i = 0; i < baudCount; i++)
  {
  	if (mRecord.BaudRate == StrToInt(baudItems[i], DEC))
  	{
  		break;
  	}
  }
  if (i >= baudCount)
  {
	  Combo_Baud.index = 1;
	}
	else
	{
		Combo_Baud.index = i;
	}
  
  Edit_MP.length = 4;
  Edit_Sort.length = 2;
  
  CreateWindow(&Form_AttributeMeter, "", 0, 0, 160, 160);
  sprintf(Form_AttributeMeter.text, "电表%s属性", CurrMPAttrib.Meter);
  AddToWindow(&Form_AttributeMeter, &Label_MP);
  AddToWindow(&Form_AttributeMeter, &Label_Port);
  AddToWindow(&Form_AttributeMeter, &Label_Proto);
  AddToWindow(&Form_AttributeMeter, &Label_Baud);
  AddToWindow(&Form_AttributeMeter, &Label_Sort);
  AddToWindow(&Form_AttributeMeter, &Combo_Port);
  AddToWindow(&Form_AttributeMeter, &Combo_Proto);
  AddToWindow(&Form_AttributeMeter, &Combo_Baud);
  AddToWindow(&Form_AttributeMeter, &Edit_MP);
  AddToWindow(&Form_AttributeMeter, &Edit_Sort);

  AddToWindow(&Form_AttributeMeter, &Button_OK);  
  AddToWindow(&Form_AttributeMeter, &Button_Cancel); 
    
  if (ShowWindow(&Form_AttributeMeter, SS_SHOWCENTER) == ID_CANCEL)
  {
    use("");
    return;
  }
  
  mRecord.MeterNo = StrToInt(Edit_MP.text, DEC);
  if(mRecord.MeterNo > 2040)
  {
    MessageBox("测量点号不能大于2040!", STR_CUE, MB_OK);
    return;
  } 
  
  mRecord.Port = Combo_Port.index + 1;
  if (mRecord.Port == 5)
  {
    mRecord.Port = 31;
  }
  
  mRecord.ProtoType = Combo_Proto.index;
  mRecord.BaudRate = StrToInt(baudItems[Combo_Baud.index], DEC);
  mRecord.Sort = StrToInt(Edit_Sort.text, HEX);   
  if (mRecord.Sort > 0xFF)
  {
  	mRecord.Sort = 0x51;
  }
  
	AttributeMeterSave(&mRecord);
}
#endif
