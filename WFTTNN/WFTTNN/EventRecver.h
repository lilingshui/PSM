#pragma once

enum Event_Type {
	Event_Type_Item_Double_Click = 1000, 
};

class CEventRecver
{
public:
	CEventRecver(void);
	virtual ~CEventRecver(void);
	
	virtual void OnEvent(unsigned int nEventID,	void *pParam) = 0;

};
