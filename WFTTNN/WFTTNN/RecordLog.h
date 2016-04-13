// RecordLog.h: interface for the CRecordLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDLOG_H__A5B2360E_C06D_4883_A399_7F52D84EA039__INCLUDED_)
#define AFX_RECORDLOG_H__A5B2360E_C06D_4883_A399_7F52D84EA039__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _LOG_FILE_INFO
{
	CString strFilePath;
	CString strDate;
	CStdioFile *pFile;
}LOG_FILE_INFO;

typedef list<CString> LIST_STR;
typedef list<CString>::iterator ITER_LIST_STR;
typedef vector<LOG_FILE_INFO> VEC_LOG_FILE;

class CRecordLog  
{
public:
	CRecordLog();
	virtual ~CRecordLog();
	void WriteLog(CString strRecord,CString strFileTitle,CString strSubDir1="");
	static CRecordLog *GetInstance();
private:
	static CRecordLog *m_pInstance;
    static VEC_LOG_FILE m_vecLogFile;
	CString GetAppPath();
	static CString GetDate();
	static void RemoveLogFile(CString strDate);
	CStdioFile *GetLogFile(CString strFilePath);
};

#endif // !defined(AFX_RECORDLOG_H__A5B2360E_C06D_4883_A399_7F52D84EA039__INCLUDED_)
