#ifndef _DATACONVERT_H
#define _DATACONVERT_H

#include "jblib.h"
#include "Global.h"

typedef struct
{
  U8 Port;
  U8 Parity;
  int Baud;
}StruCommParam;

StruCommParam  CommParam;

char* HexToStr(char* dest,char* hexstr,int lenth,char revflag,int addval);
void WritDubugInfo(unsigned char* filename, unsigned char *writinfo, int len, unsigned char flag);

void SendData(U8 buf[], U16 len)
{ 
	char sendbuf[2048] = {0};
	  
  HexToStr(sendbuf, buf, len, 0, 0);
  WritDubugInfo(DEBEGINFOTXT, sendbuf, 2*len, 1);
  
  sbuf(buf, len, 0);
}

int RecvData(U8 buf[], int len, int delay)
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
      if (nRecvLen == len)
      {
      	break;
      }      
      continue;
    }
    
    if (nRecvLen > 0 && i > 2)
  	{
  		break;
  	}
  	
  	if (getSystemMessage(MSG_MASK_KEY|MSG_MASK_UART, 1) == CM_REFRESH)
    {
			return -1; 
    }

    i++;
  }while(i < delay / 5);
     
  if (nRecvLen > 0)
  {
  	char recv[2048] = {0};
   
	  HexToStr(recv, buf, nRecvLen, 0, 0);
	  WritDubugInfo(DEBEGINFOTXT, recv, 2*nRecvLen, 2);
	} 
	
  return nRecvLen;
}

U16 RecvChar(U8 buf[], U16 len, int delay)
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
      if (nRecvLen == len)
      {
      	break;
      }      
      continue;
    }
    
    if (nRecvLen > 0 && i > 2)
  	{
  		break;
  	}

    i++;
  }while(i < delay / 5);
     
  if (nRecvLen > 0)
  {
  	char recv[2048] = {0};
   
	  HexToStr(recv, buf, nRecvLen, 0, 0);
	  WritDubugInfo(DEBEGINFOTXT, recv, 2*nRecvLen, 2);
	} 
	
  return nRecvLen;
}


U8 RecvWlsData(U8 buf[], U8 len, U8 delay)
{
  unsigned char rchar;
  unsigned char nRecvLen = 0;
  unsigned short i = 0;
  
  do
  {
    if (rport(&rchar) == 0)
    {
      buf[nRecvLen++] = rchar;     
      if (nRecvLen == len)
      {
      	break;
      }
      continue;
    }
    
    if (nRecvLen > 0 && buf[nRecvLen-1] == 0x55)
  	{
  		break;
  	}

    i++;
  }while(i < delay);
  /*for (i = 0; i < delay; i++)
  { 

    if(rport(&rchar) != 0)
    {
      continue;
    }   
  
    if (rchar == 0xAA)
    {
      buf[0] = rchar;
      len--;
      nRecvLen = rbuf(&buf[1], len, 32);
      nRecvLen++;
      break;
    }
  }
    
  if (i == delay)
  {
    return 0;
  }*/

  return nRecvLen;  
}
/*
void ShowData(U8 buf[], U8 length)
{
  U8* msg = malloc(length * 3);
  int i;
  
  cls();
  memset(msg, 0, length * 3);
  for (i = 0; i < length; i++)
  {
    sprintf(msg, "%s%02X ", msg, buf[i]);
  }
  
  MessageBox(msg, "Debug", MB_OK);
  free(msg);
}
*/
U8 CheckSum(U8 buf[], U16 index, U16 length)
{
  U8 cs = 0;
  int i;

  for (i = 0; i < length; i++)
  {
    cs += buf[index + i];
  }
  return cs;
}

