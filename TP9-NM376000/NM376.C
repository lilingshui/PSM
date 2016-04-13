/******************************************************************************************
*    功能：准备组帧的数据和对数据进行解析
*    时间：2006年9月12日
*    作者：颜勇
******************************************************************************************/

#include "..\inc\bio_func.h"
#include "..\inc\key.h"
//#include <tp_bio.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>

#include <stddef.H>
#include <stdarg.h>
#include <math.h>

#include <limits.h>
//#include <floatasm.h>
#include <errno.h>
#include <ctype.h>
//#include <tp_dbf.h>

#define debug_1		//调试信息开关

//#define FILE _FILE
#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0
//键盘定义
#define _UP  0x4800
#define _DWN 0x5000
#define _LFT 0x4b00
#define _RGT 0x4D00
#define _ENT 0x1c0d
#define _ESC 0x011b
#define HELP 0x3b00
#define _DEL 0x0e08
#define FUNCTION 0x8200
//#define SHIFT 0x4b00
#define _F1 0x9600
#define ZERO 0x0130
#define ONE 0x0231
#define TWO 0x0332
#define THREE 0x0433
#define FOUR 0x0534
#define FIVE 0x0635
#define SIX 0x0736
#define SEVEN 0x0837
#define EIGHT 0x0938
#define NINE 0x0a39
/******************************************************************************************
******************************************************************************************/
#define SCREENWIDTH 20
#define SCREENHEIGHT 8
#define	CENTERY 5
#define CENTERX 9
#define MaxNum 100							//单个数据标识包含的最大数据项数
#define ImLines 3 							//一个菜单的信息占用屏幕的最大行数
#define XPosition 0			  				//显示右移距离

#define DBFMAX 4								//同时可以打开的文件数
#define ITEMS 0
#define PROTO 1
#define SELEC 2
#define SAVE 3
#define OPENCOM 1

//
#define HEX	16
#define DEC	10
#define IDXOFMON	2
#define LENOFMON	2
#define OFFSETOFWEEK	5

/********************************************************************************************
*数据库文件结构定义开始
********************************************************************************************/

typedef struct {
		char	Version;					// offset: 0	size: 1版本信息
		char	UpdateDate[3];				// offset: 1	size: 3最后更新日期
		unsigned long Record_Num;			// offset: 4	size: 4
		unsigned short int Head_Size;		// offset: 8	size: 2
		unsigned short int Record_Size;		// offset: 10	size: 2
		char Reserve[20];					// offset: 12	size: 20
}TDBHEAD;									// Total Size: 32 bytes

typedef	struct {
		char	Field_Name[11];				// offset: 0	size: 11
		char	Field_Type;					// offset: 11	size: 1
		unsigned long Field_Place;			// offset: 12	size: 4
		unsigned char Field_Lenth; 			// offset: 16	size: 1
		unsigned char  Field_Decimal;		// offset: 17	size: 1
		char	Reserve1[2];				// offset: 18	size: 2
		char	Field_WorkArea;				// offset: 20	size: 1
		char	Reserve2[11];				// offset: 21	size: 11
}TDBFIELD;									// Total Size: 32 bytes

typedef struct{
	char FieldName[12];						//字段名称
	unsigned long FieldOffset;				//字段在记录中的偏移度
	unsigned short int FieldLen;			//字段长度
}TSField;

typedef struct {
	FILE* p_file;
	unsigned short int m_headsize;			//文件头大小
	unsigned short int  m_RecordSize;		//单条记录所占的字节数
	unsigned long m_RecordNum;				//数据库中记录总数
	unsigned long m_CurrRecord;				//当前所指的记录索引号(0-n)
	unsigned char  m_FieldNum;				//数据库中字段总数
	TSField p_FiledInfo[16];
}TDBF;
/********************************************************************************************
*数据库文件结构定义结束
********************************************************************************************/

/********************************************************************************************
*菜单现场定义开始
********************************************************************************************/
//保存当前菜单当前屏顶端和底端的信息
typedef struct{
	unsigned short int CurrIndex;     		//当前的菜单的序号
	unsigned char BNIndex;       			//前（后）一个菜单项的序号
	unsigned char Lines;		   			//当前菜单占用的行数
	unsigned char DispLines;				//当前显示的行数
	char Line[ImLines][21];					//当前菜单第一行的内容，
}TManulHT;					 				//限定一个菜单在顶端和底端最多分三行显示
//保存菜单当前选择项的信息
typedef struct{
	unsigned char Items;      				//当前菜单的选择项数
	unsigned short int CurrIndex;  			//当前选择的菜单项的序号
	unsigned char CurrLineNo; 				//当前显示行的行号
	TManulHT ScreenTop;
	TManulHT ScreenBottum;
}TManulStatus;

typedef struct{
	int FDNIndex;				//记录当前屏的第一个数据的索引号
	int DataNums;				//记录当前屏的数据个数，只在当前屏第一个数组中有用
}TUIInfor;

typedef struct{//
	char Msg[60];
	char Value[10];
}TEnumValue;

typedef struct{
	long Part1;
	long Part2;
	long Part3;
}TLConvert;

/********************************************************************************************
*菜单现场定义结束
********************************************************************************************/

const char* Ok = "确认";
const char* Cance = "退出";

char* PartData[MaxNum];			  			//数据分割
char* PartFormat[MaxNum];		  			//数据格式分割
char* PartDName[MaxNum];			  		//数据名称分割
char* PartUnit[MaxNum];			  		    //数据单位分割
unsigned int DNameNum;				 		//数据名称项数
unsigned int DFormatNum;			  		//数据格式项数
unsigned int DUnitNum;				  		//数据单位数
unsigned int SDNameNum;				 		//数据名称项数
unsigned int SDFormatNum;					//保存时的数据格式项数
unsigned int SUnitNum;				  		//数据单位数
unsigned int ModuNum;			  	  		//数据项数
char ReceiBuff[800];
char SendBuff[800];
int RFrameLength;						//当前接收帧中的数据域长度
unsigned char TRANSMODE = 0x01;				//通信规则
unsigned char TRANSMODEVEN = 0x09;			//串口使用偶校验
unsigned char TeleCom;
unsigned char TelType;						//校验的类别 0表示无 1表示偶校验
unsigned int Botelv;
unsigned long waittime;
int NumSys;              //设置终端地址的进制 0表示十六进制 1表示10进制

char* PartFormat_Bak[MaxNum];			  	//全局动态内存指针,用于最后释放内存
char* PartDName_Bak[MaxNum];			  	//全局动态内存指针,用于最后释放内存
char* PartUnit_Bak[MaxNum];			  		//全局动态内存指针,用于最后释放内存
/******************************************************************************************
*数据库文件操作变量定义开始
******************************************************************************************/
TDBF m_file[4];
/******************************************************************************************
* 数据库文件操作变量定义结束
******************************************************************************************/

/******************************************************************************************
*菜单操作变量定义开始
******************************************************************************************/
unsigned char Items;      					//当前菜单的选择项数
unsigned short int CurrIndex;  				//当前选择的菜单项的序号
unsigned char CurrLineNo; 					//当前显示行的行号
TManulHT ScreenTop;							//顶部信息
TManulHT ScreenBottum;						//底部信息
/******************************************************************************************
*菜单操作变量定义结束
******************************************************************************************/

/***********************************************************************************************
*公用的工具函数声明开始
************************************************************************************************/
void SelectCom();
unsigned short int ParaHisCfg(unsigned char* DataBuff,int Flag);
void hidecursor(unsigned char flag);
char* itoa(unsigned int value,char* string, int radix);
int StrToInt(char* sStr, char Radix);
short int Scanf(char * str, unsigned char length);
char* StrtoHex(char* str,
		 	   int lenth ,char revflag,
		       char preflag ,int addval);
char* HextoStr(char* dest,char* hexstr,int lenth,char revflag,int addval);
char* AStrToHexStr(char* Buff, char* PASCIIStr);
char* DateTimeToStr(char* DTStr);
char* ReverseStr(char* sstr);
void FormatDT(char *DT);
void BuffRecon(char* Buff, char Length);
void strupr(unsigned char *str);
short int MessageBox(char* str,
					 char* DataBuff,
					 unsigned char Length,
					 unsigned char Flag);
unsigned short int MessageBox1(char *Msg, char* HintMsg);
short int MessageBox2(unsigned char* DataBuff);
void DisplayBott(char* LStr, char* RStr);
unsigned char CalculItemLines(char* Item);
unsigned char SepStringOrN(char* string, unsigned char Lines, unsigned char Ypos);
void FormatStr( char* Str, int length, char Stuff, char Flag);
unsigned int SeparateSTR(char* String, char Flag, char Separator);
int InputAddr(char* AddrBuff);
int InputSignalData(int StartDataIndex);
int InputData(char* SDIdentify, char* SModels);
int FullDataInfo(char* DataIdentify, int Modules, int StartIndex);
void ParseReceiFrame(int StartIndex, int StartDIndex);
void DisplayData(void);
int AnalysisData(char* FunCode, char Flag);
void BigNumConvert(char* DecStr, long Model, TLConvert* ConvertData);
char* AnalysisDot(char* Str, char* Formate, char Flag);
void FreeDataInfor(char* dIdentify, char Flag);
/***********************************************************************************************
*公用的工具函数声明结束
************************************************************************************************/
/******************************************************************************************
*数据库文件操作函数声明开始
******************************************************************************************/
int GetFieldIndex(const char* FieldName, int WIndex);
int ReadField(const char* FieldName, char* Buff, int WIndex);
int SearchField(const char* FieldName, const char* Value, int WIndex);
unsigned long GetCount(int WIndex);
int GoRecord(unsigned long Index, int WIndex);
int OpenDbf(char* FileName);
void CloseDbf(char* FileName);
short int GetSelectValue(char* Buff, char* Formate, char* SlecValue);
/******************************************************************************************
*数据库文件操作函数声明结束
/******************************************************************************************
*菜单操作函数声明开始
******************************************************************************************/
unsigned char GetOmegaScr(TManulStatus *ScrStatus,char** MString, char Items);
unsigned char CopyManulItems(TManulStatus *ScrStatus, char* Item, unsigned char Flag);
unsigned char CalcuLinesManul(char** Manul, short int Items);
void DisplayMenu(TManulStatus *ScrStatus, char** MString, char* Title);
void InitManul(TManulStatus *ScrStatus,
			   char** FManul, char ItemNum,
			   char CurrItem, char CurrLine);
unsigned short int SelectItem(TManulStatus *ScrStatus, char** MString, char* CurrTitle);
/******************************************************************************************
*菜单操作函数声明结束
******************************************************************************************/
/******************************************************************************************
*通讯操作函数声明结束
******************************************************************************************/
void SendFrame(char* sframe,unsigned short int framelen);
short int RecvFrame();
/******************************************************************************************
*通讯操作函数声明结束
******************************************************************************************/
/******************************************************************************************
*组帧处理函数声明开始
******************************************************************************************/
int SetData(char* Data);
int SetFormat(char* Format);
int SetDataName(char* DataName);
int SetUnits(char* Units);
int BuildFrame(char* Addr, char* CtrlCode, char* DIdentify, char* FunCode, unsigned short int* CurrFrameLength);
int DecodeFrame(char* Frame);
unsigned char GetCheck(unsigned char* str,int start,int lenth);

void BuildTimeAdjustStr( char* targetstr ); 
/******************************************************************************************
*组帧处理函数声明开始
******************************************************************************************/

void MainItems(char* PITEMIND, char* MaulTitle,char* Address);
void Processor(char* PITEMIND,char* SAddress,char* FunCode);
int GetDataStr(char* DataStr, char* SCtlCode, char* SFunCode,char*SDIdentify, char* SModels);
void DebugTool(short int ManuInd);
void RepCopy(char* SAddress, short int Select);
int DatapointInput(char *string, int type);

/*电表档案批量设置扩展函数*/
typedef struct{
	unsigned char ValidFlag;
	int MeterNo;
	unsigned char MeterAddr[6];
	unsigned char ProtocolType;
	unsigned char BaudRate;
	unsigned char Port;//端口号
	unsigned char BSort;//大分类
	unsigned char SSort;//小分类
}MeterRecordStruct;
MeterRecordStruct MRecordBuf[8];//电表记录保存缓存
MeterRecordStruct LastRecord;//上一次输入内容
int RecordNum;//本次设置的记录总数，保存在文件开头
char* RecordPath = "\\长沙威胜\\Record.dat";
char* ipconfig = "\\长沙威胜\\ipconfig.txt";
unsigned char RecordReadFlag = 0;//电表档案读取标志 0表示未读取 需要从记录文件中读出，1表示已读到内存
unsigned char RecordChangeFlag = 1;//电表档案修改标志 0表示已改变，需要提示是否保存 1表示未改变
char Addr[12] = {0};
//档案读取函数
void MeterRecordRead(MeterRecordStruct *DataBuf, int* RecordNum);
//档案保存函数
void MeterRecordSave(MeterRecordStruct *DataBuf, int RecordNum);
//档案显示刷新函数
void MeterRecordShow(int RecordNo,int Num,int ChooseNum);//起始记录序号，显示个数，被选中记录号 一页显示4项
//档案修改函数
int MeterRecordEdit(int RecordNo);
//设置报文组装函数
int MeterRecordFrameBuild(unsigned char *FrameBuf,int RecordNo, int* CurrFrameLength);//从记录的第一条开始组装
//档案设置主处理函数
void MetarRecordProcess();
//档案管理主函数
void MeterRecordMangage();
//档案下发主函数
void MeterRecordSend();
//新增档案函数
void MeterRecordAdd();
//档案查看修改函数
void MeterRecordView();
//清空档案函数
void MeterRecordClear();


int HBAddrFrameBuild(char* zipaddr,short int * CurrFrameLength);
int HBAddr(short int * CurrFrameLength);
/****************************************************************************************/
/*
 **  //GetOnDateTime()
 *
 *  PARAMETERS:
 *	   DateStr: 要格式化的日期时间数组的地址
 *     aFormat: 目标格式的地址
 *	   RetunStr: 要返回的数据的地址
 *  DESCRIPTION:
 *	   将日期时间格式化成 年-月-日 时：分：秒 周
 *  RETURNS:
 *		无
 */
void GetOnDateTime(char* DateStr,char* aFormat,char* RetunStr)
{
	char tmp_data[3] = {0};
	char tmp_format[3] = {0};
	char Year[3] = {0};
	char Month[3] = {0};
	char Day[3] = {0};
	char Hour[3] = {0};
	char Minute[3] = {0};
	char Second[3] = {0};
	char Week[3] = {0};
	unsigned int i,StrLen,Flag;

	StrLen = strlen(DateStr);
	for(i=0; i<StrLen; i+=2)
	{
		strncpy(tmp_format, aFormat+i, 2);
	   	if (strcmp(tmp_format,"YY")==0 || strcmp(tmp_format,"TY")==0)//年
          		strncpy(Year, DateStr+i, 2);
       		else if (strcmp(tmp_format,"MM")==0 || strcmp(tmp_format,"TM")==0)//月
          		strncpy(Month, DateStr+i, 2);
       		else if (strcmp(tmp_format,"WM")==0)//月
		{
			strncpy(tmp_data, DateStr+i, 2);
			if(((tmp_data[0]<='f')&&(tmp_data[0]>='a'))||((tmp_data[0]<='F')&&(tmp_data[0]>='A')))
				tmp_data[0] = tmp_data[0] - 1;
			if((tmp_data[0]%2)==0)
				tmp_data[0] = '0';
			else
				tmp_data[0] = '1';
			strncpy(Month, tmp_data, 2);
//	          	Week[0] =DateStr[strlen(DateStr) -1];
		}
	   	else if (strcmp(tmp_format,"DD")==0 || strcmp(tmp_format,"TD")==0)//日
          		strncpy(Day, DateStr+i, 2);
	   	else if (strcmp(tmp_format,"hh")==0 || strcmp(tmp_format,"Th")==0)//时
          		strncpy(Hour, DateStr+i, 2);
	   	else if (strcmp(tmp_format,"mm")==0 || strcmp(tmp_format,"Tm")==0)//分
          		strncpy(Minute, DateStr+i, 2);
	   	else if (strcmp(tmp_format,"ss")==0 || strcmp(tmp_format,"Ts")==0)//秒
          		strncpy(Second, DateStr+i, 2);
//	   	else if (strcmp(tmp_format,"WW")==0 || strcmp(tmp_format,"TW")==0 )//周
	}
	*RetunStr = 0;
	if(Year[0] != 0)
	{
		strcat(RetunStr, Year);
		strcat(RetunStr, "-");
	}
	if(Month[0] != 0)
	{
		strcat(RetunStr, Month);
		strcat(RetunStr, "-");
	}
	if(Day[0] != 0)
	{
		strcat(RetunStr, Day);
		strcat(RetunStr, " ");
	}
	if(Hour[0] != 0)
	{
		strcat(RetunStr, Hour);
		strcat(RetunStr, ":");
	}
	if(Minute[0] != 0)
	{
		strcat(RetunStr,Minute);
	}
	if(Second[0] != 0)
	{
		strcat(RetunStr,":");
		strcat(RetunStr,Second);
	}
	if(Week[0] != 0)
	{
		strcat(RetunStr," ");
		strcat(RetunStr,Week);
	}
}

/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<    		 工具函数定义开始			 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/
//#if debug
void Check(U8* TArray)
{
	U32 i;
	char mid;
	U8 m;
	S8 test[200]={0};
	for(i=0;i<4;i++)
	{
		m = TArray[i];
		test[2*i]=((m&0xf0)>>4)+0x30;
		test[2*i+1]=(m&0xf)+0x30;
	}
	_cls();
	_putstr(test);
	_bioskey(0);
} /**/
//#endif

/******************************************************************************************
*   功能：打开或关闭光标。
*   参数：flag = 1 打开
		  flag = 0 关闭
******************************************************************************************/
void hidecursor(unsigned char flag)
{
	char status,width,height,blink;

	_lcd_getcursor(&status, &width, &height, &blink);
	if(!flag)
		_lcd_setcursor(flag, width, height, blink);
	else if(flag == 1)
	{
		_lcd_setcursor(4, width, height, blink);
		width = 2;
		_lcd_setcursor(flag, width, height, blink);
	}
}
/******************************************************************************************
* 	获取当前光标的列位置
******************************************************************************************/
char getx(void)
{
   return (_getx());
}

/******************************************************************************************
* 	获取当前光标的行位置
******************************************************************************************/
char gety(void)
{
   return (_gety());
}

/******************************************************************************************
* 	相对振中的而言，兰德的库函数对实现指定显示颜色,*只能针对当前的显示颜色进行反转
*	所以要注意这个函数的配对使用
******************************************************************************************/
void setdispcolor(unsigned char color)
{
	_setdispcolor(color);
}

/******************************************************************************************
* 	接收按键，但当前被按键的值的显示以'*'代替
******************************************************************************************/
short int key_secret(char* p, char len)
{
	short int ks;
	unsigned char chr,i = 0;
	unsigned char x,y,length;


	x = getx();
	y = gety();
	length = len;
	hidecursor(1);
	while(1)
	{
		ks = _bioskey(0);
		chr = ks & 0xff;
		if(chr >= '0' && chr <= '9')
		{
			if(!(i%(SCREENWIDTH)) && i != 0)
				_putstr("\n");
			*(p + i) = chr;
			_putch('*');
			i++;
		}
		else if(ks == _DEL)
		{
			if(i == 0)
				continue;
			*(p + i - 1) = 0;
			if(getx() != x && gety() == y)
			{
				_gotoxy(getx()-1, gety());
				_putch(' ');
				_gotoxy(getx()-1, gety());
			}
			else if (getx() == 0 && gety() != 1)
			{
				_gotoxy(SCREENWIDTH, gety() -1);
				_putch(' ');
				_gotoxy(SCREENWIDTH, gety());
			}
			i--;
		}
		else if(ks == _ENT || ks == _ESC)
		{
			*(p + i) = 0;
			if(ks == _ESC)
			   	memset(p, 0, length);
			hidecursor(0);
			return (ks);
		}
		if(i >= length)
		{
			hidecursor(0);
			*(p + length) = 0;
			return 0;
		}
	}
}
void FormatDT(char *DT)//格式化时间和日期,输出格式为yyyy-mm-dd hh.mm.ss yy add
{
	char s1[12] = {0};

	_getdate(s1);
	memset(DT, 0, 20);
	s1[4] = '-';
	s1[7] = '-';
	s1[10] = ' ';
	strcpy(DT, s1);
	memset(s1, 0, 12);
	_gettime(s1);
	strcat(DT, s1);
	*(DT + 19) = 0;
}

char* DateTimeToStr(char* DTStr)
{
	char TmpStr[14] = {0};
	char TmpYear;
	char i,k,j;
	char* Pstr;

	Pstr = DTStr;
	TmpYear = DTStr[2];
	k = (strlen(Pstr)/3);
	if(k == 2)
		k = 3;
	if(TmpYear != '/' && TmpYear != '-' && TmpYear != ':')
		j = 2;
	else
		j = 0;
	for(i = 0; (i < k && *(Pstr+3*i+j)); i++)
		strncat(TmpStr, Pstr+3*i+j, 2);
	memset(DTStr, 0, strlen(DTStr));
	strcpy(DTStr, TmpStr);
	return DTStr;
}

