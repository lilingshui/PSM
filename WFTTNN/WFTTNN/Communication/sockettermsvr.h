#if !defined(AFX_SOCKETTERMSVR_H__5AF09FF2_FA26_49EB_8873_03487AB89989__INCLUDED_)
#define AFX_SOCKETTERMSVR_H__5AF09FF2_FA26_49EB_8873_03487AB89989__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// sockettermsvr.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CSocketTermSvr command target
#define CONNECT_STEP_NONE                   0       //无状态
#define CONNECT_STEP_CONNECTING_TERM        1       //正在连接终端服务器
#define CONNECT_STEP_CONNECTED_TERM         2       //连接终端服务器成功
#define CONNECT_STEP_CONNECTED_TERM_FAIL    3       //连接终端服务器成功

class CSocketTermSvr : public CSocket
{
// Attributes
public:

// Operations
public:
	CSocketTermSvr();
	virtual ~CSocketTermSvr();

// Overrides
public:
	virtual void OnConnectTCP(int nErrorCode);
	int ConnectTermSvr();

	int m_connect_step;
	UINT m_port;
	CString m_ip;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketTermSvr)
	public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSocketTermSvr)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKETTERMSVR_H__5AF09FF2_FA26_49EB_8873_03487AB89989__INCLUDED_)
