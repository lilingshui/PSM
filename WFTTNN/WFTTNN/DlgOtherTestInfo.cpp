// DlgOtherTestInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "WFTTNN.h"
#include "DlgOtherTestInfo.h"
#include ".\dlgothertestinfo.h"
#include "ItemData.h"


// CDlgOtherTestInfo 对话框

IMPLEMENT_DYNAMIC(CDlgOtherTestInfo, CDialog)
CDlgOtherTestInfo::CDlgOtherTestInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOtherTestInfo::IDD, pParent)
	, m_strTermNo(_T(""))
	, m_strTermFact(_T(""))
	, m_strTermUnit(_T(""))
	, m_nSSC(0)
	, m_nLAMP(0)
	, m_nBARCODE(0)
	, m_nLID(0)
	, m_nPEDESTAL(0)
	, m_nNAMEPLATE(0)
	, m_nSCREW(0)
	, m_strSSC(_T(""))
	, m_strLAMP(_T(""))
	, m_strBARCODE(_T(""))
	, m_strLID(_T(""))
	, m_strPEDESTAL(_T(""))
	, m_strNAMEPLATE(_T(""))
	, m_strSCREW(_T(""))
	, m_nCmb1(0)
	, m_nCmb2(0)
	, m_nCmb3(0)
	, m_nCmb4(0)
	, m_nCmb5(0)
	, m_nCmb6(0)
	, m_nCmb7(0)
	, m_nCmb8(0)
	, m_nCmb9(0)
	, m_nCmb10(0)
	, m_nCmb11(0)
	, m_nCmb12(0)
	, m_nCmb13(0)
	, m_nCmb14(0)
	, m_nCmb15(0)
	, m_nCmb16(0)
	, m_nSJZC(0)
	, m_nCSSZ(0)
	, m_nQXSJ(0)
	, m_nRSJ(0)
	, m_nYSJ(0)
	, m_nSJJL(0)
	, m_nGJJL(0)
	, m_nZDDS(0)
	, m_nSJTC(0)
{
}

CDlgOtherTestInfo::~CDlgOtherTestInfo()
{
}

void CDlgOtherTestInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TXT_TERMNO, m_strTermNo);
	DDX_Text(pDX, IDC_TXT_TERMFACT, m_strTermFact);
	DDX_Text(pDX, IDC_TXT_TERMUNIT, m_strTermUnit);
	DDX_CBIndex(pDX, IDC_CMB_SSC, m_nSSC);
	DDX_CBIndex(pDX, IDC_CMB_LAMP, m_nLAMP);
	DDX_CBIndex(pDX, IDC_CMB_BARCODE, m_nBARCODE);
	DDX_CBIndex(pDX, IDC_CMB_LID, m_nLID);
	DDX_CBIndex(pDX, IDC_CMB_PEDESTAL, m_nPEDESTAL);
	DDX_CBIndex(pDX, IDC_CMB_NAMEPLATE, m_nNAMEPLATE);
	DDX_CBIndex(pDX, IDC_CMB_SCREW, m_nSCREW);
	DDX_Text(pDX, IDC_TXT_SSC, m_strSSC);	
	DDX_Text(pDX, IDC_TXT_LAMP, m_strLAMP);	
	DDX_Text(pDX, IDC_TXT_BARCODE, m_strBARCODE);	
	DDX_Text(pDX, IDC_TXT_LID, m_strLID);	
	DDX_Text(pDX, IDC_TXT_PEDESTAL, m_strPEDESTAL);	
	DDX_Text(pDX, IDC_TXT_NAMEPLATE, m_strNAMEPLATE);	
	DDX_Text(pDX, IDC_TXT_SCREW, m_strSCREW);	
	DDX_CBIndex(pDX, IDC_CMB_1, m_nCmb1);
	DDX_CBIndex(pDX, IDC_CMB_2, m_nCmb2);
	DDX_CBIndex(pDX, IDC_CMB_3, m_nCmb3);
	DDX_CBIndex(pDX, IDC_CMB_4, m_nCmb4);
	DDX_CBIndex(pDX, IDC_CMB_5, m_nCmb5);
	DDX_CBIndex(pDX, IDC_CMB_6, m_nCmb6);
	DDX_CBIndex(pDX, IDC_CMB_7, m_nCmb7);
	DDX_CBIndex(pDX, IDC_CMB_8, m_nCmb8);
	DDX_CBIndex(pDX, IDC_CMB_9, m_nCmb9);
	DDX_CBIndex(pDX, IDC_CMB10, m_nCmb10);
	DDX_CBIndex(pDX, IDC_CMB11, m_nCmb11);
	DDX_CBIndex(pDX, IDC_CMB12, m_nCmb12);
	DDX_CBIndex(pDX, IDC_CMB13, m_nCmb13);
	DDX_CBIndex(pDX, IDC_CMB14, m_nCmb14);
	DDX_CBIndex(pDX, IDC_CMB15, m_nCmb15);
	DDX_CBIndex(pDX, IDC_CMB16, m_nCmb16);
	DDX_Text(pDX, IDC_TXT1, m_strVal1);	
	DDX_Text(pDX, IDC_TXT2, m_strVal2);	
	DDX_Text(pDX, IDC_TXT3, m_strVal3);	
	DDX_Text(pDX, IDC_TXT4, m_strVal4);	
	DDX_Text(pDX, IDC_TXT5, m_strVal5);	
	DDX_Text(pDX, IDC_TXT6, m_strVal6);	
	DDX_Text(pDX, IDC_TXT7, m_strVal7);	
	DDX_Text(pDX, IDC_TXT8, m_strVal8);	
	DDX_Text(pDX, IDC_TXT9, m_strVal9);	
	DDX_Text(pDX, IDC_TXT10, m_strVal10);	
	DDX_Text(pDX, IDC_TXT11, m_strVal11);	
	DDX_Text(pDX, IDC_TXT12, m_strVal12);	
	DDX_Text(pDX, IDC_TXT13, m_strVal13);	
	DDX_Text(pDX, IDC_TXT14, m_strVal14);	
	DDX_Text(pDX, IDC_TXT15, m_strVal15);	
	DDX_Text(pDX, IDC_TXT16, m_strVal16);	
	DDX_CBIndex(pDX, IDC_CMB_SJZC, m_nSJZC);
	DDX_CBIndex(pDX, IDC_CMB_CSSZ, m_nCSSZ);
	DDX_CBIndex(pDX, IDC_CMB_QXSJ, m_nQXSJ);
	DDX_CBIndex(pDX, IDC_CMB_RSJ, m_nRSJ);
	DDX_CBIndex(pDX, IDC_CMB_YSJ, m_nYSJ);
	DDX_CBIndex(pDX, IDC_CMB_SJJL, m_nSJJL);
	DDX_CBIndex(pDX, IDC_CMB_GJJL, m_nGJJL);
	DDX_CBIndex(pDX, IDC_CMB_ZDDS, m_nZDDS);
	DDX_CBIndex(pDX, IDC_CMB_SJTC, m_nSJTC);
	DDX_Text(pDX, IDC_TXT_SJZC, m_strSJZC);
	DDX_Text(pDX, IDC_TXT_CSSZ, m_strCSSZ);
	DDX_Text(pDX, IDC_TXT_QXSJ, m_strQXSJ);
	DDX_Text(pDX, IDC_TXT_RSJ, m_strRSJ);
	DDX_Text(pDX, IDC_TXT_YSJ, m_strYSJ);
	DDX_Text(pDX, IDC_TXT_SJJL, m_strSJJL);
	DDX_Text(pDX, IDC_TXT_GJJL, m_strGJJL);
	DDX_Text(pDX, IDC_TXT_ZDDS, m_strZDDS);
	DDX_Text(pDX, IDC_TXT_SJTC, m_strSJTC);
}

