#include "stdafx.h"
#include "comutil.h"
#include "ADORecordSet.h"


CADORecordSet::CADORecordSet(void)
{
	m_RecordsetPtr.CreateInstance( __uuidof(Recordset));
	m_bOpen = false;
}

CADORecordSet::~CADORecordSet(void)
{
	if (m_bOpen)
	{
		m_RecordsetPtr->Close();
		m_RecordsetPtr.Release();
	}
}

void CADORecordSet::Close()
{
	if (m_bOpen)
	{
		m_RecordsetPtr->Close();
		m_bOpen = false;
	}
}

bool CADORecordSet::IsOpen()
{
	return m_bOpen;
}


void CADORecordSet::SetOpen()
{
	m_bOpen = true;
}

bool CADORecordSet::IsEndEOF(void)
{
	int nRet= m_RecordsetPtr->adoEOF;
	if (nRet==0)
		return false;

	return true;
}


bool CADORecordSet::IsBeginBOF(void)
{	
	int nRet= m_RecordsetPtr->BOF;
	if (nRet==0)
		return false;

	return true;
}

bool CADORecordSet::RecordMoveNext(void)
{
	try
	{
		m_RecordsetPtr->MoveNext();
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	return true;
	
}

bool CADORecordSet::RecordMovePre(void)
{	
	try
	{
		m_RecordsetPtr->MovePrevious();
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	return true;
	
}

bool CADORecordSet::RecordMoveFirst(void)
{
	try
	{
		m_RecordsetPtr->MoveFirst();
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	return true;
	
}

bool CADORecordSet::RecordMoveLast(void)
{	
	try
	{
		m_RecordsetPtr->MoveLast();
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	return true;
	
}

void RemoveBlank(char *strValue,int valueSize)
{
	int nLen = (int)strlen(strValue);
	if (nLen > valueSize)
		nLen = valueSize;
	for (int n=nLen-1;n>=0;n--)
	{
		if (strValue[n] == 0x20)
			strValue[n] = 0;
		else
			break;
	}
}

bool CADORecordSet::GetFieldValue(int nIndex,_variant_t & vRet)
{
	_variant_t vtField((long)nIndex);
	try 
	{
		vRet = m_RecordsetPtr->GetCollect(vtField);
	} 
	catch (_com_error & e) 
	{
		SetLastError(&e);
		return false;
	}

	return true;
}

bool CADORecordSet::GetFieldValue(const char *pFieldName,_variant_t & vRet)
{
	_variant_t vtField(pFieldName);
	try 
	{
		vRet = m_RecordsetPtr->GetCollect(vtField);
	} 
	catch (_com_error & e) 
	{
		SetLastError(&e);
		return false;
	}

	return true;
}

bool CADORecordSet::GetFieldValue(const char *pFieldName,char *strValue,int valueSize)
{
	_variant_t vRet;
	_variant_t vtField(pFieldName);
	try
	{
		vRet=m_RecordsetPtr->GetCollect (vtField );
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	if (vRet.vt!=VT_NULL)
	{
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)vRet.bstrVal, -1, strValue, valueSize, NULL, NULL);
		RemoveBlank(strValue, valueSize);
		return true;
	}
	return false;
}

bool CADORecordSet::GetFieldValue(const char *pFieldName,short int & nValue)
{
	nValue=0;
	_variant_t vRet;
	_variant_t vtField(pFieldName);
	try
	{
		vRet=m_RecordsetPtr->GetCollect( vtField );
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	if (vRet.vt!=VT_NULL)
	{
		nValue=vRet.iVal;
	}
	return true;
}

bool CADORecordSet::GetFieldValue(const char *pFieldName,long & lValue)
{
	lValue=0;
	_variant_t vRet;
	_variant_t vtField(pFieldName);
	try
	{
		vRet=m_RecordsetPtr->GetCollect( vtField );
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	if (vRet.vt!=VT_NULL)
	{
		lValue=vRet;
	}
	return true;
}


bool CADORecordSet::GetFieldValue(const char *pFieldName,int & nValue)
{
	nValue=0;
	_variant_t vRet;
	_variant_t vtField(pFieldName);
	try
	{
		vRet=m_RecordsetPtr->GetCollect( vtField );
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	if (vRet.vt!=VT_NULL)
	{
		nValue=vRet.uintVal;
	}
	return true;
}

bool CADORecordSet::GetFieldValue(const char *pFieldName,unsigned int & nValue)
{
	nValue=0;
	_variant_t vRet;
	_variant_t vtField(pFieldName);
	try
	{
		vRet=m_RecordsetPtr->GetCollect( vtField );
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	if (vRet.vt!=VT_NULL)
	{
		nValue=vRet.uintVal;
	}
	return true;
}

bool CADORecordSet::GetFieldValue(const char *pFieldName,unsigned long & ulValue)
{
	ulValue=0;
	_variant_t vRet;
	_variant_t vtField(pFieldName);
	try
	{
		vRet=m_RecordsetPtr->GetCollect( vtField );
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	if (vRet.vt!=VT_NULL)
	{
		ulValue=vRet.ulVal;
	}
	return true;
}


bool CADORecordSet::GetFieldValue(const char *pFieldName,float & fValue)
{
	fValue=0;
	_variant_t vRet;
	_variant_t vtField(pFieldName);
	try
	{
		vRet=m_RecordsetPtr->GetCollect( vtField );
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	if (vRet.vt!=VT_NULL)
	{
		fValue=vRet.fltVal;
	}
	return true;
}


bool CADORecordSet::GetFieldValue(const char *pFieldName,double & dbValue)
{
	dbValue=0;
	_variant_t vRet;
	_variant_t vtField(pFieldName);
	try
	{
		vRet=m_RecordsetPtr->GetCollect( vtField );
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	if (vRet.vt!=VT_NULL)
	{
		dbValue=vRet.dblVal;
	}
	return true;
}



bool CADORecordSet::GetFieldValue(const char *pFieldName,SYSTEMTIME &lpSystemTime)
{
   _variant_t vRet;
   _variant_t vtField(pFieldName);

	try
	{
		vRet=m_RecordsetPtr->GetCollect( vtField );
	}
	catch(_com_error e)///²¶×½Òì³£
	{
		SetLastError(&e);
		return false;
	}
	if (vRet.vt==VT_DATE)
	{
		VariantTimeToSystemTime(vRet.date,&lpSystemTime);
	}
	return true;
}

bool CADORecordSet::SetFieldValue(const char *pFieldName, const char *pValue)
{
	_variant_t vtValue(pValue);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}
	return true;
}

bool CADORecordSet::SetFieldValue(const char *pFieldName, int nValue)
{
	long lValue=nValue;
	_variant_t vtValue(lValue);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}

	return true;
}

bool CADORecordSet::SetFieldValue(const char *pFieldName, unsigned int nValue)
{
	/*
	unsigned long lValue=nValue;
	_variant_t vtValue(lValue);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}
	*/

	return true;
}


bool CADORecordSet::SetFieldValue(const char *pFieldName, long lValue)
{
	_variant_t vtValue(lValue);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}
	return true;
}

bool CADORecordSet::SetFieldValue(const char *pFieldName, float fValue)
{
	_variant_t vtValue(fValue);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}
	return true;
}

bool CADORecordSet::SetFieldValue(const char *pFieldName, short int nValue)
{
	_variant_t vtValue(nValue);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}
	return true;
}

bool CADORecordSet::SetFieldValue(const char *pFieldName, double dbValue)
{
	_variant_t vtValue(dbValue);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}
	return true;
}

bool CADORecordSet::SetFieldValue(const char *pFieldName, unsigned long ulValue)
{
	/*
	_variant_t vtValue(ulValue);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}
	*/
	return true;
}

bool CADORecordSet::SetLongTime(const char *pFieldName, SYSTEMTIME lpSystemTime)
{
	VARIANT varTemp;
	varTemp.vt=VT_DATE;
	SystemTimeToVariantTime(&lpSystemTime,&varTemp.date);
	_variant_t vtValue(varTemp);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}
	return true;
}

bool CADORecordSet::SetShortTime(const char *pFieldName,SYSTEMTIME lpSystemTime)
{
	VARIANT varTemp;
	varTemp.vt=VT_DATE;
	SystemTimeToVariantTime(&lpSystemTime,&varTemp.date);
	int nTemp=(int)varTemp.date/1;
	varTemp.date=varTemp.date-nTemp;
	_variant_t vtValue(varTemp);
	_variant_t vtField(pFieldName);
	try
	{
		m_RecordsetPtr->PutCollect (vtField ,vtValue);
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return false;
	}
	return true;
}

bool CADORecordSet::Update(void)
{
	try
	{
		m_RecordsetPtr->Update();
	}
	catch(_com_error e)
	{
		SetLastError(&e);
		return FALSE;
	}
	
	return true;
}

Recordset20Ptr CADORecordSet::GetRecordsetPtr()
{
	return m_RecordsetPtr;
}

void CADORecordSet::SetLastError(_com_error *e)
{
	wchar_t *pstr=NULL;
	e->ErrorInfo()->GetDescription(&pstr);
	char buf[1024];
	memset(buf,0,1024);
	if (pstr)
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pstr, -1, buf, 1024, NULL, NULL);

	m_strLastError = string (buf);
}

const char *CADORecordSet::GetLastError()
{
	return m_strLastError.c_str();
}