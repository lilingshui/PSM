#ifndef _GLOBAL_H
#define _GLOBAL_H

/******************************************************************************/
/*                              特殊数字宏定义                                */ 
/******************************************************************************/
#define NUM_1 (1)
#define NUM_2 (2)
#define NUM_3 (3)
#define NUM_4 (4)
#define NUM_5 (5)
#define NUM_6 (6)
#define NUM_7 (7)
#define NUM_8 (8)
#define NUM_9 (9)
#define NUM_10 (10)
#define NUM_11 (11)
#define NUM_12 (12)
#define NUM_13 (13)
#define NUM_14 (14)
#define NUM_15 (15)
#define NUM_16 (16)
#define NUM_17 (17)
#define NUM_18 (18)
#define NUM_19 (19)
#define NUM_20 (20)
#define NUM_21 (21)
#define NUM_22 (22)
#define NUM_23 (23)
#define NUM_24 (24)
#define NUM_25 (25)
#define NUM_26 (26)
#define NUM_27 (27)
#define NUM_28 (28)
#define NUM_29 (29)
#define NUM_30 (30)
#define NUM_31 (31)
#define NUM_32 (32)
#define NUM_50 (50)
#define NUM_64 (64)
#define NUM_100 (100)
#define NUM_128 (128)
#define NUM_200 (200)
#define NUM_255 (255)
#define NUM_400 (400)
#define NUM_600 (600)
#define NUM_800 (800)
#define NUM_1000 (1000)
#define NUM_1500 (1500)
#define NUM_2000 (2000)

#define ERROR (-1)
#define ERR_DENY (1)
#define ERR_FAIL (2)
#define ERR_NULL (3)
#define ERR_PARSE (4)
#define ERR_LOGIN (5)

#define DEC NUM_10
#define HEX NUM_16

#define WS_PAGEENABLE		0x002000  //多行菜单翻页
#define MAX_FILENAME_LEN 20
#define MAX_FILE_LEN NUM_20

#define MAX_DEBGFILE_LEN 261120
/******************************************************************************/


/******************************************************************************/
/*                        DBF数据库字段宏定义                                 */ 
/******************************************************************************/
#define FD_FLAG 0
#define FD_TERMNO 1
#define FD_TERMADDR 2
#define FD_METERNO 3
#define FD_METERADDR 4
#define FD_COLLNO 5
#define FD_COLLADDR 6
#define FD_DEVNO 7
#define FD_MPNO 8
#define FD_PORT 9
#define FD_BAUD 10
#define FD_PROTO 11
#define FD_CKRES 12
#define FD_ENERGY 13
#define FD_RATES 14
#define FD_SORT 15
#define FD_RTDATA 16
#define FD_DAYDATA 17
#define FD_RESERVE 18

#define FD_ADDRTYPE 3

#define FD_TERMCHECK 0
#define FD_METERCHECK 1
#define FD_ADDRDEC 2
#define FD_GETTERMRULE 3
#define FD_GETMETERULE 7
#define FD_GETCOLLRULE 11
#define FD_GETTERM 15
#define FD_GETMETER 16
#define FD_GETCOLLECT 17
#define FD_COMMPORT 18
#define FD_COMMBAUD 19
#define FD_NWTYPE 20
#define FD_WLSTYPE 21
#define FD_SETRECDNUM 22

#define FD_PARAMTYPE 0
#define FD_PARAMID 1
#define FD_PARAMAFN 2
#define FD_PARAMPN 3
#define FD_PARAMFN 4
#define FD_PARAMNAME 5
#define FD_PARAMITEM 6
#define FD_PITEMTYPE 7
#define FD_PITEMVALUE 8
#define FD_PITEMLEN 9
#define FD_PARAMVALUE 10

/******************************************************************************/


/******************************************************************************/
/*                           常用波特率宏定义                                 */ 
/******************************************************************************/
#define BAUD_600 (600)
#define BAUD_1200 (1200)
#define BAUD_2400 (2400)
#define BAUD_4800 (4800)
#define BAUD_7200 (7200)
#define BAUD_9600 (9600)
#define BAUD_19200 (19200)
#define BAUD_38400 (38400)
#define BAUD_57600 (57600)
#define BAUD_115200 (115200)

#define PORT_IR 0x05
#define DELAY_OPENPORT 150
/******************************************************************************/


