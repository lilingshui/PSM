#pragma once


#include "EventRecver.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "Channel/Channel.h"
#include "task.h"
#include <deque>
#include "afxdtctl.h"
#include "ptcl_codec\ptcl_packet_csg.h"
#include "Channel\ChannelReceiver.h"
#include "CheckListCtrl.h"

// CDlgReadEventData 对话框

class CDlgReadEventData : public CDialog, public CEventRecver, public CChannelReceiver
{
	DECLARE_DYNAMIC(CDlgReadEventData)
public:
	typedef struct DataTypeParam
	{
		int type;
		CString desc;
	};
	void InitMeterList();

private:
	void InitDataItem();	
	int m_nTermNo;
	CString m_strTerminalAddr;
	CLayoutCtrl m_layoutCtrl;
	CChannel *m_pChannel;
	std::deque <Task> m_qTask;
	int m_taskState;
	int m_retryCount;
	int m_timeout;
	time_t m_taskTime;
	ptcl_packet_csg packet;

	std::map <CString,DataTypeParam> m_mDataType;

public:
	CDlgReadEventData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgReadEventData();

// 对话框数据
	enum { IDD = IDD_FORM_READ_EVENT_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual void OnEvent(unsigned int nEventID,	void *pParam);
	virtual void OnReceive(int state, char *buf, int len, u_int64 termAddr);
	virtual void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	CCheckListCtrl m_ctrlDataItemList;
	CListCtrl m_ctrlOptResult;
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CCheckListCtrl m_ctrlDeviceList;
	CCheckListCtrl m_ctrlTermEventItem;
	

	CDateTimeCtrl m_ctrlDateStart;
	CDateTimeCtrl m_ctrlTimeStart;
	CDateTimeCtrl m_ctrlDateEnd;
	CDateTimeCtrl m_ctrlTimeEnd;
	afx_msg void OnNMDblclkListOptResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnEventStop();
};
