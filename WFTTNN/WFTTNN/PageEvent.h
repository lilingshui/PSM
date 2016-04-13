#pragma once
#include "afxcmn.h"
#include "CheckListCtrl.h"
#include "atltime.h"

// CPageEvent dialog

class CPageEvent : public CDialogEx
{
	DECLARE_DYNAMIC(CPageEvent)

public:
	CPageEvent(int nDataType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageEvent();

// Dialog Data
	enum { IDD = IDD_TABPAGE_EVENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CTime m_ctBeginDate;
	CTime m_ctBeginTime;
	CTime m_ctEndDate;
	CTime m_ctEndTime;
	int m_nDataType;
	void InitDataitem();

public:	
	CCheckListCtrl m_ctrlDataItem;
	void GetTime(time_t &tBegin, time_t &tEnd, int &cycle);
	void RefreshDataItem(CString strTaskId);	
};
