#if !defined(AFX_DLGRNDREAD_H__44863A7F_A5CE_4D62_805D_2E732405AB0C__INCLUDED_)
#define AFX_DLGRNDREAD_H__44863A7F_A5CE_4D62_805D_2E732405AB0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRndRead.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRndRead dialog

typedef struct _DATA_TYPE
{
	CString strParamID;
	CString strParmName;
}DATA_TYPE;

class CDlgRndRead : public CDialog
{
// Construction
public:
	CDlgRndRead(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRndRead)
	enum { IDD = IDD_DIALOG_RNDREAD };
	CDateTimeCtrl	m_EndDate;
	CDateTimeCtrl	m_EndTime;
	CDateTimeCtrl	m_StartTime;
	CDateTimeCtrl	m_StartDate;
	CComboBox	m_cbCurDataType;
	CComboBox	m_cbDataType;
	CListCtrl	m_ShowRndReadInfoListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRndRead)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void InitialDataType();
	void InitialCurDataType();

	void ParseTerminalTime(char *pDataBuff, int nDataLen);

	void ParseTerminalVersion(char *pDataBuff, int nDataLen);

	int StringtoByte(char *instr,char *outstr,short int *outlen);
	int Convertstr(char *str,int ilen);
	int   HextoValue(char ch1,char ch2);
	int chrtobyte(char ch);
	


// Implementation
protected:

	LRESULT OnRndReadPacketEventNotify(WPARAM wParam,LPARAM lParam);
	// Generated message map functions
	//{{AFX_MSG(CDlgRndRead)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonHistorydata();
	afx_msg void OnButtonCurdata();
	afx_msg void OnButtonTerminaltime();
	afx_msg void OnButtonTerminalversion();
	afx_msg void OnSelchangeComboCurdatatype();
	afx_msg void OnSelchangeComboDatatype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRNDREAD_H__44863A7F_A5CE_4D62_805D_2E732405AB0C__INCLUDED_)
