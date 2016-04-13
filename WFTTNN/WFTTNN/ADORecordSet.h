#pragma once

#include "DBAccess_Global.h"

class  CADORecordSet
{
public:
	CADORecordSet(void);
	~CADORecordSet(void);
	
	void Close();
	
	bool IsEndEOF(void);
	bool IsBeginBOF(void);
	bool RecordMoveNext(void);
	bool RecordMovePre(void);
	bool RecordMoveFirst(void);
	bool RecordMoveLast(void);
	
	bool GetFieldValue(int nIndex,_variant_t & vRet);
	bool GetFieldValue(const char *pFieldName,_variant_t & vRet);
	
	bool GetFieldValue(const char *pFieldName,char *strValue,int valueSize);
	bool GetFieldValue(const char *pFieldName,short int & nValue);
	bool GetFieldValue(const char *pFieldName,long & lValue);
	bool GetFieldValue(const char *pFieldName,int & nValue);
	bool GetFieldValue(const char *pFieldName,unsigned int & nValue);
	bool GetFieldValue(const char *pFieldName,unsigned long & ulValue);
	bool GetFieldValue(const char *pFieldName,float & fValue);
	bool GetFieldValue(const char *pFieldName,double & dbValue);
	bool GetFieldValue(const char *pFieldName,SYSTEMTIME &lpSystemTime);
	
	bool SetFieldValue(const char *pFieldName, const char *pValue);
	bool SetFieldValue(const char *pFieldName, int nValue);
	bool SetFieldValue(const char *pFieldName, unsigned int nValue);
	bool SetFieldValue(const char *pFieldName, float fValue);
	bool SetFieldValue(const char *pFieldName, long lValue);
	bool SetFieldValue(const char *pFieldName, short int nValue);
	bool SetFieldValue(const char *pFieldName, double dbValue);
	bool SetFieldValue(const char *pFieldName, unsigned long ulValue);
	bool SetLongTime(const char *pFieldName, SYSTEMTIME lpSystemTime);
	bool SetShortTime(const char *pFieldName,SYSTEMTIME lpSystemTime);
	
	bool Update(void);
	
	const char *GetLastError();
	Recordset20Ptr GetRecordsetPtr();
	
	bool IsOpen();
	void SetOpen();
	
protected :
	Recordset20Ptr m_RecordsetPtr;
	
	void SetLastError(_com_error *e);
	
	string m_strLastError;
	bool m_bOpen;
};