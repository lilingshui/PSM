#pragma once
#include "afxcmn.h"

#include "EventRecver.h"
#include "afxwin.h"
#include "ptcl_codec/ptcl_module_csg.h"
#include "task.h"
#include <queue>
#include "CheckListCtrl.h"
#include "Channel/ChannelReceiver.h"
#include "Channel/ChannelMgr.h"

// CDlgReadCurrentData 对话框

class CDlgReadCurrentData : public CDialog, public CEventRecver, public CChannelReceiver
{
	DECLARE_DYNAMIC(CDlgReadCurrentData)
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
	std::queue<Task> m_qTask;
	int m_taskState;
	time_t m_taskTime;
	int m_timeout;
	int m_retryCount;
	ptcl_packet_csg packet;
	CChannel *m_pChannel;
	std::map <CString,DataTypeParam> m_mDataType;

public:
	CDlgReadCurrentData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgReadCurrentData();

// 对话框数据
	enum { IDD = IDD_FORM_READ_CURRENT_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual void OnEvent(unsigned int nEventID,	void *pParam);
	virtual void OnReceive(int state, char *buf, int len, u_int64 strTermAddr);	
	
	DECLARE_MESSAGE_MAP()

protected:
	CCheckListCtrl m_ctrlMeterList;
	CCheckListCtrl m_ctrlDataItemList;	
	CListCtrl m_ctrlOptResult;	
	CComboBox m_cmbMeterList;
	CProgressCtrl m_pcRealtime;
	CLayoutCtrl m_layoutCtrl;	
	
private:
	void ProcessAFN00(unsigned char *buf, int len);
public:
	afx_msg void OnNMDblclkListOptResult(NMHDR *pNMHDR, LRESULT *pResult);
};
