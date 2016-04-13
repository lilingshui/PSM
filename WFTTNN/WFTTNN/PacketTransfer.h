// MsgOutput.h: interface for the CMsgOutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGOUTPUT_H__D3B395D8_8FDB_4137_8A8E_4931DC6DFE271__INCLUDED_)
#define AFX_MSGOUTPUT_H__D3B395D8_8FDB_4137_8A8E_4931DC6DFE271__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define RNDREADPACKET_EVENT_NOTIFY		WM_USER+1
#define SETPARAMPACKET_EVENT_NOTIFY		WM_USER+2
#define AUTOCESHIPACKET_EVENT_NOTIFY	WM_USER+3

typedef struct _FRAME_BUFFER
{
	BYTE buf[2048];
	int  len;
}FRAME_BUFFER;


class CPacketTransfer
{
public:
	CPacketTransfer();
	virtual ~CPacketTransfer();
	static CPacketTransfer *GetInstance();
	void  RegisterRndReadPacketWnd(CWnd *pWnd);
	void  RegisterSetParamPacketWnd(CWnd *pWnd);
	void  RegisterAutoCeShiPacketWnd(CWnd *pWnd);

	void  RndReadPacketTransfer(FRAME_BUFFER *fb);
	void  SetParamPacketTransfer(FRAME_BUFFER *fb);
	void  AutoCeShiPacketTransfer(FRAME_BUFFER *fb);
	
private:
	static CPacketTransfer* m_pInstance;
	CWnd   *m_pRndReadPacketWnd;
	CWnd   *m_pSetParamPacketWnd;
	CWnd   *m_pAutoCeShiPacketWnd;
};

//void ShowPacketLog(CString strFrame);

#endif // !defined(AFX_MSGOUTPUT_H__D3B395D8_8FDB_4137_8A8E_4931DC6DFE27__INCLUDED_)
