// LogFile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILE_H__5EECBFB5_16AA_4DD9_9053_4871B229DBFB__INCLUDED_)
#define AFX_LOGFILE_H__5EECBFB5_16AA_4DD9_9053_4871B229DBFB__INCLUDED_
#include "CommFunctions.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef _WIN32   //windows OS
const char SZ_SIGN_DIR[] = "\\";
const TCHAR WSZ_SIGN_DIR[] = _TEXT("\\");
const char CH_SIGN_DIR = '\\';
const TCHAR WCH_SIGN_DIR= '\\';
#else 
const char SZ_SIGN_DIR[] = "/";
const TCHAR WSZ_SIGN_DIR[] = _TEXT("/");
const char CH_SIGN_DIR = '/';
const TCHAR WCH_SIGN_DIR = '/';
#endif

class CLogFile  
{
public:
	BOOL CreateLogFile( char* pFileType, char* pPath=NULL );
	void RemoveExpiredLogFile( char* pFileType, int32_t nRetainedDays=7, char* pPath=NULL );
	void CloseLogFile();
	void AddString(char* pStr);	
	void AddString(TCHAR* pStr);
	CLogFile();
	virtual ~CLogFile();
private:
	CStdioFile m_File;
	int32_t m_nRetainedDays;

	CString csDate;
	CString csFileName;
	CString csPathName;
};

#endif // !defined(AFX_LOGFILE_H__5EECBFB5_16AA_4DD9_9053_4871B229DBFB__INCLUDED_)
