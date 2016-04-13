#include "stdafx.h"
#include "ADODataBase.h"

CADODataBase::CADODataBase(void)
{
	
}

CADODataBase::~CADODataBase(void)
{
}

const char *CADODataBase::GetConnectName()
{
	return m_strConnectName.c_str();
}

void CADODataBase::SetConnectName(const char* pConnectName)
{
	m_strConnectName = pConnectName;
}

bool CADODataBase::OpenDB_ACCESS(const char* pFileName,const char* pPWD)
{
	char strConnectString[1024];
	sprintf(strConnectString,"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password=%s",pFileName,pPWD);
	return OpenDB(strConnectString);
}

bool CADODataBase::OpenDB_EXCEL(const char* pFileName)
{
	char strConnectString[1024];
	sprintf(strConnectString,"DRIVER={MICROSOFT EXCEL DRIVER (*.XLS)};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",pFileName,pFileName);
	return OpenDB(strConnectString);
}


bool CADODataBase::OpenDB_SQLServer(const char* pHost,const char* pDBName,const char* pUser,const char* pPWD)
{
	char strConnectString[1024];
	sprintf(strConnectString,"Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s",pPWD,pUser,pDBName,pHost);
	return OpenDB(strConnectString);
}



bool CADODataBase::OpenDB_ORACLE(const char* pConnectName,const char* pUserName,const char* pPWD)
{
	char strConnectString[1024];
	//sprintf(strConnectString,"Provider=MSDAORA.1;Password=%s;Persist Security Info=True;User ID=%s;Data Source=%s",pPWD,pUserName,pConnectName);
	sprintf(strConnectString,"Provider=OraOLEDB.Oracle.1;Password=%s;Persist Security Info=True;User ID=%s;Data Source=%s",pPWD,pUserName,pConnectName);
	return OpenDB(strConnectString,"","");
}

bool CADODataBase::OpenDB_SYBASE(const char* pConnectName,const char* pUserName,const char* pPWD)
{
	char strConnectString[1024];
	sprintf(strConnectString,"Provider=MSDASQL.1;Persist Security Info=True;Mode=ReadWrite;WorkArounds2=24;Extended Properties=\"DRIVER={Sybase ASE ODBC Driver};NA=%s;\"",pConnectName);
	//sprintf(strConnectString,"Provider=MSDASQL.1;Persist Security Info=True;Mode=ReadWrite;WorkArounds2=24;Extended Properties=\"DRIVER={Adaptive Server Enterprise};NA=%s;\"",pConnectName);
	return OpenDB(strConnectString,pUserName,pPWD);

	return true;
}

bool CADODataBase::OpenDB_SYBASE15_2(const char* pConnectName,const char* pUserName,const char* pPWD)
{
	char strConnectString[1024];
	sprintf(strConnectString,"Provider=MSDASQL.1;Persist Security Info=True;Mode=ReadWrite;WorkArounds2=24;Extended Properties=\"DRIVER={Adaptive Server Enterprise};NA=%s;\"",pConnectName);
	return OpenDB(strConnectString,pUserName,pPWD);

	return true;
}

bool CADODataBase::OpenDB(const char* pConnectString,const char *pUserName,const char *pPSW)
{
	CoInitialize(NULL);

	HRESULT hr = m_ConnectionPtr.CreateInstance( __uuidof(Connection));

	try
	{
		_bstr_t vtField(pConnectString);
		_bstr_t vtField1(pUserName);
		_bstr_t vtField2(pPSW);
		hr = m_ConnectionPtr->Open(vtField,vtField1,vtField2,adConnectUnspecified);///连接数据库	
		m_strConnectString = string(pConnectString);
		m_strUserName = string(pUserName);
		m_strPSW = string(pPSW);
	}
	catch(_com_error e)///捕捉异常
	{
		SetLastError("打开数据库失败",&e);
		return false;
	}

	return true;
}

bool CADODataBase::CloseDB(void)
{
	try
	{
		m_ConnectionPtr->Close();
		m_ConnectionPtr.Release();
	}
	catch(_com_error e)///捕捉异常
	{
		SetLastError("关闭数据库失败",&e);
		return false;
	}
	return true;
}


bool CADODataBase::ExecSQL(const char *pSQL,CADORecordSet &RecordSet,CursorTypeEnum cursorType,LockTypeEnum lockType)
{

	HRESULT hr = 0;
	try
	{
		hr = RecordSet.GetRecordsetPtr()->Open(	pSQL,
											m_ConnectionPtr.GetInterfacePtr(),
											cursorType,lockType,adCmdText);
	}
	catch(_com_error e)///捕捉异常
	{
	
		char strInfo[2048];
		memset(strInfo,0,2048);
		strcpy(strInfo,"数据库查询失败:");
		if (strlen(pSQL)<2000)
		{	
			strcat(strInfo,pSQL);
		}
		SetLastError(strInfo,&e);

		/*
		if (CheckDBConnect() == false)
		{
			ReConnect();
		}
		*/

		return false;
	}
	RecordSet.SetOpen();
	return true;
}


bool CADODataBase::ExecSQL(const char *pSQL)
{
	try
	{
		m_ConnectionPtr->Execute(pSQL,NULL,adCmdUnknown|adExecuteNoRecords);
	}
	catch(_com_error e)///捕捉异常
	{
		char strInfo[2048];
		memset(strInfo,0,2048);
		strcpy(strInfo,"数据库执行失败:");
		if (strlen(pSQL)<2000)
		{	
			strcat(strInfo,pSQL);
		}
		SetLastError(strInfo,&e);

		if (CheckDBConnect() == false)
		{
			ReConnect();
		}

		return false;
	}
	return true;
}

long CADODataBase::BeginTransaction() 
{
	return m_ConnectionPtr->BeginTrans();
}

long CADODataBase::CommitTransaction() 
{
	return m_ConnectionPtr->CommitTrans();
}

long CADODataBase::RollbackTransaction() 
{
	return m_ConnectionPtr->RollbackTrans();
}

void CADODataBase::SetLastError(const char* strInfo,_com_error *e)
{
	wchar_t *pstr=NULL;
	e->ErrorInfo()->GetDescription(&pstr);
	char buf[1024];
	memset(buf,0,1024);
	if (pstr)
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pstr, -1, buf, 1024, NULL, NULL);

	m_strLastError = string (buf);

}

const char *CADODataBase::GetLastError()
{
	return m_strLastError.c_str();
}


bool CADODataBase::CheckDBConnect()
{
	CADORecordSet RecordSet;
	
	try
	{
		
		RecordSet.GetRecordsetPtr()->Open(	"SELECT * FROM APP_SUBSYSTEM WHERE 1=0",
											m_ConnectionPtr.GetInterfacePtr(),
											adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch(_com_error e)///捕捉异常
	{
		return false;
	}

	RecordSet.SetOpen();

	return true;
	
}

bool CADODataBase::ReConnect()
{
	CloseDB();

	return OpenDB(m_strConnectString.c_str(),m_strUserName.c_str(),m_strPSW.c_str());
}