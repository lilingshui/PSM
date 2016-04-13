#pragma once
#include "afxcmn.h"
class CCheckListCtrl :
	public CListCtrl
{
public:
	CCheckListCtrl(void);
	~CCheckListCtrl(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuitemSelectall();
	afx_msg void OnMenuitemSelectinvert();
};