BEGIN_MESSAGE_MAP(CDlgOtherTestInfo, CDialog)
	ON_BN_CLICKED(IDC_BTN_OTHER_SAVE, &CDlgOtherTestInfo::OnBnClickedBtnOtherSave)
	ON_CBN_SELCHANGE(IDC_CMB_SSC, &CDlgOtherTestInfo::OnCbnSelchangeCmbSSC)
	ON_CBN_SELCHANGE(IDC_CMB_LAMP, &CDlgOtherTestInfo::OnCbnSelchangeCmbLAMP)
	ON_CBN_SELCHANGE(IDC_CMB_BARCODE, &CDlgOtherTestInfo::OnCbnSelchangeCmbBARCODE)
	ON_CBN_SELCHANGE(IDC_CMB_LID, &CDlgOtherTestInfo::OnCbnSelchangeCmbLID)
	ON_CBN_SELCHANGE(IDC_CMB_PEDESTAL, &CDlgOtherTestInfo::OnCbnSelchangeCmbPEDESTAL)
	ON_CBN_SELCHANGE(IDC_CMB_NAMEPLATE, &CDlgOtherTestInfo::OnCbnSelchangeCmbNAMEPLATE)
	ON_CBN_SELCHANGE(IDC_CMB_SCREW, &CDlgOtherTestInfo::OnCbnSelchangeCmbSCREW)
	ON_CBN_SELCHANGE(IDC_CMB_1, &CDlgOtherTestInfo::OnCbnSelchangeCmb1)
	ON_CBN_SELCHANGE(IDC_CMB_2, &CDlgOtherTestInfo::OnCbnSelchangeCmb2)
	ON_CBN_SELCHANGE(IDC_CMB_3, &CDlgOtherTestInfo::OnCbnSelchangeCmb3)
	ON_CBN_SELCHANGE(IDC_CMB_4, &CDlgOtherTestInfo::OnCbnSelchangeCmb4)
	ON_CBN_SELCHANGE(IDC_CMB_5, &CDlgOtherTestInfo::OnCbnSelchangeCmb5)
	ON_CBN_SELCHANGE(IDC_CMB_6, &CDlgOtherTestInfo::OnCbnSelchangeCmb6)
	ON_CBN_SELCHANGE(IDC_CMB_7, &CDlgOtherTestInfo::OnCbnSelchangeCmb7)
	ON_CBN_SELCHANGE(IDC_CMB_8, &CDlgOtherTestInfo::OnCbnSelchangeCmb8)
	ON_CBN_SELCHANGE(IDC_CMB_9, &CDlgOtherTestInfo::OnCbnSelchangeCmb9)
	ON_CBN_SELCHANGE(IDC_CMB10, &CDlgOtherTestInfo::OnCbnSelchangeCmb10)
	ON_CBN_SELCHANGE(IDC_CMB11, &CDlgOtherTestInfo::OnCbnSelchangeCmb11)
	ON_CBN_SELCHANGE(IDC_CMB12, &CDlgOtherTestInfo::OnCbnSelchangeCmb12)
	ON_CBN_SELCHANGE(IDC_CMB13, &CDlgOtherTestInfo::OnCbnSelchangeCmb13)
	ON_CBN_SELCHANGE(IDC_CMB14, &CDlgOtherTestInfo::OnCbnSelchangeCmb14)
	ON_CBN_SELCHANGE(IDC_CMB15, &CDlgOtherTestInfo::OnCbnSelchangeCmb15)
	ON_CBN_SELCHANGE(IDC_CMB16, &CDlgOtherTestInfo::OnCbnSelchangeCmb16)
	ON_CBN_SELCHANGE(IDC_CMB_SJZC, &CDlgOtherTestInfo::OnCbnSelchangeCmbSJZC)
	ON_CBN_SELCHANGE(IDC_CMB_CSSZ, &CDlgOtherTestInfo::OnCbnSelchangeCmbCSSZ)
	ON_CBN_SELCHANGE(IDC_CMB_QXSJ, &CDlgOtherTestInfo::OnCbnSelchangeCmbQXSJ)
	ON_CBN_SELCHANGE(IDC_CMB_RSJ, &CDlgOtherTestInfo::OnCbnSelchangeCmbRSJ)
	ON_CBN_SELCHANGE(IDC_CMB_YSJ, &CDlgOtherTestInfo::OnCbnSelchangeCmbYSJ)
	ON_CBN_SELCHANGE(IDC_CMB_SJJL, &CDlgOtherTestInfo::OnCbnSelchangeCmbSJJL)
	ON_CBN_SELCHANGE(IDC_CMB_GJJL, &CDlgOtherTestInfo::OnCbnSelchangeCmbGJJL)
	ON_CBN_SELCHANGE(IDC_CMB_ZDDS, &CDlgOtherTestInfo::OnCbnSelchangeCmbZDDS)
	ON_CBN_SELCHANGE(IDC_CMB_SJTC, &CDlgOtherTestInfo::OnCbnSelchangeCmbSJTC)
END_MESSAGE_MAP()


// CDlgOtherTestInfo 消息处理程序
BOOL CDlgOtherTestInfo::OnInitDialog()
{
	__super::OnInitDialog();

	GetDlgItem(IDC_TXT_SSC)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_LAMP)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_BARCODE)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_LID)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_PEDESTAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_NAMEPLATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_SCREW)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT3)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT4)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT5)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT6)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT7)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT8)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT9)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT10)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT11)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT12)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT13)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT14)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT15)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT16)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_SJZC)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_CSSZ)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_QXSJ)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_RSJ)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_YSJ)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_SJJL)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_GJJL)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_ZDDS)->EnableWindow(FALSE);
	GetDlgItem(IDC_TXT_SJTC)->EnableWindow(FALSE);
	
	return true;
}


void CDlgOtherTestInfo::OnEvent(unsigned int nEventID, void *pParam)
{	
	if (Event_Type_Item_Double_Click != nEventID || !::IsWindowVisible(this->GetSafeHwnd()))
		return;

	ItemData *data = (ItemData *)pParam;
	if (ITEM_TYPE_TERMINAL  == data->m_type)
	{
		if (InitTerminalInfo(data->m_id))
		{
			InitOtherTestShape(data->m_id);
			InitOtherTestEMC(data->m_id);
			InitOtherTestFunction(data->m_id);
			RefreshState();
		}		
	}
}

