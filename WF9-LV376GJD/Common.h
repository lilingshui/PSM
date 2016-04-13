#ifndef _Common_H
#define _Common_H

#include "jblib.h"
#include "Global.h"
#include "DataConvert.h"

#define TERM 0
#define METER 2
#define COLLECT 4

#define MAXTERMINALCOUNT 128
#define MAXCOLLECTCOUNT 256
#define MAXMETERCOUNT 1024

#define MAXSEPSTRLEN 12

TItem ListBox_TerminalItems[MAXTERMINALCOUNT];
TItem ListBox_CollectItems[MAXCOLLECTCOUNT];
TItem ListBox_MeterItems[MAXMETERCOUNT];

int TerminalCount;
int CollectCount;
int MeterCount;

typedef struct
{
  U8 type;											//设备类型（0——集中器、2——电表、4——采集器）
  U8 rule;											//地址获取方式（0——手动输入、1——扫描截取、2——扫描查询）
  U8 addrDec;										//终端逻辑地址进制
  U8 startPos;									//截取地址起始位置
  U8 sepLen;										//截取长度
  U8 frontStr[MAXSEPSTRLEN];		//前补字符
  U8 backStr[MAXSEPSTRLEN];			//后补字符
}StruGetAddrRule;

typedef struct
{
  unsigned char NWType;        //网络架构类型
  int Flag;
  U16 MPLocate[MAXMETERCOUNT];   //数据库中行号
  //U16 MPNo[MAXMETERCOUNT];       //测量点号
  U16 MPNum;                     //测量点总数
  unsigned char AddrType; //终端地址进制
  unsigned char Port;
  char Term[10];        
  char Collect[13];
  char Meter[13];
}
StruMPAttrib;
 
StruMPAttrib CurrMPAttrib;

enum GetAddrRule
{
	Rule_Manual,
	Rule_ScanCut,
	Rule_ScanFind,
};

enum ConstConfErr
{
	Err_Conf = 1,
	Err_Rule,
	Err_Find,
	Err_Term1,
	Err_Term2,
	Err_Term3,
	Err_Meter,
	Err_Collect,
	Err_Dbf,
	Err_Pos,
};

const char *ConstConfERRStr[] = {"", 
	"您还没有进行系统配置!", 
	"截取规则错误,请重新配置!", 
	"数据库中无此记录!", 
	"终端地址必须为8位!", 
	"终端地址不能小于8位!", 
	"终端地址不能超过9位!",
	"电表地址不能超过12位!",
	"采集器地址不能超过12位!", 	
	"数据库打开异常!",
	"起始位置必须大于0!",};


void Edit_OnMsg(HWND Obj, PMsg Msg)
{
  if (Msg->Msg == WM_CHAR && Msg->WParam == VK_FUNCTION)
  {
    ImmNextIme(IME_ID_123 | IME_ID_ABC);
    Refresh(Obj);
  }
}

char copy(unsigned char *sour, unsigned char *dest)
{
  MYFILE *sourFile, *destFile;
  unsigned char buff[1024] = {};
  unsigned int length = 0;
  
  if (fileexits(sour) == FSNULL)
  {
    return -1;
  }
  
  if (fileexits(dest) != FSNULL)
  {
    use("");
    filedelete(dest);
  }
  
  sourFile = fopen(sour, "r");
  fseek(sourFile, 0, SEEK_SET);
  destFile = fopen(dest, "war");
  fseek(destFile, 0, SEEK_SET);
  
  while (1)
  {
    length = fread(buff, sizeof(buff), sourFile);
    if (length == 0)
    {
      break;
    }
    fwrite(buff, length, destFile);
  }
  
  fclose(sourFile);
  fclose(destFile);
  use("");
}


