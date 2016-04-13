#pragma once


#include "DBAccess_Global.h"
#include "ADORecordSet.h"

class  CADODataBase
{
public:
	CADODataBase(void);
	~CADODataBase(void);
	
	const char *GetConnectName();
	void SetConnectName(const char* pConnectName);
	
	bool OpenDB_ACCESS(const char* pFileName,const char* pPWD);
	bool OpenDB_EXCEL(const char* pFileName);
	bool OpenDB_SQLServer(const char* strHost,const char* pDBName,const char* pUser,const char* pPWD);
	bool OpenDB_ORACLE(const char* pConnectName,const char* pUserName,const char* pPWD);
	bool OpenDB_SYBASE(const char* pConnectName,const char* pUserName,const char* pPWD);
	bool OpenDB_SYBASE15_2(const char* pConnectName,const char* pUserName,const char* pPWD);//Çý¶¯°æ±¾15.2
	bool OpenDB(const char* strConnectString,const char *pUserName = "",const char *pPSW = "");
	bool CloseDB(void);
	
	bool ExecSQL(const char *pSQL);
	bool ExecSQL(const char *pSQL,CADORecordSet &RecordSet,
		CursorTypeEnum cursorType = adOpenDynamic,
		LockTypeEnum lockType = adLockOptimistic);
	
	long BeginTransaction();
	long CommitTransaction();
	long RollbackTransaction();
	
	const char * GetLastError();
	
protected :
	string m_strConnectName;
	_ConnectionPtr m_ConnectionPtr;
	string m_strConnectString;
	string m_strUserName;
	string m_strPSW;
	string m_strLastError;
	
	void SetLastError(const char *strInfo,_com_error *e);
	bool CheckDBConnect();
	bool ReConnect();
	
	
};
