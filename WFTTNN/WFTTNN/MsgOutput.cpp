// MsgOutput.cpp: implementation of the CMsgOutput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsgOutput.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgOutput* CMsgOutput::m_pInstance=NULL;
CMsgOutput::CMsgOutput()
{
	m_pWnd=NULL;
	m_pPacketInfoWnd=NULL;
}

CMsgOutput::~CMsgOutput()
{

}

CMsgOutput *CMsgOutput::GetInstance()
{
	if (m_pInstance==NULL)
		m_pInstance=new CMsgOutput;
	return m_pInstance;
}

void  CMsgOutput::RegisterOperateInfoWnd(CWnd *pWnd)
{
	m_pWnd=pWnd;
}

void  CMsgOutput::RegisterPacketInfoWnd(CWnd *pWnd)
{
	m_pPacketInfoWnd=pWnd;
}

void  CMsgOutput::ShowOperateInfo(CString strName,int nType, CString strInfo)
{
	if (m_pWnd && m_pWnd->GetSafeHwnd())
	{
		OperateInfo *pInfo=new OperateInfo;
		pInfo->strName=strName;
		pInfo->nType=nType;
		pInfo->strInfo=strInfo;
		m_pWnd->PostMessage(OPERATEINFO_EVENT_NOTIFY,0,(long)pInfo);
	}
}

void  CMsgOutput::ShowpPacketInfo(CString strName,int nType, CString strInfo)
{
	if (m_pPacketInfoWnd && m_pPacketInfoWnd->GetSafeHwnd())
	{
		OperateInfo *pInfo=new OperateInfo;
		pInfo->strName=strName;
		pInfo->nType=nType;
		pInfo->strInfo=strInfo;
		m_pPacketInfoWnd->PostMessage(PACKETINFO_EVENT_NOTIFY,0,(long)pInfo);
	}
}

void ShowOperateLog(CString strFrame)
{
	CMsgOutput::GetInstance()->ShowOperateInfo("",0,strFrame);
}

void ShowPacketLog(CString strFrame)
{
	CMsgOutput::GetInstance()->ShowpPacketInfo("",0,strFrame);
}