#if !defined(AFX_DLGSETPARAM_H__A439AD01_F2F2_4399_9590_BF02F11DFF43__INCLUDED_)
#define AFX_DLGSETPARAM_H__A439AD01_F2F2_4399_9590_BF02F11DFF43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetParam.h : header file
//

#include "EventRecver.h"
#include "Channel/ChannelMgr.h"
#include "ptcl_codec/ptcl_module_csg.h"
#include "ptcl_codec/data_parse_csg.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSetParam dialog

class CDlgSetParam : public CDialog, public CEventRecver, public CChannelReceiver
{
// Construction
public:
	CDlgSetParam(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetParam)
	enum { IDD = IDD_DIALOG_SETPARAM };
	CComboBox	m_cbEnergyType;
	CComboBox	m_cbMeterProtocol;
	CComboBox	m_cbMeterType;
	CComboBox	m_cbTerminalNo;
	CListCtrl	m_ShowSetParamListCtrl;
	CString	m_strMeterNo;
	CString	m_strMeterAddr;
	CString	m_strMeterName;
	CString	m_strPort;
	CComboBox	m_cbBaud;
	CComboBox	m_cbCheck;
	CComboBox	m_cbDataBit;
	CComboBox	m_cbStopBit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetParam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:	
	void InitMeterParamInfoList(CString strTerminalNo);
	void SearchByField(int nTerminalNo, int nMeterNo, CString &strMeterName);

	LRESULT OnSetParamPacketEventNotify(WPARAM wParam,LPARAM lParam);
	void ParseSetParam(unsigned char *pDataBuff, int nDataLen);
	
	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual void OnEvent(unsigned int nEventID,	void *pParam);
	virtual void OnReceive(int state, char *buf, int len, u_int64 strTermAddr);

private:
	CString m_strTerminalNo;
	CString m_strTerminalAddr;
	int m_nMeterNum;
	float m_fRatedVoltage;
	float m_fRatedCurrent;
	CLayoutCtrl m_layoutCtrl;
	int m_nBeginIndex;
	int m_nEndIndex;
	int m_nMeterIndex;
	int m_nOptType;
	time_t m_tCmdTime;
	int m_nRetryCount;
	bool m_bWaiting;
	ptcl_packet_csg packet;

	bool GetMeterInfo(int id, CString &addr, int &protocol, int &mtype, int &etype, int &port, 
		int &baud, int &check, int &databit, int &stopbit, float &fVoltage, float &fCurrent);
	void ProcessAFN04(unsigned char *buf, int len);
	void ProcessAFN0A(unsigned char *buf, int len);
	void NextSend();
	void NextCall();
	void NextDel();
	void BeginTask(int type);
	void EndTask(int type);

public:
	void RefreshUI(int nNodeValue);
	void InitialTerminalInfo();

	// Generated message map functions
	//{{AFX_MSG(CDlgSetParam)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonMeteradd();
	afx_msg void OnButtonMeterdel();
//	afx_msg void OnClickListSetparaminfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSend();
	afx_msg void OnButtonRead();
	afx_msg void OnButtonMetermodify();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
	afx_msg void OnLvnItemchangedListSetparaminfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedBtnTotemplate();
	afx_msg void OnBnClickedBtnFromtemplate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);	
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnClear();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETPARAM_H__A439AD01_F2F2_4399_9590_BF02F11DFF43__INCLUDED_)