unsigned long long StrToHex(char str[], U8 length)
{
  unsigned long long hex = 0;
  int i;

  for (i = 0; i < length; i++)
  {
    if (str[i] >= '0' && str[i] <= '9')
    {
      hex = hex * 0x10 + (str[i] - '0');
    }
    else if (str[i] >= 'A' && str[i] <= 'F')
    {
      hex = hex * 0x10 + (str[i] - 'A' + 0x0A);
    }
    else if (str[i] >= 'a' && str[i] <= 'f')
    {
      hex = hex * 0x10 + (str[i] - 'a' + 0x0A);
    }
  }
  return hex;
}

char* StrToHexStr(char* str /* srcstr */,int lenth /* hexstr lenth*/,
      char revflag /* reverse */,char preflag /*fix pre-*/,
      int addval /* add value */)
{
  int i;
  unsigned char j,k,m;
  unsigned char chr;
  char* p;

  for(p=str;*p;++p)
  {
    chr = *p;
    if(chr<=0x7A && chr>=0x61)
      chr-=0x20;
    if(chr<0x30 || chr>0x47 || (chr>0x39 && chr<0x41))
      return (str);
    *p = chr;
  }
  if(preflag) //前面补0 还是后面补0 ；true 为前面补
  {
    for(i=0;i<2*lenth-(int)strlen((char*)str);i++)
    {
      k = 0x30;
      m = 0x30;
      p = str;
      for(j=0;j<=i+(int)strlen((char*)str);j++)
      {
        if(j%2)
        {
          k = *p;
          *p = m;
        }else{
          m = *p;
          *p = k;
        }
        p++;
      }
    }
    *p = 0;
  }else{    // false 为后面补
    p = str;
    for(i=strlen((char*)str);i<2*lenth;i++)
    {
      *(p+i) = 0x30;
    }
    *(p+2*lenth) = 0;
  }
  i=0;
  j=0;
  k=0;
  m=0;
  p = str;
  do{
    j=*p++;
    k=*p++;
    if(j>0x40)
      j = j - 0x37;
    else
      j = j - 0x30;
    if(k>0x40)
      k = k - 0x37;
    else
      k = k - 0x30;
    *(str+m) = j*16+k+addval;
    m++;
    i+=2;
  }while(i<2*lenth);
  *(str+m) = 0;
  if(revflag) //是否倒置字符串
  {
    p = str;
    for(i=0;i<(int)(lenth/2);i++)
    {
      chr = *(p+i);
      *(p+i) = *(p+lenth-i-1);
      *(p+lenth-i-1) = chr;
    }
  }
  return (str);
}
int StrToInt(char* sStr, char Radix)
{
  int Decimal = 0;
  char i,StrLen;
  char TmpChar;

  StrLen = strlen(sStr);
  if(StrLen > 10)
    return 0xffffffff;
  for(i=0;i<StrLen;i++)
  {
    TmpChar = *(sStr + i);
    if(TmpChar>='A' && TmpChar<'G')
      TmpChar -= 0x37;
    else if(TmpChar>='a' && TmpChar<'g')
      TmpChar -= 0x57;
    else if(TmpChar>='0' && TmpChar<='9')
      TmpChar -= 0x30;
    else
      return 0xffffffff;
    Decimal = Decimal*Radix + TmpChar;
  }
  return Decimal;
}
/********************************************** yy add
将Str所指的字符串进行填充,根据Flag标志,如果flag标志为1,则填充在Str尾部,
如果flag标志为0,则填充在Str前面,填充长度为参数length减去Str的实际长度
**********************************************/
void FormatStr( char* Str, int length, char Stuff, char Flag)
{
  char rstr[256] = {0};
  int i;
  unsigned char Length;

  Length = strlen(Str);
  if(length > Length)
  {
    if(Flag)
    {
      for(i = Length; i<length; i++)
        Str[i] = Stuff;
    }
    else
    {
      memset(rstr, Stuff, length - Length);
      rstr[length-Length] = 0;
      strcat(rstr,Str);
      memset(Str,0,Length);
      strcpy(Str, rstr);
    }
  }
}

