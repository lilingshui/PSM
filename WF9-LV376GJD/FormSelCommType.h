#ifndef _FormSelCommType_H
#define _FormSelCommType_H

#include "jblib.h"
#include "DataConvert.h"

Window Form_SelCommType;

void FormSelCommTypeOK(HWND Sender)
{
  CloseWindow(&Form_SelCommType, ID_OK);
}

void FormSelCommTypeCancel(HWND Sender)
{
  CloseWindow(&Form_SelCommType, ID_CANCEL);
}

unsigned int FormSelCommType(P_U8 caption, StruCommParam *sCommParam)
{

	int i, index, count;
  Text Lable_Check, Lable_Baud, Lable_Type;
  Combo comboCheck, comboBaud, comboType;
  Button Button_OK, Button_Cancel;
  
  TItem checkItems[] = {"无校验", "偶校验", "奇校验",};
  TItem baudItems[] = {"1200", "2400", "4800", "9600", "19200", "38400", "115200",};
  TItem typeItems[] = {"红外口", "485口", "232口",};

  CreateText(&Lable_Type, "通信口", 15, 9, 50, 20); 
  CreateText(&Lable_Check, "校验位", 15, 35, 50, 20); 
  CreateText(&Lable_Baud, "波特率", 15, 61, 50, 20); 
  
  count = sizeof(typeItems)/sizeof(TItem);
  CreateCombo(&comboType, &typeItems, count, 66, 6, 68, 20);
  switch(sCommParam->Port)
  {
    case 0x05:
      index = 0;
      break;
    case 0x01:
      index = 1;
      break;
    case 0x08:
      index = 2;
      break;
    default:
    {
    	if (sCommParam->Port >= count)
    	{
    		index = 0;
    	}
    	break;
    }
  }
  comboType.index = index;

  count = sizeof(checkItems)/sizeof(TItem);
  CreateCombo(&comboCheck, &checkItems, count, 66, 32, 68, 20);
  switch(sCommParam->Parity)
  {
    case 0x01:
    	index = 0;
      break;
    case 0x09:
      index = 1;
      break;
    case 0x0D:
      index = 2;
      break;
    default:
      index = 0;
    	break;    	
  }
  comboCheck.index = index;
  
  count = sizeof(baudItems)/sizeof(TItem);
  CreateCombo(&comboBaud, &baudItems, count, 66, 58, 68, 20);
  
  comboBaud.index = 0;
  for (i = 0; i < count; i++)
  {
  	if (sCommParam->Baud == StrToInt(baudItems[i], DEC))
  	{
  		comboBaud.index = i;
  		break;
  	} 	
  } 
  
  CreateButton(&Button_Cancel, "取消", 17, 82, 40, 20);
  Button_Cancel.OnClick = FormSelCommTypeCancel;
  CreateButton(&Button_OK, "确定", 91, 82, 40, 20);
  Button_OK.OnClick = FormSelCommTypeOK;
  
  CreateWindow(&Form_SelCommType, caption, 0, 0, 148, 128);
  AddToWindow(&Form_SelCommType, &Lable_Type);
  AddToWindow(&Form_SelCommType, &Lable_Check);
  AddToWindow(&Form_SelCommType, &Lable_Baud);
  AddToWindow(&Form_SelCommType, &comboType);
  AddToWindow(&Form_SelCommType, &comboCheck);
  AddToWindow(&Form_SelCommType, &comboBaud);
  
  AddToWindow(&Form_SelCommType, &Button_OK);
  AddToWindow(&Form_SelCommType, &Button_Cancel);
  
  if (ShowWindow(&Form_SelCommType, SS_SHOWCENTER) == ID_CANCEL)
  {
  	return ID_CANCEL;
  }
  
  switch(comboType.index)
  {
  	case 0:
  		index = 0x05;
  		break;
  	case 1:
  		index = 0x01;
  		break;
  	case 2:
  		index = 0x08;
  		break;
  	default:
  		index = 0x05;
  		break;
  }
  sCommParam->Port = (unsigned char)index;
  
  switch(comboCheck.index)
  {
  	case 0:
  		index = 0x01;
  		break;
  	case 1:
  		index = 0x09;
  		break;
  	case 2:
  		index = 0x0D;
  		break;
  	default:
  		index = 0x01;
  		break;
  }
  sCommParam->Parity = (unsigned char)index;
  
  sCommParam->Baud = StrToInt(baudItems[comboBaud.index], DEC);
  //FormSelCommTypeSave(&tmpParam, type);
  
  return ID_OK;
}


void FormSelCommTypeSave(StruCommParam *sCommParam, int type)
{
  U8 temp[10] = {0};
  
  use(SYSCONFDBF);

  if(libsumr() == 0)
  {
    libappend();
  }
    
  if (type == 0)
  {
    
	  libset(0, 0);
	  memset(temp, 0, sizeof(temp));
	  sprintf(temp, "%02x", sCommParam->Parity);
	  libwrit(temp);
	  
	  libset(0, 18);
	  memset(temp, 0, sizeof(temp));
	  sprintf(temp, "%02x", sCommParam->Port);
	  libwrit(temp);
	  
	  libset(0, 19);
	  memset(temp, 0, sizeof(temp));
	  sprintf(temp, "%d", sCommParam->Baud);
	  libwrit(temp);
	}
	else if (type == 1)
  {
    
	  libset(0, 2);
	  memset(temp, 0, sizeof(temp));
	  sprintf(temp, "%02x", sCommParam->Parity);
	  libwrit(temp);
	  
	  libset(0, 20);
	  memset(temp, 0, sizeof(temp));
	  sprintf(temp, "%02x", sCommParam->Port);
	  libwrit(temp);
	  
	  libset(0, 21);
	  memset(temp, 0, sizeof(temp));
	  sprintf(temp, "%d", sCommParam->Baud);
	  libwrit(temp);
	}
	else if (type == 2)
	{
		libset(0, 23);
	  memset(temp, 0, sizeof(temp));
	  sprintf(temp, "%02x", sCommParam->Parity);
	  libwrit(temp);
	  
	  libset(0, 24);
	  memset(temp, 0, sizeof(temp));
	  sprintf(temp, "%02x", sCommParam->Port);
	  libwrit(temp);
	  
	  libset(0, 25);
	  memset(temp, 0, sizeof(temp));
	  sprintf(temp, "%d", sCommParam->Baud);
	  libwrit(temp);
	}
  
  use("");    
}
#endif
