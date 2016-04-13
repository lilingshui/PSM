// RS232.h: interface for the CRS232 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RS232_H__5B5195E5_C093_4E17_8C66_E6509A068C8E__INCLUDED_)
#define AFX_RS232_H__5B5195E5_C093_4E17_8C66_E6509A068C8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Communication\Com.h"

#define COM_EVT_RECV               10

class CRS232 : public CCom  
{
public:
	virtual void ProcessComEventInThread(DWORD dwEvtMask);
	virtual void ProcessMsgComEvent(DWORD eventid);
	virtual void Close();
	CRS232();
	virtual BOOL Connect();

	virtual  void OnReceive();


	virtual ~CRS232();
private:
	 
protected:
	virtual void GetCommMask(DWORD & mask);
};

#endif // !defined(AFX_RS232_H__5B5195E5_C093_4E17_8C66_E6509A068C8E__INCLUDED_)