bool CDlgOtherTestInfo::InitTerminalInfo(int nID)
{
	CString temp;
	temp.Format("%d", nID);
	if (temp.CompareNoCase(m_strTermNo) == 0)
		return false;

	m_strTermNo.Format("%d", nID);
	m_strTermFact = "";
	m_strTermUnit = "";

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();	
	CADORecordSet record;
	char strSQL[300] = {0};	
	sprintf(strSQL,"SELECT RTU_ARCH.RTU_ID AS ID, RTU_ARCH.RTU_FACT AS FACT, BATCH_INFO.InspectUnit AS UNIT FROM RTU_ARCH INNER " \
		" JOIN BATCH_INFO ON RTU_ARCH.BatchNo=BATCH_INFO.BatchNo WHERE RTU_ARCH.RTU_ID=%d", nID);	
	if (pConnect != NULL && pConnect->ExecSQL(strSQL,record) && !record.IsEndEOF())
	{
		_variant_t val;
		record.GetFieldValue("FACT", val);
		m_strTermFact = val.bstrVal;

		record.GetFieldValue("UNIT", val);
		m_strTermUnit = val.bstrVal;
	}

	UpdateData(FALSE);
	return true;
}

void CDlgOtherTestInfo::OnBnClickedBtnOtherSave()
{
	if (MessageBox("确认操作？", "提示", MB_YESNO|MB_ICONWARNING) != IDYES)
		return;
	
	SaveOtherTestShape();
	SaveOtherTestEMC();
	SaveOtherTestFunction();
}

void CDlgOtherTestInfo::InitOtherTestShape(int nID)
{
	m_nSSC = m_nLAMP = m_nBARCODE = m_nLID = m_nPEDESTAL = m_nNAMEPLATE = m_nSCREW = 0;
	m_strSSC = "";	
	m_strLAMP = "";	
	m_strBARCODE = "";	
	m_strLID = "";	
	m_strPEDESTAL = "";	
	m_strNAMEPLATE = "";	
	m_strSCREW = "";

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	CADORecordSet record;
	char strSQL[300] = {0};	
	sprintf(strSQL,"SELECT * FROM RTU_OTHER_TEST_SHAPE WHERE RTU_ID=%d", nID);	
	
	if (pConnect != NULL && pConnect->ExecSQL(strSQL,record) && !record.IsEndEOF())
	{
		_variant_t val;	
	
		record.GetFieldValue("ISQ_SSC", val);	
		m_nSSC = !val.boolVal;		
		record.GetFieldValue("DESC_SSC", val);
		m_strSSC = val.bstrVal;	

		record.GetFieldValue("ISQ_LAMP", val);	
		m_nLAMP = !val.boolVal;		
		record.GetFieldValue("DESC_LAMP", val);
		m_strLAMP = val.bstrVal;

		record.GetFieldValue("ISQ_BARCODE", val);	
		m_nBARCODE = !val.boolVal;		
		record.GetFieldValue("DESC_BARCODE", val);
		m_strBARCODE = val.bstrVal;

		record.GetFieldValue("ISQ_LID", val);	
		m_nLID = !val.boolVal;		
		record.GetFieldValue("DESC_LID", val);
		m_strLID = val.bstrVal;

		record.GetFieldValue("ISQ_PEDESTAL", val);	
		m_nPEDESTAL = !val.boolVal;		
		record.GetFieldValue("DESC_PEDESTAL", val);
		m_strPEDESTAL = val.bstrVal;
	
		record.GetFieldValue("ISQ_NAMEPLATE", val);	
		m_nNAMEPLATE = !val.boolVal;		
		record.GetFieldValue("DESC_NAMEPLATE", val);
		m_strNAMEPLATE = val.bstrVal;

		record.GetFieldValue("ISQ_SCREW", val);	
		m_nSCREW = !val.boolVal;		
		record.GetFieldValue("DESC_SCREW", val);
		m_strSCREW = val.bstrVal;
	}	

	UpdateData(FALSE);
}

