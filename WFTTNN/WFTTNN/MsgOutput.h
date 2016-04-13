// MsgOutput.h: interface for the CMsgOutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGOUTPUT_H__D3B395D8_8FDB_4137_8A8E_4931DC6DFE27__INCLUDED_)
#define AFX_MSGOUTPUT_H__D3B395D8_8FDB_4137_8A8E_4931DC6DFE27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define OPERATEINFO_EVENT_NOTIFY		WM_USER+1
#define PACKETINFO_EVENT_NOTIFY			WM_USER+2
#define RNDREADPACKET_EVENT_NOTIFY		WM_USER+3
#define SETPARAMPACKET_EVENT_NOTIFY		WM_USER+4
#define AUTOCESHIPACKET_EVENT_NOTIFY	WM_USER+5


struct OperateInfo
{
	CString strName;
	int nType;
	CString strInfo;
};

class CMsgOutput  
{
public:
	CMsgOutput();
	virtual ~CMsgOutput();
	static CMsgOutput *GetInstance();
	void  RegisterOperateInfoWnd(CWnd *pWnd);
	void  RegisterPacketInfoWnd(CWnd *pWnd);
	void  ShowOperateInfo(CString strName,int nType, CString strInfo);
	void  ShowpPacketInfo(CString strName,int nType, CString strInfo);
private:
	static CMsgOutput* m_pInstance;
	CWnd   *m_pWnd;
	CWnd   *m_pPacketInfoWnd;
};

void ShowOperateLog(CString strFrame);

void ShowPacketLog(CString strFrame);

#endif // !defined(AFX_MSGOUTPUT_H__D3B395D8_8FDB_4137_8A8E_4931DC6DFE27__INCLUDED_)