char* StrtoHex(char* str /* srcstr */,int lenth /* hexstr lenth*/,
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
	if(preflag)	//前面补0 还是后面补0 ；true 为前面补
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
	}else{		// false 为后面补
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
	if(revflag)	//是否倒置字符串
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
//Radix表示sStr所指向的字符串的类型
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

char* HextoStr(char* dest,char* hexstr,int lenth,char revflag,int addval)
{
	int i;
	char tmp[3]={0};

	if(revflag)	//是否倒置字符串
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

char* AStrToHexStr(char* Buff, char* PASCIIStr)
{
	int i,k;
	char ASCIIChar;
	char SignalHexStr[3];
	char Tmpstr[65] = {0};
	char tmpC;

	k = strlen(PASCIIStr);
	for(i = 0; i<k; i++)
	{
		memset(SignalHexStr, 0, 3);
		ASCIIChar = PASCIIStr[i];
		if(ASCIIChar != '+')
		{
			tmpC = (ASCIIChar >> 4);
			if((tmpC>9 && tmpC<16))
				SignalHexStr[0] = tmpC + 0x37;
			else
				SignalHexStr[0] = tmpC + 0x30;
			tmpC = (ASCIIChar & 0xf);
			if((tmpC>9 && tmpC<16))
				SignalHexStr[1] = tmpC + 0x37;
			else
				SignalHexStr[1] = tmpC + 0x30;
			strcat(Tmpstr, SignalHexStr);
		}
		else
			{
				SignalHexStr[0] = 0x34;
				SignalHexStr[1] = 0x30;
				strcat(Tmpstr, SignalHexStr);
			}
	}
	strcpy(Buff, Tmpstr);
}

void BigNumConvert(char* DecStr, long Model, TLConvert* ConvertData)
{
	int i,k;
	long tpart1 = 0;
	long tpart2 = 0;
	long tpart3 = 0;
	int tdata;

	k=strlen(DecStr);
	tpart1 = tpart2 = 0;
	for(i=0; i< k; i++)
	{
		tpart3 = ((tpart3%Model)%Model)*10 + DecStr[i] - 0x30;
		tpart2 = (tpart2%Model)*10 + tpart3/Model;
		tpart1 = tpart1*10 + tpart2/Model;
	}
	ConvertData->Part1 = tpart1;
	ConvertData->Part2 = tpart2%Model;
	ConvertData->Part3 = tpart3%Model;
}
/***********************************************************************************************
*功能：按这种格式显示
*	   	——————————————
*		——————————————   ____
*					.					 |
*					.					 |
*					.					间隔的行数为屏幕的高度减2
*					.					|
*	   	——————————————   _|__
*		 LStr 				   RStr
*		——————————————
* 参数: LStr：显示在屏幕左底部的信息
*		RStr：显示在屏幕左底部的信息
* 返回值：无
************************************************************************************************/
void DisplayBott(char* LStr, char* RStr)
{
	setdispcolor(0);
	_gotoxy(1,SCREENHEIGHT + 1);
	_putstr(LStr);
	_gotoxy(SCREENWIDTH - strlen(RStr), SCREENHEIGHT + 1);
	_putstr(RStr);
	_setdispcolor(1);
	_drawline(0, (SCREENHEIGHT + 1)*16 -1, SCREENWIDTH*8 -1, (SCREENHEIGHT + 1)*16 -1, 1);
	_drawline(0, (SCREENHEIGHT + 2)*16 -1, SCREENWIDTH*8 -1, (SCREENHEIGHT + 2)*16 -1, 1);
}

unsigned char CalculItemLines(char* Item)//计算显示条目所占用的行数
{
	unsigned char NoCh, Search, LineLength, Line;
	char* string;

	if(Item == NULL || !strlen(Item))
		return 0;
	string = Item;
	Line = 0;
	LineLength = ((SCREENWIDTH - XPosition)/2)*2;
	while(1)
	{
		if(strlen(string) > LineLength)
		{
			NoCh = 0;
			for(Search = 0; Search < LineLength; Search++)
			{
				if((unsigned char)*(string + Search)<128)
					NoCh++;
			}
			NoCh %= 2;
			Line++;
			string += (LineLength - NoCh);
		}
		else if(strlen(string) > 0)
		{
			Line++;
			break;
		}
		else
			break;
	}
	return Line;
}
/***********************************************************************************************
*功能：根据字符串的实际长度和屏幕的宽度分行显示
* 参数：string->字符串
*		Lines->屏幕的空闲行数
*		Ypos->行坐标（1--屏幕高度+2）
*		XPos->列坐标（1--屏幕宽度）
* 返回值：实际显示的行数
************************************************************************************************/
unsigned char SepStringOrN(char* string,
			   			   unsigned char Lines,
			   			   unsigned char Ypos)
{
	unsigned char NoCh,Search,Line = 0;
	unsigned char LineLength;
	unsigned char First[SCREENWIDTH +1 ] = {0};

	LineLength = ((SCREENWIDTH - XPosition)/2)*2;
	while(1)
	{
		_gotoxy(XPosition, Ypos + Line);
		if(strlen(string) > LineLength)
		{
			NoCh = 0;
			for(Search = 0; Search < LineLength; Search++)
			{
				if((unsigned char)*(string + Search)<128)
					NoCh++;
			}
			NoCh %= 2;
			if(Line < Lines)
			{
				strncpy(First, string, LineLength - NoCh);
				First[LineLength - NoCh] = 0;
				_putstr(First);
				_putstr("\n");
			}
			Line++;
			string += (LineLength - NoCh);
		}
		else if(strlen(string) > 0)
		{
			if(Line < Lines)
			{
				_putstr(string);
				_putstr("\n");
			}
			Line++;
			break;
		}
	}
	_drawline(0, 16, SCREENWIDTH*8 -1, 16, 1);
	_drawline(0, 17, SCREENWIDTH*8 -1, 17, 1);
	_drawline(0, (SCREENHEIGHT + 1)*16 -1, SCREENWIDTH*8 -1, (SCREENHEIGHT + 1)*16 -1, 1);
	_drawline(0, (SCREENHEIGHT + 2)*16 -1, SCREENWIDTH*8 -1, (SCREENHEIGHT + 2)*16 -1, 1);
	return Line;
}

/***********************************************************************************************
*功能：以弹出框的形式显示字符串，根据需要可以接收数据输入
* 参数：str->字符串
*		DataBuff->存放输入数据的缓冲区
*		Length->可输入数据的最大长度。Length 为0表示只显示不接收键盘的输入
*       Flag->对输入数据的现实形式。1->以密码方式显示;  2->表示以普通方式显示
* 返回值：
************************************************************************************************/
short int MessageBox(char* str, char* DataBuff, unsigned char Length, unsigned char Flag)
{
	unsigned char buff[1000] = {0};
	char tempkeybuff[20] = {0};
	unsigned char len[2] = {0};
	short int GKeyValue;
	char startx = 1;
	int length;

	length = strlen(str);
	if(Length > length)
		length = Length;

	_save_video((CENTERX - length/2)*8-1,
				(CENTERY - startx)*16-1,
				(CENTERX + length/2 + length%2)*8+5,
				(CENTERY + startx)*16+5,0,buff,0);
	_drawrect((CENTERX - length/2)*8-1,
			  (CENTERY - startx)*16-1,
			  (CENTERX + length/2 + length%2)*8,
			  (CENTERY + startx)*16,1);
	_fillbox((CENTERX - length/2)*8,
			 (CENTERY - startx)*16,
			 (CENTERX + length/2 + length%2)*8-1,
			 (CENTERY + startx)*16-1, 0);
	_fillbox((CENTERX + length/2 + length%2)*8+1,
			 (CENTERY - startx)*16+5,
			 (CENTERX + length/2 + length%2)*8+5,
			 (CENTERY + startx)*16+5, 1);
	_fillbox((CENTERX - length/2)*8+5,
			 (CENTERY + startx)*16+1,
			 (CENTERX + length/2 + length%2)*8,
			 (CENTERY + startx)*16+5, 1);
	_gotoxy(CENTERX - length/2, CENTERY -1);
	_putstr(str);
	if(!Length)
	{
		_bioskey(0);
		_bioskey(1);
		_restore_video(buff);
		return 0;
	}
	_gotoxy(CENTERX - length/2, CENTERY);
	if(Flag == 1)
		GKeyValue = key_secret(DataBuff, Length);
	if(Flag == 2)
		GKeyValue = Scanf(DataBuff, Length);
	hidecursor(0);
	_restore_video(buff);
	return GKeyValue;
}
/*字符串Msg的长度不能超过10个汉字或20个英文字符
如果带了操作信息则等待一个选择按键,返回按键值
*/
unsigned short int MessageBox1(char *Msg, char* HintMsg)
{
	unsigned char buff[1000] = {0};
	int length;
	char startx = 1;
	unsigned short int KeyValue;

	length = strlen(Msg);
	_save_video((CENTERX - length/2)*8-1,
				(CENTERY - startx)*16-1,
				(CENTERX + length/2 + length%2)*8+5,
				(CENTERY + startx)*16+5,0,buff,0);

	_drawrect((CENTERX - length/2)*8-1,
			  (CENTERY - 1)*16-1,
			  (CENTERX + length/2 + length%2)*8,
			  (CENTERY + 1)*16,1);
	_fillbox((CENTERX - length/2)*8,
			 (CENTERY - 1)*16,
			 (CENTERX + length/2 + length%2)*8-1,
			 (CENTERY + 1)*16-1, 0);
	_fillbox((CENTERX + length/2 + length%2)*8+1,
			 (CENTERY - 1)*16+5,
			 (CENTERX + length/2 + length%2)*8+5,
			 (CENTERY + 1)*16+5, 1);
	_fillbox((CENTERX - length/2)*8+5,
			 (CENTERY + 1)*16+1,
			 (CENTERX + length/2 + length%2)*8,
			 (CENTERY + 1)*16+5, 1);
	_gotoxy(CENTERX - length/2, CENTERY - 1);
	_putstr(Msg);
	_gotoxy(CENTERX - length/2, CENTERY);
	if(HintMsg != NULL)
	{
		_setdispcolor(0);
		_putstr(HintMsg);
		_setdispcolor(1);
		while(1)
		{
			KeyValue = _bioskey(0);
			if(KeyValue == _ENT || KeyValue == _ESC)
				break;
		}
	}
	_restore_video(buff);
	return KeyValue;
}

short int MessageBox2(unsigned char* DataBuff)
{
	unsigned char buff[2000] = {0};
	char startx;
	int length;
	short int KeyValue;


	startx = 1;
	length = strlen(DataBuff);

	_save_video((CENTERX - length/2)*8-1,
				(CENTERY - startx)*16-1,
				(CENTERX + length/2 + length%2)*8+5,
				(CENTERY + startx)*16+5,0,buff,0);
	_drawrect((CENTERX - length/2)*8-1,
			  (CENTERY - startx)*16-1,
			  (CENTERX + length/2 + length%2)*8,
			  (CENTERY + startx)*16,1);
	_fillbox((CENTERX - length/2)*8,
			 (CENTERY - startx)*16,
			 (CENTERX + length/2 + length%2)*8-1,
			 (CENTERY + startx)*16-1, 0);
	_fillbox((CENTERX + length/2 + length%2)*8+1,
			 (CENTERY - startx)*16+5,
			 (CENTERX + length/2 + length%2)*8+5,
			 (CENTERY + startx)*16+5, 1);
	_fillbox((CENTERX - length/2)*8+5,
			 (CENTERY + startx)*16+1,
			 (CENTERX + length/2 + length%2)*8,
			 (CENTERY + startx)*16+5, 1);
	_gotoxy(CENTERX - length/2, CENTERY-1);
	_putstr(DataBuff);
	KeyValue = _bioskey(0);
	_restore_video(buff);
	return KeyValue;
}
/***********************************************************************************************
*功能：接收指定长度的数据输入
* 参数：str->存放缓冲区
*		length->可输入数据的最大长度。
* 返回值：最后按键值（为[退出]或[确认]的键值）
************************************************************************************************/
short int Scanf(char * str, unsigned char length)
{
	short int ks;
	unsigned char chr,i = 0;
	unsigned char x,y;

	x = _getx();
	y = _gety();
	hidecursor(1);
	while(1)
	{
		ks = _bioskey(0);
		if(ks == _DEL)
		{
			if(i == 0)
				continue;
			*(str + i - 1) = 0;
			if(_getx() != x )
			{
				_gotoxy(_getx()-1, _gety());
				_putch(' ');
				_gotoxy(_getx()-1, _gety());
			}
			else if (_getx() == 0 && _gety() != 1)
			{
				_gotoxy(SCREENWIDTH-1, _gety() -1);
				_putch(' ');
				_gotoxy(SCREENWIDTH-1, _gety() -1);
			}
			else
			{

			}
			i--;
		}
		else if(ks == _ENT || ks == _ESC)
		{
			*(str + i) = 0;
			if(ks == _ESC)
			   	memset(str, 0, length);
			hidecursor(0);
			return (ks);
		}
		if(i>=length)
			continue;
		chr = ks & 0xff;
		if((chr>= '0' && chr <= '9') ||
		   (chr>='a' && chr <= 'z') ||
		   (chr >= 'A' && chr <= 'Z') ||
		   (chr =='.')|| (chr =='+') ||(chr =='-')||
		   (chr ==' ')||(chr =='*')||(chr =='/')||(chr =='(')||(chr ==')'))
		{
//			if(!(i%(SCREENWIDTH)) && i != 0)
//				_putstr("\n");
			*(str + i) = chr;
			_putch(chr);
			i++;
		}
	}
}
/****************************************************************************************
 ** ReverseStr()
 *
 *  PARAMETERS:
 *	   sstr: 要反转的字符串的地址
 *  DESCRIPTION:
 *	    将传入的指针所指向的字符串全部反转排列
 *  RETURNS:
 *		返回传入字符串的首地址
****************************************************************************************/

char* ReverseStr(char* sstr)
{
	char len,i,j;
	char* dstr;

	len = strlen(sstr);

	dstr = (char*)malloc(255);
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
	free(dstr);
	return sstr;
}

/****************************************************************************************/
/*
 ** itoa()
 *
 *  PARAMETERS:
 *	   value: 2个字节的整数
 *     string: 目标字符串
 *	   radix: 目标字符串的基数
 *  DESCRIPTION:
 *	   将2个字节的整数转换成以radix指定的基数的数值字符串
 *  RETURNS:
 *	   返回目标字符串的首地址
 */
char* itoa(unsigned int value,char* string, int radix)
{
	char tempchar,tempstr[32] = {0};
	char loop, i;
	int temp;

	if(radix<2 || radix>36)
	   return string;
	temp = value;
	i=0;
	while(1)
	{
		if(!temp)
		{
			if(!i || ((i%2) && radix != 10))
			{
				tempstr[i] = 0x30;
				i++;
			}
			break;
		}
		tempchar = temp%radix;
		temp = (temp - tempchar)/radix;
		if((tempchar> 9 && tempchar<16) && radix == 16)
			tempstr[i] = tempchar + 0x41 - 0x0a;
		else if(tempchar>= 0 && tempchar<10)
			tempstr[i] = tempchar + 0x30;
		i++;
	}
	tempstr[i] = '\0';
	i--;
	for(loop=0 ;loop < strlen(tempstr); loop++)
	{
		*(string + i) = tempstr[loop];
		i--;
	}
	*(string + strlen(tempstr)) = '\0';
	return string;
}

void strupr(unsigned char *str)
{
    int i = 0;

	while(1)
    {
        if(*(str+i) == 0)
			break;
        if((*(str+i)>=97)&&(*(str+i)<=122))
        {
           *(str+i)-=32;
        }
    }
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
/*****************************************************************yy add
根据Separator来分割字符串String,如果没有Separator,就按照' '来分割,
如果没有' ',就按照'\0'来分割.flag标志用来指定分割后的字符串存如哪
个位置.
	flag为0		存入PartData
	flag为1		存入PartFormat
	flag为2		存入PartDName
	flag为3		存入PartUnit
*****************************************************************/
unsigned int SeparateSTR(char* String, char Flag, char Separator)
{
	int i, k, result, resultSpc;
	char* pdest;
	char* pdestSpc;
	char* Buff[MaxNum];
	char cpTmpBuff[256];

    i=0;
    while(1)
	{
		pdest = strchr(String, Separator);
		result = pdest - String;
		if(pdest != NULL)
		{
			Buff[i] = (char*)malloc(result + 1);
			if(Buff[i] == NULL)
				return 0xFFFF;
			memset(Buff[i], 0, result+1);
			strncpy(Buff[i], String, result);
			Buff[i][result]='\0';
			String += (result + 1);
			i++;
		}
		else
		{
			pdestSpc = strchr(String, ' ');
			if(pdestSpc != NULL)
			{
				result = strlen(String);
				for(k = result; k>=0;)
				{
					k--;
					if(String[k] != ' ')
					{
						resultSpc = k+1;
						break;
					}
				}
				if(k<0)
					break;
				Buff[i] = (char*)malloc(resultSpc + 1);
				if(Buff[i] == NULL)
					return 0xFFFF;
				memset(Buff[i], 0, resultSpc+1);
				strncpy(Buff[i], String, resultSpc);
				Buff[i][resultSpc]='\0';
				i++;
			}
			else
			{
				pdestSpc = strchr(String, '\0' );
				resultSpc = pdestSpc - String;
				if(pdestSpc != NULL)
				{
					Buff[i] = (char*)malloc(resultSpc + 1);
					if(Buff[i] == NULL)
						return 0xFFFF;
					memset(Buff[i], 0, resultSpc+1);
					strncpy(Buff[i], String, resultSpc);
					i++;
				}
			}
			break;
		}
    }
    switch(Flag)
	{
		case 0:
			for(k=0; k<i; k++)
				PartData[k] = Buff[k];
			break;
		case 1:
			for(k=0; k<i; k++)
				PartFormat_Bak[k] = PartFormat[k] = Buff[k];
			break;
		case 2:
			for(k=0; k<i; k++)
				PartDName_Bak[k] = PartDName[k] = Buff[k];
			break;
		case 3:
			for(k=0; k<i; k++)
			{
				PartUnit_Bak[k] = PartUnit[k] = Buff[k];
			}
			break;
	}
	return i;
}
/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<    		 工具函数定义结束			 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/

/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<    		 通讯函数定义开始			 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/
/*********************************************************************************************
*用红外口发送
*********************************************************************************************/
void SendFrame(char* sframe, unsigned short int framelen)
{
	unsigned char Buff[800] = {0};

	strcpy(Buff, sframe);
	StrtoHex(Buff, framelen, 0, 0, 0);

	if( TelType == 1 )//偶校验
	{
		_com_init(OPENCOM,Botelv,TRANSMODEVEN);
	}
	else if( TelType == 0 )
	{
		_com_init(OPENCOM,Botelv,TRANSMODE);
	}
	_setcomport(TeleCom);
	_delay(100);
    _flushcom();
	_com_send_buf(Buff, &framelen);
}


short int RecvFrame()
{
	unsigned char rchar = 0;
	unsigned long j;
	unsigned short int ll = 200;
	unsigned short int time = 380;
	unsigned short int rlen;
	unsigned char rframe[800] = {0};
	//unsigned char rframe[800] = {0x68,0x4e,0x00,0x4e,0x00,0x68,0x88,0x31,0x07,0x01,0x00,0x02,0x0a,0xe8,0x00,0x00,0x20,0x0a,0x00,0x00,0x20,0x50,0x10,0x07,0x00,0x66,0x16};
	//unsigned char rframe[800] = {0x68,0xA9,0x00,0xA9,0x00,0x68,0x88,0x31,0x07,0x01,0x00,0x02,0x09,0xE3,0x00,0x00,0x01,0x01,0xFF,0x02,0xFF,0x02,0xFF,0x02,0xFF,0x02,0xFF,0x02,0xFF,0x02,0xFF,0x02,0xFF,0x02,0xFF,0x02,0xFF,0x02,0xFF,0x02,0xFF,0x02,0x03,0x55,0x56,0x14,0x09,0x00,0x88,0x16};
	//unsigned char rframe[800] = {0x68,0x4C,0x00,0x4C,0x00,0x68,0x0A,0x31,0x07,0x01,0x00,0x02,0x09,0xE3,0x00,0x00,0x01,0x0B,0x2,0x03,0x55,0x56,0x14,0x09,0x00,0x0A,0x16};
   //unsigned char rframe[800] = {0x68,0x2a,0x01,0x2a,0x01,0x68,0x88,0x31,0x07,0x01,0x00,0x02,0x0a,0xe8,0x00,0x00,0x02,0x01,0x02,0x00,0x01,0x00,0x01,0x00,0x62,0x1e,0x56,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x02,0x00,0x02,0x00,0x43,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x48,0x45,0x10,0x22,0x00,0xcf,0x16};
  // unsigned char rframe[800] =  {0x68,0x58,0x00,0x58,0x00,0x68,0x88,0x31,0x07,0x01,0x00,0x02,0x0a,0xe8,0x00,0x00,0x10,0x0A,0x07,0x31,0xFF,0xFF,0x55,0x56,0x14,0x09,0x00,0x0A,0xD7,0x16};
		char TmpStr[5] = {0};
    	rchar = 0;
    //_flushcom();
	for(j = 0; j <3000000; j++)
	{

	 if(!(_com_read(&rchar)))
		{
   	 	 if(rchar == 0x68)
			 {
				_com_read_buf(&rframe[1],&rlen,400,time);
				rframe[0] = rchar;
				break;
			 }
		}
		if(j>2999998) //超时处理 yy add
		{

			_setcomport(0);
		  _com_init(0,0,0);
			return(-1);
		}
	}/**/

	_setcomport(0);
  _com_init(0,0,0);

	if(rlen<14)
		return(-1);


	return(DecodeFrame(rframe));
}

/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<    		 通讯函数定义结束			 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/

/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<    		 数据库文件操作开始			 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/
/******************************************************************************************
*    功能：查找有指定的字段的记录.
*    参数：FieldName 字段名
*		   Value 匹配的值
*    返回：找到相应的记录返回1，否则返回-1。
******************************************************************************************/
int SearchField(const char* FieldName, const char* Value, int WIndex)
{
	int i, RecCount;
	char TmpField[255];

	RecCount = GetCount(WIndex);
	for(i = 0; i < RecCount; i++)
	{
		memset(TmpField, 0, 255);
		if(!GoRecord(i, WIndex) || !ReadField(FieldName, TmpField, WIndex))
			return -1;
		if(!strcmp(Value, TmpField))
			return i;
	}
	return -1;
}

void BuffRecon(char* Buff, char Length)
{
	char c_TmpBuff[255] = {0};
	char i;

	memcpy(c_TmpBuff, Buff, Length);
	memset(Buff, 0, Length);
	for(i=0;i<Length;i++)
		Buff[i] = c_TmpBuff[Length - i - 1];
}

/******************************************************************************************
*    功能：打开指定的文件并保存相应的文件信息
*    参数：FileName  要打开的文件名
*    返回：操作成功返回1，否则返回0。
******************************************************************************************/
int OpenDbf(char* FileName)
{
	FILE* p_TmpFlie;
	int Index, i;
	TDBHEAD* p_TmpHead = NULL;
	TDBFIELD* p_TmpField = NULL;
	TSField* p_TmpSField[16];
	char TmpFileBuff[40];
	char* Path = "\\长沙威胜\\";

char* DBFFile[] = {
	"Items.db",
	"Proto.db",
	"selec.db",
	"save.db",
};

	for(Index = 0; Index < DBFMAX; Index++)
	{
		if(!strcmp(DBFFile[Index], FileName))
			break;
	}
	memset(TmpFileBuff, 0, 40);
	memset(&m_file[Index], 0, sizeof(TDBF));
	strcpy(TmpFileBuff, Path);
	strcat(TmpFileBuff, DBFFile[Index]);
	p_TmpFlie = _fopen(TmpFileBuff,"r+");

	if(p_TmpFlie != NULL)
	{
		m_file[Index].p_file = p_TmpFlie;
		p_TmpHead = (TDBHEAD*)malloc(sizeof(TDBHEAD));
		if(p_TmpHead == NULL)
			return 0;
		_fread((unsigned char*)p_TmpHead, 32, p_TmpFlie);
		m_file[Index].m_headsize = p_TmpHead->Head_Size;
		//BuffRecon((char*)&m_file[Index].m_headsize, 2);
		m_file[Index].m_RecordSize = p_TmpHead->Record_Size;
		//BuffRecon((char*)&m_file[Index].m_RecordSize, 2);
		m_file[Index].m_RecordNum =	p_TmpHead->Record_Num;
		//BuffRecon((char*)&m_file[Index].m_RecordNum, 4);
		m_file[Index].m_CurrRecord = 0;
		m_file[Index].m_FieldNum = (m_file[Index].m_headsize - 32)/32;
		free(p_TmpHead);
		p_TmpField = (TDBFIELD*)malloc(sizeof(TDBFIELD));
		if(p_TmpField == NULL)
			return 0;

		for(i = 0; i < m_file[Index].m_FieldNum; i++)
		{
			memset((unsigned char*)p_TmpField, 0, 32);
			_fread((unsigned char*)p_TmpField, 32, p_TmpFlie);
			memcpy(m_file[Index].p_FiledInfo[i].FieldName, p_TmpField->Field_Name, 11);

			m_file[Index].p_FiledInfo[i].FieldOffset = p_TmpField->Field_Place;
			//BuffRecon((char*)&m_file[Index].p_FiledInfo[i].FieldOffset, 4);
			m_file[Index].p_FiledInfo[i].FieldLen = p_TmpField->Field_Lenth;
			//BuffRecon((char*)&m_file[Index].p_FiledInfo[i].FieldLen, 2);
		}
		free(p_TmpField);

	}
	else
	{
		_cls();
		MessageBox1("打开文件失败", NULL);

		_bioskey(0);
		return 0;
	}
	return 1;
}
/******************************************************************************************
*    功能：获得指定文件序号的记录数
*    参数：WIndex  指定的文件序号
*    返回：返回记录数。
******************************************************************************************/
unsigned long GetCount(int WIndex)
{
	return (m_file[WIndex].m_RecordNum);
}
/******************************************************************************************
*    功能：在指定文件序号的文件中定位到指定记录序号的记录上
*    参数：WIndex  指定的文件序号，Index 指定的记录序号
*    返回：操作成功返回1，否则返回0。
******************************************************************************************/
int GoRecord(unsigned long Index, int WIndex)
{
	unsigned long l_TmpOffset;

	l_TmpOffset = (unsigned long)(m_file[WIndex].m_headsize + (Index * m_file[WIndex].m_RecordSize));
	if(_fseek(m_file[WIndex].p_file, l_TmpOffset, SEEK_SET))
		return 0;
	m_file[WIndex].m_CurrRecord = Index;
	return 1;
}
/******************************************************************************************
*    功能：打开指定的文件并保存相应的文件信息
*    参数：FieldName  字段名，WIndex 文件序号
*    返回：操作成功返回字段序号（从0开始），否则返回0xffff。
******************************************************************************************/
int GetFieldIndex(const char* FieldName, int WIndex)
{
	int i_TmpIndex;

	for(i_TmpIndex = 0; i_TmpIndex < m_file[WIndex].m_FieldNum; i_TmpIndex++)
	{
		if(!strcmp(FieldName, m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldName))
			return i_TmpIndex;
	}
	_cls();
	MessageBox1("字段不匹配", NULL);

	_bioskey(0);
	return 0xFFFF;
}

int ReadField(const char* FieldName, char* Buff, int WIndex)
{
	int i_TmpIndex, i, k;
	char* cp_Buff;
	char* cp_Buff1;

	i_TmpIndex = GetFieldIndex(FieldName,WIndex);
	if(i_TmpIndex == 0xFFFF)
		return 0;

	cp_Buff = (char*)malloc(m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldLen+1);
	if(cp_Buff == NULL)
		return 0;
	memset(cp_Buff, 0, m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldLen+1);
	_fseek(m_file[WIndex].p_file, m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldOffset, SEEK_CUR);
	_fread((unsigned char*)cp_Buff, m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldLen, m_file[WIndex].p_file);
	_fseek(m_file[WIndex].p_file, -(m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldLen+m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldOffset), SEEK_CUR);
	k = strlen(cp_Buff);
	//清除前边的空格
	for(i=0;i<k;i++)
	{
		if((cp_Buff[i] != 0) && (cp_Buff[i] != ' '))
			break;
	}
	//清除后边的空格
	if(i<k)
	{
	    cp_Buff1 = &cp_Buff[i];
		k = strlen(cp_Buff1);
	    while(k > 0)
	    {
		    k--;
		    if(cp_Buff1[k] != 0 && cp_Buff1[k] != ' ')
		        break;
			else
		        cp_Buff1[k] = 0;
	    }
		if(strlen(cp_Buff1)!=0)
			memcpy(Buff, cp_Buff1, k+1);
	}
	//空字符串处理
	free(cp_Buff);
	return 1;
}
int WriteField(const char* FieldName, char Buff[], int WIndex)
{
	int i_TmpIndex;
  U32 realsize = 0;
	i_TmpIndex = GetFieldIndex(FieldName,WIndex);

	_fseek(m_file[WIndex].p_file, m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldOffset, SEEK_CUR);
	realsize = _fwrite(Buff, m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldLen, m_file[WIndex].p_file);
	_fseek(m_file[WIndex].p_file, -(m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldLen+m_file[WIndex].p_FiledInfo[i_TmpIndex].FieldOffset), SEEK_CUR);
	return 1;
}

/******************************************************************************************
*    功能：关闭本次登陆打开的所有文件，释放在打开文件时开辟的空间
*    参数：Path 为了兼容以前的操作
*    返回：无
******************************************************************************************/
void CloseDbf(char* FileName)
{
	int i_TmpDBFIndex = 0;

char* DBFFile[] = {
	"Items.db",
	"Proto.db",
	"selec.db",
		"save.db",
};
	for(i_TmpDBFIndex = 0; i_TmpDBFIndex < DBFMAX; i_TmpDBFIndex++)
	{
		if(!strcmp(DBFFile[i_TmpDBFIndex], FileName))
			break;
	}
	_fclose(m_file[i_TmpDBFIndex].p_file);
}
/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<    		 数据库文件操作结束			 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/

/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<     		    菜单操作开始			 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/
/****************************************************************************************
**  初始化指定的菜单的状态
*	参数：FManul 菜单信息
*		  TTitle 菜单标题
*		  Xpos 菜单缩进
*		  CoutItems 菜单项数
****************************************************************************************/
void InitManul(TManulStatus* ScrStatus,char** FManul, char ItemNum,
			   char CurrItem, char CurrLine)
{
	ScrStatus->Items = ItemNum;
	ScrStatus->CurrIndex = CurrItem;
	ScrStatus->CurrLineNo = CurrLine;
	ScrStatus->ScreenTop.CurrIndex = 0;
	ScrStatus->ScreenTop.DispLines = 0;
	ScrStatus->ScreenTop.Lines = CopyManulItems(ScrStatus, FManul[0], 1);
	memset(ScrStatus->ScreenTop.Line[0], 0, 63);
	ScrStatus->ScreenBottum.CurrIndex = 0xff;
}
/****************************************************************************************
**  功能：将菜单中不能显示的行的信息拷贝到顶部状态信息或底部状态信息中
*	参数：Item 菜单信息
*         Flag 控制拷贝到底部或顶部的状态信息中的标志。0->底部，1->顶部
*	返回：占用行数
****************************************************************************************/
unsigned char CopyManulItems(TManulStatus *ScrStatus, char* Item, unsigned char Flag)
{
	unsigned char Num, i, NoCh, Search, Line = 0;
	unsigned char LineLength;
	TManulHT* TOB;

	if(Flag)
		TOB = &(ScrStatus->ScreenTop);
	else
		TOB = &(ScrStatus->ScreenBottum);
	while (*(Item + strlen(Item) - 1) == ' ')
		*(Item + strlen(Item) - 1) = 0;
	LineLength = ((SCREENWIDTH - XPosition)/2)*2;
	for(i = 0 ,Line = 0; i < ImLines; i++ ,Line++)
	{
		if(strlen(Item) > LineLength)
		{
			NoCh = 0;
			for(Search = 0; Search < LineLength; Search++)
			{
				Num = (unsigned char)*(Item + Search);
				if(Num < 128)
					NoCh++;
				else
					Search++;
			}
			NoCh %= 2;
			strncpy(TOB->Line[i], Item, LineLength - NoCh);
			TOB->Line[i][LineLength - NoCh] = 0;
			Item += LineLength - NoCh;
		}
		else if(strlen(Item) > 0)
		{
			strcpy(TOB->Line[i], Item);
			Line++;
			break;
		}
		else
			break;
	}
	return Line;
}
/****************************************************************************************
**  计算菜单所有信息占用屏幕的行数
*	参数：Manul 菜单信息
*         Items 菜单项数
*	返回：占用行数
****************************************************************************************/
unsigned char CalcuLinesManul(char** Manul, short int Items)
{
	unsigned char Lines = 0;
	int i;
	for(i = 0; i < Items; i++)
		Lines += CalculItemLines(Manul[i]);
	return Lines;
}

/****************************************************************************************
**  功能：显示菜单信息，同时纪录屏幕底部的状态
*	参数：MString->菜单信息
*	返回：无
****************************************************************************************/
void DisplayMenu(TManulStatus *ScrStatus, char** MString, char* Title)
{
	unsigned char i = 0, Index = 0;//Index表示菜单的条目数,i表示显示行数 yy add

	hidecursor(0); //禁止光标
	_cls();
	_putstr(Title);
	if(ScrStatus->ScreenTop.DispLines != 0)
	{
	    for(i = 0 ; i < ScrStatus->ScreenTop.DispLines; i++)
	    {
	    	_gotoxy(XPosition, i + 1);
	    	_putstr(ScrStatus->ScreenTop.Line[i]);
			_putstr("\n");
	    }
	}
	while(1)
	{
		i += SepStringOrN(MString[Index], SCREENHEIGHT - i, i + 1);
		Index++;
		if((i > (SCREENHEIGHT - 1)) || Index > (ScrStatus->Items - 1))
			break;
	}
	//记录当前屏最底菜单项
	ScrStatus->ScreenBottum.Lines = CalculItemLines(MString[Index - 1]);
	ScrStatus->ScreenBottum.DispLines = 0;
	memset(ScrStatus->ScreenBottum.Line[0], 0, 63);
	//如果当前屏最底菜单项没有全部显示
	if(i > SCREENHEIGHT)
	{
		ScrStatus->ScreenBottum.Lines = CopyManulItems(ScrStatus, MString[Index], 0);
		ScrStatus->ScreenBottum.DispLines = i - SCREENHEIGHT;
	}
	ScrStatus->ScreenBottum.CurrIndex = ScrStatus->ScreenTop.CurrIndex + Index;
//	DisplayBott("退出", "确定");
	_setdispcolor(0);
	_gotoxy(1,SCREENHEIGHT + 1);
	_putstr((unsigned char*)Cance);
	_gotoxy(SCREENWIDTH - 5, SCREENHEIGHT + 1);
	_putstr((unsigned char*)Ok);
	_setdispcolor(1);
	hidecursor(0);
}
/****************************************************************************************
**  功能：反序计算一屏显示信息
*	参数：MString->菜单信息
*		  Items->显示在底部的菜单序号
*	返回：当前的显示行序号
****************************************************************************************/
unsigned char GetOmegaScr(TManulStatus *ScrStatus, char** MString, char Items)
{
	char Index, i, NoCh, Search ,Line = 0, k = 0;
	unsigned char LineLength, Lines = 0, CurrLines = 0;
	char* Item;

	Index = Items;
	while(1)
	{
		CurrLines = CalculItemLines(MString[Index]);
		Lines += CurrLines;
		if(Lines > (SCREENHEIGHT-1))
		{
			ScrStatus->ScreenTop.CurrIndex = Index;
	  	if(Lines == SCREENHEIGHT - 1)
			{
				ScrStatus->ScreenTop.Lines = CurrLines;
				ScrStatus->ScreenTop.DispLines = 0;
				memset(ScrStatus->ScreenTop.Line[0], 0, 63);
				break;
			}
		   	ScrStatus->ScreenTop.Lines = CurrLines;
			ScrStatus->ScreenTop.DispLines = Lines%SCREENHEIGHT;
			LineLength = ((SCREENWIDTH - XPosition)/2)*2;
			Item = MString[Index];
			for(i= 0; i < ImLines; i++)
			{
				if(strlen(Item) > LineLength)
				{
					NoCh = 0;
					for(Search = 0; Search < LineLength; Search++)
					{
						if((unsigned char)*(Item + Search) < 128)
							NoCh++;
					}
					NoCh %= 2;
					if(Line >= (ImLines - ScreenTop.DispLines))
					{
						strncpy(ScrStatus->ScreenTop.Line[k], Item, LineLength - NoCh);
						ScrStatus->ScreenTop.Line[k][LineLength - NoCh] = 0;
						k++;
					}
					Item += LineLength - NoCh;
					Line++;
				}
				else if(strlen(Item) > 0)
				{
					if(Line >= (ImLines - ScreenTop.DispLines))
					{
						strcpy(ScrStatus->ScreenTop.Line[k], Item);
						ScrStatus->ScreenTop.Line[k][strlen(Item)] = 0;
					}
					break;
				}
			}
			Lines = SCREENHEIGHT - CurrLines + ScrStatus->ScreenTop.DispLines;
			break;
		}
		Index--;
	}
	ScrStatus->ScreenBottum.CurrIndex = Items;
	memset(ScrStatus->ScreenBottum.Line[0], 0, 63);
	return (Lines + 1);
}

/****************************************************************************************
**  功能：显示当前的菜单信息，响应从键盘输入的对功能的选择，如果用数字键选择菜单，则两次按
*         键之间的时间间隔大约为0.5s，大于这个时间间隔则认为这次用数字键选择菜单结束
*	参数：MString->菜单信息
*	返回：当前的菜单序号或者[退出]键的键值
****************************************************************************************/
unsigned short int SelectItem(TManulStatus *ScrStatus, char** MString, char* CurrTitle)
{
	unsigned short int KeyValue;
	unsigned char Ret = 0;
	char Lines = 0;


	DisplayMenu(ScrStatus, MString + ScrStatus->ScreenTop.CurrIndex, CurrTitle);
	_setdispcolor(0);
	ScrStatus->CurrLineNo += SepStringOrN(MString[ScrStatus->CurrIndex],
							   CalculItemLines(MString[ScrStatus->CurrIndex]),
							   ScrStatus->CurrLineNo);
	_setdispcolor(1);
	while(1)
	{
		KeyValue = _bioskey(0);
		switch(KeyValue)
		{
			case _UP ://上移
			   	ScrStatus->CurrLineNo -= SepStringOrN(MString[ScrStatus->CurrIndex],
									       CalculItemLines(MString[ScrStatus->CurrIndex]),
									       ScrStatus->CurrLineNo - CalculItemLines(MString[ScrStatus->CurrIndex]));
				ScrStatus->CurrIndex--;
				if(ScrStatus->ScreenTop.CurrIndex == 0 && ScrStatus->CurrLineNo == 1)
				{//到菜单的顶部，跳转到此菜单的最后一屏
					Lines = CalcuLinesManul(MString, ScrStatus->Items);
					if( Lines > SCREENHEIGHT)
					{
						ScrStatus->CurrLineNo = GetOmegaScr(ScrStatus, MString, ScrStatus->Items - 1);
						ScrStatus->CurrIndex = ScrStatus->ScreenBottum.CurrIndex;
						DisplayMenu(ScrStatus, MString + ScrStatus->ScreenTop.CurrIndex, CurrTitle);
					}
					else
						ScrStatus->CurrLineNo = Lines - CalculItemLines(MString[ScrStatus->Items - 1]) + 1;
					ScrStatus->CurrIndex = ScrStatus->Items - 1;
					break;
				}
				else if( ScrStatus->CurrLineNo == 1)
				{//移出屏幕显示的前一项
					if(ScrStatus->ScreenTop.DispLines != 0)
						ScrStatus->CurrIndex = ScrStatus->ScreenTop.CurrIndex;
					ScrStatus->ScreenTop.DispLines = 0;
					memset(ScrStatus->ScreenTop.Line[0], 0, 63);
					DisplayMenu(ScrStatus, MString + ScrStatus->CurrIndex, CurrTitle);
					ScrStatus->ScreenTop.CurrIndex = ScrStatus->CurrIndex;
				}
				else
					ScrStatus->CurrLineNo -= CalculItemLines(MString[ScrStatus->CurrIndex]);
				break;
			case _DWN :
				SepStringOrN(MString[ScrStatus->CurrIndex],
							CalculItemLines(MString[ScrStatus->CurrIndex]),
							ScrStatus->CurrLineNo - CalculItemLines(MString[ScrStatus->CurrIndex]));
				ScrStatus->CurrIndex++;
				if(ScrStatus->CurrIndex > ScrStatus->Items - 1)
				{//跳转到本菜单的最前项
					ScrStatus->CurrIndex = 0;
					ScrStatus->CurrLineNo = 1;
					ScrStatus->ScreenTop.CurrIndex = 0;
					ScrStatus->ScreenTop.DispLines = 0;
					ScrStatus->ScreenTop.Lines = CopyManulItems(ScrStatus, MString[0], ScrStatus->Items - 1);
					ScrStatus->ScreenBottum.CurrIndex = 0xff;
					DisplayMenu(ScrStatus, MString, CurrTitle);
					break;
				}
					if(ScrStatus->CurrLineNo > SCREENHEIGHT)//(SCREENHEIGHT + 1))
				{//移出屏幕显示的下一项
					if(ScrStatus->ScreenBottum.DispLines != 0)
						ScrStatus->CurrIndex = ScrStatus->ScreenBottum.CurrIndex;
					ScrStatus->CurrLineNo = GetOmegaScr(ScrStatus, MString, ScrStatus->CurrIndex);
					DisplayMenu(ScrStatus, MString +ScrStatus-> ScreenTop.CurrIndex, CurrTitle);
				}
				break;
			case _LFT :
				ScrStatus->CurrLineNo -= CalculItemLines(MString[ScrStatus->CurrIndex]);
				return (KeyValue);
			case _RGT :
				ScrStatus->CurrLineNo -= CalculItemLines(MString[ScrStatus->CurrIndex]);
				return (KeyValue);
			case _ENT :
				ScrStatus->CurrLineNo -= CalculItemLines(MString[ScrStatus->CurrIndex]);
				return (ScrStatus->CurrIndex + 1);
			case _ESC :
				return (KeyValue);
			default:
				continue;
		}
		_setdispcolor(0);
		ScrStatus->CurrLineNo += SepStringOrN(MString[ScrStatus->CurrIndex],
					 CalculItemLines(MString[ScrStatus->CurrIndex]),
					 ScrStatus->CurrLineNo);
		_setdispcolor(1);
	}
}

/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<     		    菜单操作结束			 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/
/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<     	   组帧处理函数定义开始      	 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/
int SetFormat(char* Format)
{
	SDFormatNum = DFormatNum = SeparateSTR(Format, 1, ',');
	if(DFormatNum == 0xFFFF)
	{
		SDFormatNum = 0;

		return 0;
	}
	return 1;
}

int SetDataName(char* DataName)
{
	SDNameNum = DNameNum = SeparateSTR(DataName, 2, ',');
	if(DNameNum == 0xFFFF)
	{
		SDNameNum = 0;

		return 0;
	}

	return 1;
}


int SetUnits(char* Units)
{
	if(!strlen(Units) || Units == NULL)
	{

		return 1;
	}
	SUnitNum = DUnitNum = SeparateSTR(Units, 3, ',');
	if(DUnitNum == 0xFFFF)
	{
			SUnitNum = DUnitNum = 0;
     SUnitNum = 0;

		return 0;
	}

	return 1;
}
int DecodeFrame(char* Frame)
{
	char FrameLength[3] = {0};
	//short int* Flength;
	char FLengthStr[5] = {0};
	char CheckNum;
	char pri[5] = {0};
	memcpy(FrameLength, Frame+1, 2);
	HextoStr(FLengthStr,FrameLength,2,1, 0);
	RFrameLength = StrToInt(FLengthStr, 16)/4;
	memcpy(ReceiBuff, Frame, RFrameLength + 8);
	CheckNum = GetCheck(Frame, 6, RFrameLength);

	if(CheckNum != (*(Frame+6+RFrameLength)))
		return (-1);
	if(Frame[12] == 0x0a || Frame[12] == 0x0c)
		RFrameLength -= 18;
	if(Frame[12] == 0x04)
		RFrameLength -= 20;
	if(Frame[6]&0x20)
		RFrameLength -= 2;
	return 1;
}
/************************************************************yy add
计算校验和函数.
计算str字符串从start开始的lenth长度字符校验和
*************************************************************/
unsigned char GetCheck(unsigned char* str,int start,int lenth)
{
	unsigned char sum=0;
	int i;

	for(i=start;i<start+lenth;i++)
		sum += str[i];
	return sum;
}
//补足数据的信息
//参数 DataIdentify -〉当前的数据标识
//参数 Modules-〉当前要补足的块数
//操作成功返回1，否则返回0
int FullDataInfo(char* DataIdentify, int Modules, int StartIndex)
{
	int FullIndex;
	int ModuleIndex;
	int i = 0;

	ModuNum = Modules;
	if(!strcmp(DataIdentify+2, "010104") || !strcmp(DataIdentify+2, "010204")
		|| !strcmp(DataIdentify+2, "010404") || !strcmp(DataIdentify+2, "010804"))
	{
		for(FullIndex = 3; FullIndex >= 0; FullIndex--)
		{
			PartFormat[FullIndex*(1+Modules) + 2] = PartFormat[2*(FullIndex+1)];
			PartFormat[FullIndex*(1+Modules) + 3] = PartFormat[2*(FullIndex+1)+1];
			PartDName[FullIndex*(1+ Modules) + 2] = PartDName[2*(FullIndex+1)];
			PartDName[FullIndex*(1+ Modules) + 3] = PartDName[2*(FullIndex+1)+1];
			PartUnit[FullIndex*(1+ Modules) + 2] = PartUnit[2*(FullIndex+1)];
			PartUnit[FullIndex*(1+ Modules) + 3] = PartUnit[2*(FullIndex+1)+1];
			for(ModuleIndex = 1; ModuleIndex < Modules; ModuleIndex++)
			{
				PartFormat[FullIndex*(1+ Modules) + 3+ModuleIndex] = PartFormat[FullIndex*(1+Modules) + 3];
				PartDName[FullIndex*(1+ Modules) + 3+ModuleIndex] = PartDName[FullIndex*(1+Modules) + 3];
				PartUnit[FullIndex*(1+ Modules) + 3+ModuleIndex] = PartUnit[FullIndex*(1+Modules) + 3];
			}
		}
		DFormatNum = (Modules+1)*4 +StartIndex;
		DNameNum = (Modules+1)*4 +StartIndex;
		DUnitNum = (Modules+1)*4 +StartIndex;
	}
	else if(!strcmp(DataIdentify+2, "000201"))
	{
		for(ModuleIndex = 1; ModuleIndex < Modules; ModuleIndex++)
		{
			for(i = 1; i < 13; i++)
			{
				PartFormat[i+ModuleIndex*12] = PartFormat[i];
				PartDName[i+ModuleIndex*12] = PartDName[i];
				PartUnit[i+ModuleIndex*12] = PartUnit[i];
			}
		}
		DFormatNum = Modules*12 +StartIndex;
		DNameNum = Modules*12 +StartIndex;
		DUnitNum = Modules*12 +StartIndex;
	}
	else if(!strcmp(DataIdentify+2, "000401"))
	{
		for(ModuleIndex = 1; ModuleIndex < Modules; ModuleIndex++)
		{
			for(i = 1; i < 5; i++)
			{
				PartFormat[i+ModuleIndex*4] = PartFormat[i];
				PartDName[i+ModuleIndex*4] = PartDName[i];
				PartUnit[i+ModuleIndex*4] = PartUnit[i];
			}
		}
		DFormatNum = Modules*4 +StartIndex;
		DNameNum = Modules*4 +StartIndex;
		DUnitNum = Modules*4 +StartIndex;
	}
	else
	{
		for(ModuleIndex = 1; ModuleIndex < Modules; ModuleIndex++)
		{
			for(FullIndex = StartIndex; FullIndex < DFormatNum; FullIndex++)
				PartFormat[ModuleIndex*(DFormatNum - StartIndex) + FullIndex] = PartFormat[FullIndex];
			for(FullIndex = StartIndex; FullIndex < DNameNum; FullIndex++)
				PartDName[ModuleIndex*(DNameNum - StartIndex) + FullIndex] = PartDName[FullIndex];
			for(FullIndex = StartIndex; FullIndex < DUnitNum; FullIndex++)
				PartUnit[ModuleIndex*(DUnitNum -StartIndex) + FullIndex] = PartUnit[FullIndex];
		}
		DFormatNum = (Modules)*(DFormatNum -StartIndex) +StartIndex;
		DNameNum = (Modules)*(DNameNum -StartIndex) +StartIndex;
		DUnitNum = (Modules)*(DUnitNum -StartIndex) +StartIndex;
	}
}

void ParseReceiFrame(int StartDataIndex, int DataAreaOffset)
{
	int FrameOffset, i,j,k,tempDataIndexOffset;
	int DataIndexOffset;
	int SignalDataLength;
	unsigned char TmpData[50];
	char ConvetBuff[50];
	char tempbuf[800];
	char tempbuf1[800];
	char* pDot;
	int count = 0;
	int staTmp =0;
	TLConvert LongData;
	char WeekValue;
	FrameOffset = 18 + DataAreaOffset;
	for(DataIndexOffset = StartDataIndex; DataIndexOffset < DFormatNum; DataIndexOffset++)
	{
		if (!strcmp(PartFormat[DataIndexOffset], "m"))
    {
    	memset(TmpData, 0, 50);
      memcpy(TmpData, &ReceiBuff[FrameOffset], 1);
      memset(tempbuf, 0, 20);
      if(TmpData[0] == 0x02)
      {
      	sprintf(tempbuf, "GPRS/CDMA");
      } else  if (TmpData[0] == 0x04)
      {
      	sprintf(tempbuf, "以太网");

      } else  if (TmpData[0] == 0x0B)
      {
      	sprintf(tempbuf, "ETH_GPRS_SWITCH");
      }
      else
      {
      	sprintf(tempbuf, "无效");
      }
      PartData[DataIndexOffset] = (char*)malloc(strlen(tempbuf) + 1);
      sprintf(PartData[DataIndexOffset], tempbuf);
      SignalDataLength = 1;
      DataIndexOffset++;
      return;

    } else if (!strcmp(PartFormat[DataIndexOffset], "u"))
    {
    	memset(TmpData, 0, 50);
      memcpy(TmpData, &ReceiBuff[FrameOffset], 1);
      memset(tempbuf, 0, 20);
      if(TmpData[0] == 0x00)
      {
      	sprintf(tempbuf, "正常模式");
      } else  if (TmpData[0] == 0x01)
      {
      	sprintf(tempbuf, "测试模式");

      } else  if (TmpData[0] == 0x02)
      {
      	sprintf(tempbuf, "调试模式");
      }
      else
      {
      	sprintf(tempbuf, "无效");
      }
      PartData[DataIndexOffset] = (char*)malloc(strlen(tempbuf) + 1);
      sprintf(PartData[DataIndexOffset], tempbuf);
      SignalDataLength = 1;
      DataIndexOffset++;
      return;

    }else if (!strcmp(PartFormat[DataIndexOffset], "t"))
    {
    	memset(TmpData, 0, 50);
      memcpy(TmpData, &ReceiBuff[FrameOffset], 20);
      memset(tempbuf, 0, 20);
      if((TmpData[1]&0x20 ))
      {
      	sprintf(tempbuf, "记录事件 ");

      }
      else
      {
        sprintf(tempbuf, "不记录事件 ");
      }
      if(TmpData[9]&0x20 )
      	{
      		 strcat(tempbuf, "重要事件");

      	}
     else
     	{
			   strcat(tempbuf, "一般事件");

     	}
      PartData[DataIndexOffset] = (char*)malloc(strlen(tempbuf) + 1);
      sprintf(PartData[DataIndexOffset], tempbuf);
      SignalDataLength = 1;
      DataIndexOffset++;
      return;

    }
     else if (!strcmp(PartFormat[DataIndexOffset], "C"))
    {

      tempDataIndexOffset = DataIndexOffset + 1;
      memset(TmpData, 0, 50);
      memcpy(TmpData, &ReceiBuff[FrameOffset], 24);

      memset(tempbuf, 0, 250);
      sprintf(tempbuf, "搜表成功表档案序号");
      PartDName[tempDataIndexOffset] = (char*)malloc(strlen(tempbuf) + 1);
      memset(PartDName[tempDataIndexOffset], 0, strlen(tempbuf) + 1);
      strcpy(PartDName[tempDataIndexOffset], tempbuf);


      memset(tempbuf, 0, 250);
      sprintf(tempbuf, " ");
      PartUnit[tempDataIndexOffset] = (char*)malloc(strlen(tempbuf) + 1);
      memset(PartUnit[tempDataIndexOffset], 0, strlen(tempbuf) + 1);
      sprintf(PartUnit[tempDataIndexOffset], tempbuf);

      memset(tempbuf, 0, 250);
      for (j = 0; j < 24; j++)
      {
          for (k = 0; k < 8; k++)
          {
              if (((TmpData[j] >> k) & 1) == 1)
              {
                  count++;
                  sprintf(tempbuf1, "%d ", j * 8 + 1 + k);
                  strcat(tempbuf, tempbuf1);
                  if (strlen(tempbuf) + 20 > 100)
                  {
                      strcat(tempbuf, "Next");
                      PartData[tempDataIndexOffset] = (char*)malloc(strlen(tempbuf) + 1);
                      memset(PartData[tempDataIndexOffset], 0, strlen(tempbuf) + 1);
                      sprintf(PartData[tempDataIndexOffset], tempbuf);
                      DataIndexOffset++;
                      tempDataIndexOffset++;

                      memset(tempbuf, 0, 250);
                      sprintf(tempbuf, " ");
                      PartDName[tempDataIndexOffset] = (char*)malloc(strlen(tempbuf) + 1);
                      memset(PartDName[tempDataIndexOffset], 0, strlen(tempbuf) + 1);
                      strcpy(PartDName[tempDataIndexOffset], tempbuf);
                      memset(tempbuf, 0, 250);
                  }
              }
          }
      }
      strcat(tempbuf, "END");

      PartData[tempDataIndexOffset] = (char*)malloc(strlen(tempbuf) + 1);
      memset(PartData[tempDataIndexOffset], 0, strlen(tempbuf) + 1);
      sprintf(PartData[tempDataIndexOffset], tempbuf);
      DataIndexOffset++;
      tempDataIndexOffset++;

      memset(tempbuf, 0, 250);
      sprintf(tempbuf, "成功表数");
      PartDName[0] = (char*)malloc(strlen(tempbuf) + 1);
      memset(PartDName[0], 0, strlen(tempbuf) + 1);
      sprintf(PartDName[0], tempbuf);

      PartData[0] = (char*)malloc(4);
      memset(PartData[0], 0, 4);
      sprintf(PartData[0], "%d ", count);
      DataIndexOffset++;

      if (count == 0)
      {
          DataIndexOffset = 1;
      }

      DUnitNum = DNameNum = DFormatNum = DataIndexOffset;

      SignalDataLength = 24;
    }
    else if(PartFormat[DataIndexOffset][0] == 'B')
		{
			if(strlen(PartFormat[DataIndexOffset]) < 4)
				SignalDataLength = 1;
			else if(strlen(PartFormat[DataIndexOffset]) == 5)
				SignalDataLength = 2;
			else
				SignalDataLength = 6;
		}
		else if(PartFormat[DataIndexOffset][0] == 'i')
		{
			SignalDataLength = 4;
		}
		else if(PartFormat[DataIndexOffset][0] == 'I')
			SignalDataLength = 6;
		else if(!strcmp(PartFormat[DataIndexOffset], "4095") || !strcmp(PartFormat[DataIndexOffset], "15"))
			SignalDataLength = 2;
		else if(!strcmp(PartFormat[DataIndexOffset], "14") || !strcmp(PartFormat[DataIndexOffset], "31"))
			SignalDataLength = 1;
		else if(PartFormat[DataIndexOffset][0] == 'S')
		{
			if ( (PartFormat[DataIndexOffset][1]== '1' && PartFormat[DataIndexOffset][2]== '1')  ||
					 (PartFormat[DataIndexOffset][1]== '1' && PartFormat[DataIndexOffset][2]== '2')  )
			{
				FrameOffset -= 1;
			}
			else
			{
				SignalDataLength = 1;
			}
		}
		else
		{
			SignalDataLength = strlen(PartFormat[DataIndexOffset]);
			if(PartFormat[DataIndexOffset][0] != 'A')
			{
				pDot = strchr(PartFormat[DataIndexOffset],'.');
				if(pDot!=NULL)
					SignalDataLength--;
				SignalDataLength /= 2;
			}
		}
		memset(TmpData, 0, 50);
		memcpy(TmpData, &ReceiBuff[FrameOffset], SignalDataLength);
		FrameOffset += SignalDataLength;
		for(i =0; i<SignalDataLength; i++)
		{
			if(TmpData[i]!=0xee)
				break;
		}
		if(i == SignalDataLength)
		{
		 	PartData[DataIndexOffset] = (char*)malloc(7);
		 	memset(PartData[DataIndexOffset], 0, 7);
		 	strcpy(PartData[DataIndexOffset], "----");
		 	continue;
		}
		switch(PartFormat[DataIndexOffset][0])
		{
			case 'B':	//把接收到的数据按照十进制数显示
				switch(SignalDataLength)
				{
					case 1:
						PartData[DataIndexOffset] = (char*)malloc(4);
						memset(PartData[DataIndexOffset], 0, 4);
						sprintf(PartData[DataIndexOffset],"%d", TmpData[0]);
						if(!strlen(PartData[DataIndexOffset]))
							strcpy(PartData[DataIndexOffset], "0");
						break;
					case 2:
						PartData[DataIndexOffset] = (char*)malloc(6);
						memset(PartData[DataIndexOffset], 0, 6);
						memset(ConvetBuff, 0, 50);
						HextoStr(ConvetBuff, TmpData, 2, 1, 0);
						sprintf(PartData[DataIndexOffset],"%d", StrToInt(ConvetBuff, 16));
						if(!strlen(PartData[DataIndexOffset]))
							strcpy(PartData[DataIndexOffset], "0");
						break;
					default:
						PartData[DataIndexOffset] = (char*)malloc(strlen(PartFormat[DataIndexOffset]) +1);
						memset(PartData[DataIndexOffset], 0, strlen(PartFormat[DataIndexOffset]) +1);
						memset(ConvetBuff, 0, 50);
						HextoStr(ConvetBuff, TmpData, 6, 1, 0);
						BigNumConvert(ConvetBuff, 10000000, &LongData);
						memset(ConvetBuff, 0, 50);
						sprintf(ConvetBuff, "%d", LongData.Part1);
						sprintf(&ConvetBuff[strlen(ConvetBuff)], "%07d", LongData.Part2);
						sprintf(&ConvetBuff[strlen(ConvetBuff)], "%07d", LongData.Part2);
						strcpy(PartData[DataIndexOffset], ConvetBuff);
						break;
				}
				break;
			case 'i':	//IP地址显示
				PartData[DataIndexOffset] = (char*)malloc(21);
				memset(PartData[DataIndexOffset], 0, 21);
				for(i = 0;i < 4; i++)
				{
					itoa(TmpData[i], (PartData[DataIndexOffset] + strlen(PartData[DataIndexOffset])),10);
					if(i!=3)
						strcat(PartData[DataIndexOffset], ".");
				}
			break;
			case 'I':	//IP地址显示
				PartData[DataIndexOffset] = (char*)malloc(21);
				memset(PartData[DataIndexOffset], 0, 21);
				for(i = 0;i < 4; i++)
				{
					itoa(TmpData[i], (PartData[DataIndexOffset] + strlen(PartData[DataIndexOffset])),10);
					if(i!=3)
						strcat(PartData[DataIndexOffset], ".");
					else
						strcat(PartData[DataIndexOffset], " ");
				}
				memset(ConvetBuff, 0, 50);
				HextoStr(ConvetBuff, &TmpData[4], 2, 1, 0);
				sprintf((PartData[DataIndexOffset] + strlen(PartData[DataIndexOffset])), "%04u", StrToInt(ConvetBuff, 16));
				break;
			case 'A':	//ascii字符显示
				PartData[DataIndexOffset] = (char*)malloc(strlen(PartFormat[DataIndexOffset]) +1);
				memset(PartData[DataIndexOffset], 0, strlen(PartFormat[DataIndexOffset]) +1);
				if(strlen(TmpData))
					strcpy(PartData[DataIndexOffset], TmpData);
				else
					strcpy(PartData[DataIndexOffset], "----");
				break;
			case 'T':	//时间显示
				PartData[DataIndexOffset] = (char*)malloc(20);
				memset(PartData[DataIndexOffset], 0, 20);
				memset(ConvetBuff, 0, 50);
//				WeekValue = (TmpData[1]>>5) + 0x30;
//				TmpData[1] &= 0x1f;
				HextoStr(ConvetBuff, TmpData, SignalDataLength, 1, 0);
//				ConvetBuff[strlen(ConvetBuff) -1] = WeekValue;
				GetOnDateTime(ConvetBuff,PartFormat[DataIndexOffset],PartData[DataIndexOffset]);
				//strcpy(PartData[DataIndexOffset], ConvetBuff);
				break;
			case 'F':
				PartData[DataIndexOffset] = (char*)malloc(strlen(PartFormat[DataIndexOffset]) +3);
				memset(PartData[DataIndexOffset], 0, strlen(PartFormat[DataIndexOffset]) +1);
				if(TmpData[SignalDataLength - 1] & 0x80)
				{
					strcpy(PartData[DataIndexOffset], "-");
					TmpData[SignalDataLength - 1] &= 0x7F;
				}
				memset(ConvetBuff, 0, 50);
				HextoStr(ConvetBuff, TmpData, SignalDataLength, 1, 0);
				AnalysisDot(ConvetBuff, PartFormat[DataIndexOffset], 1);
				if(!strlen(PartData[DataIndexOffset]))
					strcpy(PartData[DataIndexOffset], ConvetBuff);
				else
					strcat(PartData[DataIndexOffset], ConvetBuff);
				break;
			case 'X':	//把接收到的数据原样显示，中间插入小数点
				PartData[DataIndexOffset] = (char*)malloc(strlen(PartFormat[DataIndexOffset]) +1);
				memset(PartData[DataIndexOffset], 0, strlen(PartFormat[DataIndexOffset]) +1);
				HextoStr(PartData[DataIndexOffset], TmpData, SignalDataLength, 1, 0);
				AnalysisDot(PartData[DataIndexOffset], PartFormat[DataIndexOffset], 1);
				break;
			case 'E':	//把接收到的数据原样显示，中间插入小数点
				PartData[DataIndexOffset] = (char*)malloc(strlen(PartFormat[DataIndexOffset]) +1);
				memset(PartData[DataIndexOffset], 0, strlen(PartFormat[DataIndexOffset]) +1);
				HextoStr(PartData[DataIndexOffset], TmpData, SignalDataLength, 0, 0);
				AnalysisDot(PartData[DataIndexOffset], PartFormat[DataIndexOffset], 1);
				break;
			case 'P':		//电话号码显示
				PartData[DataIndexOffset] = (char*)malloc(strlen(PartFormat[DataIndexOffset]) +1);
				memset(PartData[DataIndexOffset], 0, strlen(PartFormat[DataIndexOffset]) +1);
				HextoStr(PartData[DataIndexOffset], TmpData, SignalDataLength, 0, 0);
				for(i = 0; i<strlen(PartData[DataIndexOffset]); i++)
				{
					if(PartData[DataIndexOffset][i] == 'f' || PartData[DataIndexOffset][i] == 'F')
					{
						PartData[DataIndexOffset][i] = 0;
						break;
					}
				}
				break;
			case 'S':	//选择值
				//F8特殊处理
				if(PartFormat[DataIndexOffset][1]== '1' && PartFormat[DataIndexOffset][2]== '0')
				{
					staTmp = TmpData[0];
					TmpData[0] = (staTmp >> 7) & 0x03;
				}
				else if(PartFormat[DataIndexOffset][1]== '1' && PartFormat[DataIndexOffset][2]== '1')
				{
					TmpData[0] = (staTmp >> 4) & 0x03;
				}
				else if(PartFormat[DataIndexOffset][1]== '1' && PartFormat[DataIndexOffset][2]== '2')
				{
					TmpData[0] = staTmp & 0x03;
				}

				sprintf(ConvetBuff, "%d", TmpData[0]);
				if(!strlen(ConvetBuff))
					strcpy(ConvetBuff, "0");
				memset(TmpData, 0, 50);
				GetSelectValue(TmpData, PartFormat[DataIndexOffset], ConvetBuff);
				PartData[DataIndexOffset] = (char*)malloc(strlen(TmpData) +1);
				memset(PartData[DataIndexOffset], 0, strlen(TmpData) +1);
				strcpy(PartData[DataIndexOffset], TmpData);

				break;
			default:
				if(!strcmp(PartFormat[DataIndexOffset], "4095"))
				{
					PartData[DataIndexOffset] = (char*)malloc(5);
					memset(PartData[DataIndexOffset], 0, 5);
					memset(ConvetBuff, 0, 50);
					HextoStr(ConvetBuff, TmpData, 2, 1, 0);
					sprintf(PartData[DataIndexOffset],"%d", StrToInt(ConvetBuff, 16)&0xfff);
					if(!strlen(PartData[DataIndexOffset]))
						strcpy(PartData[DataIndexOffset], "0");
					DataIndexOffset++;
					memset(TmpData, 0, 50);
					sprintf(TmpData,"%d", StrToInt(ConvetBuff, 16)>>12);
					if(!strlen(TmpData))
						strcpy(TmpData, "0");
					PartData[DataIndexOffset] = (char*)malloc(5);
					memset(PartData[DataIndexOffset], 0, 5);
					strcpy(PartData[DataIndexOffset], TmpData);
				}
				//698新增操作
				if(!strcmp(PartFormat[DataIndexOffset], "15"))
				{
					PartData[DataIndexOffset] = (char*)malloc(3);
					memset(PartData[DataIndexOffset], 0, 3);
					sprintf(PartData[DataIndexOffset],"%d", TmpData[0]);
					if(!strlen(PartData[DataIndexOffset]))
						strcpy(PartData[DataIndexOffset], "0");
					DataIndexOffset++;
					memset(ConvetBuff, 0, 50);
					sprintf(ConvetBuff,"%d", (TmpData[1]&0x0c)>>2);
					if(!strlen(ConvetBuff))
						strcpy(ConvetBuff, "0");
					PartData[DataIndexOffset] = (char*)malloc(8);
					memset(PartData[DataIndexOffset], 0, 8);
					GetSelectValue(PartData[DataIndexOffset], PartFormat[DataIndexOffset], ConvetBuff);

					DataIndexOffset++;
					memset(ConvetBuff, 0, 50);
					sprintf(ConvetBuff,"%d", (TmpData[1]&0x03));
					if(!strlen(ConvetBuff))
						strcpy(ConvetBuff, "0");
					PartData[DataIndexOffset] = (char*)malloc(8);
					memset(PartData[DataIndexOffset], 0, 8);
					GetSelectValue(PartData[DataIndexOffset], PartFormat[DataIndexOffset], ConvetBuff);

				}
				if(!strcmp(PartFormat[DataIndexOffset], "14"))
				{
					PartData[DataIndexOffset] = (char*)malloc(3);
					memset(PartData[DataIndexOffset], 0, 3);
					sprintf(PartData[DataIndexOffset],"%d", TmpData[0]>>4);
					if(!strlen(PartData[DataIndexOffset]))
						strcpy(PartData[DataIndexOffset], "0");
					DataIndexOffset++;
					memset(ConvetBuff, 0, 50);
					sprintf(ConvetBuff,"%d", (TmpData[0]&0x0c)>>2);
					if(!strlen(ConvetBuff))
						strcpy(ConvetBuff, "0");
					PartData[DataIndexOffset] = (char*)malloc(8);
					memset(PartData[DataIndexOffset], 0, 8);
					GetSelectValue(PartData[DataIndexOffset], PartFormat[DataIndexOffset], ConvetBuff);

					DataIndexOffset++;
					memset(ConvetBuff, 0, 50);
					sprintf(ConvetBuff,"%d", (TmpData[0]&0x03));
					if(!strlen(ConvetBuff))
						strcpy(ConvetBuff, "0");
					PartData[DataIndexOffset] = (char*)malloc(8);
					memset(PartData[DataIndexOffset], 0, 8);
					GetSelectValue(PartData[DataIndexOffset], PartFormat[DataIndexOffset], ConvetBuff);
				}
				if(!strcmp(PartFormat[DataIndexOffset], "31"))
				{
					PartData[DataIndexOffset] = (char*)malloc(5);
					memset(PartData[DataIndexOffset], 0, 5);
					memset(ConvetBuff, 0, 50);
					HextoStr(ConvetBuff, TmpData, 2, 1, 0);
					sprintf(PartData[DataIndexOffset],"%d", StrToInt(ConvetBuff, 16)&0x1f);
					if(!strlen(PartData[DataIndexOffset]))
						strcpy(PartData[DataIndexOffset], "0");
					DataIndexOffset++;
					memset(TmpData, 0, 50);
					sprintf(TmpData,"%d", StrToInt(ConvetBuff, 16)>>5);
					if(!strlen(TmpData))
						strcpy(TmpData, "0");
					PartData[DataIndexOffset] = (char*)malloc(10);
					memset(PartData[DataIndexOffset], 0, 10);
					GetSelectValue(PartData[DataIndexOffset], PartFormat[DataIndexOffset], TmpData);
				}
				break;
		}
	}
}
void BuildTimeAdjustStr( char* targetstr ) //对时组帧
{
	char tmp[20] ={0};
	char tmpwm[3] ={0}; 
	S16 wm = 0;
	S16 week = 0;
	//memset(TmpBuff, 0, 20);
	FormatDT(tmp);
	DateTimeToStr(tmp);
	week = _getweek();
	week = week==0?7:week;

	strncpy(tmpwm,&tmp[IDXOFMON],LENOFMON);
	wm = (week << OFFSETOFWEEK) + StrToInt(tmpwm,DEC);
	itoa (wm,tmpwm,HEX);	
	strncpy(&tmp[IDXOFMON],tmpwm, LENOFMON);
	ReverseStr(tmp);
	strcpy(targetstr,tmp);

	return;
}

int BuildFrame(char* Addr,	char* CtrlCode, char* DIdentify, char* FunCode, unsigned short int* CurrFrameLength)
{
	int i,k;
	int TmpInt = 0;
	char ConvBuff[100];
	char ConvBuff1[10] = {0};
	char UserArea[800] = {0};
	char TmpBuff[20];
	char* TmpDataPointer;
	int fNum;
	TLConvert LongData;

	strcpy(UserArea, CtrlCode);
	strcat(UserArea, Addr);
	strcat(UserArea, FunCode);
	strcat(UserArea, "E8");
	strcat(UserArea, DIdentify);
	if(!strcmp(CtrlCode,"4A"))
	{
		for(i = 0; i<DFormatNum; i++)
		{
			TmpDataPointer = PartData[i];
			memset(ConvBuff, 0, 100);
			switch(PartFormat[i][0])
			{
				case 'u':
					strcat(ConvBuff, PartData[0]);
					break;
				case 'm':
				  strcat(ConvBuff, PartData[0]);
				  break;
			  case 'i':
				  {
				  	for(k = 0; k < 4; k++)
				  	{
				  		memset(TmpBuff, 0, 20);
				  		memcpy(TmpBuff, TmpDataPointer, 3);
							sprintf(&ConvBuff[k*2], "%02x",StrToInt(TmpBuff, 10));
							TmpDataPointer += 4;
				  	}
				  }
				  break;
				case 'I': //组合IP和端口号
					for(k = 0; k < 5; k++)
					{
						memset(TmpBuff, 0, 20);
						if(k == 4)
						{
							strcpy(TmpBuff,TmpDataPointer);
							sprintf(ConvBuff1, "%04x",StrToInt(TmpBuff, 10));
							ReverseStr(ConvBuff1);
							strcat(ConvBuff, ConvBuff1);
						}
						else
						{
							memcpy(TmpBuff, TmpDataPointer, 3);
							sprintf(&ConvBuff[k*2], "%02x",StrToInt(TmpBuff, 10));
						}
						TmpDataPointer += 4;
					}
					break;
				case 'S': //组合特殊值（通过选择得到的数据以及和这些数据有关联的数据的整合）
					sprintf(ConvBuff, "%02x", StrToInt(TmpDataPointer, 10));
					if(PartFormat[i][1]== '1' && PartFormat[i][2]== '0')//F8特殊处理
					{
						TmpInt = StrToInt(PartData[i], 10)*128 + StrToInt(PartData[i+1], 10)*16
															+StrToInt(PartData[i+2], 10);

						sprintf(ConvBuff, "%02x", TmpInt);

					}
					else if( (PartFormat[i][1]== '1' && PartFormat[i][2]== '1') ||
								(PartFormat[i][1]== '1' && PartFormat[i][2]== '2')  )
					{
						ConvBuff[0]='\0';
					}
					break;
				case 'X': //组合16进制的BCD编码的数据
				case 'T': //组合时间数据
				case 'F': //组合时间数据
					strcpy(ConvBuff, TmpDataPointer);

					AnalysisDot(ConvBuff, PartFormat[i], 0);

					//if(PartFormat[i][0] == 'T')
					ReverseStr(ConvBuff);
					break;
				case 'E': //组合16进制的BCD编码的数据不反转
					strcpy(ConvBuff, TmpDataPointer);
					AnalysisDot(ConvBuff, PartFormat[i], 0);
					break;
				case 'P': //组合电话号码
					strcpy(ConvBuff, TmpDataPointer);
					break;
				case 'A': //组合ASCII编码的数据
					AStrToHexStr(ConvBuff, TmpDataPointer);
					FormatStr(ConvBuff,strlen(PartFormat[i])*2,'0', 1);
					break;
				case 'B': //组合二进制编码的数据
					if(strlen(TmpDataPointer) == 5)
						sprintf(ConvBuff, "%04x", StrToInt(TmpDataPointer, 10));
					else if(strlen(TmpDataPointer) < 4)
						sprintf(ConvBuff, "%02x", StrToInt(TmpDataPointer, 10));
					else
					{
						BigNumConvert(TmpDataPointer, 0x1000000, &LongData);
						//sprintf(ConvBuff, "%04x", LongData.Part1);
						sprintf(&ConvBuff[strlen(ConvBuff)], "%06x", LongData.Part2);
						sprintf(&ConvBuff[strlen(ConvBuff)], "%06x", LongData.Part3);
					}
					ReverseStr(ConvBuff);
					break;
				default:
					if(!strcmp(PartFormat[i], "4095"))
					{
						TmpInt = StrToInt(TmpDataPointer, 10);
						i++;
						TmpInt = (StrToInt(PartData[i], 10)<<12) + TmpInt;
						sprintf(ConvBuff,"%04x",TmpInt);
						ReverseStr(ConvBuff);
					}
					else if(!strcmp(PartFormat[i], "14"))
					{
						TmpInt = StrToInt(TmpDataPointer, 10);
						TmpInt = TmpInt <<4;
						i++;
						TmpInt = TmpInt + (*PartData[i] - 0x30)*4 + (*PartData[++i] - 0x30);
						sprintf(ConvBuff,"%02x",TmpInt);
					}
					else if(!strcmp(PartFormat[i], "15"))
					{
						TmpInt = StrToInt(TmpDataPointer, 10);
						i++;
						TmpInt = TmpInt + ((*PartData[i] - 0x30)<<12) + ((*PartData[++i] - 0x30)<<8);
						TmpInt = (TmpInt % 0x100)*0x100 + TmpInt / 0x100;
						sprintf(ConvBuff,"%04x",TmpInt);
					}
					else if(!strcmp(PartFormat[i], "31"))
					{
						TmpInt = StrToInt(TmpDataPointer, 10);
						i++;
						TmpInt = TmpInt + ((*PartData[i] - 0x30)<<5);
						sprintf(ConvBuff,"%02x",TmpInt);
					}else if(!strcmp(PartFormat[i], "192"))
					{
						if(StrToInt(TmpDataPointer, 10) > 192)
						{
								_cls();
								_putstr("输入值超出范围");
								_bioskey(0);
								return 0;
						}
						else
						{

							TmpInt = StrToInt(TmpDataPointer, 10);
							if(i==0)//起始序号
							{
								fNum = TmpInt;
							}
							if(i == 1)//截止序号
							{
								if (fNum > TmpInt)
								{
									_cls();
									_putstr("截止序号必须大于等于起始序号");
									_bioskey(0);
									return 0;
								}
							}
							sprintf(ConvBuff,"%02x",TmpInt);
						}

					}
					else if(!strcmp(PartFormat[i], "duishi"))
					{
						BuildTimeAdjustStr(ConvBuff);
						//MessageBox2(ConvBuff);
					}
					break;
			}

			if(!strcmp(DIdentify,"00000400"))//终端通讯参数
			{
				if(i == 0)//主站IP
				{
					ParaHisCfg(PartData[i],1);
				}
				else if(i == 1 || i==2)//备用IP
				{
					ParaHisCfg(PartData[i],2);
				}
			}

			strcat(UserArea, ConvBuff);
		}
	}
	if(!strcmp(CtrlCode,"4A") && !strcmp(DIdentify,"00008000"))
	{
	   strcat(UserArea,"FFFFFF");
	}
	if(!strcmp(CtrlCode,"4A") && !strcmp(DIdentify,"00004000"))
	{
	   strcat(UserArea,"000000000000000000000000");
	}

	if(!strcmp(CtrlCode,"4B"))
	{
		if(!strcmp(DIdentify,"00000201"))//Fn10 输入读取测量点号
		{
			if(0 == DatapointInput(UserArea,2))//输入错误
			{
				return 0;
			}
		}
		else if(!strcmp(DIdentify,"00000401"))//Fn11 输入读取点号
		{
			if(0 == DatapointInput(UserArea,1))
			{
				return 0;
			}
		}
	}
	memset(TmpBuff, 0, 20);
	if(!strcmp(CtrlCode,"4A") || !strcmp(CtrlCode,"41"))
		sprintf(TmpBuff,"%04x", (strlen(UserArea)/2 +22)*4 +2);
	else
		sprintf(TmpBuff,"%04x", (strlen(UserArea)/2 +6)*4 +2);
	ReverseStr(TmpBuff);

	strcpy(SendBuff, "68");
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, "68");
	if(!strcmp(CtrlCode,"4A"))
		strcat(UserArea,"0000000000000000000000000000000000");
	else if(!strcmp(CtrlCode,"41"))
	    strcat(UserArea,"0000000000000000000000000000000000");
	else
		strcat(UserArea,"00");
	memset(TmpBuff, 0, 20);
	FormatDT(TmpBuff);
	DateTimeToStr(TmpBuff);
	memset(ConvBuff, 0, 100);
	strcpy(ConvBuff, &TmpBuff[4]);
	ReverseStr(ConvBuff);
	strcat(UserArea,ConvBuff);
	strcat(SendBuff, UserArea);
	k = strlen(UserArea)/2;
	StrtoHex(UserArea, k, 0, 0, 0);
	memset(TmpBuff, 0, 20);
	sprintf(TmpBuff, "%02x", GetCheck(UserArea, 0, k));
	strcat(SendBuff, "00");
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, "16");

	CurrFrameLength[0] = strlen(SendBuff)/2;
	return 1;
}
/******************************************************************************************
*<<<<<<<<<<<<<<<<<<<<<<<<<     	   组帧处理函数定义结束      	 >>>>>>>>>>>>>>>>>>>>>>>>>>
******************************************************************************************/

void Setcursor(U8 flag)
{
	if (flag == 0)
	{
		_lcd_setcursor(0, 1, 16, 9);
	}
	else
	{
		_lcd_setcursor(4, 1, 16, 9);
		_lcd_setcursor(2, 1, 16, 9);
	}
}
//=================================标签=========================================
void ShowLable(U8 x, U8 y, char lable[])
{
	_gotoxy(x, y);
	_putstr(lable);
}

//=================================输入框=======================================
S16 ShowTextBox(U8 x, U8 y, char text[], U8 len)
{
	S16 key;
	U8 index = strlen(text);
	char chr;

	ShowLable(x, y, text);

	_gotoxy(x + strlen(text), y);
	Setcursor(1);

	while(1)
	{
		key = _bioskey(0);
		switch(key)
		{
			case ESC:
				Setcursor(0);
				return key;
				break;
			case UP:
			case DOWN:
			case ENTER:
				Setcursor(0);
				return key;
				break;
			case BKSPACE:
				if (index == 0)
				{
					continue;
				}
				text[index-1] = 0;
				index--;
				_putch(key);
				break;
// 			case LEFT:
// 				if (index == 0)
// 				{
// 					continue;
// 				}
// 				index--;
// 				_gotoxy(x + index, y);
// 				break;
// 			case RIGHT:
// 				if (index == len)
// 				{
// 					continue;
// 				}
// 				index++;
// 				_gotoxy(x + index, y);
// 				break;
			default:
				if (index == len)
				{
					continue;
				}

				chr = key & 0xFF;
				if (chr >= '0' && chr <= 'Z')
				{
					text[index++] = chr;
					_putch(chr);
				}
				else
				{
					continue;
				}
				break;
		}
	}
}
S16 OptionDo(U8 x, U8 y, U8 **item, U8 count, U8 *selected)
{
	S16 key;
	U8 select;
	U8 color = _getcolor();
	color = (color == 1)? 0:1;
	select = *(selected);
	_setdispcolor(color);
	_gotoxy(x, y);
	_putstr(item[select]);

	while(1)
	{
		key = _bioskey(0);

		if (key == RIGHT)
		{
			select++;
			if (select >= count)
				select = 0;
			_gotoxy(x, y);
			_putstr(item[select]);
			continue;
		}
		else if (key == LEFT)
		{
			/*select -= 1;
			if (select < 0)
				select = count - 1;
			_gotoxy(x, y);
			_putstr(item[select]);*/
			continue;
		}
		else if ((key == ENTER) || (key == ESC) || (key == UP) || (key == DOWN))
		{
			color = (color == 1)? 0:1;
			_setdispcolor(color);
			_gotoxy(x, y);
			_putstr(item[select]);
			break;
		}
		else
		{
			continue;
		}
	}
	*selected = select;
	return key;
}

int SetsaveData(U8 FieldValue[][10])
{
	int i;
	short int KeyValue;

  U8 addr[5] = {0};
  U8 teAddr[6] = {0};
	U8 *checkitem[] = {"无校验\0", "偶校验\0"};
	U8 *commtpye[] = {"串  口\0", "红外口\0"};
	U8 *bauditem[] = {" 1200 \0", " 2400 \0", " 4800 \0", " 9600 \0", " 38400\0", "115200\0"};
	U8 *addrnums[] = {"十六进制\0", "  十进制\0"};
  U8 dx = 10 , dy = 1;
  U8 *FieldHead[] ={"CHECK","COMM","BAUD","CITY","TEADDR","NUMSYS"};
	U8 selpar=0, selcom=0, selbaud=0, selnum=0;//从数据库中取出值，并赋给这三个变量

  selpar = StrToInt(FieldValue[0], 10);
  selcom = StrToInt(FieldValue[1], 10);
  selbaud = StrToInt(FieldValue[2], 10);
  sprintf(addr,"%s",FieldValue[3]);
  sprintf(teAddr,"%s",FieldValue[4]);
  selnum = StrToInt(FieldValue[5], 10);
  //屏幕头部分
	_cls();
	_putstr("本地通讯参数配置");
	_drawline(0, 17, SCREENWIDTH*8 -1, 17, 1);
	//屏幕中间部分

  _gotoxy(0,2);
  _putstr("校验方式：");
  _gotoxy(10,2);_setdispcolor(0);_putstr(checkitem[selpar]);
  _gotoxy(0,3);
  _setdispcolor(1);
  _putstr("通信端口：");
  _gotoxy(10,3);_putstr(commtpye[selcom]);
  _gotoxy(0,4);
  _putstr("通信速率：");
  _gotoxy(10,4);_putstr(bauditem[selbaud]);
  _gotoxy(0,5);
  _putstr("区 县 码：");
  _gotoxy(10,5);_putstr(FieldValue[3]);
  _gotoxy(0,6);
  _setdispcolor(1);
  _putstr("地址进制：");
  _gotoxy(10,6);_putstr(addrnums[selnum]);
  _gotoxy(0,7);
  _putstr("终端地址：");
  _gotoxy(10,7);_putstr(FieldValue[4]);

	//尾部
	_drawline(0, (SCREENHEIGHT + 1)*16 -1, SCREENWIDTH*8 -1, (SCREENHEIGHT + 1)*16 -1, 1);
	_setdispcolor(0);
	_gotoxy(1,SCREENHEIGHT + 1);
	_putstr("退出");
	_gotoxy(SCREENWIDTH - 5, SCREENHEIGHT + 1);
	_putstr("保存");
	_setdispcolor(1);

  i=1;
	while(1)
	{
		  dy = 1 + i;
			switch(dy)
			{
				case 2:
					KeyValue = OptionDo(dx, dy, checkitem, 2, &selpar);
				  break;
				case 3:
					KeyValue = OptionDo(dx, dy, commtpye, 2, &selcom);
					break;
				case 4:
					KeyValue = OptionDo(dx, dy, bauditem, 6, &selbaud);
					break;
				case 5:
					KeyValue = ShowTextBox(dx, dy, addr, 4);
					break;
				case 6:
					KeyValue = OptionDo(dx, dy, addrnums, 2, &selnum);
					break;
				case 7:
					KeyValue = ShowTextBox(dx, dy, teAddr, 5);
					break;
				default:
					break;
			}

		  if(KeyValue == _UP)
		  {
			    i = ((i <= 1)? 7:i);
				  i--;
				  continue;
		  }
		  else if(KeyValue == _DWN)
		  {
			    i = ((i >= 6)? 0:i);
				  i++;
				  continue;
		  }
		  else if(KeyValue == _ENT)
		  {
		    	break;
		  }
		  else if(KeyValue == _ESC)
		  {
			    return 2;//退出返回
		  }
	}
	switch(selbaud)
	{
		case 0: Botelv = 1200; break;
		case 1: Botelv = 2400; break;
		case 2: Botelv = 4800; break;
		case 3: Botelv = 9600; break;
		case 4: Botelv = 38400; break;
		case 5: Botelv = 115200; break;
		default: break;
	}
	NumSys = selnum;
	TeleCom = selcom + 1;
	switch(selpar)
	{
		case 0: TelType = 0x01; break;
		case 1: TelType = 0x09; break;
		default: break;
	}


  //保存设置好的配置参数
  if(!OpenDbf("save.db"))
  {
  	  return 1;//异常返回
  }
  GoRecord(0, SAVE);
  sprintf(FieldValue[0],"%d",selpar);
  sprintf(FieldValue[1],"%d",selcom);
  sprintf(FieldValue[2],"%d",selbaud);
  sprintf(FieldValue[3],"%s",addr);
  sprintf(FieldValue[4],"%s",teAddr);
  sprintf(FieldValue[5],"%d",selnum);
  for(i = 0 ; i < 6 ; i++)
  {
	    WriteField(FieldHead[i], FieldValue[i], SAVE);
  }
  CloseDbf("save.db");
  /*if(NumSys == 1 )//十进制
  {
  	i = StrToInt(FieldValue[4], 10);
  	sprintf(FieldValue[4],"%x",i);
  }*/
  _gotoxy(5,8);
  _putstr("保存成功！");
  _delay(1000);

  return 0;
}
void SelectMain()
{
	U8 *FieldHead[] ={"CHECK","COMM","BAUD","CITY","TEADDR","NUMSYS"};
  U8 FieldValue[6][10];
	U8 selbaud=0;
  int retrCode = 0;
	int i = 0;
	short int KeyValue;
	_cls();
	_gotoxy(1,1);
	_putstr("进入主菜单前，请选择：");
	_gotoxy(3,4);
	_putstr("1.修改配置参数");
	_gotoxy(3,5);
	_putstr("2.默认上次设置");
	hidecursor(0);//禁止光标

    //从数据库取出保存的上一次设置的值
  if(!OpenDbf("save.db"))
  {
  	  return;
  }
  GoRecord(0, SAVE);
  for(i = 0 ; i < 6 ; i++)
  {
      memset(FieldValue[i], 0, sizeof(FieldValue[i]));
      ReadField((char const*)(FieldHead[i]), FieldValue[i], SAVE);
  }
  CloseDbf("save.db");
	while(1)
	{
		KeyValue = _bioskey(0);
		if(KeyValue == ONE)
		{
			retrCode = SetsaveData(FieldValue);//调用参数配置的界面
			if(retrCode == 1) break;
			else if(retrCode == 2)
			{
				 _cls();
				 _gotoxy(1,4);
				 _putstr("默认上次配置，进入主菜单中…");
				 _delay(1500);
			}
		}
	  if(KeyValue == TWO || retrCode == 2|| retrCode == 0)
		{
      TelType = StrToInt((char*)(FieldValue[0]), 10);
      TeleCom = StrToInt((char*)(FieldValue[1]), 10) + 1;
      selbaud = StrToInt((char*)(FieldValue[2]), 10);
      NumSys   = StrToInt((char*)(FieldValue[5]), 10);
      //当前的终端通信地址
      FormatStr(FieldValue[3], 4, '0', 0);
	    ReverseStr(FieldValue[3]);
	    if(NumSys == 1 )//十进制
  		{
  			i = StrToInt(FieldValue[4], 10);
  			sprintf(FieldValue[4],"%x",i);
  		}
      FormatStr(FieldValue[4], 4, '0', 0);
	    ReverseStr(FieldValue[4]);

      sprintf(Addr,"%s%s02",FieldValue[3],FieldValue[4]);//将获取的区县码和终端地址赋给全局变量终端地址Addr

	    switch(selbaud)
	    {
	    	 		case 0: Botelv = 1200; break;
        		case 1: Botelv = 2400; break;
        		case 2: Botelv = 4800; break;
        		case 3: Botelv = 9600; break;
        		case 4: Botelv = 38400; break;
        		case 5: Botelv = 115200; break;
	    }

      if(TelType == 0)
      {
      		 _com_init(OPENCOM,Botelv,TRANSMODE);
      }
     	else
     	{
     			 _com_init(OPENCOM,Botelv,TRANSMODEVEN);
     	}
	    _setcomport(TeleCom);

			break;
		}
	}

}
void VersionShow()
{
	_cls();
  _gotoxy(0,2);
  _putstr("  网络表调试工具");
  _gotoxy(0,4);
  _putstr("软件版本：");
  _gotoxy(0,5);
  _putstr(" TP9-NM376000-2.4");
  _gotoxy(0,6);
  _putstr(" 20150929");
  _delay(1800);
}
void main(void)
{
	int i;
	unsigned char Version[20]= {0};
	InitHeap();
	_sysinit();  //硬件初始化
	_file_sys_init();
	_setscreenmode(1);  //设置中文显示模式
	_getsysinfo(0,Version);
	if(!strncmp((char const*)Version,"TP900",5))//型号为TP900 设置按键模式为三键模式
	{
		_setkeymode(3);
	}
  VersionShow();
  SelectMain();

	//FormatStr(Addr, 4, '0', 0);//当前的终端通信地址
	//ReverseStr(Addr);
	//strcat(Addr,"FFFFFFFF03");

	if(!OpenDbf("Items.db"))
		return;
	for(i = 0; i < MaxNum; i++ )
	{
		PartData[i] = NULL;			  			//数据分割
		PartFormat[i] = NULL;		  			//数据格式分割
		PartDName[i] = NULL;			  		//数据名称分割
		PartUnit[i] = NULL;			  		    //数据单位分割

		PartFormat_Bak[i] = NULL;
		PartDName_Bak[i] = NULL;
		PartUnit_Bak[i] = NULL;
	}

	MainItems("0","主菜单", Addr);
	CloseDbf("Items.db");
	return;
}
void MainItems(char* PITEMIND, char* MaulTitle, char* Address)
{
	TManulStatus CManulStatus;
	short int Choice;
	unsigned long MoveIndex,RecordNums;
	char* Items[11];
	char SelectFlag[11] = {0};
	char OItemsIndex[11][5] = {0};
	char PItemsIndex[11];
	char DataCode[11] = {0};
	char ModuleFlag[5] = {0};
	int Index = 0;
	int i;

	RecordNums = GetCount(ITEMS);
	for(MoveIndex = 0; MoveIndex < RecordNums; MoveIndex++)
	{
		GoRecord(MoveIndex, ITEMS);
		memset(PItemsIndex, 0, 11);
		ReadField("PITEMIND", PItemsIndex, ITEMS);
		if(!strcmp(PITEMIND, PItemsIndex))
		{
			Items[Index] = malloc(40);
			memset(Items[Index], 0, 40);
			sprintf(Items[Index], "%d", Index+1);
			strcat(Items[Index], ".");
			ReadField("ITEMNAME", &Items[Index][strlen(Items[Index])], ITEMS);
			memset(PItemsIndex, 0, 11);
			ReadField("LEAFLAG", PItemsIndex, ITEMS);
			SelectFlag[Index] = PItemsIndex[0];
			ReadField("OITEMIND", OItemsIndex[Index], ITEMS);
			Index++;
		}
	}
	if(!Index)
	{
		MessageBox1("此功能当前不开放", NULL);
		_bioskey(0);
		return;
	}
	InitManul(&CManulStatus, Items, Index, 0, 1);
	while(1)
	{
		Choice = SelectItem(&CManulStatus, Items, MaulTitle);
		if(Choice == _ESC)
		{
			for(i = 0; i < Index; i++)
				free(Items[i]);
			return;
		}
		else if(SelectFlag[Choice -1] == '0')
		{
			memset(PItemsIndex, 0, 10);
			strcpy(PItemsIndex, PITEMIND);
			strcat(PItemsIndex, OItemsIndex[Choice -1]);
			MainItems(PItemsIndex, strchr(Items[Choice -1], '.') + 1, Address);
		}
		else if(SelectFlag[Choice -1] == '1')
		{
			memset(DataCode, 0, 10);
			memset(PItemsIndex, 0, 10);
			strcpy(PItemsIndex, PITEMIND);
			strcat(PItemsIndex, OItemsIndex[Choice -1]);
			Processor(PItemsIndex, Address, DataCode);
			memset(ModuleFlag, 0, 5);
			ReadField("MODELS", ModuleFlag, PROTO);
			FreeDataInfor(DataCode, ModuleFlag[0]);
			CloseDbf("Proto.db");
		}
		else if(SelectFlag[Choice -1] == '2')
			DebugTool(Choice);
		else if(SelectFlag[Choice -1] == '3')	 //中继抄表
			RepCopy(Addr, Choice);
	}
}
int InputAddr(char*AddrBuff)//AddrBuff输出为:行政区划码+终端地址+组标志地址 yy add
{
	char Buff[8];
	char Buff1[8] = {0};
	char Flag, k;
	char i;
	short int KeyValue;

	_cls();
	_putstr("请输入行政区划码");
	_gotoxy(0, 2);
	_putstr("请输入终端地址");
	_gotoxy(0, 4);
	_putstr("请输入主站地址");
	_gotoxy(0, 6);
	_putstr("请选择组地址标志\n");
	_putstr("1->单地址 2->组地址");
	for(i = 0; i < 4; i++)
	{
		_gotoxy(0,1+i*2);
		switch(i)
		{
			case 0:
				KeyValue = Scanf(Buff, 4);
				FormatStr(Buff, 4, '0', 0);
				ReverseStr(Buff);
				break;
			case 1:
				KeyValue = Scanf(Buff, 4);
				FormatStr(Buff, 4, '0', 0);
				ReverseStr(Buff);
				break;
			case 2:
				while(1)
				{
					KeyValue = Scanf(Buff, 3);
					if(KeyValue == _ESC)
						break;
					if(StrToInt(Buff, 10) > 127)
					{
						KeyValue = MessageBox1("输入值太大", "退出  重输");
						if(KeyValue == _ESC)
							return 0;
						_clsn(1+i*2, 1);
						_gotoxy(0,1+i*2);
						memset(Buff, 0, 6);
					}
					else
						break;
				}
				break;
			case 3:
				hidecursor(1);
				while(1)
				{
					KeyValue = _bioskey(0);
					if(KeyValue == TWO || KeyValue == ONE)
					{
						Flag = (KeyValue & 0xf) - 1;
						k = (StrToInt(Buff, 10))<<1;
						k += Flag;
						memset(Buff, 0, 6);
						sprintf(Buff, "%02x",k);
						break;
					}
					if(KeyValue == _ESC)
						break;
				}
				hidecursor(0);
				break;
		}
		if(KeyValue == _ESC)
			return 0;
		if(i != 2)
		{
			if(i == 1)
			{
				if(Buff[0] != 0)//字符串非空
				{
					sprintf(Buff1,"%04x",StrToInt(Buff,16));
					ReverseStr(Buff1);
					strcat(AddrBuff, Buff1);
					memset(Buff, 0, 6);
				}
				else
				{
					strcat(AddrBuff,"FFFF");
					memset(Buff,0,6);
				}
			}
			else
			{
				if(strcmp(Buff,"0000"))//字符串非空
				{
					strcat(AddrBuff + strlen(AddrBuff), Buff);
				}
				else
				{
					strcat(AddrBuff,"FFFF");
				}

			}
		}
	}
	return 1;
}
char* AnalysisDot(char* Str, char* Formate, char Flag)
{
	int pos = 0;
	int IntLength,DecLength;
	char TmpInt[100];
	char TmpBuff[43];
	char TmpDec[11];
	char* PoitPosF;
	char* PoitPosD;

	memset(TmpInt, 0, 100);
	memset(TmpDec, 0, 11);
	PoitPosF = strchr(Formate, '.');
	if(!Flag)
	{
		if(PoitPosF != NULL)
		{
			IntLength = PoitPosF - Formate;
			DecLength = strlen(Formate) - IntLength - 1;
			PoitPosD = strchr(Str, '.');
			if(PoitPosD == NULL)
			{
				strncpy(TmpInt, Str + strlen(Str)-IntLength,IntLength);
				FormatStr(TmpInt, IntLength, '0', 0);
				FormatStr(TmpInt, IntLength + DecLength, '0', 1);
			}
			else
			{
				strncpy(TmpInt, PoitPosD - IntLength, IntLength);
				FormatStr(TmpInt, IntLength, '0', 0);
				strcpy(TmpDec, PoitPosD+1);
				FormatStr(TmpDec, DecLength, '0', 1);
				strcat(TmpInt, TmpDec);
			}
		}
		else
		{
			strcpy(TmpInt, Str);
			FormatStr(TmpInt, strlen(Formate), '0', 0);
		}
	}
	else
	{
		if(PoitPosF != NULL)
		{
			IntLength = PoitPosF - Formate;
			DecLength = strlen(Formate) - IntLength - 1;
			memcpy(TmpInt, Str, IntLength);
			memcpy(TmpDec, &Str[IntLength], DecLength);
			strcat(TmpInt, ".");
			strcat(TmpInt, TmpDec);
			memset(TmpBuff, 0, 43);

			for(pos = 0; pos < IntLength; pos++)
			{
				if(TmpInt[pos] != '0' || pos == (IntLength - 1))
					break;
			}
			strcpy(TmpBuff, &TmpInt[pos]);
			memset(TmpInt, 0, 100);
			pos = strlen(TmpBuff) -1;
			for(; DecLength>2&&pos>0; DecLength--)
			{

				if(TmpBuff[pos] == '0')
				{
					TmpBuff[pos] = 0;
					pos--;
				}
				else
					break;
			}
			strcpy(TmpInt, TmpBuff);
		}
		else
		{
			strcpy(TmpInt, Str);
			FormatStr(TmpInt, strlen(Formate), '0', 0);
		}
	}
	memset(Str, 0, strlen(Str));
	strcpy(Str, TmpInt);
	return Str;
}


void DisplayData(void)
{
	int TopIndex[MaxNum] = {0};
	int TopDataNum[MaxNum] = {0};
	int ItemIndex, ScreenIndex,CreenNums;
	unsigned char CurrScreenLines;
	short int KeyValue;

	CreenNums = ScreenIndex = 0;
	while(1)
	{
		CurrScreenLines = 0;
	 	if(ScreenIndex == CreenNums && TopIndex[ScreenIndex] != 0xff)
		{
			TopDataNum[ScreenIndex] = 0;
			for(ItemIndex = TopIndex[ScreenIndex]; ItemIndex < DNameNum; ItemIndex++)
			{
		 		CurrScreenLines += CalculItemLines(PartDName[ItemIndex]);
		 		CurrScreenLines += CalculItemLines(PartData[ItemIndex]);
		 		//CurrScreenLines += CalculItemLines(PartUnit[ItemIndex]);
				if(CurrScreenLines <= (SCREENHEIGHT + 2))
					TopDataNum[ScreenIndex]++;
				if(CurrScreenLines == (SCREENHEIGHT + 2))
				{
					CreenNums++;
					TopIndex[CreenNums] = ItemIndex+1;
					break;
				}
				else if(CurrScreenLines > (SCREENHEIGHT + 2))
				{
					CreenNums++;
					TopIndex[CreenNums] = ItemIndex;
					break;
				}
				else if(ItemIndex == DNameNum-1)
				{
					CreenNums++;
					TopIndex[CreenNums] = 0xff;
					break;
				}
			}
		}
		if(ScreenIndex == CreenNums && !CurrScreenLines)
			ScreenIndex = 0;
		_cls();
		for(ItemIndex = 0; ItemIndex < TopDataNum[ScreenIndex]; ItemIndex++)
		{
			if(PartDName[ItemIndex+TopIndex[ScreenIndex]])
			{
				_putstr((unsigned char*)(PartDName[ItemIndex+TopIndex[ScreenIndex]]));
				if((strlen(PartDName[ItemIndex+TopIndex[ScreenIndex]])%20))
					_putstr("\n");
			}
			if(PartData[ItemIndex+TopIndex[ScreenIndex]])
			{
				_putstr((unsigned char*)(PartData[ItemIndex+TopIndex[ScreenIndex]]));
				if((strlen(PartData[ItemIndex+TopIndex[ScreenIndex]])%20))
					_putstr(" ");
			}
			if(PartUnit[ItemIndex+TopIndex[ScreenIndex]] && PartFormat[ItemIndex+TopIndex[ScreenIndex]][0] != 'T')
			{
		   		_putstr((unsigned char*)(PartUnit[ItemIndex+TopIndex[ScreenIndex]]));
		   		//if((strlen(PartUnit[ItemIndex+TopIndex[ScreenIndex]])%20))
		   		if((strlen(PartData[ItemIndex+TopIndex[ScreenIndex]])%20))//数据满行状态不添加换行
		   			if(((strlen(PartData[ItemIndex+TopIndex[ScreenIndex]])+1)%20))//还需要判断增加空格后满行的情况
		   			_putstr("\n");
			}
			else
			{
				if((strlen(PartUnit[ItemIndex+TopIndex[ScreenIndex]])%20))
				_putstr("\n");
			}

		}
		while(1)
		{
			KeyValue = _bioskey(0);
			switch(KeyValue)
			{
				case _ENT:
				case _ESC:
					return;
				case _UP:
					if(ScreenIndex > 0)
						ScreenIndex--;
					break;
				case _DWN:
					ScreenIndex++;
					break;

			}
			break;
		}
	}
}
void FreeDataInfor(char* dIdentify, char Flag)
{
	int i;
	for(i = 0; i<SDFormatNum; i++)
	{
		if(PartFormat_Bak[i]!=NULL)
			free(PartFormat_Bak[i]);
	}
	for(i = 0; i<SDNameNum; i++)
	{
		if(PartDName_Bak[i]!=NULL)
			free(PartDName_Bak[i]);
	}
	for(i = 0; i<SUnitNum; i++)
	{
		if(PartUnit_Bak[i]!=NULL)
			free(PartUnit_Bak[i]);
	}
	for(i = 0; i<MaxNum; i++)
	{
		if(PartData[i]!=NULL)
			free(PartData[i]);
	}
	for(i = 0; i<MaxNum; i++)
	{
		PartFormat[i]=NULL;
		PartData[i]=NULL;
		PartDName[i]=NULL;
		PartUnit[i]=NULL;

		PartFormat_Bak[i]=NULL;
		PartDName_Bak[i]=NULL;
		PartUnit_Bak[i]=NULL;
	}
}

int AnalysisData(char* FunCode, char Flag)
{
	int CurrModules = 0;
	char DateTime[11] = {0};
	int StartIndex = 0;
	switch(Flag)
	{
		case '1':
			break;
		case '2':
			PartData[0] = (char*)malloc(4);
			memset(PartData[0], 0, 4);
			if(!strcmp(FunCode,"00000201"))//Fn10 输入读取测量点号
			{
				CurrModules = ReceiBuff[19]*0x100+ReceiBuff[18];//吉林698
			}
			else
			{
			CurrModules = ReceiBuff[18];
			}
			sprintf(PartData[0],"%d",CurrModules);
			FullDataInfo(FunCode,CurrModules,1);
			if(!strcmp(FunCode,"00000201"))//Fn10 输入读取测量点号
			{
				CurrModules = 2;
			}
			else
			{
			CurrModules = 1;
			}
			StartIndex = 1;
			break;
		case '3':
			PartData[0] = (char*)malloc(11);
			memset(PartData[0], 0, 11);
			memcpy(DateTime, &ReceiBuff[18], 5);
			HextoStr(PartData[0], DateTime, 5, 1, 0);
			PartData[1] = (char*)malloc(4);
			memset(PartData[1], 0, 4);
			CurrModules = ReceiBuff[23];
			ModuNum = CurrModules;
			sprintf(PartData[1],"%d",CurrModules);
			FullDataInfo(FunCode,CurrModules,2);
			StartIndex = 2;
			CurrModules = 6;
			break;
		default:
		/*else
		{
			CurrModules = RFrameLength/5;
			StartIndex = 0;
			FullDataInfo(SDIdentify,CurrModules,StartIndex);
			CurrModules = 0;
		}*/
			break;
	}

	ParseReceiFrame(StartIndex, CurrModules);
	DisplayData();
	
	return 0;
}

void RepCopy(char* SAddress, short int Select)
{
	char UserArea[110] = {0};
	char MeterAddr[13] = {0};
	char DepartFrame[40] = {0};
	char TmpBuff[20] = {0};
//	char port[3] = {0};
//	char Hport[3] = {0};
	int TmpLen = 0;
	unsigned char ComCode = 0;

	memset(MeterAddr,0,sizeof(MeterAddr));
	if(MessageBox("请输入表计地址", MeterAddr, 12, 2) == _ESC)
		return;
	FormatStr(MeterAddr, 12, '0', 0);
	ReverseStr(MeterAddr);
	strcpy(DepartFrame, "68");
	strcat(DepartFrame, MeterAddr);
	strcat(DepartFrame, "68");
	switch(Select)
	{
		case 1:
			strcat(DepartFrame,"110433333333");
			ComCode = 0x6B;
			break;
		case 2:
			strcat(DepartFrame,"110433333433");
			ComCode = 0x6B;
			break;
		case 3:
			strcat(DepartFrame,"110435343337");
			ComCode = 0x6B;
			break;
		case 4:
			strcat(DepartFrame,"010243C3");
			ComCode = 0x4B;
			break;
		case 5:
			strcat(DepartFrame,"010244F3");
			ComCode = 0x4B;
			break;
		case 6:
			strcat(DepartFrame,"");
	}

	memset(MeterAddr,0,sizeof(MeterAddr));
	memcpy(MeterAddr,SAddress,12);
	strcpy(UserArea,"4B");
	strcat(UserArea,MeterAddr);
	strcat(UserArea,"10EC0000010001");


	memset(TmpBuff,0,sizeof(TmpBuff));
	sprintf(TmpBuff,"%02x", ComCode);

	strcat(UserArea,TmpBuff);
	strcat(UserArea,"3232");

	memset(TmpBuff,0,sizeof(TmpBuff));
	sprintf(TmpBuff,"%04x", strlen(DepartFrame)/2 + 6);
	ReverseStr(TmpBuff);
	strcat(UserArea,TmpBuff);

	strcat(UserArea, "FEFEFEFE");

	strcat(UserArea, DepartFrame);

	TmpLen = strlen(DepartFrame)/2;
	memset(TmpBuff,0,sizeof(TmpBuff));
	StrtoHex(DepartFrame, TmpLen, 0, 0, 0);
	sprintf(TmpBuff, "%02x", GetCheck((unsigned char*)DepartFrame, 0, TmpLen));

	strcat(UserArea,TmpBuff);
	strcat(UserArea,"16");

	memset(DepartFrame, 0, sizeof(DepartFrame));
	memset(TmpBuff,0,sizeof(TmpBuff));
	FormatDT(TmpBuff);
	DateTimeToStr(TmpBuff);
	strcpy(DepartFrame, &TmpBuff[4]);
	ReverseStr(DepartFrame);

	memset(TmpBuff,0,sizeof(TmpBuff));
	sprintf(TmpBuff,"%04x", (strlen(UserArea)/2 +22)*4 +2);
	ReverseStr(TmpBuff);

	strcat(UserArea,"0000000000000000000000000000000000");
	strcat(UserArea,DepartFrame);
	strcat(UserArea,"00");

	memset(SendBuff,0,sizeof(SendBuff));
	strcpy(SendBuff,"68");
	strcat(SendBuff,TmpBuff);
	strcat(SendBuff,TmpBuff);
	strcat(SendBuff,"68");
	strcat(SendBuff,UserArea);

	TmpLen = strlen(UserArea)/2;
	StrtoHex(UserArea, TmpLen, 0, 0, 0);
	memset(TmpBuff, 0, sizeof(TmpBuff));
	sprintf(TmpBuff,"%02x", GetCheck((unsigned char*)UserArea, 0, TmpLen));
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, "16");

	_cls();
	_putstr("通信中...");
	SendFrame(SendBuff, strlen(SendBuff)/2);
	memset(ReceiBuff, 0, 800);
	if((RecvFrame() == (-1)) ||((ReceiBuff[12]==0x0)&&(ReceiBuff[14]==0x0)&&(ReceiBuff[15]==0x0)&&(ReceiBuff[16]==0x2)&&(ReceiBuff[17]==0x0)))
	{
		MessageBox2("接收数据失败");
		return;
	}
	else
	{
		if(ReceiBuff[22] == 2)
		{
			MessageBox2("<<<<无数据>>>>");
			return;
		}
		memset(TmpBuff, 0, sizeof(TmpBuff));
		memset(DepartFrame, 0, sizeof(DepartFrame));
		switch(Select)
		{
			case 1:
				memcpy(TmpBuff, &ReceiBuff[35], 4);
				HextoStr(DepartFrame, TmpBuff, 4, 1, -51);
				AnalysisDot(DepartFrame, "XXXXXX.XX", 1);
				_cls();
				_putstr("当前组合有功总电量");
				_putstr("\n");
				_putstr((unsigned char*)DepartFrame);
				_putstr(" kWh");
				break;
			case 2:
				memcpy(TmpBuff, &ReceiBuff[35], 4);
				HextoStr(DepartFrame, TmpBuff, 4, 1, -51);
				AnalysisDot(DepartFrame, "XXXXXX.XX", 1);
				_cls();
				_putstr("当前正向有功总电量");
				_putstr("\n");
				_putstr((unsigned char*)DepartFrame);
				_putstr(" kWh");
				break;
			case 3:
				memcpy(TmpBuff, &ReceiBuff[35], 3);
				HextoStr(DepartFrame, TmpBuff, 3, 1, -51);
				memset(TmpBuff,0,sizeof(TmpBuff));
				GetOnDateTime(DepartFrame,"hhmmss",TmpBuff);
				_cls();
				_putstr("电表时间");
				_putstr("\n");
				_putstr((unsigned char*)TmpBuff);
				break;
			case 4:
				memcpy(TmpBuff, &ReceiBuff[33], 4);
				HextoStr(DepartFrame, TmpBuff, 4, 1, -51);
				AnalysisDot(DepartFrame, "XXXXXX.XX", 1);
				_cls();
				_putstr("当前正向有功总电量");
				_putstr("\n");
				_putstr((unsigned char*)DepartFrame);
				_putstr(" kWh");
				break;
			case 5:
				memcpy(TmpBuff, &ReceiBuff[33], 3);
				HextoStr(DepartFrame, TmpBuff, 3, 1, -51);
				memset(TmpBuff,0,sizeof(TmpBuff));
				GetOnDateTime(DepartFrame,"hhmmss",TmpBuff);
				_cls();
				_putstr("电表时间");
				_putstr("\n");
				_putstr((unsigned char*)TmpBuff);
				break;
		}

		_bioskey(0);
		return;
	}
}
/*void RepCopy(char* Addr)
{
	char RepCopyF[110] = {0};
	char HRepCopyF[110] = {0};
	char MeterAddr[13] = {0};
	char DepartFrame[30] = {0};
	char HDepartF[30] = {0};

	strcpy(SendBuff, "68AD00AD0068");
	strcpy(RepCopyF,"4B");
	strcat(RepCopyF,Addr);
	strcat(RepCopyF,"10EC00000100014B323212FEFEFEFE");
	memset(MeterAddr,0,sizeof(MeterAddr));
	if(MessageBox("请输入表计地址", MeterAddr, 12, 2) == _ESC)
		return;
	FormatStr(MeterAddr, 12, '0', 0);
	ReverseStr(MeterAddr);
	strcpy(DepartFrame, "68");
	strcat(DepartFrame, MeterAddr);
	strcat(DepartFrame, "68010243C3");
	strcpy(HDepartF, DepartFrame);
	StrtoHex(HDepartF, strlen(DepartFrame)/2, 0, 0, 0);
	memset(MeterAddr, 0, 13);
	sprintf(MeterAddr,"%02x", GetCheck(HDepartF, 0, strlen(DepartFrame)/2));
	strcat(DepartFrame, MeterAddr);
	strcat(DepartFrame, "16");
	strcat(RepCopyF, DepartFrame);

	memset(DepartFrame, 0, 30);
	FormatDT(DepartFrame);
	DateTimeToStr(DepartFrame);
	memset(HDepartF, 0, 30);
	strcpy(HDepartF, &DepartFrame[4]);
	ReverseStr(HDepartF);
	strcat(RepCopyF,"0000000000000000000000000000000000");
	strcat(RepCopyF,HDepartF);
	strcat(RepCopyF,"00");

	strcpy(HRepCopyF, RepCopyF);
	StrtoHex(HRepCopyF, strlen(RepCopyF)/2, 0, 0, 0);

	memset(MeterAddr, 0, 13);
	sprintf(MeterAddr,"%02x", GetCheck(HRepCopyF, 0, strlen(RepCopyF)/2));
	strcat(RepCopyF, MeterAddr);


	strcat(SendBuff, RepCopyF);
	strcat(SendBuff, "16");

	_cls();
	_putstr("通信中...");
	SendFrame(SendBuff, strlen(SendBuff)/2);
	memset(ReceiBuff, 0, 800);
	if((RecvFrame() == (-1)) ||((ReceiBuff[12]==0x0)&&(ReceiBuff[14]==0x0)&&(ReceiBuff[15]==0x0)&&(ReceiBuff[16]==0x2)&&(ReceiBuff[17]==0x0)))
	{
		MessageBox2("接收数据失败");
		return;
	}
	else
	{
		if(ReceiBuff[22] == 2)
		{
			MessageBox2("<<<<无数据>>>>");
			return;
		}
		memset(MeterAddr, 0, 13);
		memset(DepartFrame, 0, 30);
		memcpy(MeterAddr, &ReceiBuff[31], 4);
		HextoStr(DepartFrame, MeterAddr, 4, 1, -51);
		AnalysisDot(DepartFrame, "XXXXXX.XX", 1);
		_cls();
		_putstr("当前正向有功总电量");
		_putstr("\n");
		_putstr(DepartFrame);
		_putstr(" kWh");
		_bioskey(0);
	}
}*/
int HBAddr(short int * CurrFrameLength)
{
	char Buff[6];
	char hbaddr[4] = {0};
	char zipaddr[8] = {0};
	char i;
	short int KeyValue;
	_cls();
	_putstr("请输入行政区划码");
	_gotoxy(0, 2);
	_putstr("请输入终端地址");
	for(i = 0; i < 2; i++)
	{
		_gotoxy(0,1+i*2);
		switch(i)
		{
		case 0:
			memset(Buff, 0, 6);
			KeyValue = Scanf(Buff, 4);
			FormatStr(Buff, 4, '0', 0);
			ReverseStr(Buff);
			break;
		case 1:
			memset(Buff, 0, 6);
			KeyValue = Scanf(Buff, 5);
			FormatStr(Buff, 5, '0', 0);
			break;
		}
		if(KeyValue == _ESC)
			return 0;
		if(i == 1)
			{
				if(strcmp(Buff,"00000"))//字符串非空
				{
					sprintf(hbaddr,"%04x",StrToInt(Buff,10));
					ReverseStr(hbaddr);
					strcat(zipaddr, hbaddr);
				}
				else
				{
					strcat(zipaddr,"FFFF");
				}
			}
		else
			{
				if(strcmp(Buff,"0000"))//字符串非空
				{
					strcpy(zipaddr, Buff);
				}
				else
				{
					strcpy(zipaddr,"FFFF");
				}

			}
	}
	HBAddrFrameBuild(zipaddr,CurrFrameLength);
	return 1;
}
int HBAddrFrameBuild(char* zipaddr,short int * CurrFrameLength)
{
	int k;
	char ConvBuff[100];
	char UserArea[800] = {0};
	char TmpBuff[20];
	//按照电表档案组装报文

	//报文数据域组装
	strcpy(UserArea, "4A");
	strcat(UserArea, Addr);
	strcat(UserArea, "04");
	strcat(UserArea, "E8");
	strcat(UserArea, "000002140300");
	memset(ConvBuff,0x0,100);
	strcat(UserArea,zipaddr);
	strcat(UserArea,"000000000000000000000000000000000000000000000000000000000000000000000000000000");
	memset(TmpBuff, 0, 20);
	sprintf(TmpBuff,"%04x", (strlen(UserArea)/2+5)*4 +2);
	ReverseStr(TmpBuff);
	strcpy(SendBuff, "68");
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, "68");
	memset(TmpBuff, 0, 20);
	FormatDT(TmpBuff);
	DateTimeToStr(TmpBuff);
	memset(ConvBuff, 0, 100);
	strcpy(ConvBuff, &TmpBuff[4]);
	ReverseStr(ConvBuff);
	strcat(UserArea,ConvBuff);
	strcat(SendBuff, UserArea);
	k = strlen(UserArea)/2;
	StrtoHex(UserArea, k, 0, 0, 0);
	memset(TmpBuff, 0, 20);
	sprintf(TmpBuff, "%02x", GetCheck((unsigned char*)UserArea, 0, k));
	strcat(SendBuff, "00");
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, "16");
	*CurrFrameLength = strlen(SendBuff)/2;
	return 1;
}


