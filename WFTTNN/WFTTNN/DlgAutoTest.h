#pragma once
#include "afxcmn.h"

#include "EventRecver.h"
#include "LayoutCtrl.h"
#include <queue>
#include "task.h"
#include "ptcl_codec/ptcl_module_csg.h"
#include "ptcl_codec/ptcl_module_dlt.h"
#include "Channel/ChannelMgr.h"
#include "afxwin.h"

// CDlgAutoTest 对话框

class CDlgAutoTest : public CDialog, public CEventRecver,public CChannelReceiver
{
	DECLARE_DYNAMIC(CDlgAutoTest)
	
	typedef struct ChannelParam
	{
		unsigned int nId;
		CString sChannelDesc;
	};
	std::vector<ChannelParam> m_vChannelId;

	typedef struct TaskParam
	{
		unsigned int nId;
		CString sTaskDesc;
	};
	std::vector<TaskParam> m_vTaskId;

public:
	CDlgAutoTest(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAutoTest();

// 对话框数据
	enum { IDD = IDD_FORM_AUTO_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual BOOL OnInitDialog();
	virtual void OnEvent(unsigned int nEventID,	void *pParam);
	virtual void OnReceive(int state, char *buf, int len, u_int64 addr);
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnTaskManage();
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnReport();
	afx_msg void OnBnClickedButtonResult();
	afx_msg void OnCbnSelchangeComboResult();
	afx_msg void OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult);

private:
	void InitMap();
	void InitTask();
	
	bool ExcuteTask(AutoTask *pTask);
	// 事件
	bool ExcuteTask_ReadEvent(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E2010004(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E2010005(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E2010006(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E2010007(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E2010008(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E2010009(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E201000B(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E201000C(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E201000D(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E2010001(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E2010014(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	// 日冻结
	bool ExcuteTask_ReadHistory_Day(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	// 月冻结
	bool ExcuteTask_ReadHistory_Month(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_00010001(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_01010001(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel, UINT32 nDataItem);
	void GetHistoryData(ptcl_packet_csg &packet, AutoTask *pTask, CString &strValue, CString &strVirtualValue, CString &strResult, CString &strTime);
	// 曲线
	bool ExcuteTask_ReadHistory_Curve(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_RHC_FLOW1(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_RHC_FLOW2(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel, UINT32 nDataItem);
	// 告警
	bool ExcuteTask_ReadAlarm(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E200003E(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E200002F(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	// 当前数据
	bool ExcuteTask_ReadCurrent(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_00010000(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_04000101(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_01010000(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel, UINT32 nDataItem);
	// 其它
	bool ExcuteTask_Other(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	bool ExcuteTask_E0000130(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);	
	bool ExcuteTask_E3020001(AutoTask *pTask, CChannel *pRtuChannel, CChannel *pMeterChannel);
	
	void ProcessFailed(AutoTask *pTask);
	void ProcessSucceeded(AutoTask *pTask);
	void ProcessPacket(ptcl_packet_csg &packet, AutoTask *pTask);
	void ProcessPacket(ptcl_packet_dlt &packet, AutoTask *pTask);

	void InsertMeterTask(int ChannelId, int TaskId,int TermId, std::string TermAddr, int ParamType,
						unsigned int StartTime, unsigned int EndTime, std::string sDataItem, std::string sStrDataItem, int Densy); //MODIFY BY WY 20140114

	void InsertChildTask(int ChannelId, int TaskId, int TermId, CString TermAddr);

	void WriteResultDb(int batch_id , int term_id, CString strPn, CString strChannelDesc, 
								CString strTaskDesc,CString strVirtualValue, CString strValue,
								CString strDataItemType, CString strDataItem, CString strResult, CString strTime);	

	CString GetChildTaskDesc(int type);
	CTime GetOnDay(const CTime tmSrc);
	CTime GetOnHour(const CTime tmSrc);
	CTime GetNextMonth(const CTime tmSrc);
	CTime GetOnMonth(const CTime tmSrc);
	CTime GetLastMonth(const CTime tmSrc);
		
private:
	CListCtrl m_ctrlTerminalList;
	CListCtrl m_ctrlChannelList;
	CListCtrl m_ctrlTaskList;
	CListCtrl m_ctrlReportList;
	CEdit m_EditTerm;
	CComboBox m_comboResult;	
	CLayoutCtrl m_layout;

	int m_termId;
	int m_batchId;
	CString m_termAddr;
		
	std::map<CString, CString> m_mTaskDesc;
	std::map<string, string> m_mapSg2Dlt;
	std::map<string, string> m_mapDlt2Sg;
	std::map<string, string> m_mCurveSg2Dlt;
	std::map<string, string> m_mCurveDlt2sg;

	std::vector<AutoTask> m_vTask;
	AutoTask *m_pAutoTask;
	int m_nCurrentTestCount;
	int m_nDayTestCount;
	int m_nMonthTestCount;
	int m_nEventTestCount;
	int m_nCurveTestCount;
	int m_nAlarmTestCount;

	time_t m_taskTime;
	int m_timeout;
	int m_retryCount;
	const static UINT64 m_virtualDeviceId = 999999;

public:
	void InitChannelList();
	void InitTaskList();
};