void CDlgOtherTestInfo::SaveOtherTestShape()
{
	UpdateData(TRUE);

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	char strSQL[300] = {0};
	sprintf(strSQL, "DELETE FROM RTU_OTHER_TEST_SHAPE WHERE RTU_ID=%s", m_strTermNo);
	pConnect->ExecSQL(strSQL);

	int bSSC = (m_nSSC > 0) ? 0 : 1;
	int bLAMP = (m_nLAMP > 0) ? 0 : 1;
	int bBARCODE = (m_nBARCODE > 0) ? 0 : 1;
	int bLID = (m_nLID > 0) ? 0 : 1;
	int bPEDESTAL = (m_nPEDESTAL > 0) ? 0 : 1;
	int bNAMEPLATE = (m_nNAMEPLATE > 0) ? 0 : 1;
	int bSCREW = (m_nSCREW > 0) ? 0 : 1;
	m_strSSC = bSSC ? "" : m_strSSC;
	m_strLAMP = bLAMP ? "" : m_strLAMP;
	m_strBARCODE= bBARCODE ? "" : m_strBARCODE;
	m_strLID = bLID ? "" : m_strLID;
	m_strPEDESTAL = bPEDESTAL ? "" : m_strPEDESTAL;
	m_strNAMEPLATE = bNAMEPLATE ? "" : m_strNAMEPLATE;
	m_strSCREW = bSCREW ? "" : m_strSCREW;

	sprintf(strSQL, "INSERT INTO RTU_OTHER_TEST_SHAPE(RTU_ID, ISQ_SSC, DESC_SSC, ISQ_LAMP, DESC_LAMP, ISQ_BARCODE, DESC_BARCODE, " \
		"ISQ_LID, DESC_LID, ISQ_PEDESTAL, DESC_PEDESTAL, ISQ_NAMEPLATE, DESC_NAMEPLATE, ISQ_SCREW, DESC_SCREW) " \
		" VALUES(%s, %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s')", 
		m_strTermNo, bSSC, m_strSSC, bLAMP, m_strLAMP, bBARCODE, m_strBARCODE, bLID, m_strLID, 
		bPEDESTAL, m_strPEDESTAL, bNAMEPLATE, m_strNAMEPLATE, bSCREW, m_strSCREW);
	pConnect->ExecSQL(strSQL);	
}

void CDlgOtherTestInfo::InitOtherTestEMC(int nID)
{
	m_nCmb1 = m_nCmb2 = m_nCmb3 = m_nCmb4 = m_nCmb5 = m_nCmb6 = m_nCmb7 = m_nCmb8 = 0;
	m_nCmb9 = m_nCmb10 = m_nCmb11 = m_nCmb12 = m_nCmb13 = m_nCmb14 = m_nCmb15 = m_nCmb16 = 0;
	m_strVal1 = "";
	m_strVal2 = "";
	m_strVal3 = "";
	m_strVal4 = "";
	m_strVal5 = "";
	m_strVal6 = "";
	m_strVal7 = "";
	m_strVal8 = "";
	m_strVal9 = "";
	m_strVal10 = "";
	m_strVal11 = "";
	m_strVal12 = "";
	m_strVal13 = "";
	m_strVal14 = "";
	m_strVal15 = "";
	m_strVal16 = "";	

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	CADORecordSet record;
	char strSQL[300] = {0};	
	sprintf(strSQL,"SELECT * FROM RTU_OTHER_TEST_EMC WHERE RTU_ID=%d", nID);	
	
	if (pConnect != NULL && pConnect->ExecSQL(strSQL,record) && !record.IsEndEOF())
	{
		_variant_t val;	
	
		record.GetFieldValue("ISQ1", val);	
		m_nCmb1 = !val.boolVal;		
		record.GetFieldValue("DESC1", val);
		m_strVal1 = val.bstrVal;

		record.GetFieldValue("ISQ2", val);	
		m_nCmb2 = !val.boolVal;		
		record.GetFieldValue("DESC2", val);
		m_strVal2 = val.bstrVal;

		record.GetFieldValue("ISQ3", val);	
		m_nCmb3 = !val.boolVal;		
		record.GetFieldValue("DESC3", val);
		m_strVal3 = val.bstrVal;

		record.GetFieldValue("ISQ4", val);	
		m_nCmb4 = !val.boolVal;		
		record.GetFieldValue("DESC4", val);
		m_strVal4 = val.bstrVal;

		record.GetFieldValue("ISQ5", val);	
		m_nCmb5 = !val.boolVal;		
		record.GetFieldValue("DESC5", val);
		m_strVal5 = val.bstrVal;

		record.GetFieldValue("ISQ6", val);	
		m_nCmb6 = !val.boolVal;		
		record.GetFieldValue("DESC6", val);
		m_strVal6 = val.bstrVal;

		record.GetFieldValue("ISQ7", val);	
		m_nCmb7 = !val.boolVal;		
		record.GetFieldValue("DESC7", val);
		m_strVal7 = val.bstrVal;

		record.GetFieldValue("ISQ8", val);	
		m_nCmb8 = !val.boolVal;		
		record.GetFieldValue("DESC8", val);
		m_strVal8 = val.bstrVal;

		record.GetFieldValue("ISQ9", val);	
		m_nCmb9 = !val.boolVal;		
		record.GetFieldValue("DESC9", val);
		m_strVal9 = val.bstrVal;

		record.GetFieldValue("ISQ10", val);	
		m_nCmb10 = !val.boolVal;		
		record.GetFieldValue("DESC10", val);
		m_strVal10 = val.bstrVal;

		record.GetFieldValue("ISQ11", val);	
		m_nCmb11 = !val.boolVal;		
		record.GetFieldValue("DESC11", val);
		m_strVal11 = val.bstrVal;

		record.GetFieldValue("ISQ12", val);	
		m_nCmb12 = !val.boolVal;		
		record.GetFieldValue("DESC12", val);
		m_strVal12 = val.bstrVal;

		record.GetFieldValue("ISQ13", val);	
		m_nCmb13 = !val.boolVal;		
		record.GetFieldValue("DESC13", val);
		m_strVal13 = val.bstrVal;

		record.GetFieldValue("ISQ14", val);	
		m_nCmb14 = !val.boolVal;		
		record.GetFieldValue("DESC14", val);
		m_strVal14 = val.bstrVal;

		record.GetFieldValue("ISQ15", val);	
		m_nCmb15 = !val.boolVal;		
		record.GetFieldValue("DESC15", val);
		m_strVal15 = val.bstrVal;

		record.GetFieldValue("ISQ16", val);	
		m_nCmb16 = !val.boolVal;		
		record.GetFieldValue("DESC16", val);
		m_strVal16 = val.bstrVal;	
	}
	
	UpdateData(FALSE);
}