//设备地址获取方式读取
char ReadGetAddrRule(StruGetAddrRule *sRule)
{
	unsigned char f;
	unsigned char temp[MAXSEPSTRLEN] = {0};
		
  if(use(SYSCONFDBF) != 0)
  {
    sRule->rule = 0;
    sRule->addrDec = HEX;
    return 0;
  }
  
  if(libsumr())
  {      	
  	switch(sRule->type)
  	{
  		case TERM:
  			f = FD_GETTERM;
  			break;
  		case METER:
  			f = FD_GETMETER;
  			break;
  		case COLLECT:
  			f = FD_GETCOLLECT;
  			break;
  		default:
  			f = FD_GETMETER;
  			break;  			
  	}
  	
  	libset(0, f);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    sRule->rule = StrToInt(temp, DEC);  	
   
  	switch(sRule->type)
  	{
  		case TERM:
  			f = FD_GETTERMRULE;
  			break;
  		case METER:
  			f = FD_GETMETERULE;
  			break;
  		case COLLECT:
  			f = FD_GETCOLLRULE;
  			break;
  		default:
  			f = FD_GETMETERULE;
  			break;  			
  	}
				
    libset(0, f++);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    sRule->startPos = StrToInt(temp, DEC);
        
    libset(0, f++);
    memset(temp, 0, sizeof(temp));
    libread(temp);  
    sRule->sepLen = StrToInt(temp, DEC);
        
    libset(0, f++);
    memset(temp, 0, sizeof(temp));
    libread(temp);  
    sprintf(sRule->frontStr, "%s", temp);
    
    libset(0, f++);
    memset(temp, 0, sizeof(temp));
    libread(temp);   
    sprintf(sRule->backStr, "%s", temp); 
  }
  else
  {
    libappend();
    sRule->rule = 0;
  }

	libset(0, FD_ADDRDEC);
  memset(temp, 0, sizeof(temp));
  libread(temp);
  sRule->addrDec = StrToInt(temp, DEC); 
  if((sRule->addrDec != DEC) && (sRule->addrDec != HEX))
  {
  	sRule->addrDec = HEX;
  }
      
	use("");
	return 1;
}


//检测设备地址截取规则
char CheckGetAddrRule(StruGetAddrRule *sRule)
{
  int len = 0;
  unsigned char temp[8] = {0};
  char result = 0;
    
	if (sRule->startPos < 1 && sRule->sepLen != 0)
	{
		return Err_Pos;
	}
  
  len = sRule->sepLen + strlen(sRule->frontStr) + strlen(sRule->backStr);

	switch(sRule->type)
	{
		case TERM:
		{
			if (!len)
			{
				break;
			}
	    if (len > 9)
	    {
	    	result = Err_Term3;
	    } 
	    if (len < 8)
	    {
	    	result = Err_Term2;
	    }	    			
	    if ((sRule->addrDec == HEX) && (len != 8))
	    {
	      result = Err_Term1;
	    }   
			break;
		}
		case METER:
		default:
		{
			if (len > 12)
			{
				result = Err_Meter;
			}
			break;
		}
		case COLLECT:
		{
			if (len > 12)
			{
				result = Err_Collect;
			}
			break;
		} 			
	}
   
  return result; 
}


//保存设备地址截取规则
char SaveGetAddrRule(StruGetAddrRule *sRule)
{
	unsigned char f;
	unsigned char temp[MAXSEPSTRLEN] = {0};
		
  if(use(SYSCONFDBF) != 0)
  {
    MessageBox("数据库打开失败!", STR_CUE, MB_OK);
    return (char)Err_Dbf;
  }
  
  if(libsumr() == 0)
  {
  	libappend();
  }
  
  f = CheckGetAddrRule(sRule);
  if (f)
  {
  	MessageBox(ConstConfERRStr[f], STR_CUE, MB_OK);
  	use("");
  	return f;
  }
  
	switch(sRule->type)
	{
		case TERM:
			f = FD_GETTERMRULE;
			break;
		case METER:
			f = FD_GETMETERULE;
			break;
		case COLLECT:
			f = FD_GETCOLLRULE;
			break;
		default:
			f = FD_GETMETERULE;
			break;  			
	}
			
  libset(0, f++);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", sRule->startPos);
  libwrit(temp);
      
  libset(0, f++);
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", sRule->sepLen);
  libwrit(temp);
      
  libset(0, f++);
  libwrit(sRule->frontStr);
  
  libset(0, f++);
  libwrit(sRule->backStr); 
 
	use("");
	return 0;
}


char* FindAddr(unsigned char *assetnum, unsigned char fieldno)  //fieldno: term 0, meter 2, collect 4
{
  unsigned char *addr;
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int r = 0; 
  int count = 0;

  addr = assetnum; 
  
  if (0 != use(ASSETDBF))
  {
  	return addr;
  }
  count = libsumr();    
  
  for(r = 0; r < count; r++)
  {
    libset(r, fieldno);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if(strcmp(addr, temp) == 0)
    { 
      libset(r, fieldno + 1);
      memset(temp, 0, sizeof(temp));
      libread(temp);
      memset(assetnum, 0, strlen(assetnum));
      memcpy(assetnum, temp, strlen(temp));
      break;
    } 
  } 
  
  use("");
  
  return addr;      
}


