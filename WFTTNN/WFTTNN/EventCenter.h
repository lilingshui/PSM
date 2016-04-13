#pragma once

#include "EventRecver.h"
#include <vector>
using namespace std;


class CEventCenter
{
public:
	CEventCenter(void);
	virtual ~CEventCenter(void);
	
	void AddRecver(CEventRecver *pRecver);
	void RemoveRecver(CEventRecver *pRecver);

protected:
	vector <CEventRecver *> m_vRecver;

	void DeliveryEvent(unsigned int nEventID, void *pParam);
};