//DataBuff 需要写入文件的数据 Flag 操作标志 0表示从文件读取 1表示新建记录 2表示后续记录
unsigned short int ParaHisCfg(unsigned char* DataBuff,int Flag)
{
	FILE *fp;
	unsigned char FileBuff[61];//APN最大长度16字节
	unsigned long FileLength;//文件长度
	unsigned short int KeyValue = 0;
	int DealedLength = 0, PartNum = 0;
	int i;
	_cls();
	switch(Flag)
	{
		case 0://0表示从文件读取参数 显示并判断
		{
			if((fp = _fopen((signed char*)ipconfig,"r")) == NULL)//文件打开失败 不存在
			{
//				_putstr("IP地址记录不存在，按任意键开始配置!");
//				_bioskey(0);
				break;
			}
			else
			{
				FileLength = _filelength(fp);
				_fread(FileBuff,FileLength,fp);
				for(i = 0;i<FileLength;i++)
				{
					if(FileBuff[i] == '\n')//遇到换行符 说明数据项结束
					{
						FileBuff[i] = '\0';

//						PartData[PartNum] = (unsigned char*)malloc(strlen(FileBuff+DealedLength));
//						strcpy(PartData[PartNum],FileBuff+DealedLength);//拷贝对应数据
						PartData[PartNum] = (char*)malloc(i+2-DealedLength);
						memcpy(PartData[PartNum],FileBuff+DealedLength,i+2-DealedLength);
						PartNum++;
						DealedLength = (i+1);
					}
				}
				//显示
				for(i = 0;i<PartNum;i++)
				{
					_putstr((unsigned char*)(PartDName[i]));
					_putstr("\n");
					_putstr((unsigned char*)(PartData[i]));
					if(strlen(PartData[i]) < 20)
					{
						_putstr("\n");
					}
				}
				_gotoxy(0,9);
				_putstr("修改");
				_gotoxy(16,9);
				_putstr("发送");
				KeyValue = _bioskey(0);
				if(KeyValue != _ENT)//直接发送
				{
					for(i=0;i<PartNum;i++)
					{
						free(PartData[i]);
						PartData[i] = NULL;
					}
				}
				PartNum = 0;//计数复位
				DealedLength = 0;
				break;
			}
		}
		case 1://文件写入
		{
			if(!_fileexist((unsigned char*)ipconfig))//存在返回0
			{
				_filedelete((signed char*)ipconfig);//删除上次记录
			}
			if((fp=_fopen((signed char*)ipconfig,"a"))!=NULL)//文件打开或者新建成功
			{
				_fwrite(DataBuff,strlen((char const*)DataBuff),fp);//写入当前文件
				_fwrite("\n",1,fp);
			}
			break;
		}
		case 2://后续记录写入
		{
			if((fp=_fopen((signed char*)ipconfig,"a"))!=NULL)//文件打开或者新建成功
			{
				_fwrite(DataBuff,strlen((char const*)DataBuff),fp);//写入当前文件
				_fwrite("\n",1,fp);
			}
			break;
		}

		default:
			KeyValue = _ENT;
	}

	if(fp != NULL)
	{
		_fclose(fp);
		fp = NULL;
	}
	return KeyValue;
}

