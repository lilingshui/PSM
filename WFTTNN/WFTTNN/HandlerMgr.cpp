#include "StdAfx.h"
#include "HandlerMgr.h"

CHandlerMgr *CHandlerMgr::m_pInstance=NULL;
CHandlerMgr::CHandlerMgr(void)
{
}

CHandlerMgr::~CHandlerMgr(void)
{
}

CHandlerMgr *CHandlerMgr::GetInstance()
{
	if (m_pInstance==NULL)
		m_pInstance=new CHandlerMgr;
	return m_pInstance;
}

void CHandlerMgr::TransferToFep(FRAME_BUF fb)
{
	for (UINT i=0;i<m_vAcceptChannel.size();i++)
	{
		unsigned char pBuff[1024] = 
		{0x68, 0x11, 0x00, 0x11, 0x00, 0x68, 0x00, 0x00, 0x31, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x79, 0x16};

		FRAME_BUF fb;
		memcpy(fb.buf, pBuff, 25);
		fb.len = 25;
		
		m_vAcceptChannel[i]->GetStream().Send(fb);
	}

}

void CHandlerMgr::TransferToMaster(FRAME_BUF fb)
{
	ShowPacketLog(TraceBuf(fb.buf,fb.len,"·¢ËÍ"));
	for (UINT i=0;i<m_vAcceptChannel.size();i++)
	{
		m_vAcceptChannel[i]->GetStream().Send(fb);
	}
	
}