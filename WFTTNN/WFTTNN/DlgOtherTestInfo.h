#pragma once

#include "EventRecver.h"
#include "afxwin.h"

// CDlgOtherTestInfo 对话框

class CDlgOtherTestInfo : public CDialog, public CEventRecver
{
	DECLARE_DYNAMIC(CDlgOtherTestInfo)

public:
	CDlgOtherTestInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgOtherTestInfo();

// 对话框数据
	enum { IDD = IDD_FORM_OTHER_TEST };

protected:
	virtual void OnOK()	{}
	virtual void OnCancel()	{}
	virtual void OnEvent(unsigned int nEventID,	void *pParam);
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnOtherSave();
	afx_msg void OnCbnSelchangeCmbSSC();
	afx_msg void OnCbnSelchangeCmbLAMP();
	afx_msg void OnCbnSelchangeCmbBARCODE();
	afx_msg void OnCbnSelchangeCmbLID();
	afx_msg void OnCbnSelchangeCmbPEDESTAL();
	afx_msg void OnCbnSelchangeCmbNAMEPLATE();
	afx_msg void OnCbnSelchangeCmbSCREW();
	afx_msg void OnCbnSelchangeCmb1();
	afx_msg void OnCbnSelchangeCmb2();
	afx_msg void OnCbnSelchangeCmb3();
	afx_msg void OnCbnSelchangeCmb4();
	afx_msg void OnCbnSelchangeCmb5();
	afx_msg void OnCbnSelchangeCmb6();
	afx_msg void OnCbnSelchangeCmb7();
	afx_msg void OnCbnSelchangeCmb8();
	afx_msg void OnCbnSelchangeCmb9();
	afx_msg void OnCbnSelchangeCmb10();
	afx_msg void OnCbnSelchangeCmb11();
	afx_msg void OnCbnSelchangeCmb12();
	afx_msg void OnCbnSelchangeCmb13();
	afx_msg void OnCbnSelchangeCmb14();
	afx_msg void OnCbnSelchangeCmb15();
	afx_msg void OnCbnSelchangeCmb16();
	afx_msg void OnCbnSelchangeCmbSJZC();
	afx_msg void OnCbnSelchangeCmbCSSZ();
	afx_msg void OnCbnSelchangeCmbQXSJ();
	afx_msg void OnCbnSelchangeCmbRSJ();
	afx_msg void OnCbnSelchangeCmbYSJ();
	afx_msg void OnCbnSelchangeCmbSJJL();
	afx_msg void OnCbnSelchangeCmbGJJL();
	afx_msg void OnCbnSelchangeCmbZDDS();
	afx_msg void OnCbnSelchangeCmbSJTC();

private:
	int m_nSSC;
	int m_nLAMP;
	int m_nBARCODE;
	int m_nLID;
	int m_nPEDESTAL;
	int m_nNAMEPLATE;
	int m_nSCREW;
	CString m_strSSC;	
	CString m_strLAMP;	
	CString m_strBARCODE;	
	CString m_strLID;	
	CString m_strPEDESTAL;	
	CString m_strNAMEPLATE;	
	CString m_strSCREW;
	int m_nCmb1;
	int m_nCmb2;
	int m_nCmb3;
	int m_nCmb4;
	int m_nCmb5;
	int m_nCmb6;
	int m_nCmb7;
	int m_nCmb8;
	int m_nCmb9;
	int m_nCmb10;
	int m_nCmb11;
	int m_nCmb12;
	int m_nCmb13;
	int m_nCmb14;
	int m_nCmb15;
	int m_nCmb16;
	CString m_strVal1;
	CString m_strVal2;
	CString m_strVal3;
	CString m_strVal4;
	CString m_strVal5;
	CString m_strVal6;
	CString m_strVal7;
	CString m_strVal8;
	CString m_strVal9;
	CString m_strVal10;
	CString m_strVal11;
	CString m_strVal12;
	CString m_strVal13;
	CString m_strVal14;
	CString m_strVal15;
	CString m_strVal16;
	int m_nSJZC;
	int m_nCSSZ;
	int m_nQXSJ;
	int m_nRSJ;
	int m_nYSJ;
	int m_nSJJL;
	int m_nGJJL;
	int m_nZDDS;
	int m_nSJTC;	
	CString m_strSJZC;
	CString m_strCSSZ;
	CString m_strQXSJ;
	CString m_strRSJ;
	CString m_strYSJ;
	CString m_strSJJL;
	CString m_strGJJL;
	CString m_strZDDS;
	CString m_strSJTC;

private:
	CString m_strTermNo;
	CString m_strTermFact;
	CString m_strTermUnit;
	bool InitTerminalInfo(int nID);
	void InitOtherTestShape(int nID);
	void InitOtherTestEMC(int nID);
	void InitOtherTestFunction(int nID);
	void SaveOtherTestShape();
	void SaveOtherTestEMC();
	void SaveOtherTestFunction();
	void RefreshState();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()	
};
