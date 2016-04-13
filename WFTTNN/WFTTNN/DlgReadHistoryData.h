#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "EventRecver.h"
#include "ptcl_codec/data_format_csg.h"
#include "task.h"
#include <queue>
#include "CheckListCtrl.h"
#include "Channel/ChannelReceiver.h"
#include "Channel/ChannelMgr.h"
#include "afxdtctl.h"

// CDlgReadHistoryData 对话框

class CDlgReadHistoryData : public CDialog, public CEventRecver, public CChannelReceiver
{
	DECLARE_DYNAMIC(CDlgReadHistoryData)

public:
	typedef struct DataTypeParam
	{
		int type;
		CString desc;
	};
	void InitMeterList();

private:
	void InitDataItem();
	void InitDayDataItem();
	void InitMonDataItem();	
	int m_nTermNo;
	CString m_strTerminalAddr;
	std::queue<Task> m_qTask;
	int m_taskState;
	time_t m_taskTime;
	int m_timeout;
	int m_retryCount;
	ptcl_packet_csg packet;
	CChannel *m_pChannel;
	CLayoutCtrl m_layoutCtrl;
	std::map <CString,DataTypeParam> m_mDataType;

public:
	CDlgReadHistoryData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgReadHistoryData();

// 对话框数据
	enum { IDD = IDD_FORM_READ_HISTORY_DATA };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual void OnEvent(unsigned int nEventID,	void *pParam);
	virtual void OnReceive(int state, char *buf, int len, u_int64 strTermAddr);	

	DECLARE_MESSAGE_MAP()

private:
	CCheckListCtrl m_ctrlDataItemList;
	CCheckListCtrl m_ctrlMeterList;
	CComboBox m_cmbMeterList;
	CListCtrl m_ctrlOptResult;
	afx_msg void OnBnClickedBtnStart();		
	CProgressCtrl m_pcHistory;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CDateTimeCtrl m_ctrlDate_start;
	CDateTimeCtrl m_ctrlTime_start;
	CDateTimeCtrl m_ctrlDate_end;
	CDateTimeCtrl m_ctrlTime_end;
	CComboBox CComboBox_deny;
private:
	int  getDenyTime(int denyType); 
	CTime GetNextMonth(const CTime tmSrc);  
	CTime GetOnHour(const CTime tmSrc);
	CTime GetOnDay(const CTime tmSrc);
	CTime GetOnMonth(const CTime tmSrc);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnStart2();
	CCheckListCtrl m_ctrlDayDataItemList;
	CCheckListCtrl m_ctrlMonDataItemList;
	afx_msg void OnNMDblclkListOptResult(NMHDR *pNMHDR, LRESULT *pResult);
};