/******************************************************************************/
/*                          常用文件名或字符串宏定义                          */ 
/******************************************************************************/
#define SYSCONFDBF ("sysconf.dbf")
#define ASSETDBF ("asset.dbf")
#define PARAMDBF_GW ("param376.dbf")
#define TERM_RECD ("termrecd.dbf")
#define TERM_RECD_NEW ("termrecd_new.dbf")
#define TERM_RECD_BAK ("termrecd_bak.dbf")
#define METE_RECD_GW ("meterecd.dbf")
#define METE_RECD_NEW_GW ("meterecd_new.dbf")
#define METE_RECD_BAK_GW ("meterecd_bak.dbf")
#define METE_RECD_RCV_GW ("meterecd_rcv.dbf")
#define METE_DATA_RCV_GW ("mpdata_rcv.dbf")
#define DEBEGINFOTXT ("debuginfo.txt")
#define TEMPPARAMTXT ("tempparam.txt")

#define PARAMDBF ("param.dbf")
#define VERSIONMIF ("GW376.mif")
#define MAINFORMNAME ("国网集抄系统")
#define SEND_OK ("发送成功!")
#define SEND_NULL ("无返回数据!")
#define SEND_FAIL ("返回数据异常!")
#define SEND_DENY ("终端否认!")
#define PARSE_ERR ("解析失败!")
#define STR_CUE ("提示")
#define STR_WARN ("警告")
#define STR_RES_SET ("设置结果")
#define STR_RES_READ ("抄读结果")
#define STR_NL ("\r\n")
#define STR_OK ("成功")
#define STR_FAIL ("失败")
#define STR_DEL ("删除")
#define STR_NULL ("无效")
#define SEPCHAR (',')
#define NULLCHAR 0xEE

#define STR_PROTO_97 ("DL/T 645-1997")
#define STR_PROTO_07 ("DL/T 645-2007")
#define STR_PROTO_SH ("上海规约")
/******************************************************************************/


/******************************************************************************/
/*        DL/T645协议,国网体系规约,浙电体系规约帧头帧尾字符宏定义             */ 
/******************************************************************************/
#define BUFF_FRONT 0xFE
#define BUFF_START 0x68
#define BUFF_END 0x16
#define BUFF_FRONT_NUM 0
#define DATA645DEAL (0x33)

#define MAXSENDBYTES NUM_400
#define MAXRECVBYTES NUM_400
#define MINSENDBYTES NUM_255
#define MINRECVBYTES NUM_100

#define MAXRECVDELAY NUM_400
#define MINRECVDELAY NUM_255

#define MIN698BUFFLEN 14
#define MAX_SENDRECDNUM_IR 8
#define MAX_SENDRECDNUM_485 32

#define MAX_READMETERNUM 8
/******************************************************************************/


/******************************************************************************/
/*                         DL/T698协议特殊字符宏定义                          */ 
/******************************************************************************/
#define AFN_698SET 0x04
#define AFN_698CMD 0x05
#define AFN_698CONF 0x09
#define AFN_698READ 0x0A
#define AFN_698RTDATA 0x0C
#define AFN_698HISDATA 0x0D
#define AFN_698FWDCMD 0x10

#define MAX_MPNO 2040

#define FN_1 1
#define FN_2 2
#define FN_3 3
#define FN_4 4
#define FN_5 5
#define FN_6 6
#define FN_7 7
#define FN_8 8
#define FN_9 9
#define FN_RECD 10
#define FN_11 11
#define FN_33 33
#define FN_85 85
#define FN_89 89
#define FN_100 100
#define FN_129 129
#define FN_161 161

#define C_698READ 0x4B
#define C_698SET 0x4A
#define C_698REBOOT 0x41

#define I_ADDR NUM_7
#define ADDRBYTES NUM_5
#define PNBYTES NUM_2
#define FNBYTES NUM_2
#define CSBYTES 1
#define ENDBYTES 1

#define INI_698SEQ 0xE8
#define MAX_SENDRECD_NUM NUM_8
#define MAX_READRECD_NUM NUM_6

#define MAX_ASSETNO_DIGITS NUM_23
#define BATCH_SENDRECD_NUM NUM_10

#define TYPE_READSET 0x01     		//Terminal Param Type--Read & Set 
#define TYPE_SETONLY 0x02     		//Terminal Param Type--Only Set
#define TYPE_READONLY 0x03    		//Terminal Param Type--Only Read
#define TYPE_READ 0x13        		//Terminal Param Type--Read
#define TYPE_SET 0x12         		//Terminal Param Type--Set
//#define TYPE_SINGLE 0xFF      	//Terminal Param Type--Single Param Item


/******************************************************************************/
/*                         DL/T645协议特殊字符宏定义                          */ 
/******************************************************************************/
#define PROTO_97 NUM_1
#define PROTO_07 NUM_30
#define PROTO_SH NUM_3
#define METEADDR_LEN NUM_6
#define MAX_ULEN NUM_255

