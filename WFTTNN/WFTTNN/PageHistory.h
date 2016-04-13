#pragma once
#include "afxcmn.h"
#include "atltime.h"
#include "afxwin.h"
#include "CheckListCtrl.h"

// CPageHistory dialog

class CPageHistory : public CDialogEx
{
	DECLARE_DYNAMIC(CPageHistory)

public:
	CPageHistory(int nDataType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageHistory();

// Dialog Data
	enum { IDD = IDD_TABPAGE_HISTORY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual BOOL OnInitDialog();

private:	
	CTime m_tBeginDate;
	CTime m_tBeginTime;
	CTime m_tEndDate;
	CTime m_tEndtime;
	CComboBox m_cbCycle;
	int m_nDataType;	
	void InitDataitem();

public:
	CCheckListCtrl m_ctrlDataItem;
	void GetTime(time_t &tBegin, time_t &tEnd, int &cycle);
	void RefreshDataItem(CString strTaskId);
};
