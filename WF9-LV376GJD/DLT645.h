#ifndef	_DLT645_H
#define _DLT645_H

#include "jblib.h"
#include "Global.h"
#include "DataParse.h"
#include "DataConvert.h"

int DLT645_RequestWS(DLT645INFO *di,DLT645DATA *ddin,DLT645DATA *ddout);

U8 rbuff(U8 buf[], U8 len, int delay)
{
  unsigned char rchar = 0;
  unsigned short nRecvLen = 0;
  unsigned short i = 0;
  
  do
  {
    if (rport(&rchar) == 0)
    {
      buf[nRecvLen++] = rchar;
      i = 0;
      
      if (nRecvLen >= len - 1)
      {
      	break;
      }
      
      continue;
    }
    if (nRecvLen > 12 && buf[0] == 0x68 && i > delay / 50)
  	{
  		break;
  	}

    i++;
  }while(i < delay / 5);
   
  return nRecvLen;
}

int DLT645_RequestWS(DLT645INFO *di,DLT645DATA *ddin,DLT645DATA *ddout)
{
  unsigned char buff[NUM_255];
  int index = 0;
  int length = 0;
  int i = 0;
  
  for (i = 0; i < BUFF_FRONT_NUM; i++)
  {
  	buff[index++] = BUFF_FRONT;
  }
  buff[index++] = di->sTag;
  memcpy(buff + index, di->addr, di->addrlen);
  index += di->addrlen;
  buff[index++] = di->sTag;

  buff[index++] = ddin->CCode;
  buff[index++] = ddin->datalen;
  i = index;
  memcpy(buff + index, ddin->databuf, ddin->datalen);
  index += ddin->datalen;
  for (; i < index; i++)
  {
    buff[i] += DATA645DEAL;
  }
  
  buff[index++] = CheckSum(buff, BUFF_FRONT_NUM, ddin->datalen + NUM_10);
  buff[index++] = di->eTag;
  
  SendData(buff, index);
  
  memset(buff, 0, sizeof(buff));
  length = rbuff(buff, sizeof(buff), NUM_100);
  if (length < NUM_12)
  {
    return ERROR;
  }
  
  length = Check645Frame(buff, length);
  if (length == ERROR)
  {
  	return ERROR;
  }

  index = 1;
  memset(di->addr, 0, di->addrlen);
  /*
  for (i = 0; i < di->addrlen; i++)
  {
    sprintf(di->addr, "%d%s", di->addr, buff[index++]); 
  }
  */
  memcpy(di->addr, buff + index, di->addrlen);
  index += di->addrlen;
  index++;
  
  ddout->CCode = buff[index++];
  if (((ddout->CCode >> NUM_7) & 0x01) == 0)
  {
  	return ERROR;
  }
  ddout->datalen = buff[index++];
  /*
  for (i = index; i < index + ddin->datalen; i++)
  {
    buff[i] -= DATA645DEAL;
  }
  */
  memset(ddout->databuf, 0, ddout->datalen);
  memcpy(ddout->databuf, buff + index, ddout->datalen);
  for (i = 0; i < ddout->datalen; i++)
  {
  	ddout->databuf[i] -= DATA645DEAL;
  }
  //index += ddout->datalen;
  
  return 0;  
}
#endif
