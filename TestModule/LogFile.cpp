// LogFile.cpp: implementation of the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFile::CLogFile()
{
	m_nRetainedDays=7;

	CString csDate=_T("");
	CString csFileName= _T("");
	CString csPathName= _T("");
}

CLogFile::~CLogFile()
{

}

void CLogFile::RemoveExpiredLogFile(char* pFileType, int32_t nRetainedDays, char* pPath)
{	
	CTime ct = CTime::GetCurrentTime();
	CTimeSpan ts( nRetainedDays,0,0,0);		
	CTime timeEnd = ct-ts;

	CFileFind find;	

	TCHAR wszFileType[40] = _TEXT("");
	CharToTchar(pFileType, wszFileType);

	if( pPath==NULL )
		csPathName.Format(_T("*%s"), wszFileType);
	else
	{
		TCHAR wszPath[MAX_PATH] = _TEXT("");
		CharToTchar(pPath, wszPath);

		if(pPath[strlen(pPath)-1]!= '\\'  && pPath[strlen(pPath) - 1] != '/')  //路径参数中不包含目录符号'\\'
			csPathName.Format(_T("%s%s*%s"), wszPath, WSZ_SIGN_DIR, wszFileType);
		else//路径参数中包含目录符号'\\'
			csPathName.Format(_T("%s*%s"), wszPath, wszFileType);
	}

	BOOL bDone = find.FindFile( csPathName );
	while( bDone )
	{
		bDone = find.FindNextFile();
		{
			CTime timeCreate;
			find.GetCreationTime( timeCreate );
			csPathName = find.GetFilePath();

			if( timeCreate<timeEnd ) CFile::Remove( csPathName );
		}
	}

	find.Close(); //whyan added
}

BOOL CLogFile::CreateLogFile(char * pFileType, char * pPath)
{
	TCHAR wszFileType[40] = _TEXT("");
	CharToTchar(pFileType, wszFileType);
	csFileName.Format(_T("%s"), wszFileType);// pFileType);

	CTime ct = CTime::GetCurrentTime();
	csDate = ct.Format( _T("%Y%m%d" ));
		
	CString csPathName = _T("");
	if( pPath==NULL )
	{
		csPathName = csDate + csFileName;
		//csPathName.Format(_T("%s%s"), csDate, csFileName );
	}
	else
	{
		TCHAR wszPath[MAX_PATH] = _TEXT("");
		CharToTchar(pPath, wszPath);
		if (pPath[strlen(pPath) - 1] == '/' || pPath[strlen(pPath) - 1] == '\\')
			csPathName.Format(_T("%s"), wszPath);
		else//路径参数中不包含目录符号'\\'
			csPathName.Format(_T("%s%s"), wszPath, WSZ_SIGN_DIR);
		csPathName = csPathName + csDate + csFileName;
	}

	if(m_File.Open( csPathName, CFile::modeCreate
       | CFile::modeWrite|CFile::modeNoTruncate))
	{
	   m_File.Seek(NULL,CFile::end);
	   return true;
	}
	else
	   return false;
}

void CLogFile::CloseLogFile()
{
	m_File.Close();
}

void CLogFile::AddString(char* pStr)
{
	TCHAR wszStr[LEN_FULL_MSG] = _TEXT("");
	CharToTchar(pStr, wszStr);
	m_File.WriteString(LPCTSTR(wszStr));
}

void CLogFile::AddString(TCHAR* pStr)
{
	m_File.WriteString(LPCTSTR(pStr));
}