char FindAddress(const char *filename, const char *address, unsigned char type)  
{
  unsigned char temp[16] = {0};
  int r = 0; 
  int count = 0;
  char flag = 0;
 
  if (0 != use(filename))
  {
  	return 0;
  }
  count = libsumr();    
  
  for (r = 0; r < count; r++)
  {
  	libset(r, 0);
    memset(temp, 0, sizeof(temp));
    libread(temp);      	
  	if (strcmp(temp, STR_DEL) == 0)
  	{
  		continue;
  	}
  	
    libset(r, FD_COLLADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strstr(address, temp) != NULL)
    { 
    	flag = 1;
    	break;    	     
    } 
    
    if (type)
    {
    	continue;
    }
    
    libset(r, FD_METERADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strstr(address, temp) != NULL)
    { 
    	flag = 1;
    	break;    	     
    }    
  } 
    
  use("");
  
  return flag;      
}


char GetAddress(unsigned char *address, unsigned char flag) //type 0term,2meter,4collect
{

	int len;
  unsigned char temp[23] = {0};
  StruGetAddrRule sRule;

  strcpy(temp, address);
  
  memset((unsigned char *)(&sRule), 0, sizeof(sRule));
  sRule.type = flag;
  ReadGetAddrRule(&sRule);
      
  if(sRule.rule == ((U8)Rule_ScanFind))   //从数据库中查找 
  { 
     FindAddr(temp, flag);
     if(strcmp(address, temp) == 0)
     {
       return Err_Find;
     }
     memset(address, 0, strlen(address));
     memcpy(address, temp, strlen(temp));
     return 0;      
  }
        
  memset(address, 0, strlen(address));
  len = sRule.sepLen + strlen(sRule.frontStr) + strlen(sRule.backStr);
  
  if (sRule.rule == ((U8)Rule_Manual) || len == 0)
  {
  	len = strlen(temp);
  	
  	if (!flag)
  	{
  		if (len > 9)
  		{
  			return Err_Term3;
  		}
  		if (len < 8)
  		{
  			return Err_Term2;
  		}
  		if (sRule.addrDec == HEX && len != 8)
  		{
  			return Err_Term1;
  		}
  		else
  		{
  			strcpy(address, temp);
  			return 0;
  		}  		
  	}
  	else
  	{
	    if (len > 12)
	    {
	    	if (flag == COLLECT)
	    	{
	    		return Err_Collect;
	    	}
	    	else
	    	{
	    		return Err_Meter;
	    	}      
	    }
	    else
	    {
	      FormatStr(temp, 12, '0', 0);
	      strcpy(address,temp);
	    }
	    return 0; 		
  	}  	
  }
  else
  {
	  if ((sRule.startPos + sRule.sepLen) > strlen(temp) + 1)
	  {
			return Err_Rule;
	  } 
  	strcpy(address, sRule.frontStr);
    memcpy(address + strlen(address), temp + sRule.startPos - 1, sRule.sepLen);
    strcat(address, sRule.backStr);
  }
    
  return 0;
}


U8 CreateDbf(P_U8 DbfName,P_U8 DbfName1)
{
  MYFILE *fp;
  int iLen,i;
  U8 iStr[3],iBuf[1000];
  
  if(fileexits(DbfName1)==0xffff) return;
  
  memset(iBuf,0x00,1000);
  memset(iStr,0x00,3);
  fp = fopen(DbfName1,"r");
  fseek(fp,8,SEEK_SET);//dbf的8、9为空库长度
  fread(iStr,2,fp);
  iLen = (iStr[1]<<8) + iStr[0];//获取dbf空库的字节数
  fseek(fp,0,SEEK_SET);
  fread(iBuf,iLen,fp);
  fclose(fp);
  iBuf[4]=0x00;//
  iBuf[5]=0x00;//dbf的记录数清零
  fclose(fp);
  use("");
  
  filedelete(DbfName);
  fp = fopen(DbfName,"war");
  fseek(fp,0,SEEK_SET);
  fwrite(iBuf,iLen,fp);
  fclose(fp);
}

StruMPAttrib* GetMPLocate(StruMPAttrib* MPAttrib, char * filename)//(char *terminal, char *filename)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0}; 
  int r, count;
  
  MPAttrib->Flag = 0;
  MPAttrib->MPNum = 0;
  use(filename);
  count = libsumr();
  for (r = 0; r < count; r++)
  {
    libset(r, 0);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      continue;
    } 
    
    libset(r, FD_METERADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    
    if(temp[0] == '\0')
    {
      continue;
    }
    //MPAttrib->MPNo[MPAttrib->MPNum] = StrToInt(temp, 10);   
    MPAttrib->MPLocate[MPAttrib->MPNum] = r;
    MPAttrib->MPNum++;
  }
  
  use("");
  return MPAttrib;
}

