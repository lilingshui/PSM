#pragma once


// CTreeListHeaderCtrl
//#include "CommonUI_Global.h"

class CTreeListHeader : public CXTFlatHeaderCtrl
{
	DECLARE_DYNAMIC(CTreeListHeader)

public:
	CTreeListHeader();
	virtual ~CTreeListHeader();


protected:
	DECLARE_MESSAGE_MAP()

protected:
	
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int GetColumnsWidth();
	int GetColumnWidth(int nIndex);
	int GetColumnIndex(int xPos);
};


