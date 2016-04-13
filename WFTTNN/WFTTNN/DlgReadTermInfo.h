#pragma once

#include "EventRecver.h"
#include "Channel/ChannelMgr.h"
#include "afxdtctl.h"

// CDlgReadTermInfo 对话框

class CDlgReadTermInfo : public CDialog, public CEventRecver, public CChannelReceiver
{
	DECLARE_DYNAMIC(CDlgReadTermInfo)

public:
	CDlgReadTermInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgReadTermInfo();

// 对话框数据
	enum { IDD = IDD_FORM_READ_TERMINAL_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnBnClickedBtnCall();
	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual void OnEvent(unsigned int nEventID,	void *pParam);
	virtual void OnReceive(int state, char *buf, int len, u_int64 strTermAddr);

	DECLARE_MESSAGE_MAP()


private:
	CString m_strTerminalAddr;
	void ProcessAFN04(unsigned char *buf, int len);
	CDateTimeCtrl m_ctrlDate;
	CDateTimeCtrl m_ctrlTime;
	CString m_strHardVersion;
	CString m_strSoftVersion;
	BOOL m_bHardVer;
	BOOL m_bSoftver;
	BOOL m_bTime;	
	BOOL m_bUseSystemTime;
	BOOL m_bMeterReadTime; //ADD BY WM 20131112
	BOOL m_bDataReset; //ADD BY WM 20131115
	BOOL m_bTerminalReset; //ADD BY WM 20131127
	BOOL m_bAlarmNoShield; //ADD BY WM 20131206
	BOOL m_bEventNoShield; //ADD BY WM 20131206
	BOOL m_bZDSBDisable; // ADD BY NH 20140520
	BOOL m_bZDCBCS;
	BOOL m_bRS485[8];

public:
	afx_msg void OnBnClickedCheckTime();
	afx_msg void OnBnClickedCheckSystemtime();
};
