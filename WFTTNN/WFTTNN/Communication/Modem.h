// ModemCom.h: interface for the CModem class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MODEMCOM_H__5B029E30_BFC7_4C82_B0E0_3667B3612C2F__INCLUDED_)
#define AFX_MODEMCOM_H__5B029E30_BFC7_4C82_B0E0_3667B3612C2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#pragma pack(1) 
#include "..\Communication\Com.h"

#define COM_EVT_RECV     10
#define COM_EVT_SEND     11
#define COM_EVT_RLSD     12
#define COM_EVT_RING     13

#define CONNECT_STEP_INIT_MODEM             10       //初始化MODEM
#define CONNECT_STEP_DIALING                11       //正在拨号
#define CONNECT_STEP_DIAL_SUCCESS           12       //拨号成功
#define CONNECT_STEP_DIAL_FAIL              13       //拨号失败

#define CONNECT_STEP_TURNING_ORDER          14       //正在转向命令方式
#define CONNECT_STEP_HUNGINGUP              15       //正在挂断
#define CONNECT_STEP_HUNGEDUP               16       //挂断成功

#define DIAL_METHOD_TONE                    0		 //音频拨号
#define DIAL_METHOD_PLUSE                   1		 //脉冲拨号

#define  CONN_RESULT_RING                 2     //振铃  
#define  CONN_RESULT_NODIALONE            6     //没有拨号音
#define  CONN_RESULT_BUSY                 7     //线路忙
#define  CONN_RESULT_NOANSWER             8     //无回应
#define  CONN_RESULT_NOCARRIER            3     //载波消失或者未侦测载波
#define  CONN_RESULT_ERROR                4     //错误
#define  CONN_RESULT_DELAYED              24      //不能连
#define  CONN_RESULT_BLACKLISTED          32      //不能拨
#define  CONN_RESULT_LINE_USER            83      //线路在使用

class CModem : public CCom   
{
public:
	virtual void Close();
	void InitModemInfo(char * sComName,WORD baudRate,BYTE stopBits,BYTE parity,BYTE byteSize,BYTE dialMode,char *initStr,char *szCall);
	virtual int  OnConnecting(DWORD connstate,CString sResult,BYTE nResult);
	virtual int  OnInitResult(DWORD result, CString sResult);
	int m_connect_step;
	int ProcessResult(DWORD connstate, CString sResult, BYTE nResult);
	virtual void ProcessComEventInThread(DWORD dwEvtMask);
	void OnResponse();
	void ProcessData();
	virtual BOOL Connect();
	BOOL Dial();
	BOOL InitModem();
	BYTE m_dialMode;

 


	CString m_initStr;
	CString m_sCallTel;

	BYTE IsConnect;
	CModem();
	virtual ~CModem();
	int CloseModem();
protected:
	virtual void ProcessMsgComEvent(DWORD eventid);
	virtual void GetCommMask(DWORD & mask);
	CString m_replayInfo;
	

private:
	void SeekConnect();
	int JudgeResult();
	
};

#endif // !defined(AFX_MODEMCOM_H__5B029E30_BFC7_4C82_B0E0_3667B3612C2F__INCLUDED_)