void Processor(char* PITEMIND,char* SAddress,char* FunCode)
{
	unsigned long CurrIndex;
	unsigned long Count;
	unsigned short int KeyValue;
	char CurrItem[11] = {0};
	char SFormates[255] = {0};
	char SDataNames[255] = {0};
	char SUnits[255] = {0};
	char SDIdentify[17] = {0};
	char SOperFlag[11] = {0};
	char SCtlCode[4] = {0};
	char SModels[4] = {0};
	char SFunCode[4] = {0};
	int paramMethod = 0;
	int tranMethod = 0;
	char Buff[10] = {0};
	char IsHB = 0;
	if(!OpenDbf("Proto.db"))
		return;
	memset(SendBuff, 0, 800);
	Count = GetCount(PROTO);
	if(!Count)
	{
		MessageBox1("数据库文件坏", NULL);
		_bioskey(0);
		return;
	}

	for(CurrIndex = 0; CurrIndex < Count; CurrIndex++)//读取组帧的相应信息
  {
    if(SearchField("ITEMIND",PITEMIND, PROTO) != (-1))
		{
			ReadField("FORMATS", SFormates, PROTO);
//			if(!strcmp(PITEMIND,"019")))//新加终端地址设置查询进制变动
//			{
//				if(NumSys ==0 )//十六进制地址
//				{
//					sprintf(SFormates,"%s","XXXX,XXXX");
//				}
//				else
//				{
//					sprintf(SFormates,"%s","XXXX,BBBBB");
//				}
//			}
			ReadField("NAMES", SDataNames, PROTO);
			ReadField("UNITS", SUnits, PROTO);
			ReadField("DIDENTIFY", SDIdentify, PROTO);

			if((!strcmp(SDIdentify, "0000010B"))
				|| (!strcmp(SDIdentify, "0000100A")))//新加终端地址设置查询进制变动
			{
				if(NumSys ==0 )//十六进制地址
				{
					sprintf(SFormates,"%s","XXXX,XXXX");
				}
				else
				{
					sprintf(SFormates,"%s","XXXX,BBBBB");
				}
			}

			strcpy(FunCode, SDIdentify);
			if(SDIdentify[0] == 'X')
			{
				KeyValue = MessageBox("请输入测量点号", CurrItem, 4, 2);//测量点范围1-2040
				if(KeyValue == _ESC)
					return;
				else
				{
					if(StrToInt(CurrItem, 10) > 2040)//大于测量点上限
					{
						_cls();
						_putstr("超出测量点范围！");
						_bioskey(0);
						return;
					}
					sprintf(SModels, "%02x", (1<<((StrToInt(CurrItem, 10)-1) % 8)));//DA2计算方式为除8取余
					memcpy(SDIdentify, SModels, 2);
					memset(SModels, 0, 4);
					sprintf(SModels, "%02x", ((StrToInt(CurrItem, 10)-1) / 8 + 1));//DA1计算方式为除8取整+1
					memcpy(SDIdentify + 2 , SModels, 2);
					memset(SModels, 0, 4);
				}
			}
			ReadField("CTLCODE", SOperFlag, PROTO);
			ReadField("MODELS", SModels, PROTO);

			if(SOperFlag[0] == '0')//参数操作 yy add
			{
				_cls();
				_putstr("     请选择操作     ");
				_gotoxy(0, 3);
				_putstr("     1->查看\n     2->设置");
				while(1)
				{
					KeyValue = _bioskey(0);
					if(KeyValue == ONE)
					{
						strcpy(SCtlCode, "4B");
						strcpy(SFunCode, "0A");
						break;
					}
					if(KeyValue == TWO)
					{
						strcpy(SCtlCode, "4A");
						strcpy(SFunCode, "04");
						if(!strcmp(SDIdentify, "00000201"))//电表档案设置
						{
							MetarRecordProcess();
							return;
						}
						if(!strcmp(SDIdentify, "0000401E"))
						{
							_cls();
							_putstr("          ");
							_gotoxy(0, 3);
							_putstr("     1->GPRS/CDMA\n     2->以太网\n     3->ETH_GPRS_SWITCH");
								while(1)
								{
									KeyValue = _bioskey(0);
									if(KeyValue == ONE)
									{
										paramMethod = 1;
										break;

								  }
								  if(KeyValue == TWO)
									{
										paramMethod = 2;
										break;

								  }
								   if(KeyValue == THREE)
									{
										paramMethod = 3;
										break;

								  }
								}

						}
						if(!strcmp(SDIdentify, "0000200A"))
						{
							while(1)
							{
						 		_cls();
						 		_putstr("请输入设置密码");
						    _gotoxy(0, 1);
						    KeyValue = Scanf(Buff, 8);
								if(strcmp(Buff, "88619832"))
								{
								  KeyValue = MessageBox1("密码错误!", "退出  重输");
							    if(KeyValue == _ESC)
										return;
								}
								else
									break;
						  }
							_cls();
							_putstr("          ");
							_gotoxy(0, 3);
							_putstr("     1->正常模式\n     2->测试模式\n     3->调试模式");
								while(1)
								{
									KeyValue = _bioskey(0);
									if(KeyValue == ONE)
									{
										tranMethod = 1;
										break;

								  }
								  if(KeyValue == TWO)
									{
										tranMethod = 2;
										break;

								  }
								   if(KeyValue == THREE)
									{
										tranMethod = 3;
										break;

								  }
								}
						}
						break;
					}
					if(KeyValue == _ESC)
						return;
				}
			}
			else if(SOperFlag[0] == '1')//请求1类数据 yy add
			{
				strcpy(SCtlCode, "4B");
				strcpy(SFunCode, "0C");
			}
			else if(SOperFlag[0] == '2')
			{
				strcpy(SCtlCode, "4A");
				strcpy(SFunCode, "05");
			}
			else if(SOperFlag[0] == '3')
			{
				strcpy(SCtlCode, "4B");
				strcpy(SFunCode, "09");
			}
			else if(SOperFlag[0] == '4')
			{
				strcpy(SCtlCode, "41");
				strcpy(SFunCode, "01");
			}
			else if(SOperFlag[0] == '5')//请求2类数据
			{
				strcpy(SCtlCode, "4B");
				strcpy(SFunCode, "0D");

				KeyValue = MessageBox("请输入时标:YYMMDD", CurrItem, 6, 2);
				if(KeyValue == _ESC)
					return;
				else
				{
					memcpy(SDIdentify+8,CurrItem+4,2);//DD
					memcpy(SDIdentify+10,CurrItem+2,2);//MM
					memcpy(SDIdentify+12,CurrItem,2);//YY

				}
			}

			if(!SetFormat(SFormates) || !SetDataName(SDataNames) || !SetUnits(SUnits))
			{
				return;
			}


			if(!strcmp(SDIdentify, "00000400")&& !strcmp(SCtlCode, "4A"))
			{
				KeyValue = ParaHisCfg(NULL,0);//历史值显示和选择
				if(KeyValue != _ENT)
				{
					if(!InputData(SDIdentify, SModels))
						return;
				}
			}
			else if(!strcmp(SCtlCode, "4A") && strcmp(SDIdentify, "0000401E") && strcmp(SDIdentify, "0000200A")&&strcmp(SDIdentify, "00000214")&&strcmp(SDIdentify, "00004003"))//设置数据输入
			//else if(!strcmp(SCtlCode, "4A") && strcmp(SDIdentify, "0000200A")&&strcmp(SDIdentify, "00000214"))//设置数据输入
			{
				if(!InputData(SDIdentify, SModels))
					return;
			}
			if(!strcmp(SDIdentify, "00000214")&&KeyValue==TWO)//华北特殊处理
			{

				_cls();
				_putstr("          ");
				_gotoxy(0, 3);
				_putstr("     1->普通模式\n     2->调试模式");
				while(1)
				{
					KeyValue = _bioskey(0);
					if(KeyValue == ONE)
					{
						IsHB = 1;
						if (!HBAddr(&KeyValue))
						{
							return;
						}
						break;

					}
					if(KeyValue == TWO)
					{
						if(!InputData(SDIdentify, SModels))
							return;
						break;
					}
				}
			}

			if(!strcmp(SDIdentify, "0000200A"))
			{
				PartData[0] = (char*)malloc(2);
				if(tranMethod == 1){
					sprintf(PartData[0], "00");

				 }else if(tranMethod == 2)
				 {
		     			 sprintf(PartData[0], "01");
				 }
				 else if(tranMethod == 3)
				 {
		     			 sprintf(PartData[0], "02");
		         }
			}
			if(!strcmp(SDIdentify, "0000401E"))
			{
				PartData[0] = (char*)malloc(2);
				if(paramMethod == 1){
					sprintf(PartData[0], "02");

				 }else if(paramMethod == 2)
				 {
		     			 sprintf(PartData[0], "04");
				 }
				 else if(paramMethod == 3)
				 {
		     			 sprintf(PartData[0], "0B");
				 }
			}

			if (IsHB)
			{
				//如果湖北地址则已经处理。
			}
			else if(!BuildFrame(SAddress, SCtlCode, SDIdentify, SFunCode, &KeyValue))
			{
				MessageBox2("输入错误");
				return;
			}

			//debug
			_cls();
			_putstr("通信中...");

			SendFrame(SendBuff, KeyValue);

			memset(ReceiBuff, 0, 800);
			if((RecvFrame() == (-1)) ||((ReceiBuff[12]==0x0)&&(ReceiBuff[14]==0x0)&&(ReceiBuff[15]==0x0)&&(ReceiBuff[16]==0x2)&&(ReceiBuff[17]==0x0)))
			{
				MessageBox2("接收数据失败");
				return;
			}
			else
			{
				if(!strcmp(SCtlCode, "4A"))//设置数据输入
					MessageBox2("设置成功");
				else if(!strcmp(SCtlCode, "41"))
				{
					MessageBox2("终端确认");
				}
				else
				{
					if(RFrameLength>0)
						AnalysisData(SDIdentify, SModels[0]);
					else
						MessageBox2(" 无数据... ");
				}
				return;
			}
		}
    }
}

