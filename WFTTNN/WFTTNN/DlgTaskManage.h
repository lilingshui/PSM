#pragma once
#include "afxcmn.h"
#include "PageNow.h"
#include "PageHistory.h"
#include "PageEvent.h"

// CDlgTaskManage 对话框

class CDlgTaskManage : public CDialog
{
	DECLARE_DYNAMIC(CDlgTaskManage)

public:
	CDlgTaskManage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTaskManage();

// 对话框数据
	enum { IDD = IDD_FORM_TASK_MANAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	afx_msg void OnTcnSelchangeTabDataType(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedAdd();

	DECLARE_MESSAGE_MAP()

private:		
	void InitTaskList();	
	void AddOneTaskItemToDB(int nTaskId, CString strItemId, CString strItemName, int nType, time_t tBegin, time_t tEnd, int nCycle);

	CTabCtrl m_tabDataType;
	CListCtrl m_ctrlTaskList;
	CPageNow *m_pPageNow;
	CPageHistory *m_pPageQX;
	CPageHistory *m_pPageRDJ;
	CPageHistory *m_pPageYDJ;
	CPageEvent *m_pPageEvent;
	CPageNow *m_pPageParam;
	CPageEvent *m_pPageAlarm;
	CString m_strTaskName;
public:
	afx_msg void OnNMDblclkListTask(NMHDR *pNMHDR, LRESULT *pResult);
};