void CDlgOtherTestInfo::SaveOtherTestEMC()
{
	UpdateData(TRUE);

	m_strVal1 = m_nCmb1 > 0 ? m_strVal1 : "";
	m_strVal2 = m_nCmb2 > 0 ? m_strVal2 : "";
	m_strVal3 = m_nCmb3 > 0 ? m_strVal3 : "";
	m_strVal4 = m_nCmb4 > 0 ? m_strVal4 : "";
	m_strVal5 = m_nCmb5 > 0 ? m_strVal5 : "";
	m_strVal6 = m_nCmb6 > 0 ? m_strVal6 : "";
	m_strVal7 = m_nCmb7 > 0 ? m_strVal7 : "";
	m_strVal8 = m_nCmb8 > 0 ? m_strVal8 : "";
	m_strVal9 = m_nCmb9 > 0 ? m_strVal9 : "";
	m_strVal10 = m_nCmb10 > 0 ? m_strVal10 : "";
	m_strVal11 = m_nCmb11 > 0 ? m_strVal11 : "";
	m_strVal12 = m_nCmb12 > 0 ? m_strVal12 : "";
	m_strVal13 = m_nCmb13 > 0 ? m_strVal13 : "";
	m_strVal14 = m_nCmb14 > 0 ? m_strVal14 : "";
	m_strVal15 = m_nCmb15 > 0 ? m_strVal15 : "";
	m_strVal16 = m_nCmb16 > 0 ? m_strVal16 : "";

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	char strSQL[10240] = {0};
	sprintf(strSQL, "DELETE FROM RTU_OTHER_TEST_EMC WHERE RTU_ID=%s", m_strTermNo);
	pConnect->ExecSQL(strSQL);
		
	sprintf(strSQL, "INSERT INTO RTU_OTHER_TEST_EMC(RTU_ID, ISQ1, DESC1, ISQ2, DESC2, ISQ3, DESC3, ISQ4, DESC4, ISQ5, DESC5, " \
		"ISQ6, DESC6, ISQ7, DESC7, ISQ8, DESC8, ISQ9, DESC9, ISQ10, DESC10, ISQ11, DESC11, ISQ12, DESC12, ISQ13, DESC13, " \
		"ISQ14, DESC14, ISQ15, DESC15, ISQ16, DESC16) " \
		"VALUES(%s, %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', "\
		"%d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s')", 
		m_strTermNo, !m_nCmb1, m_strVal1, !m_nCmb2, m_strVal2, !m_nCmb3, m_strVal3, !m_nCmb4, m_strVal4, !m_nCmb5, m_strVal5, !m_nCmb6, m_strVal6,
		!m_nCmb7, m_strVal7, !m_nCmb8, m_strVal8, !m_nCmb9, m_strVal9, !m_nCmb10, m_strVal10, !m_nCmb11, m_strVal11, !m_nCmb12, m_strVal12, 
		!m_nCmb13, m_strVal13, !m_nCmb14, m_strVal14, !m_nCmb15, m_strVal15, !m_nCmb16, m_strVal16);
	pConnect->ExecSQL(strSQL);
}

