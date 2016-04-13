// RecordLog.cpp: implementation of the CRecordLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecordLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define LOG_DIR_NAME    "Log"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecordLog *CRecordLog::m_pInstance=NULL;
VEC_LOG_FILE CRecordLog::m_vecLogFile;
CRecordLog::CRecordLog()
{

}

CRecordLog::~CRecordLog()
{
	RemoveLogFile("--");
}

CRecordLog *CRecordLog::GetInstance()
{
	if (m_pInstance==NULL)
		m_pInstance=new CRecordLog;
	RemoveLogFile(GetDate());
	return m_pInstance;
}

void CRecordLog::WriteLog(CString strRecord,CString strFileTitle,CString strSubDir1)
{
	CString strPath=GetAppPath()+LOG_DIR_NAME+"\\";
	if (::GetFileAttributes(strPath) == -1)
		::CreateDirectory(strPath,NULL);
	CString strDate=GetDate();
	strPath+=strDate+"\\";
	if (::GetFileAttributes(strPath) == -1)
		::CreateDirectory(strPath,NULL);
	if (strSubDir1 != "")
	{
		strPath+=strSubDir1+"\\";
		if (::GetFileAttributes(strPath) == -1)
			::CreateDirectory(strPath,NULL);
	}
	strPath+=strFileTitle+".txt";
	CStdioFile *pFile=GetLogFile(strPath);
	if (pFile==NULL)
	{
		pFile=new CStdioFile;
		if (::GetFileAttributes(strPath)==-1)
			pFile->Open(strPath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone);
		else
		{
			pFile->Open(strPath,CFile::modeWrite|CFile::shareDenyNone);
			pFile->SeekToEnd();
		}
		LOG_FILE_INFO lfi;
		lfi.strFilePath=strPath;
		lfi.strDate=strDate;
		lfi.pFile=pFile;
		m_vecLogFile.push_back(lfi);
	}
	CString strInfo=CTime::GetCurrentTime().Format("[%H:%M:%S] ")+strRecord+"\n";
	pFile->WriteString(strInfo);
	pFile->Flush();
}


CString CRecordLog::GetAppPath()
{
	char szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    CString strPath = szPath;
    strPath = strPath.Left(strPath.ReverseFind('\\')+1);
	return strPath;
}

CString CRecordLog::GetDate()
{
	CTime tm=CTime::GetCurrentTime();
	CString str;
	str.Format("%d-%02d-%02d",tm.GetYear(),tm.GetMonth(),tm.GetDay());
	return str;
}

void CRecordLog::RemoveLogFile(CString strDate)
{
	int nPos=0;
	while (nPos<m_vecLogFile.size())
	{
		if (m_vecLogFile[nPos].strDate!=strDate)
		{
			CStdioFile *pFile=m_vecLogFile[nPos].pFile;
			pFile->Close();
			delete pFile;
			m_vecLogFile.erase(m_vecLogFile.begin()+nPos);
		}
		else
		{
			nPos++;
		}
	}
}

CStdioFile *CRecordLog::GetLogFile(CString strFilePath)
{
	for (int i=0;i<m_vecLogFile.size();i++)
	{
		if (m_vecLogFile[i].strFilePath==strFilePath)
			return m_vecLogFile[i].pFile;
	}
	return NULL;
}