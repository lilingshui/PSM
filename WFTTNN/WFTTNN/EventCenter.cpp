#include "StdAfx.h"
#include "Eventcenter.h"
#include "EventRecver.h"

CEventCenter::CEventCenter(void)
{
	
}

CEventCenter::~CEventCenter(void)
{
}

void CEventCenter::AddRecver(CEventRecver *pRecver)
{
	if (pRecver != NULL)
	{
		int nCount = (int)m_vRecver.size();
		for(int n=0;n<nCount;n++)
		{
			if (pRecver == m_vRecver[n])
				return;
		}
		m_vRecver.push_back(pRecver);
	}
}

void CEventCenter::RemoveRecver(CEventRecver *pRecver)
{
	int nCount = (int)m_vRecver.size();
	for(int n=0;n<nCount;n++)
	{
		if (pRecver == m_vRecver[n])
		{
			m_vRecver.erase(m_vRecver.begin() + n);
			return;
		}
	}
}

void CEventCenter::DeliveryEvent(unsigned int nEventID, void *pParam)
{
	int nCount = (int)m_vRecver.size();
	for(int n=0;n<nCount;n++)
	{
		CEventRecver *pRecver = m_vRecver[n];
		if (pRecver != NULL)
		{
			pRecver->OnEvent(nEventID, pParam);
		}
	}
}