int GetCommParam(StruCommParam *sCommParam, int type)
{ 
	unsigned char tmpParam[NUM_10] = {0};

  use(SYSCONFDBF); 
  if(libsumr() == 0)
  {
    use("");
    return ERROR;
  } 
  
  if (type == 0)
  {
  	memset(tmpParam, 0, sizeof(tmpParam));
	  libset(0, FD_TERMCHECK);
	  libread(tmpParam);	  
	  sCommParam->Parity = StrToInt(tmpParam, HEX);
	  
	  memset(tmpParam, 0, sizeof(tmpParam)); 
	  libset(0, FD_COMMPORT);	  
	  libread(tmpParam);
	  sCommParam->Port = StrToInt(tmpParam, HEX);
	  	  
	  memset(tmpParam, 0, sizeof(tmpParam));
	  libset(0, FD_COMMBAUD);
	  libread(tmpParam);
	  sCommParam->Baud = StrToInt(tmpParam, DEC);
	}
	else if (type == 1)
  {
  	memset(tmpParam, 0, sizeof(tmpParam));
	  libset(0, 2);
	  libread(tmpParam);	  
	  sCommParam->Parity = StrToInt(tmpParam, HEX);
	  
	  memset(tmpParam, 0, sizeof(tmpParam)); 
	  libset(0, 20);	  
	  libread(tmpParam);
	  sCommParam->Port = StrToInt(tmpParam, HEX);
	  	  
	  memset(tmpParam, 0, sizeof(tmpParam));
	  libset(0, 21);
	  libread(tmpParam);
	  sCommParam->Baud = StrToInt(tmpParam, DEC);
	}
  
  use("");
  return 0;
}


char *GetRecdFileName(const char *term, char *filename)
{
	sprintf(filename, "_%s.dbf", term);
	if (fileexits(filename) != FSNULL)
	{
		return filename;
	}
	else
	{
		return NULL;
	}
	
}

//数据库查找电表
U8 LookUpMeter(unsigned char *meter, char *filename)
{
  unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
  int i, j, r;
  
  use(filename);
  r = libsumr();
  
  for(i = 0; i < r; i++)
  {
    memset(temp, 0, sizeof(temp));
    libset(i, 0);
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      continue;
    }  
             
    memset(temp, 0, sizeof(temp));
    libset(i, FD_METERADDR);
    libread(temp);
    if(strncmp(meter, temp, NUM_12) == 0)
    {
    	break;     
    }    
  }
  use("");
  
  if(i != r)
  {
     return 1;
  }
    
  return 0;
}

char GetMeterProto(const char *address, const char *filename)
{
	unsigned char temp[MAX_ASSETNO_DIGITS] = {0};
	int r = 0;
	int count = 0;
	
  use(filename);
  count = libsumr();
  
  for(r = 0; r < count; r++)
  {    
    libset(r, 0);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      continue;
    }  
                
    libset(r, FD_METERADDR);
    memset(temp, 0, sizeof(temp));
    libread(temp);
    if(strncmp(address, temp, NUM_12) == 0)
    {
    	libset(r, FD_PROTO);
    	memset(temp, 0, sizeof(temp));
    	libread(temp);
    	break;     
    }    
  }
  use("");
  
  if(r == count)
  {
     return 0;
  }
    	
  if (strcmp(temp, STR_PROTO_97) == 0)
  {
  	return (PROTO_97);
  }
  else if (strcmp(temp, STR_PROTO_SH) == 0)
  {
  	return (PROTO_SH);
  }
  else
  {
  	return 0;
  }
  	
}


