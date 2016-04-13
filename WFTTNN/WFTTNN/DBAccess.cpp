#include "stdafx.h"
#include "DBAccess.h"
#include "ADODataBase.h"
#include "ADORecordSet.h"

CDBAccess *CDBAccess::m_pInstance = NULL;
CDBAccess::CDBAccess()
{
	m_defaultConnect=NULL;
}

CDBAccess::~CDBAccess()
{
	m_vConnect.clear();
}

CDBAccess *CDBAccess::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CDBAccess();
	}
	return m_pInstance;
}


CADODataBase *CDBAccess::GetDefaultConnect()
{
	return m_defaultConnect;
}

void CDBAccess::SetDefaultConnect(CADODataBase *connect)
{
	m_defaultConnect = connect;
}

CADODataBase *CDBAccess::GetConnect(const char * connectName)
{
	int nCount = (int)m_vConnect.size();
	for (int i=0;i<nCount;i++)
	{
		CADODataBase *pDBConnect = m_vConnect[i];
		if (pDBConnect)
		{
			if (pDBConnect->GetConnectName() == string (connectName))
				return pDBConnect;
		}
	}

	return NULL;
}


CADODataBase *CDBAccess::AddConnect_ACCESS(const char * connectName,const char * fileName,const char * psw)
{
	CADODataBase *pConnect = new CADODataBase();
	if (pConnect->OpenDB_ACCESS(fileName,psw) == false)
	{
		strcpy(m_strlastError,pConnect->GetLastError());
		delete pConnect;
		return NULL;
	}
	pConnect->SetConnectName(connectName);
	m_vConnect.push_back(pConnect);
	return pConnect;
}


CADODataBase *CDBAccess::AddConnect_EXCEL(const char * connectName,const char * fileName)
{
	CADODataBase *pConnect = new CADODataBase();
	if (pConnect->OpenDB_EXCEL(fileName) == false)
	{
		strcpy(m_strlastError,pConnect->GetLastError());
		delete pConnect;
		return NULL;
	}
	pConnect->SetConnectName(connectName);
	m_vConnect.push_back(pConnect);
	return pConnect;
}


CADODataBase *CDBAccess::AddConnect_ORACLE(const char * connectName,const char * dbName,const char * userName,const char * psw)
{
	CADODataBase *pConnect = new CADODataBase();
	if (pConnect->OpenDB_ORACLE(dbName,userName,psw) == false)
	{
		strcpy(m_strlastError,pConnect->GetLastError());
		delete pConnect;
		return NULL;
	}
	pConnect->SetConnectName(connectName);
	m_vConnect.push_back(pConnect);
	return pConnect;
}

CADODataBase *CDBAccess::AddConnect_SYBASE(const char * connectName,const char * dbName,const char * userName,const char * psw)
{
	CADODataBase *pConnect = new CADODataBase();
	if (pConnect->OpenDB_SYBASE(dbName,userName,psw) == false)
	{
		strcpy(m_strlastError,pConnect->GetLastError());
		delete pConnect;
		return NULL;
	}
	pConnect->SetConnectName(connectName);
	m_vConnect.push_back(pConnect);
	return pConnect;
}

CADODataBase *CDBAccess::AddConnect_SYBASE15_2(const char * connectName,const char * dbName,const char * userName,const char * psw)
{
	CADODataBase *pConnect = new CADODataBase();
	if (pConnect->OpenDB_SYBASE15_2(dbName,userName,psw) == false)
	{
		strcpy(m_strlastError,pConnect->GetLastError());
		delete pConnect;
		return NULL;
	}
	pConnect->SetConnectName(connectName);
	m_vConnect.push_back(pConnect);
	return pConnect;
}

CADODataBase *CDBAccess::AddConnect_SQLServer(const char * connectName,const char * dbName,const char * userName,const char * psw)
{
	char strHost[256];
	memset(strHost,0,256);
	char strDBName[256];
	memset(strDBName,0,256);
	const char *p = strstr(dbName,",");
	if (p)
	{
		strncpy(strHost,dbName,p-dbName);
		strcpy(strDBName,p+1);
	}

	CADODataBase *pConnect = new CADODataBase();
	if (pConnect->OpenDB_SQLServer(strHost,strDBName,userName,psw) == false)
	{
		strcpy(m_strlastError,pConnect->GetLastError());
		delete pConnect;
		return NULL;
	}
	pConnect->SetConnectName(connectName);
	m_vConnect.push_back(pConnect);
	return pConnect;
}

void CDBAccess::RemoveConnect(const char * connectName)
{
	int nCount = (int)m_vConnect.size();
	for(int n=0;n<nCount;n++)
	{
		CADODataBase *pDBConnect = m_vConnect[n];
		if (pDBConnect)
		{
			if (pDBConnect->GetConnectName() == string(connectName))
			{
				m_vConnect.erase(m_vConnect.begin() + n);
				pDBConnect->CloseDB();
				delete pDBConnect;
				return;
			}
		}
	}

}

const char *CDBAccess::GetLastError()
{
	return m_strlastError;
}