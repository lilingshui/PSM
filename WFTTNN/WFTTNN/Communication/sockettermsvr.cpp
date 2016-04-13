// sockettermsvr.cpp : implementation file
//

#include "stdafx.h"
//#include "..\ComTest\comtest.h"
#include "sockettermsvr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSocketTermSvr

CSocketTermSvr::CSocketTermSvr()
{
	m_connect_step = CONNECT_STEP_NONE;
}

CSocketTermSvr::~CSocketTermSvr()
{

}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CSocketTermSvr, CSocket)
	//{{AFX_MSG_MAP(CSocketTermSvr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CSocketTermSvr member functions

void CSocketTermSvr::OnConnect(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(nErrorCode == 0)
	{
		m_connect_step = CONNECT_STEP_CONNECTED_TERM;		
	}
	else
	{
		m_connect_step = CONNECT_STEP_CONNECTED_TERM_FAIL;		
	}
	OnConnectTCP(nErrorCode);

	CSocket::OnConnect(nErrorCode);
}

void CSocketTermSvr::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	CSocket::OnReceive(nErrorCode);
}

void CSocketTermSvr::OnSend(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class	
	CSocket::OnSend(nErrorCode);
}

void CSocketTermSvr::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	CSocket::OnClose(nErrorCode);
}

int CSocketTermSvr::ConnectTermSvr()
{
	m_connect_step = CONNECT_STEP_CONNECTING_TERM;
	if(Create())
	{
		AsyncSelect(); 
		return Connect(m_ip,m_port);
	}
	return FALSE;	
}

void CSocketTermSvr::OnConnectTCP(int nErrorCode)
{
		
}