short int FormatInput(char *Buff,  char* format, char nums, char compartment)
{
	char i, x, y;
	char TmpStr[4];

	x = _getx();
	y = _gety();
	_gotoxy(x, y);
	for(i = 0; i < nums; i++ )
	{
		memset(TmpStr, 0, 4);
		if(Scanf(TmpStr, strlen(format)) == _ESC)
			return _ESC;
		FormatStr(TmpStr, 3, '0', 0);
		if(i == 0)
			strcpy(Buff, TmpStr);
		else
			strcat(Buff, TmpStr);
		if(i< (nums - 1))
		{
			strcat(Buff, ".");
			_gotoxy((i+1)*(strlen(format)+1),y);
		}
		else
			strcat(Buff, " ");
	}
	return 1;
}
/******************************************************************************************
*    功能：接收IP地址和端口号的输入
*    参数：Buff  接收数据的缓冲区
*    返回：操作成功返回1，否则返回_ESC。
******************************************************************************************/
short int GetIPAddrAndPort(char* Buff)
{
	//char Port[5] = {0};
	char DPort[6] = {0};

	if(FormatInput(Buff, "XXX", 4, '.') == _ESC)
		return _ESC;
	_gotoxy(_getx()+1, _gety());
	if(Scanf(DPort, 4) == _ESC)
		return _ESC;
	FormatStr(DPort, 4, '0', 0);
	strcat(Buff,DPort);
	return _ENT;
}
/******************************************************************************************
*    功能：接收IP地址和端口号的输入
*    参数：Buff  接收数据的缓冲区
*    返回：操作成功返回1，否则返回_ESC。
******************************************************************************************/
short int GetIPAddr(char* Buff)
{
	if(FormatInput(Buff, "XXX", 4, '.') == _ESC)
		return _ESC;
	_gotoxy(_getx()+1, _gety());
	return _ENT;
}
/******************************************************************************************
*    功能：获取指定的枚举的值
*    参数：Buff  接收数据的缓冲区,指示数据域的格式
*    返回：操作成功返回1，否则返回_ESC。
******************************************************************************************/
short int GetSelectValue(char* Buff, char* Formate, char* SlecValue)
{
	TEnumValue TmpEnumValue[10];
	int i,Records;
	int k=0;
	char TmpMl[11];
	short int KeyValue;

	for(i = 0; i< 10; i++)
		memset(&TmpEnumValue[i], 0, sizeof(TEnumValue));
	if(!OpenDbf("selec.db"))
		return _ESC;
	Records = GetCount(SELEC);
	for(i = 0; i < Records; i++)
	{
		GoRecord(i, SELEC);
		memset(TmpMl, 0, 11);
		ReadField("FUCCODE", TmpMl, SELEC);
		if(!strcmp(TmpMl, Formate))
		{
			ReadField("NAME", TmpEnumValue[k].Msg, SELEC);
			ReadField("VLAUE", TmpEnumValue[k].Value, SELEC);
			k++;
		}
	}
	if(SlecValue != NULL)
	{
		for(i = 0; i < k; i++)
		{
			if(!strcmp(SlecValue, TmpEnumValue[i].Value))
			{
				strcpy(Buff, TmpEnumValue[i].Msg);
				CloseDbf("selec.db");
				return 1;
			}
		}
		strcpy(Buff, "00");
		CloseDbf("selec.db");
		return 1;
	}
	for(i = 0 ; i < k;)
	{
		KeyValue = MessageBox2((unsigned char*)(TmpEnumValue[i].Msg));
		switch(KeyValue)
		{
			case _ENT:
				strcpy(Buff, TmpEnumValue[i].Value);
			case _ESC:
				CloseDbf("selec.db");
				return KeyValue;
			case _UP:
				if(i == 0)
					i = k-1;
				else
					i--;
				break;
			case _DWN:
				if(i == (k-1))
					i = 0;
				else
					i++;
				break;
			default:
				continue;
		}
	}
	return KeyValue;
}

