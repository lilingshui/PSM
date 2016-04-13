#pragma once
#include "afxcmn.h"
#include "CheckListCtrl.h"

// CPageNow dialog

class CPageNow : public CDialogEx
{
	DECLARE_DYNAMIC(CPageNow)

public:
	CPageNow(int nDataType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageNow();

// Dialog Data
	enum { IDD = IDD_TABPAGE_NOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual BOOL OnInitDialog();
	
private:	
	int m_nDataType;	
	void InitDataitem();

public:	
	CCheckListCtrl m_ctrlDataItem;

	void GetTime(time_t &tBegin, time_t &tEnd, int &cycle)
	{
		tBegin = 0;
		tEnd = 0;
		cycle = 0;
	}

	void RefreshDataItem(CString strTaskId);
};