#define C_READ 0x00
#define C_READ_OK 0x80
#define C_READ_ERR 0xC0

#define C_READ97 0x01
#define C_READ97_OK 0x81
#define C_READ97_ERR 0xC1

#define C_READ07 0x11
#define C_READ07_OK 0x91
#define C_READ07_ERR 0xD1

#define C_SET 0x00
#define C_SET_OK 0x80
#define C_SET_ERR 0xC0

#define C_SET07 0x14
#define C_SET07_OK 0x94
#define C_SET07_ERR 0xD4

#define DI_BYTES NUM_2
#define DI07_BYTES NUM_4
#define MIN_DI_JS 0x04A00101
#define DI_METERECORD_JS 0x04A00301
#define DI_PARAMEVENT_QH 0x04A00601
#define DI_READFAIL_QH 0x04A00602
#define DI_LOGIN_QH 0x04A00604
#define DI_SETPW_QH 0x04A00501

#define TYPE_MP_READ 0x59        	//Meter Data --Read
#define TYPE_MP_READ97 0x05      	//Meter Data Now--Read (DL/T645 1997)
#define TYPE_MP_DATA_MON97 0x06  	//Meter Data Month--Read (DL/T645 1997)
#define TYPE_MP_DATA_DAY97 0x07  	//Meter Data Day--Read (DL/T645 1997)
#define TYPE_MP_DATA_HOU97 0x08  	//Meter Data Hour--Read (DL/T645 1997)
#define TYPE_MP_READ07 0x09      	//Meter Data Now--Read (DL/T645 2007)
#define TYPE_MP_DATA_MON07 0x0A  	//Meter Data Month--Read (DL/T645 2007)
#define TYPE_MP_DATA_DAY07 0x0B  	//Meter Data Day--Read (DL/T645 2007)
#define TYPE_MP_DATA_HOU07 0x0C  	//Meter Data Hour--Read (DL/T645 2007)

#define TYPE_MP_READSH 0x0D      //Meter Data Now--Read (上海表规约)

/******************************************************************************/


/******************************************************************************/
/*                         DL/T645协议，DL/T698协议公共结构                   */ 
/******************************************************************************/
#define MAX_PARAMITEM_NUM NUM_32
#define MAX_SUBITEM_NUM NUM_32

typedef struct
{
  U8 Ccode;									//控制域
  U8 Saddr[6];							//地址域
  U8 AFN;										//应用层功能码
  U8 SEQ;										//帧序列号
  U16 Pn;										//信息点
  U16 Fn;										//信息类
  U8 Userbuf[1024];					//用户数据
  U16 Ulen;									//用户数据长度
}Stru698Data;

typedef struct
{
  U8 protype;								//规约类型
  U8 address[13];						//地址域
  U8 ctrcode;								//控制码
  U32 dataitm;  						//数据标识
  U8 userbuf[MAX_ULEN];			//用户数据
  U8 datalen;								//数据长度 全部长度
}Stru645Data;								

typedef struct{
  char Term[10];						//所属终端
  short int DevNo;					//装置序号
  short int MeterNo;				//测量点号
  char MeterAddr[13];				//电表地址
  char ColAddr[13];					//采集器地址
  unsigned char ProtoType;	//规约类型
  int BaudRate;							//波特率
  unsigned char Port;				//端口号
  unsigned char Rates; 			//费率数
  unsigned char Sort;				//大类小类号
}StruMeterRecord;

//参数与数据库关联结构
typedef struct
{
	U32 Param_ID;										//参数ID或索引
	U8 Param_Item_No;								//参数子项序号
	char Param_Item_Name[32];				//参数子项名称
	U8 Param_Item_Mask;							//参数子项类型
	char Param_Item_Value[128];			//参数子项格式
	U8 Param_Item_Clen;							//参数子项数据长度
	char Param_Value[128];					//参数子项内容
}Stru_ParamItem;
typedef struct
{
	U8 Type;									//参数类型
	U32 Param_ID;							//参数ID或索引
	U8 AFN;										//应用层功能码
	U16 Pn;										//信息点标识
	U8 Fn;										//信息类
	char Param_Name[32];			//参数名称关联菜单显示文本
	U8 Param_Num;							//参数子项个数
	U8 Rn;										//参数在数据库做记录号
	U8 Flag;									//当前抄读标志             
}Stru_Param;

Stru_Param GlobalParam[MAX_PARAMITEM_NUM];
Stru_ParamItem GlobalParamItem[MAX_SUBITEM_NUM];

/******************************************************************************/
#endif