int InputSignalData(int StartDataIndex)
{
	//char Buff[100];
	TUIInfor* DisplayInfor[50];
	int ScreenIndex,CurrScreIndex,CurrDataNums,DataIndex;
	char CurrLines;
	char Position[50][5][2];
	int CurrLoop;
	short int KeyValue;

	DataIndex = StartDataIndex;
	ScreenIndex = CurrScreIndex = 0;
	for(CurrLoop = StartDataIndex; CurrLoop < MaxNum; CurrLoop++)
		PartData[CurrLoop] = NULL;
	DisplayInfor[CurrScreIndex] = (TUIInfor*)malloc(sizeof(TUIInfor));
	memset(DisplayInfor[CurrScreIndex], 0, sizeof(TUIInfor));
	DisplayInfor[ScreenIndex]->FDNIndex = StartDataIndex;
	while(1)
	{
		CurrLoop = CurrDataNums = 0;
		if(CurrScreIndex == ScreenIndex)//计算一屏可以显示多少数据项
		{
			CurrLines = 0;
			memset(Position[CurrScreIndex], 0, 10);
			for(DataIndex = DisplayInfor[ScreenIndex]->FDNIndex; ; DataIndex++)
			{
				Position[CurrScreIndex][CurrDataNums][0] = CurrLines;
				CurrLines += CalculItemLines(PartDName[DataIndex]);
				Position[CurrScreIndex][CurrDataNums][1] = CurrLines;
				if(PartFormat[DataIndex][0] == 'I' || PartFormat[DataIndex][0] == 'i' )
					CurrLines += 1;
				else
					CurrLines += CalculItemLines(PartFormat[DataIndex]);
				if(CurrLines == (SCREENHEIGHT + 2))
				{
					CurrDataNums++;
					ScreenIndex++;
					DisplayInfor[ScreenIndex] = (TUIInfor*)malloc(sizeof(TUIInfor));
					memset(DisplayInfor[ScreenIndex], 0, sizeof(TUIInfor));
					if(DataIndex == (DNameNum -1))
						DisplayInfor[ScreenIndex]->FDNIndex = 0xff;
					else
						DisplayInfor[ScreenIndex]->FDNIndex = DataIndex+1;
					  DisplayInfor[CurrScreIndex]->DataNums = CurrDataNums;
					break;
				}
				else if(CurrLines > (SCREENHEIGHT + 2))
				{
					ScreenIndex++;
					DisplayInfor[ScreenIndex] = (TUIInfor*)malloc(sizeof(TUIInfor));
					memset(DisplayInfor[ScreenIndex], 0, sizeof(TUIInfor));
					DisplayInfor[ScreenIndex]->FDNIndex = (DataIndex);
					DisplayInfor[CurrScreIndex]->DataNums = CurrDataNums;
					break;
				}
				else if(DataIndex == (DNameNum -1))
				{
					CurrDataNums++;
					ScreenIndex++;
					DisplayInfor[ScreenIndex] = (TUIInfor*)malloc(sizeof(TUIInfor));
					memset(DisplayInfor[ScreenIndex], 0, sizeof(TUIInfor));
					DisplayInfor[ScreenIndex]->FDNIndex = 0xff;
					DisplayInfor[CurrScreIndex]->DataNums = CurrDataNums;
					break;
				}
				CurrDataNums++;
			}
		}
		if(!CurrLines && CurrScreIndex == ScreenIndex)//控制右循环翻页
			CurrScreIndex = 0;
		_cls();
		for(CurrLoop = 0; CurrLoop < DisplayInfor[CurrScreIndex]->DataNums;CurrLoop++)
		{//显示数据项名称和特殊格式（IP地址输入格式）
			_gotoxy(0,Position[CurrScreIndex][CurrLoop][0]);
			_putstr((unsigned char*)(PartDName[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]));
			_gotoxy(0,Position[CurrScreIndex][CurrLoop][1]);
			if(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop])
				_putstr((unsigned char*)(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]));
			else if(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'I' ||PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'i')
				_putstr("   .   .   .");
		}
		for(CurrLoop = 0; CurrLoop < DisplayInfor[CurrScreIndex]->DataNums;)
		{//接收一屏数据开始
			_gotoxy(0,Position[CurrScreIndex][CurrLoop][1]);
			//输入IP地址和端口号 yy add
			if(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'I' ||PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'i')
			{
				if(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop])
				{
					_clsn(Position[CurrScreIndex][CurrLoop][1], 1);
					_putstr("   .   .   .");
				}
				else
					PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop] = (char*)malloc(21);
				memset(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop], 0, 21);
				if(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'I' )
					{
				    KeyValue = GetIPAddrAndPort(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]);
			    }else
			    {
			    	KeyValue = GetIPAddr(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]);
			    }
			}
			//取得selec.db中指定的值 yy add
			else if(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'S')
			{
				if(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop])
					_clsn(Position[CurrScreIndex][CurrLoop][1], 1);
				else
					PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop] = (char*)malloc(4);
				memset(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop], 0, 4);
				KeyValue = GetSelectValue(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop],
										PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop],NULL);
				_gotoxy(0,Position[CurrScreIndex][CurrLoop][1]);
				_putstr((unsigned char*)(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]));
			}
			else
			{
				if(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop])
					_clsn(Position[CurrScreIndex][CurrLoop][1], Position[CurrScreIndex][CurrLoop+1][0] - Position[CurrScreIndex][CurrLoop][1]);
				else
					PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop] =
						(char*)malloc(strlen(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]) +1);
				memset(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop], 0,
						strlen(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]) +1);
				KeyValue = Scanf(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop],
								strlen(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]));
				if(KeyValue == _ENT)
				{
					if(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'P')
					{
						FormatStr(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop],
								  strlen(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]),
								  'F', 1);
					}
					else if(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'X' ||
							PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'F' ||
							PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop][0] == 'B')
						//在输入字符前面填0
						FormatStr(PartData[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop],
							  		strlen(PartFormat[DisplayInfor[CurrScreIndex]->FDNIndex + CurrLoop]),
							  		'0', 0);

				}
			}
			if(KeyValue == _ENT)
			{
				CurrLoop++;
				//break;
			}
			else if(KeyValue == _ESC)
			{
				for(CurrLoop = 0; CurrLoop < ScreenIndex; CurrLoop++)
					free(DisplayInfor[CurrLoop]);;
				return 0;
			}
		}//接收一屏数据结束

		while(1)//控制翻屏和退出接收输入数据
		{

			switch(KeyValue)
			{
				case _ENT:
					if(CurrScreIndex<ScreenIndex)
					{

						CurrScreIndex++;
						if(DisplayInfor[CurrScreIndex]->FDNIndex == 0xff)
							return 1;
						break;
					}
					else
					{

						for(CurrLoop = 0; CurrLoop < ScreenIndex; CurrLoop++)
							free(DisplayInfor[CurrLoop]);
						return 1;
					}
				case _ESC:
					for(CurrLoop = 0; CurrLoop < ScreenIndex; CurrLoop++)
						free(DisplayInfor[CurrLoop]);
					return 0;
				case _DWN:
					CurrScreIndex++;
					if(DisplayInfor[CurrScreIndex]->FDNIndex == 0xff)
						return 1;
					break;
			}
			break;
//			KeyValue = _bioskey(0);
//			if(KeyValue == _DWN)
//				break;
		}

	}

}
int InputData(char* SDIdentify, char* SModels)
{
	short int KeyValue;
	int Modules;
	int Modules1;

	if(SModels[0] == '2')//多块数据输入
	{
		_cls();
		_putstr((unsigned char*)(PartDName[0]));
		_putstr("\n");
		PartData[0] = (char*)malloc(strlen(PartFormat[0]) + 1);
		memset(PartData[0], 0, strlen(PartFormat[0]) + 1);
		KeyValue = Scanf(PartData[0], strlen(PartFormat[0]));
		if(KeyValue == _ESC)
			return 0;
		FormatStr(PartData[0], strlen(PartFormat[0]),'0', 0);
		Modules1 = StrToInt(PartData[0], 10);
		for(Modules = 1; Modules < Modules1; Modules++)
		{//补齐数据信息
			for(KeyValue = 1; KeyValue < DFormatNum; KeyValue++)
				PartFormat[Modules*(DFormatNum -1) + KeyValue] = PartFormat[KeyValue];
			for(KeyValue = 1; KeyValue < DNameNum; KeyValue++)
				PartDName[Modules*(DNameNum - 1) + KeyValue] = PartDName[KeyValue];
			for(KeyValue = 1; KeyValue < DUnitNum; KeyValue++)
				PartUnit[Modules*(DUnitNum -1) + KeyValue] = PartUnit[KeyValue];
		}
		DFormatNum  = (DFormatNum -1)*Modules1+1;
		DNameNum  = (DNameNum -1)*Modules1+1;
		DUnitNum  = (DUnitNum- 1)*Modules1+1;
		if(!InputSignalData(1))
			return 0;
	}
	else //单块数据输入
	{
		if(!InputSignalData(0))
			return 0;
	}
	return 1;
}

