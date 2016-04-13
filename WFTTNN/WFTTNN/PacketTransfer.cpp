// MsgOutput.cpp: implementation of the CMsgOutput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PacketTransfer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacketTransfer* CPacketTransfer::m_pInstance=NULL;

CPacketTransfer::CPacketTransfer()
{
	m_pRndReadPacketWnd = NULL;
	m_pSetParamPacketWnd = NULL;
	m_pAutoCeShiPacketWnd = NULL;
}

CPacketTransfer::~CPacketTransfer()
{
}

CPacketTransfer *CPacketTransfer::GetInstance()
{
	if (m_pInstance==NULL)
		m_pInstance=new CPacketTransfer;
	return m_pInstance;
}

void CPacketTransfer::RegisterRndReadPacketWnd(CWnd *pWnd)
{
	m_pRndReadPacketWnd = pWnd;
}

void CPacketTransfer::RegisterSetParamPacketWnd(CWnd *pWnd)
{
	m_pSetParamPacketWnd = pWnd;
}

void CPacketTransfer::RegisterAutoCeShiPacketWnd(CWnd *pWnd)
{
	m_pAutoCeShiPacketWnd = pWnd;
}

void CPacketTransfer::RndReadPacketTransfer(FRAME_BUFFER *fb)
{
	if (m_pRndReadPacketWnd && m_pRndReadPacketWnd->GetSafeHwnd())
	{
		m_pRndReadPacketWnd->PostMessage(RNDREADPACKET_EVENT_NOTIFY,0,(long)fb);
	}
}

void CPacketTransfer::SetParamPacketTransfer(FRAME_BUFFER *fb)
{
	if (m_pSetParamPacketWnd && m_pSetParamPacketWnd->GetSafeHwnd())
	{
		m_pSetParamPacketWnd->PostMessage(SETPARAMPACKET_EVENT_NOTIFY,0,(long)fb);
	}
}

void CPacketTransfer::AutoCeShiPacketTransfer(FRAME_BUFFER *fb)
{
	if (m_pAutoCeShiPacketWnd && m_pAutoCeShiPacketWnd->GetSafeHwnd())
	{
		m_pAutoCeShiPacketWnd->PostMessage(AUTOCESHIPACKET_EVENT_NOTIFY,0,(long)fb);
	}
}
/*
void ShowPacketLog(CString strFrame)
{
	//CPacketTransfer::GetInstance()->ShowpPacketInfo("",0,strFrame);
}
*/