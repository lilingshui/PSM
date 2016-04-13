#ifndef COM_H
#define COM_H

//#pragma pack(1)
int InitCom();
/////////////////////////////////////////////////////////////////////////////
// CComWnd window

//#define WM_COM_EVENT_INFO                         WM_USER + 9000

class CCom;
class CComWnd : public CWnd
{
// Construction
public:
	CComWnd();
	
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	int PostMessageComEvent(CCom * pCom,DWORD event);
	virtual ~CComWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void OnMsgComEvent(WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////
//     CCom
class CCom 
{
	friend class CComWnd;  
public:
	virtual void ProcessComEventInThread(DWORD dwEvtMask);
	
	CString m_errorInfo;
    CString GetLastErrorInfo();
	
	CCom();
	virtual void OnReceive();
	virtual BOOL Connect()=0;
	void WatchComProc();
	virtual int OnError(DWORD err);

	CString m_sComName;
	BYTE m_StopBits;
	BYTE m_Parity;
	BYTE m_ByteSize;
	DWORD m_BaudRate;

	int IsOpen;
	
	DWORD Read(BYTE * pBuff,DWORD NumRead);
	DWORD Write(BYTE * buff,DWORD bufflen,BOOL bOver = FALSE);
	virtual void Close();	
	
	int Open();
	virtual ~CCom();
	void InitComInfo(char * sComName,WORD baudRate,BYTE stopBits,BYTE parity,BYTE byteSize);
protected:
	short m_reqthreadexitcount;
	int CloseEvent();
	int CloseThread();
	int InitThread();
	int InitEvent();
	int ClearBuffError();
	HANDLE hComDev;	
	OVERLAPPED read_os;
	OVERLAPPED os;
	OVERLAPPED write_os;
	virtual void GetCommMask(DWORD & mask);
	void PostMsgComEvent (int event);
	BOOL bConnected;
	HANDLE m_hResponseEvent;
	BOOL ThreadExit;
	HANDLE hThreadWatchComm;
	DWORD Thread_TimeOut_id;
	void WaitResponseEvent();
	void SetResponseEvent();
	void ResetResponseEvent();
	virtual void ProcessMsgComEvent(DWORD eventid);	

	
};


#endif
/////////////////////////////////////////////////////////////////////////////
