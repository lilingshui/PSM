#if !defined(AFX_DLGAUTOCESHI_H__899DB0D3_130F_4471_8449_5CB4AC3A9697__INCLUDED_)
#define AFX_DLGAUTOCESHI_H__899DB0D3_130F_4471_8449_5CB4AC3A9697__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAutoCeShi.h : header file
//
//#include "PtclModule_NN.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgAutoCeShi dialog
//------------------------------
#pragma pack (1)
typedef	struct 
{
	unsigned char buf[256];//帧数据
	short int ilen;//帧长度
	char sterminalno[20];//终端编号
	char saddr[20];//终端地址
	long pn;//测量点号
	char sparamid[10];//数据标识
	char soper[50];//操作类型
	char sparam[20];//参数	
}TASKNODE,	*LPTASKNODE; 
typedef	struct 
{
	LPTASKNODE pTASKNODE;
	long maxcount;//节点最大数
	long maxcurcount;//本次最大节点数
	long curpos;//当前执行位置 -1表示没有任务在执行
	short int flag;//执行状态 0-未执行，1-在执行，2-执行完成
	short int execout;//执行次数
	time_t tm;//开始时间
	unsigned char buf[1024];//收到数据帧
	short int ilen;//帧长度
	
}TASK,	*LPTASK;

#pragma pack ()
//------------------------------
typedef struct 
{
	long ParamID;
	CString strParmName;
}TASK_TYPE;
class CDlgAutoCeShi : public CDialog
{
// Construction
public:
	CDlgAutoCeShi(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAutoCeShi)
	enum { IDD = IDD_DIALOG_AUTOTEST };
	CListCtrl	m_ShowTerminalInfoListCtrl;
	CListCtrl	m_ShowTerminalInfoListCtrl1;
	CListCtrl	m_Result;

	CComboBox	m_cbDataType;

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int m_nTerminalNum;

	void InitTerminalInfoList();
	void SetParam(CString strTerminalNo,CString strTerminalAddr);
	void SetTime(CString strTerminalNo,CString strTerminalAddr);
	void ReadHis(CString strTerminalNo,CString strTerminalAddr);
	void ReadReal(CString strTerminalNo,CString strTerminalAddr);
	CString GetTestId();
	CString g_testid;
	///BYTE sTime[5],eTime[5];

	char g_st[20],g_et[20];//起始时间
	HANDLE g_EventWaite;


	
	//int SendFrame(CPtclPacket_NN &packet_NN,unsigned char *outdata,int *outlen);
	int SendFrame(unsigned char *inbuf,int inlen);


	

	void AddResultOneLine(char *stime,char *terminaladdr,long pn,char *stype,char *paramid,char *sresult,char *svalue);
	void GetCurTimeFmt(char *sfmttm);

	void DoMessage();


	//------------------
	TASK m_TASK;//任务队列
	int InitTask(long maxcount);
	int ResetTask();
	LPTASKNODE  GetCurTask();
	LPTASKNODE  GetNextTask();
	void FreeTask();
	int  AddTaskNode(LPTASKNODE pTASKNODE);
	void ShowAndToDB(LPTASKNODE pTASKNODE,char *sresult,char *svalue);
	int bbusy;

	void InitialTask();

	void EexeTask(CString strTerminalNo,CString strTerminalAddr,long TaskID);//执行任务
	int VarianttoStr(_variant_t val,char *str);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAutoCeShi)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	LRESULT OnAutoCeShiPacketEventNotify(WPARAM wParam,LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CDlgAutoCeShi)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnStart();
	afx_msg void OnButton5();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnViewtask();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAUTOCESHI_H__899DB0D3_130F_4471_8449_5CB4AC3A9697__INCLUDED_)
