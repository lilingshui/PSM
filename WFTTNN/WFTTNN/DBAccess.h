#ifndef DBACCESS_H
#define DBACCESS_H

#include "DBAccess_global.h"

#include "ADODataBase.h"
#include "ADORecordSet.h"

class  CDBAccess
{
public:
	
	CDBAccess();
	~CDBAccess();
	
	static CDBAccess *GetInstance();
	
	CADODataBase *GetDefaultConnect();
	void SetDefaultConnect(CADODataBase *connect);
	
	CADODataBase *GetConnect(const char *connectName);
	
	//打开平台提供的dbcon连接
	CADODataBase *AddConnect_ACCESS(const char * connectName,const char * fileName,const char * psw);
	CADODataBase *AddConnect_EXCEL(const char * connectName,const char * fileName);
	CADODataBase *AddConnect_ORACLE(const char * connectName,const char * dbName,const char * userName,const char * psw);
	CADODataBase *AddConnect_SYBASE(const char * connectName,const char * dbName,const char * userName,const char * psw);
	CADODataBase *AddConnect_SYBASE15_2(const char * connectName,const char * dbName,const char * userName,const char * psw);//驱动版本15.2
	CADODataBase *AddConnect_SQLServer(const char * connectName,const char * dbName,const char * userName,const char * psw);
	
	void RemoveConnect(const char * connectName);
	
	const char *GetLastError();
private:
	
	static CDBAccess *m_pInstance;
	
	CADODataBase *m_defaultConnect;
	
	vector <CADODataBase *> m_vConnect;
	
	char m_strlastError[2024];
	
};


#endif // DBACCESS_H
