#ifndef _DataEncap_H
#define _DataEncap_H

#include "jblib.h"
#include "Global.h"
#include "DataConvert.h"

U16 Build645SendBuff(U8 sendbuff[], Stru645Data* senddata);

U16 BuildAFN04F10Buff(U8 userbuff[], U16 mpNo, U16 rNo)
{ 
  int r = 0;
  int count = 0;
  unsigned short len = 0;
  unsigned short tmpint = 0;
  unsigned short devNo = 0;
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  
  //count = libsumr();
  r = rNo;
  
  /*if (r < count)
  {  		  
	  libset(r, FD_DEVNO);
	  memset(temp, 0, sizeof(temp));
	  libread(temp);
	  devNo = StrToInt(temp, DEC);
	  
	  if (devNo == mpNo)
	  {
	  	libset(rNo, 0);
	  	memset(temp, 0, sizeof(temp));
	  	libread(temp);
	  	if (strcmp(temp, STR_DEL) == 0)
	  	{
	  		r = -1;;
	  	}
	  }
	  else
	  {
	  	r = -1;
	  } 	
  }
  else
  {
  	r = -1;
  }
  
  if (r == -1)
  {
	  for (r = 0; r < count; r++)
	  {
	  	libset(r, 0);
	  	memset(temp, 0, sizeof(temp));
	  	libread(temp);
	  	if (strcmp(temp, STR_DEL) == 0)
	  	{
	  		continue;
	  	}
	
		  memset(temp, 0, sizeof(temp));
		  libset(r, FD_DEVNO);
		  libread(temp);
		  devNo = StrToInt(temp, DEC);
		  if (devNo == mpNo)
		  {
		  	break;
		  }
	  }
	  
	  if (r >= count)
	  {
	  	return 0;
	  }
	}*/
	
	libset(r, FD_DEVNO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  devNo = StrToInt(temp, DEC);
   
  userbuff[len++] = (unsigned char)(devNo);
  userbuff[len++] = (unsigned char)(devNo >> 8);
  
  libset(r, FD_MPNO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  tmpint = StrToInt(temp, DEC);
  userbuff[len++] = (unsigned char)(tmpint);
  userbuff[len++] = (unsigned char)(tmpint >> 8);
  
  libset(r, FD_BAUD);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  tmpint = StrToInt(temp, DEC);    
  switch(tmpint)
  {
    case 2400:
      tmpint = 0x60;
      break;
    case 1200:
      tmpint = 0x40;
      break;
    case 9600:
      tmpint = 0xC0;  
      break;    
    default:
      tmpint = 0x60;
      break;
  }
         
  libset(r, FD_PORT);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  tmpint += StrToInt(temp, DEC);
  userbuff[len++] = (unsigned char)(tmpint);
  
  libset(r, FD_PROTO);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  if (strcmp(temp, "DL/T 645-1997") == 0)
  {
    userbuff[len++] = 1;
  }
  else if (strcmp(temp, "交采协议") == 0)
  {
  	userbuff[len++] = 2;
  }
  else
  {
  	userbuff[len++] = 30;
  }
    
  libset(r, FD_METERADDR);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  FormatStr(temp, 12, '0', 0);
  StrToHexStr(temp, 6, 1, 0, 0);
  memcpy(userbuff+len, temp, 6);
  len += 6;

	memset(temp, 0, sizeof(temp));
  memcpy(userbuff+len, temp, 6);
  len += 6;	

  userbuff[len++] = 0x04;  
  userbuff[len++] = 0x09;//2小数位和6整数位 
  
/*  memset(temp, 0, sizeof(temp));
  libset(r, 13);
  libread(temp)
  tmpint = StrToInt(temp, 10);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%02x", tmpint);
  strcat(userbuff, temp);
  strcat(userbuff, "05");
*/  
  
  libset(r, FD_COLLADDR);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  FormatStr(temp, 12, '0', 0);
  StrToHexStr(temp, 6, 1, 0, 0);
  memcpy(userbuff+len, temp, 6);
  len += 6;
  
  libset(r, FD_SORT);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  tmpint = StrToInt(temp, HEX);
  userbuff[len++] = (unsigned char)(tmpint);

  //userbuff[len++] = 0x51;  

  return len;               
}



U16 BuildAFN10F01Buff(U8 userbuff[], int recordnum)
{ 
  int i;
  char temp[13] = {0};
  char tmp645buf[200] = {0};
  U16 len = 0, code = 0x6B;  
  U8 port = 1, slen = 0; 
  Stru645Data Send645Data;
  
  memset(userbuff, 0, strlen(userbuff));
  i = recordnum; 
  Send645Data.protype = 30;
          
  libset(i, 4);
  memset(temp, 0, sizeof(temp));
  memset(Send645Data.address, 0, sizeof(Send645Data.address));
  libread(temp);
  memcpy(Send645Data.address, temp, 12);
  
  libset(i, 9);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  if(temp[0] != '\0')
    port = StrToInt(temp, 10);
   
  libset(i, 10);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  if(temp[0] != '\0')
    code = StrToInt(temp, 10);
       
  libset(i, 11);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  if (strcmp(temp, "DL/T 645-1997") == 0)
    Send645Data.protype = 1;

  switch(code)
  {
    case 0x6B:
      break;
    case 1200:
      code = 0x4B;
      break;
    default:
      code = 0x6B;
      break;
  }    

  userbuff[len++] = (unsigned char)port;
  userbuff[len++] = (unsigned char)code; 

  userbuff[len++] = 0x32;
  userbuff[len++] = 0x32;
   
  switch(Send645Data.protype)
  {
    case 1:
      Send645Data.ctrcode = 0x01;
      Send645Data.datalen = 0x02;
      Send645Data.dataitm = 0x9010;
      break;
    case 30:
      Send645Data.ctrcode = 0x11;
      Send645Data.datalen = 0x04;
      Send645Data.dataitm = 0x00010000;     
      break;
  } 
  
  slen = Build645SendBuff(tmp645buf, &Send645Data);
  
  userbuff[len++] = (unsigned char)slen;
  userbuff[len++] = 0x00;
  memcpy(userbuff + len, tmp645buf, slen);
  len += slen;
  
  return len;               
}
  
  
  
U16 Build698SendBuff(U8 sendbuff[], Stru698Data* pData)
{
  U16 sendlen = 0;
  U32 bcdtime = 0;
  int i;
  
  sendlen = 6;
  
  sendbuff[sendlen++] = pData->Ccode;
  memcpy(sendbuff + sendlen, pData->Saddr, ADDRBYTES); 
  sendlen += ADDRBYTES;
  sendbuff[sendlen++] = pData->AFN;
  sendbuff[sendlen++] = pData->SEQ;
  
  if(pData->Pn == 0)
  {
    sendbuff[sendlen++] = 0x00;
    sendbuff[sendlen++] = 0x00;
  }   
  else
  {
    sendbuff[sendlen++] = 1 << ((pData->Pn - 1) % 8);   //1 << ((Pn - 1) % 8 )    
    sendbuff[sendlen++] = (pData->Pn - 1) / 8 + 1;      //Pn - 1) / 8 + 1
  }
  
  if(pData->Fn == 0)
  {
    return 0;
  }   
  else
  {
    sendbuff[sendlen++] = 1 << ((pData->Fn - 1) % 8);   //1 << ((Fn - 1) % 8 )    
    sendbuff[sendlen++] = (pData->Fn - 1) / 8;      		//Fn - 1) / 8
  }
  
  memcpy(sendbuff + sendlen, pData->Userbuf, pData->Ulen);
  sendlen += pData->Ulen;
  
  if((pData->Ccode == 0x4A) || (pData->Ccode == 0x41))
  {
    for(i = 0; i < 16; i++)
    {
      sendbuff[sendlen++] = 0x00;				//密码16字节
    }
  }
  
  sendbuff[sendlen++] = 0x00;   				//帧序号计数器PFC
  
  GetTime(&bcdtime);
  for(i = 0; i < 3; i++)								//发送时标
  {
    sendbuff[sendlen++] = (unsigned char)(bcdtime >> (8 * i));
  }
  GetDate(&bcdtime);
  sendbuff[sendlen++] = (unsigned char)(bcdtime);
    
  sendbuff[sendlen++] = 0x00;						//允许发送传输延时时间
    
  i = (sendlen - 6) * 4 + 2;
  sendbuff[0] = BUFF_START;
  sendbuff[1] = (unsigned char)(i);
  sendbuff[2] = (unsigned char)(i >> 8);
  sendbuff[3] = sendbuff[1];
  sendbuff[4] = sendbuff[2]; 
  sendbuff[5] = BUFF_START;

  sendbuff[sendlen++] = CheckSum(sendbuff + 6, 0, sendlen - 6);
  sendbuff[sendlen++] = BUFF_END;
   
  return sendlen;
}



U8 *GetAddr(U8 *addr, char * term)
{
  char temp[10] = {};
  int tmpint = 0;
  U8 addrtype = 0;
  use(SYSCONFDBF);
  if(libsumr() == 0)
  {
    use("");
    return;
  }
  
  libset(0,2);
  libread(temp);
  addrtype = StrToInt(temp,10);
  if((addrtype != 10) && (addrtype != 16)) 
  {
    use("");
    return;
  }
  use("");
  
  memset(temp, 0, sizeof(temp));
  memcpy(temp, term, 4);
  StrToHexStr(temp, 2, 0, 0, 0);
  addr[0] = temp[1];
  addr[1] = temp[0];
  
  memset(temp, 0, sizeof(temp));
  if(addrtype == 10)
  {
    memcpy(temp, term + 4, strlen(term) - 4);
    tmpint = StrToInt(temp, 10);
    addr[2] = (unsigned char)tmpint;
    addr[3] = (unsigned char)(tmpint >> 8);    
  }
  else if(addrtype == 16) 
  {
    memcpy(temp, term + 4, 4);
    StrToHexStr(temp, 2, 0, 0, 0);
    addr[2] = temp[1];
    addr[3] = temp[0];
  }
  addr[4] = 0x02;
  
  return addr;     
}


U16 Build645SendBuff(U8 sendbuff[], Stru645Data* senddata)
{
  U8 tmpbuff[MAX_ULEN] = {0};
  U16 sendlen = 0;
  U8 i, j;

  for (i = 0; i < BUFF_FRONT_NUM; i++)
  {
    sendbuff[sendlen++] = BUFF_FRONT;
  }
  sendbuff[sendlen++] = BUFF_START;

  /*if ((senddata->protype == PROTO_97)||(senddata->protype == PROTO_07))
  {
    StrToHexStr(senddata->address, METEADDR_LEN, 1, 0, 0);
  }
  else if (senddata->protype == PROTO_SH)
  {
    StrToHexStr(senddata->address, METEADDR_LEN, 1, 0, 0);
//    ProShhAddr(tmpbuff, senddata.address);
  }*/
  
  StrToHexStr(senddata->address, METEADDR_LEN, 1, 0, 0);
  memcpy(sendbuff + sendlen, senddata->address, METEADDR_LEN);
  sendlen += METEADDR_LEN;
  
  sendbuff[sendlen++] = BUFF_START;
  sendbuff[sendlen++] = senddata->ctrcode;
  sendbuff[sendlen++] = senddata->datalen;
  
  memset(tmpbuff,0,sizeof(tmpbuff));
  if (senddata->protype == PROTO_97)
  {
    sendbuff[sendlen++] = (U8)(senddata->dataitm) + DATA645DEAL;
    sendbuff[sendlen++] = (U8)(senddata->dataitm >> 8) + DATA645DEAL;
    memcpy(tmpbuff, senddata->userbuf, senddata->datalen - DI_BYTES);
    for (j = 0; j < senddata->datalen - DI_BYTES; j++)
    {
      sendbuff[sendlen++] = tmpbuff[j] + DATA645DEAL;
    }
  }
  else if (senddata->protype == PROTO_07)
  {
    for (i = 0; i < DI07_BYTES; i++)
    {
      sendbuff[sendlen++] = (U8)(senddata->dataitm >> 8 * i) + DATA645DEAL;
    }
    memcpy(tmpbuff, senddata->userbuf, senddata->datalen - DI07_BYTES);
    for (j = 0; j < senddata->datalen - DI07_BYTES; j++)
    {
      sendbuff[sendlen++] = tmpbuff[j] + DATA645DEAL;
    }
  }
  else
  {
    memcpy(tmpbuff, senddata->userbuf, senddata->datalen);
    for (j = 0; j < senddata->datalen; j++)
    {
      sendbuff[sendlen++] = tmpbuff[j] + DATA645DEAL;
    }
  }

  sendbuff[sendlen++] = CheckSum(sendbuff, BUFF_FRONT_NUM, sendlen - BUFF_FRONT_NUM);
  sendbuff[sendlen++] = BUFF_END;  
  
  return sendlen;
}
#endif