void CDlgOtherTestInfo::InitOtherTestFunction(int nID)
{
	m_nSJZC = m_nCSSZ = m_nQXSJ = m_nRSJ = m_nYSJ = m_nSJJL = m_nGJJL = m_nZDDS = m_nSJTC = 0;
	m_strSJZC = "";
	m_strCSSZ = "";
	m_strQXSJ = "";
	m_strRSJ = "";
	m_strYSJ = "";
	m_strSJJL = "";
	m_strGJJL = "";
	m_strZDDS = "";
	m_strSJTC = "";

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();	
	CADORecordSet record;
	char strSQL[300] = {0};	
	sprintf(strSQL,"SELECT * FROM RTU_OTHER_TEST_FUNCTION WHERE RTU_ID=%d", nID);

	if (pConnect != NULL && pConnect->ExecSQL(strSQL,record) && !record.IsEndEOF())
	{
		_variant_t val;	
	
		record.GetFieldValue("ISQ_SJZC", val);
		m_nSJZC = !val.boolVal;
		record.GetFieldValue("DESC_SJZC", val);
		m_strSJZC = val.bstrVal;
	
		record.GetFieldValue("ISQ_CSSZ", val);
		m_nCSSZ = !val.boolVal;
		record.GetFieldValue("DESC_CSSZ", val);
		m_strCSSZ = val.bstrVal;

		record.GetFieldValue("ISQ_QXSJ", val);
		m_nQXSJ = !val.boolVal;
		record.GetFieldValue("DESC_QXSJ", val);
		m_strQXSJ = val.bstrVal;

		record.GetFieldValue("ISQ_RSJ", val);
		m_nRSJ = !val.boolVal;
		record.GetFieldValue("DESC_RSJ", val);
		m_strRSJ = val.bstrVal;

		record.GetFieldValue("ISQ_YSJ", val);
		m_nYSJ = !val.boolVal;
		record.GetFieldValue("DESC_YSJ", val);
		m_strYSJ = val.bstrVal;

		record.GetFieldValue("ISQ_SJJL", val);
		m_nSJJL = !val.boolVal;
		record.GetFieldValue("DESC_SJJL", val);
		m_strSJJL = val.bstrVal;

		record.GetFieldValue("ISQ_GJJL", val);
		m_nGJJL = !val.boolVal;
		record.GetFieldValue("DESC_GJJL", val);
		m_strGJJL = val.bstrVal;

		record.GetFieldValue("ISQ_ZDDS", val);
		m_nZDDS = !val.boolVal;
		record.GetFieldValue("DESC_ZDDS", val);
		m_strZDDS = val.bstrVal;

		record.GetFieldValue("ISQ_SJTC", val);
		m_nSJTC = !val.boolVal;
		record.GetFieldValue("DESC_SJTC", val);
		m_strSJTC = val.bstrVal;
	}	

	UpdateData(FALSE);
}