U32 GetHisDataDate(U32 sourDate)
{
	U32 destDate = 0;
	int dYear = 0;
	U8 dMonth = 0;
	U8 dDay = 0;	
	unsigned char v_date[8] = {0};
	
	ImmNextIme(IME_ID_123);
  if (InputBox("格式:YYMMDD", "请输入冻结日期", v_date, 6) == ID_CANCEL)
  {
  	return 0;
  }
  
  if (v_date[0] != '\0')
  {
	  destDate = StrToInt(v_date, HEX);
	  return destDate;
	}
		  
	GetDate(&destDate);
	dDay = (unsigned char)destDate - 1;
	dMonth = (unsigned char)(destDate >> 8);
	dYear = (unsigned char)(destDate >> 16);
	
	if (dDay)
	{
		if (0x0f == dDay & 0x0f)
		{
			dDay = (dDay & 0xf0) + 0x09;
		}
		destDate = (destDate & 0xffff00) + dDay; 	
	}
	else
	{
		dMonth--;
		switch(dMonth)
		{
			case 0:
				dMonth = 0x12;
				destDate -= 0x010000; 
				break;
			case 0x01:
	  	case 0x03:
	  	case 0x05:
	  	case 0x07:
	  	case 0x08:
	  	case 0x10:
	  		dDay = 0x31;
	  		break;
	  	case 0x02:
	  		if (0 == dYear % 4)
	  		{
	  			dDay = 0x29;
	  		}
	  		else
	  		{
	  			dDay = 0x28;
	  		}
	  		break;
	  	case 0x0f:
	  		dMonth = 0x09;
	  		dDay = 0x30;
	  	default:
	  		dDay = 0x30;
	  		break;	  		
		}
		destDate = (destDate & 0xff0000) + dMonth * 0x100 + dDay; 		
	}
	
	return destDate;	
}


unsigned char GetWlsModuleType(void)
{
	unsigned char temp[8] = {0};
	unsigned char type = 0;
	
	if (use(SYSCONFDBF) != 0)
	{
		return 0;
	}
	
	if (libsumr())
	{
		libset(0, FD_WLSTYPE);
		libread(temp);
		type = StrToInt(temp, DEC);
	}
	
	use("");
	
	return type;
}


unsigned char GetBatchSendRecordNum(void)
{
	unsigned char temp[8] = {0};
	unsigned char number = 8;
	
	if (use(SYSCONFDBF) != 0)
	{
		return number;
	}
	
	if (libsumr())
	{
		libset(0, FD_SETRECDNUM);
		libread(temp);
		number = (StrToInt(temp, DEC) + 1) * number;
	}
	
	use("");
	
	return number;
}

void WritMeterRecord(char *meter, char *meterno,char *Terminal, char *Collect)
{
  char temp[MAX_ASSETNO_DIGITS] = {0};
  int i, j, r;
  int mpNo = 1;
  unsigned char appendFlag = 1;
  
  use(METE_RECD_GW);
  r = libsumr();
  
  for(i = 0; i < r; i++)
  {
    memset(temp, 0, sizeof(temp));
    libset(i, 0);
    libread(temp);
    if (strcmp(temp, STR_DEL) == 0)
    {
      continue;
    }  
       
    memset(temp, 0, sizeof(temp));
    libset(i, FD_COLLADDR);
    libread(temp);
    if (strcmp(CurrMPAttrib.Collect, temp) != 0)
    {
      continue;
    }   
    
    memset(temp, 0, sizeof(temp));
    libset(i, FD_METERADDR);
    libread(temp);   
    if (temp[0] = '\0')
    {
    	appendFlag = 0;
    	break;
    }      
  }
  
  if (appendFlag)
  { 
	  libappend();
	  r = libgetr();  
	}
	else
	{
		r = i;
	} 
	
	if (0 == r)
	{
		libset(r, FD_TERMNO);
	  libwrit(Terminal);
	  libset(r, FD_TERMADDR);
	  libwrit(CurrMPAttrib.Term);  
	}
	 
  libset(r, FD_COLLNO);
  libwrit(Collect);
  libset(r, FD_COLLADDR);
  libwrit(CurrMPAttrib.Collect);  
  libset(r, FD_METERNO);
  libwrit(meterno);
  libset(r, FD_METERADDR);
  libwrit(meter);
   
  if (CurrMPAttrib.NWType > 1)
  {
  	mpNo = r + 2;  	
  }
  else
  {
	  mpNo = r + 1;
	}
  memset(temp, 0, sizeof(temp));
  sprintf(temp, "%d", mpNo);
  libset(r, FD_DEVNO);
  libwrit(temp);
  libset(r, FD_MPNO);
  libwrit(temp);
  
  libset(r, FD_PORT);
  memset(temp, 0, sizeof(temp));
  if (CurrMPAttrib.Port == 0)
  {
	  switch(CurrMPAttrib.NWType)
	  { 
	    case 0:
	    default:
	    	CurrMPAttrib.Port = 1;
	      break;
	    case 1:
	    case 3:
	    	CurrMPAttrib.Port = 31;
	    	break;
	    case 2:
	      CurrMPAttrib.Port = 2;
	      break;
	  }
	}
	sprintf(temp, "%d", CurrMPAttrib.Port);
	libwrit(temp);	
  
  libset(r, FD_RATES);
  libwrit("4");  
  libset(r, FD_SORT);
  libwrit("51");
  
  use("");
}
#endif