void DebugTool(short int ManuInd)
{
	short int KeyValue;
	char KeyBuff[11];
	int TmpValue;
	char HexStr[10];
	char TmpStr[800] = {0};
	char ScreenStr[200];
	char FrameLength[3] = {0};
	char FLengthStr[5] = {0};
	int i;
				//FILE* log_file;

	switch(ManuInd)
	{
		case 1:
			_cls();
			_putstr("上一发送帧:");
			_drawline(0, 16, 159, 16, 1);
			_drawline(0, 17, 159, 17, 1);
			TmpValue = strlen(SendBuff)/180;
			if((strlen(SendBuff)%180))
				TmpValue++;
			for(i =0;;)
			{
				_clsn(1, 9);
				_gotoxy(0,1);
				memset(ScreenStr, 0, 200);
				strncpy(ScreenStr, &SendBuff[i*180], 180);
				_putstr((unsigned char*)ScreenStr);
				while(1)
				{
					KeyValue = _bioskey(0);
					if(KeyValue == _UP)
					{
						if(!i)
							i = TmpValue - 1;
						else
							i--;
						break;
					}
					else if(KeyValue == _DWN)
					{
						if(i == (TmpValue - 1))
							i = 0;
						else
							i++;
						break;
					}
					else if(KeyValue ==_ESC || KeyValue == _ENT)
						return;
				}
			}
		case 2:
			_cls();
			_putstr("上一接收帧:");
			_drawline(0, 16, 159, 16, 1);
			_drawline(0, 17, 159, 17, 1);
			memcpy(FrameLength, ReceiBuff+1, 2);
			HextoStr(FLengthStr,FrameLength,2,1, 0);
			RFrameLength = StrToInt(FLengthStr, 16)/4;
			if(ReceiBuff[0] != 0x68)
			{
				MessageBox2("接受报文没有数据");
				return;
			}
			HextoStr(TmpStr, ReceiBuff, RFrameLength + 8, 0, 0);

			//写日志

//				log_file = _fopen("\\NM376\\log.txt","a");
//				_fwrite(TmpStr,strlen(TmpStr),log_file);
//				_fclose(log_file);

			TmpValue = strlen(TmpStr)/180;
			if((strlen(TmpStr)%180))
				TmpValue++;
			for(i =0;;)
			{
				_clsn(1, 9);
				_gotoxy(0,1);
				memset(ScreenStr, 0, 200);
				strncpy(ScreenStr, &TmpStr[i*180], 180);
				_putstr((unsigned char*)ScreenStr);
				while(1)
				{
					KeyValue = _bioskey(0);
					if(KeyValue == _UP)
					{
						if(!i)
							i = TmpValue - 1;
						else
							i--;
							break;
					}
					else if(KeyValue == _DWN)
					{
						if(i == (TmpValue - 1))
							i = 0;
						else
							i++;
							break;
					}
					else if(KeyValue ==_ESC || KeyValue == _ENT)
						return;
				}
			}

		case 3:
			while(1)
			{
				_cls();
				_putstr("10到16进制转换:");
				_drawline(0, 16, 159, 16, 1);
				_drawline(0, 17, 159, 17, 1);
				_gotoxy(0,2);
				_putstr("请输入10进制串:");
				_gotoxy(3,3);
				KeyValue = Scanf(KeyBuff, 10);
				if(KeyValue == _ESC)
					return;
				if(((KeyBuff[0]&0xf)> 4 || (KeyBuff[2]&0xf)> 2) && strlen(KeyBuff) == 10)
				{
					MessageBox1("输入值过大", NULL);
					DisplayBott("退出", "重输");
					while(1)
					{
						KeyValue = _bioskey(0);
						if(KeyValue == _ESC)
							return;
						if(KeyValue == _ENT)
							break;
					}
					continue;
				}
				TmpValue = StrToInt(KeyBuff,10);
				sprintf(HexStr,"%x",TmpValue);
				_putstr("\n");
				_putstr("16进制值:");
				_gotoxy(3,5);
				_putstr((unsigned char*)HexStr);
				KeyValue = _bioskey(0);
				if(KeyValue == _ESC)
					return;
			}
		 	//break;
	}
}
int DatapointInput(char *string, int type)
{
	int DataNum = 0;
	int InputLen = 0;
	int KeyValue = 0;
	int i = 0,tmp = 0;
	char InputBuf[10] = {0};
	char DispBuf[20] = {0};
	char DataBuf[10] = {0};
	if(type == 2)
	{
		InputLen = 5;
	}
	else
	{
		InputLen = 3;
	}
	_cls();
	_putstr("本次查询数量：");
	_putstr("\n");
	//memset(InputBuf[0], 0, sizeof(InputBuf));
	KeyValue = Scanf(InputBuf, InputLen);
	if(KeyValue == _ESC)
		return 0;
	_putstr("\n");
	if(InputBuf[0] == 0)
	{
		return 0;
	}
	FormatStr(&InputBuf[0], sizeof(InputBuf),'0', 0);
	DataNum = StrToInt(InputBuf, 10);
	if(type == 2)
	{
		sprintf(DataBuf,"%04x",DataNum);
	}
	else
	{
		sprintf(DataBuf,"%02x",DataNum);
	}
	ReverseStr(DataBuf);
	strcat(string,DataBuf);
	memset(DataBuf,0,sizeof(DataBuf));
	for(i = 0;i < DataNum;i++)
	{
		sprintf(DispBuf,"查询对象序号%d:\n",i+1);
		_putstr((unsigned char*)DispBuf);
		memset(&InputBuf[0], 0, sizeof(InputBuf));
		KeyValue = Scanf(InputBuf, InputLen);
		if(KeyValue == _ESC)
			return 0;
		_putstr("\n");
		FormatStr(&InputBuf[0], sizeof(InputBuf),'0', 0);
		tmp = StrToInt(InputBuf, 10);
		if(type == 2)
		{
			if(tmp > 2040)
			{
				_cls();
				_putstr("超出测量点范围！");
				_bioskey(0);
				return 0;
			}
			sprintf(DataBuf,"%04x",tmp);
		}
		else
		{
			if(tmp > 64)
			{
				_cls();
				_putstr("超出脉冲点范围！");
				_bioskey(0);
				return 0;
			}
			sprintf(DataBuf,"%02x",tmp);
		}
		ReverseStr(DataBuf);
		strcat(string,DataBuf);
	}
	return 1;
}