void CDlgOtherTestInfo::SaveOtherTestFunction()
{
	UpdateData(TRUE);

	m_strSJZC = m_nSJZC > 0 ? m_strSJZC : "";
	m_strCSSZ = m_nCSSZ > 0 ? m_strCSSZ : "";
	m_strQXSJ = m_nQXSJ > 0 ? m_strQXSJ : "";
	m_strRSJ = m_nRSJ > 0 ? m_strRSJ : "";
	m_strYSJ = m_nYSJ > 0 ? m_strYSJ : "";
	m_strSJJL = m_nSJJL > 0 ? m_strSJJL : "";
	m_strGJJL = m_nGJJL > 0 ? m_strGJJL : "";
	m_strZDDS = m_nZDDS > 0 ? m_strZDDS : "";
	m_strSJTC = m_nSJTC > 0 ? m_strSJTC : "";

	CADODataBase *pConnect = CDBAccess::GetInstance()->GetDefaultConnect();
	if (pConnect == NULL)
		return ;
	char strSQL[10240] = {0};
	sprintf(strSQL, "DELETE FROM RTU_OTHER_TEST_FUNCTION WHERE RTU_ID=%s", m_strTermNo);
	pConnect->ExecSQL(strSQL);

	// 追加检定时间
	CString strCheckDT = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:00");
		
	sprintf(strSQL, "INSERT INTO RTU_OTHER_TEST_FUNCTION(RTU_ID, ISQ_SJZC, DESC_SJZC, ISQ_CSSZ, DESC_CSSZ, ISQ_QXSJ, DESC_QXSJ, "\
		"ISQ_RSJ, DESC_RSJ, ISQ_YSJ, DESC_YSJ, ISQ_SJJL, DESC_SJJL, ISQ_GJJL, DESC_GJJL, ISQ_ZDDS, DESC_ZDDS, ISQ_SJTC, DESC_SJTC, " \
		"CHECK_DATETIME) "		
		"VALUES(%s, %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s', %d, '%s','%s')", 
		m_strTermNo, !m_nSJZC, m_strSJZC, !m_nCSSZ, m_strCSSZ, !m_nQXSJ, m_strQXSJ, !m_nRSJ, m_strRSJ, !m_nYSJ, m_strYSJ, 
		!m_nSJJL, m_strSJJL, !m_nGJJL, m_strGJJL, !m_nZDDS, m_strZDDS, !m_nSJTC, m_strSJTC, strCheckDT);
	pConnect->ExecSQL(strSQL);
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbSSC()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_SSC)->EnableWindow(m_nSSC);
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbLAMP()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_LAMP)->EnableWindow(m_nLAMP);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbBARCODE()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_BARCODE)->EnableWindow(m_nBARCODE);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbLID()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_LID)->EnableWindow(m_nLID);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbPEDESTAL()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_PEDESTAL)->EnableWindow(m_nPEDESTAL);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbNAMEPLATE()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_NAMEPLATE)->EnableWindow(m_nNAMEPLATE);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbSCREW()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_SCREW)->EnableWindow(m_nSCREW);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb1()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT1)->EnableWindow(m_nCmb1);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb2()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT2)->EnableWindow(m_nCmb2);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb3()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT3)->EnableWindow(m_nCmb3);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb4()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT4)->EnableWindow(m_nCmb4);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb5()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT5)->EnableWindow(m_nCmb5);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb6()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT6)->EnableWindow(m_nCmb6);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb7()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT7)->EnableWindow(m_nCmb7);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb8()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT8)->EnableWindow(m_nCmb8);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb9()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT9)->EnableWindow(m_nCmb9);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb10()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT10)->EnableWindow(m_nCmb10);
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb11()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT11)->EnableWindow(m_nCmb11);
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb12()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT12)->EnableWindow(m_nCmb12);
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb13()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT13)->EnableWindow(m_nCmb13);
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb14()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT14)->EnableWindow(m_nCmb14);
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb15()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT15)->EnableWindow(m_nCmb15);
}

void CDlgOtherTestInfo::OnCbnSelchangeCmb16()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT16)->EnableWindow(m_nCmb16);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbSJZC()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_SJZC)->EnableWindow(m_nSJZC);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbCSSZ()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_CSSZ)->EnableWindow(m_nCSSZ);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbQXSJ()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_QXSJ)->EnableWindow(m_nQXSJ);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbRSJ()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_RSJ)->EnableWindow(m_nRSJ);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbYSJ()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_YSJ)->EnableWindow(m_nYSJ);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbSJJL()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_SJJL)->EnableWindow(m_nSJJL);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbGJJL()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_GJJL)->EnableWindow(m_nGJJL);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbZDDS()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_ZDDS)->EnableWindow(m_nZDDS);	
}

void CDlgOtherTestInfo::OnCbnSelchangeCmbSJTC()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TXT_SJTC)->EnableWindow(m_nSJTC);	
}

void CDlgOtherTestInfo::RefreshState()
{
	OnCbnSelchangeCmbSSC();
	OnCbnSelchangeCmbLAMP();
	OnCbnSelchangeCmbBARCODE();
	OnCbnSelchangeCmbLID();
	OnCbnSelchangeCmbPEDESTAL();
	OnCbnSelchangeCmbNAMEPLATE();
	OnCbnSelchangeCmbSCREW();
	OnCbnSelchangeCmb1();
	OnCbnSelchangeCmb2();
	OnCbnSelchangeCmb3();
	OnCbnSelchangeCmb4();
	OnCbnSelchangeCmb5();
	OnCbnSelchangeCmb6();
	OnCbnSelchangeCmb7();
	OnCbnSelchangeCmb8();
	OnCbnSelchangeCmb9();
	OnCbnSelchangeCmb10();
	OnCbnSelchangeCmb11();
	OnCbnSelchangeCmb12();
	OnCbnSelchangeCmb13();
	OnCbnSelchangeCmb14();
	OnCbnSelchangeCmb15();
	OnCbnSelchangeCmb16();
	OnCbnSelchangeCmbSJZC();
	OnCbnSelchangeCmbCSSZ();
	OnCbnSelchangeCmbQXSJ();
	OnCbnSelchangeCmbRSJ();
	OnCbnSelchangeCmbYSJ();
	OnCbnSelchangeCmbSJJL();
	OnCbnSelchangeCmbGJJL();
	OnCbnSelchangeCmbZDDS();
	OnCbnSelchangeCmbSJTC();
}