/*
char* ReverseStr(char* sstr)
{
  char len,i,j;
//  char* dstr;
  char dstr[255] = {0};

  len = strlen(sstr);

//  dstr = (char*)malloc(255);
  if(dstr == NULL)
    return NULL;
  i=0;
    for(j=(len/2);j>=1;j--)
  {
        dstr[i]=sstr[j*2-2]; 
    dstr[i+1]=sstr[j*2-1];
    i+=2;
  }
  dstr[i] = '\0';
  strcpy(sstr,dstr);
//  free(dstr);
  return sstr;
}*/

char* ReverseStr(char* sstr, char flag)
{
  char len,i,j;
  char dstr[255] = {0};

  len = strlen(sstr);
  i = 0;
  
  if (flag == 'a' || flag == 'A')
  {
	  for (j = 0; j < len; j++)
	  {
	  	dstr[i] = sstr[len - j - 1];
	  	i++;
	  }
	}
	else
	{
	  for (j=(len/2); j>=1; j--)
	  {
	    dstr[i] = sstr[j*2-2]; 
	    dstr[i+1] = sstr[j*2-1];
	    i += 2;
	  }
	}
  dstr[i] = '\0';
  strcpy(sstr, dstr);
  return sstr;
}

char* HexToStr(char* dest,char* hexstr,int lenth,char revflag,int addval)
{
  int i;
  char tmp[3]={0};

  if(revflag) //是否倒置字符串
  {
    for(i=0;i<lenth;i++)
    {
      sprintf(tmp,"%02x",(unsigned char)(*(hexstr+lenth-i-1)+addval));
      *(dest+2*i) = tmp[0];
      *(dest+2*i+1) = tmp[1];
    }
  }else
  {
    for(i=0;i<lenth;i++)
    {
      sprintf(tmp,"%02x",(unsigned char)((*hexstr++)+addval));
      *(dest+2*i) = tmp[0];
      *(dest+2*i+1) = tmp[1];
    }
  }
  *(dest+2*lenth) = 0x0;
  return (dest);
}


void DelFile(unsigned char *filename, unsigned int maxLen)
{
  MYFILE *fp;
  
  if (fileexits(filename) == FSNULL)
  {
    return;
  }

  fp = fopen(filename, "r");
  
  if (flength(fp) > maxLen)
  {
  	fclose(fp);
  	filedelete(filename);  	
  }
  else
  {
  	fclose(fp);
  }
    
}


void WritDubugInfo(unsigned char *filename, unsigned char *writinfo, int len, unsigned char flag)
{

  char *DubugInfoType[3] = {"Debg: \0", "Send: \0", "Recv: \0"};
  
  MYFILE *fp;
  if (len == 0)
  {
    return;
  }
  
  DelFile(filename, 2*MAX_DEBGFILE_LEN);

  fp = fopen(filename, "war");
    
  if (flag < 3) 	  
  {
	  fseek(fp, 0, SEEK_END);
	  fwrite(DubugInfoType[flag], strlen(DubugInfoType[flag]), fp);
	
	  fseek(fp, 0, SEEK_END);
	  fwrite(writinfo, len, fp);
	
	  fseek(fp, 0, SEEK_END);
	  fwrite("\r\n", 2, fp);
	}
  else if (flag >= 10 && flag < 13)
  {
	  fseek(fp, 0, SEEK_END);
	  fwrite(DubugInfoType[flag - 10], strlen(DubugInfoType[flag - 10]), fp);
	
	  unsigned char temp[512] = {0};
	  HexToStr(temp, writinfo, len, 0, 0);

	  fseek(fp, 0, SEEK_END);
	  fwrite(temp, 2*len, fp);
	
	  fseek(fp, 0, SEEK_END);
	  fwrite("\r\n", 2, fp);  	
  }
  else
  {
  	fseek(fp, 0, SEEK_END);
	  fwrite(writinfo, len, fp);
  }
      
  fclose(fp);
}
#endif