/*
//档案读取函数
void MeterRecordRead(MeterRecordStruct *DataBuf, int* RecordNum);
//档案保存函数
void MeterRecordSave(MeterRecordStruct *DataBuf, int RecordNum);
//档案显示刷新函数
void MeterRecordShow(int RecordNo,int Num,int ChooseNum);//起始记录序号，显示个数，被选中记录号 一页显示4项
//档案修改函数
int MeterRecordEdit(int RecordNo);
//设置报文组装函数
int MeterRecordFrameBuild(unsigned char *FrameBuf,int RecordNo, int* CurrFrameLength);//从给定起始记录号开始组装 最多8条
//档案设置主处理函数
void MetarRecordProcess();
//档案管理主函数
void MeterRecordMangage();
//档案下发主函数
void MeterRecordSend();
*/
void MetarRecordProcess()
{
	int KeyValue = 0;
	if(!RecordReadFlag)//档案未读取
	{
		MeterRecordRead(MRecordBuf,&RecordNum);
		RecordReadFlag = 1;
	}
	while(1)
	{
		_cls();
		_gotoxy(0,0);
		_putstr("电表档案设置");
		_gotoxy(0,2);
		_putstr("1.档案管理\n");
		_putstr("2.档案下发");
		DisplayBott("退出","");
		KeyValue = _bioskey(0);
		switch(KeyValue)
		{
			case ONE:
				MeterRecordMangage();
				break;
			case TWO:
				MeterRecordSend();
				break;
			case _ESC:
				return;
			default:
				MessageBox2("选择错误!");
		}
	}
}

void MeterRecordMangage()
{
	int KeyValue = 0;

	while(1)
	{
		_cls();
		_gotoxy(0,0);
		_putstr("档案管理");
		_gotoxy(0,2);
		_putstr("1.新增档案\n");
		_putstr("2.档案查看\n");
		_putstr("3.档案清空\n");
		DisplayBott("退出","保存");
		KeyValue = _bioskey(0);
		switch(KeyValue)
		{
			case ONE:
				MeterRecordAdd();
				break;
			case TWO:
				MeterRecordView();
				break;
			case THREE:
				MeterRecordClear();
				break;
			case _ENT:
				MeterRecordSave(MRecordBuf,RecordNum);
				MessageBox2("参数已保存");
				RecordChangeFlag = 1;
				break;
			case _ESC:
				if(RecordChangeFlag == 0)//档案有修改，退出时需要提示是否保存
				{
					KeyValue = MessageBox1("参数已修改","取消  保存");
					if(KeyValue == _ENT)
					{
						MeterRecordSave(MRecordBuf,RecordNum);
					}
					RecordChangeFlag = 1;
				}
				return;
			default:
				MessageBox2("无效按键！");
		}
	}
}

int MeterRecordFrameBuild(unsigned char *FrameBuf,int RecordNo, int* CurrFrameLength)
{
	int i,k;
	int RecordCount = 0;
	char ConvBuff[100];
	char UserArea[800] = {0};
	char TmpBuff[20];
	//按照电表档案组装报文
	if((RecordNum - RecordNo +1) >= 6)
	{
		RecordCount = 6;
	}
	else
	{
		RecordCount = RecordNum - RecordNo + 1;
	}
	//报文数据域组装
	strcpy(UserArea, "4A");
	strcat(UserArea, Addr);
	strcat(UserArea, "04");
	strcat(UserArea, "E8");
	strcat(UserArea, "00000201");
	memset(ConvBuff,0x0,100);
	sprintf(ConvBuff,"%02x",RecordCount);//档案个数
	strcat(UserArea,ConvBuff);
	strcat(UserArea,"00");
	for(i = 0;i < RecordCount;i++)
	{
		memset(ConvBuff,0x0,100);
		sprintf(ConvBuff,"%02x00%02x00%02x%02x%02x%02x%02x%02x%02x%02x",
			MRecordBuf[RecordNo+i-1].MeterNo,MRecordBuf[RecordNo+i-1].MeterNo,
			(MRecordBuf[RecordNo+i-1].BaudRate + MRecordBuf[RecordNo+i-1].Port),
			MRecordBuf[RecordNo+i-1].ProtocolType,
			MRecordBuf[RecordNo+i-1].MeterAddr[0],MRecordBuf[RecordNo+i-1].MeterAddr[1],
			MRecordBuf[RecordNo+i-1].MeterAddr[2],MRecordBuf[RecordNo+i-1].MeterAddr[3],
			MRecordBuf[RecordNo+i-1].MeterAddr[4],MRecordBuf[RecordNo+i-1].MeterAddr[5]);
		strcat(ConvBuff,"0000000000000405000000000000");//电表档案固定部分
		memset(TmpBuff,0x0,20);
		sprintf(TmpBuff,"%02x",(MRecordBuf[RecordNo+i-1].BSort << 4) +
		MRecordBuf[RecordNo+i-1].SSort);
		strcat(ConvBuff,TmpBuff);
		strcat(UserArea,ConvBuff);
	}
	memset(TmpBuff, 0, 20);
	sprintf(TmpBuff,"%04x", (strlen(UserArea)/2 +22)*4 +1);
	ReverseStr(TmpBuff);
	strcpy(SendBuff, "68");
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, "68");
	strcat(UserArea,"0000000000000000000000000000000000");
	memset(TmpBuff, 0, 20);
	FormatDT(TmpBuff);
	DateTimeToStr(TmpBuff);
	memset(ConvBuff, 0, 100);
	strcpy(ConvBuff, &TmpBuff[4]);
	ReverseStr(ConvBuff);
	strcat(UserArea,ConvBuff);
	strcat(SendBuff, UserArea);
	k = strlen(UserArea)/2;
	StrtoHex(UserArea, k, 0, 0, 0);
	memset(TmpBuff, 0, 20);
	sprintf(TmpBuff, "%02x", GetCheck((unsigned char*)UserArea, 0, k));
	strcat(SendBuff, "00");
	strcat(SendBuff, TmpBuff);
	strcat(SendBuff, "16");
	*CurrFrameLength = strlen(SendBuff)/2;
	return RecordCount;
}

void MeterRecordSend()
{
	int StartNo = 1;
	int SendLen = 0;
	int DealedRecordCount = 0;
	int CurrFrameRecordNum = 0;
	char tmpBuf[50] = {0};
	int KeyValue = 0;
	for(;DealedRecordCount < RecordNum;)
	{
		_cls();
		_gotoxy(0,0);
		sprintf(tmpBuf,"记录总数为%d",RecordNum);
		_putstr((unsigned char*)tmpBuf);
		CurrFrameRecordNum = MeterRecordFrameBuild((unsigned char*)SendBuff,StartNo,&SendLen);
		_gotoxy(0,3);
		sprintf(tmpBuf,"正在发送第%d条至第%d条记录",StartNo,StartNo+CurrFrameRecordNum-1);
		_putstr((unsigned char*)tmpBuf);
		SendFrame(SendBuff, SendLen);
		memset(ReceiBuff, 0, 800);
		if((RecvFrame() == (-1)) ||((ReceiBuff[12]==0x0)&&(ReceiBuff[14]==0x0)&&(ReceiBuff[15]==0x0)&&(ReceiBuff[16]==0x2)&&(ReceiBuff[17]==0x0)))
		{
			KeyValue = MessageBox1("发送失败","退出 重发");
			if(KeyValue == _ENT)
			{
				continue;
			}
			else
			{
				return;
			}
		}
		else
		{
			DealedRecordCount += CurrFrameRecordNum;
			StartNo += CurrFrameRecordNum;
		}
	}
	MessageBox2("发送成功");
}

void MeterRecordRead(MeterRecordStruct *DataBuf, int* RecordNum)
{
	FILE* fp = NULL;
	int Offset = 0;
	
	fp = _fopen((signed char*)RecordPath,"r");//读模式打开文件
	if(fp != NULL)
	{
		_fread((unsigned char *)RecordNum,4,fp);//读取记录个数
		if(*RecordNum != 0)//存在档案
		{
			_fseek(fp,4,SEEK_SET);//文件指针偏移4字节
			_fread((unsigned char *)DataBuf,sizeof(MeterRecordStruct)*(*RecordNum),fp);
			Offset = (int)(RecordNum - 1);
			memcpy(&LastRecord,DataBuf+Offset,sizeof(MeterRecordStruct));//保留最新一条档案的设置信息
		}
		_fclose(fp);
	}
	else//文件不存在
	{
		MessageBox2("无配置文件");
		*RecordNum = 0;//记录个数初始化
	}
	return;

}

void MeterRecordSave(MeterRecordStruct *DataBuf, int RecordNum)
{
	FILE* fp;
	//int Offset = 0;
	//unsigned char PathBuff[40];
	if(_fileexist((unsigned char*)RecordPath) == 0)//判断记录文件是否存在，如果存在则先删除
	{
		_filedelete((signed char*)RecordPath);
	}
	fp = _fopen((signed char*)RecordPath,"a");//写模式打开文件，如文件不存在则新建
	if(fp != NULL)
	{
		_fseek(fp,0,SEEK_SET);//指针跳到文件开头
		_fwrite((unsigned char *)&RecordNum,4,fp);//读取记录个数
		if(RecordNum != 0)//存在档案
		{
			_fseek(fp,4,SEEK_SET);//文件指针偏移4字节
			_fwrite((unsigned char *)DataBuf,sizeof(MeterRecordStruct)*RecordNum,fp);
		}
		_fclose(fp);
	}
	else
	{
		MessageBox2("文件写入失败");
	}
	return;
}

void MeterRecordClear()
{
	int KeyValue = 0;
	KeyValue = MessageBox1("清除全部记录?","取消 确认");
	if(KeyValue == _ENT)
	{
		memset(MRecordBuf,0x0,sizeof(MeterRecordStruct)*RecordNum);//档案记录初始化
		RecordNum = 0;//档案个数初始化
		RecordChangeFlag = 0;
	}
}

void MeterRecordView()
{
	int StartNum = 1;//当前屏幕起始记录序号
	int ChooseNnm = 1;//当前选中记录序号
	int DispRecordNum = 4;
	int KeyValue = 0;
	while(1)
	{
		_cls();
		_gotoxy(0,0);
		_putstr("电表档案");
		_gotoxy(0,1);
		if((RecordNum - StartNum+1)>= 4)
		{
			DispRecordNum = 4;
		}
		else
		{
			DispRecordNum = RecordNum - StartNum+1;
		}
		MeterRecordShow(StartNum,DispRecordNum,ChooseNnm);
		DisplayBott("退出","修改");
		KeyValue = _bioskey(0);
		switch(KeyValue)
		{
			case _UP:
			{
				if(ChooseNnm == 1)
				{
					ChooseNnm = DispRecordNum;
				}
				else
				{
					ChooseNnm -= 1;
				}
				break;
			}
			case _DWN:
			{
				if(ChooseNnm == DispRecordNum)
				{
					ChooseNnm = 1;
				}
				else
				{
					ChooseNnm += 1;
				}
				break;
			}
			case _LFT:
			{
				if(StartNum == 1)
				{
					MessageBox2("已经是第一页");
				}
				else
				{
					StartNum -= 4;
				}
				break;
			}
			case _RGT:
			{
				if((RecordNum -StartNum + 1) <= 4)
				{
					MessageBox2("已经是最后一页");
				}
				else
				{
					StartNum += 4;
				}
				break;
			}
			case _ENT:
			{
				MeterRecordEdit(StartNum+ChooseNnm-2);
				break;
			}
			case _ESC:
				return;
			default:
				MessageBox2("无效按键！");
		}
	}

}

void MeterRecordShow(int RecordNo,int Num,int ChooseNum)
{
	char ShowBuf[30];
	int i;

	for(i = 0; i < Num;i++)
	{
		if(i == ChooseNum - 1)
		{
			_setdispcolor(0);//选中项，反显
		}
		sprintf(ShowBuf,"%d:%02x%02x%02x%02x%02x%02x\n",MRecordBuf[RecordNo+i-1].MeterNo,
				MRecordBuf[RecordNo+i-1].MeterAddr[5],MRecordBuf[RecordNo+i-1].MeterAddr[4],
				MRecordBuf[RecordNo+i-1].MeterAddr[3],MRecordBuf[RecordNo+i-1].MeterAddr[2],
				MRecordBuf[RecordNo+i-1].MeterAddr[1],MRecordBuf[RecordNo+i-1].MeterAddr[0]);
		_putstr((unsigned char*)ShowBuf);
		if(MRecordBuf[RecordNo+i-1].ProtocolType == 1)//645-97规约
		{
			sprintf(ShowBuf,"645-97 ");
		}
		else if(MRecordBuf[RecordNo+i-1].ProtocolType == 30)//645-07规约
		{
			sprintf(ShowBuf,"645-07 ");
		}
		else if(MRecordBuf[RecordNo+i-1].ProtocolType == 60)//MODBUS协议
		{
			sprintf(ShowBuf,"MODBUS ");
		}

		_putstr((unsigned char*)ShowBuf);
		if(MRecordBuf[RecordNo+i-1].BaudRate == 0x40)//1200波特率
		{
			sprintf(ShowBuf,"1200Bps ");
		}
		else if(MRecordBuf[RecordNo+i-1].BaudRate == 0x60)//2400波特率
		{
			sprintf(ShowBuf,"2400Bps  ");
		}/**/
		 _putstr((unsigned char*)ShowBuf);
		 sprintf(ShowBuf,"%d\n",MRecordBuf[RecordNo+i-1].Port);
		 //,MRecordBuf[RecordNo+i-1].BSort, MRecordBuf[RecordNo+i-1].SSort
		 _putstr((unsigned char*)ShowBuf);
		_setdispcolor(1);
	}
}

void MeterRecordAdd()
{
	if(MeterRecordEdit(RecordNum))//按照当前个数确定新增电表序号
	{
		RecordNum++;
	}
	return;
}

int MeterRecordEdit(int RecordNo)
{
	char TmpStrBuff[20] = {"\0"};
	unsigned char length = 0;
	int KeyValue = 0;
	MeterRecordStruct TmpRecord;
	while(1)
	{
		TmpRecord.MeterNo = RecordNo + 1;
		_cls();
		_gotoxy(0,0);
		sprintf(TmpStrBuff,"电表序号%04d",RecordNo+1);
		_putstr((unsigned char*)TmpStrBuff);
		_gotoxy(0,1);
		_putstr("电表地址");
		_gotoxy(0,3);
		_putstr("规约类型");
		_gotoxy(0,4);
		_putstr("1.97 2.07 3.MODBUS");
	   // _gotoxy(0,5);
	   // _putstr("3.MODBUS");
	  //_gotoxy(0,6);
	  //_putstr("1.1200Bps  2.2400Bps");
	  _gotoxy(0,5);
		_putstr("端口号");
		_gotoxy(0,6);
	//	_gotoxy(0,9);
	//	_putstr("用户大类号");
	//	_gotoxy(0,11);
	//	_putstr("用户小类号");
		DisplayBott("退出","保存");
		_gotoxy(0,2);
		length = 12;//电表地址12字节
		KeyValue = Scanf(TmpStrBuff,length);//电表地址一定要输入，因此不设置初始值
		if(KeyValue == _ESC)
		{
			return 0;
		}
		FormatStr(TmpStrBuff,length,'0',0);
		StrtoHex(TmpStrBuff,6,1,0,0);
		memcpy(TmpRecord.MeterAddr,TmpStrBuff,6);
		while(1)//选择规约类型
		{
			_gotoxy(0,4);
			_setdispcolor(0);
			_putstr("1.97 2.07 3.MODBUS");
			_setdispcolor(1);
			KeyValue = _bioskey(0);
			if(KeyValue == _ESC)
			{
				return 0;
			}
			else if(KeyValue == ONE)
			{
				TmpRecord.ProtocolType = 1;
				TmpRecord.BaudRate = 0x40;
				_gotoxy(0,4);
				_clsn(4,1);
				_putstr("645-97");
				break;
			}
			else if(KeyValue == TWO)
			{
				TmpRecord.ProtocolType = 30;
				TmpRecord.BaudRate = 0x60;
				_gotoxy(0,4);
				_clsn(4,1);
				_putstr("645-07");
				break;
			}
			 else if(KeyValue == THREE)
			{
				TmpRecord.ProtocolType = 60;
				TmpRecord.BaudRate = 0x60;
				_gotoxy(0,4);
				_clsn(4,1);
				_putstr("MODBUS");
				break;
			}
			else
			{
				MessageBox2("输入错误！");
			}

		}
		/*while(1)//选择波特率
		{
			_gotoxy(0,6);
			_setdispcolor(0);
			_putstr("1.1200Bps  2.2400Bps");
			_setdispcolor(1);
			KeyValue = _bioskey(0);
			if(KeyValue == _ESC)
			{
				return 0;
			}
			else if(KeyValue == ONE)
			{
				TmpRecord.BaudRate = 0x40;
				_gotoxy(0,6);
				_clsn(6,1);
				_putstr("1200Bps");
				break;
			}
			else if(KeyValue == TWO)
			{
				TmpRecord.BaudRate = 0x60;
				_gotoxy(0,6);
				_clsn(6,1);
				_putstr("2400Bps");
				break;
			}
			else
			{
				MessageBox2("输入错误！");
			}
		}*/
		while(1)
		{
		    _gotoxy(0,6);
		    length = 2;//端口号
		    KeyValue = Scanf(TmpStrBuff,length);
		    if(KeyValue == _ESC)
	    	{
			    return 0;
		    }
		    if(StrToInt(TmpStrBuff,10) > 0 &&  StrToInt(TmpStrBuff,10) < 32)
		    {
				TmpRecord.Port = StrToInt(TmpStrBuff,10);
        TmpRecord.BSort = 5;
        TmpRecord.SSort = 1;
				/*_cls();
				_gotoxy(0,0);
				_putstr("用户大类号");
				_gotoxy(0,2);
				_putstr("用户小类号");*/
				 break;
		    }
		   	MessageBox2("端口输入错误！");
	   }
		/* while(1)
		{
			_gotoxy(0,1);
			length = 2;//用户大类号
			KeyValue = Scanf(TmpStrBuff,length);//用户大类号
			if(KeyValue == _ESC)
    		{
				return;
			}
			if(StrToInt(TmpStrBuff,10) >= 0 &&  StrToInt(TmpStrBuff,10) < 16)
			{
	    		TmpRecord.BSort = StrToInt(TmpStrBuff,10);
	    		 break;
			}
	   		MessageBox2("用户大类号输入错误！");
		}
	    while(1)
		{
		    _gotoxy(0,3);
		    length = 2;//用户小类号
		    KeyValue = Scanf(TmpStrBuff,length);//用户小类号
		    if(KeyValue == _ESC)
	    	{
			    return;
		    }
		    if(StrToInt(TmpStrBuff,10) >= 0 &&  StrToInt(TmpStrBuff,10) < 16)
		    {
		    	TmpRecord.SSort = StrToInt(TmpStrBuff,10);
		    	 break;
		    }
		   	MessageBox2("用户小类号输入错误！");
	    }*/
		if(KeyValue == _ENT)
		{
			TmpRecord.ValidFlag = 1;
			memcpy(&LastRecord,&TmpRecord,sizeof(MeterRecordStruct));
			memcpy(MRecordBuf+RecordNo,&TmpRecord,sizeof(MeterRecordStruct));
			RecordChangeFlag = 0;//置保存标志
			MessageBox2("档案已保存");
			return 1;
		}
		else if(KeyValue == _ESC)
		{
			return 0;
		}
		else
		{
			MessageBox2("输入错误!");
		}
	}

}

