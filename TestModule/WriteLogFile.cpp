// WriteLogFile1.cpp: implementation of the CWriteLogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <direct.h>
#include <io.h>
#include <errno.h>

#include "WriteLogFile.h"
#include "LogFile.h"
#include "MyuserMessage.h"
#include <direct.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWriteLogFile::CWriteLogFile() //δָ��·��
{
	char appPath[256]="";	
	_getcwd(appPath, sizeof(appPath));

	sprintf(m_szLogPath,"%s%s%s",appPath, SZ_SIGN_DIR,"LOG");
}

CWriteLogFile::CWriteLogFile(char* szUpperLogFolder) //ָ��·��
{
	if(szUpperLogFolder[strlen(szUpperLogFolder)-1]=='\\' || szUpperLogFolder[strlen(szUpperLogFolder)-1]=='/')
		sprintf(m_szLogPath,"%s%s",szUpperLogFolder,"LOG");
	else
		sprintf(m_szLogPath,"%s%s%s",szUpperLogFolder, SZ_SIGN_DIR,"LOG");
}

CWriteLogFile::~CWriteLogFile()
{
	m_strLogType.Empty();
}

void CWriteLogFile::WriteErrorMsg( const char* msg)
{		
	LoadMessage(msg);
	AddOneMsg("_Error.log");
}

void CWriteLogFile::WriteErrorMsg()
{
	if(strlen(m_strThisMsg)>0)
	{
		LoadMessage(m_strThisMsg);
		AddOneMsg("_Error.log");
		sprintf(m_strThisMsg,"\0");
	}	
}

void CWriteLogFile::WriteStatusMsg( const char* msg)
{	
	LoadMessage(msg);
	AddOneMsg("_Status.log");
}

void CWriteLogFile::WriteStatusMsg()
{	
	if(strlen(m_strThisMsg)>0)
	{
		LoadMessage(m_strThisMsg);
		AddOneMsg("_Status.log");
		sprintf(m_strThisMsg,"\0");
	}	
}


void CWriteLogFile::ClearLogFiles()
{
	CLogFile logFile;

	logFile.RemoveExpiredLogFile("_Error.log",15,m_szLogPath);//"LOG");
	logFile.RemoveExpiredLogFile("_Status.log",15,m_szLogPath);//"LOG");
}

//LOgĿ¼���ϼ�Ŀ¼
void CWriteLogFile::ClearLogFiles(char* szLogPath) 
{
	if(szLogPath[strlen(szLogPath)-1]=='\\' || szLogPath[strlen(szLogPath)-1]=='/')
		sprintf(m_szLogPath,"%s%s",szLogPath,"LOG");
	else
		sprintf(m_szLogPath,"%s%s%s",szLogPath, SZ_SIGN_DIR,"LOG");

	ClearLogFiles();
}


void CWriteLogFile::AddOneMsg(char *strLogType)
{
	CLogFile logFile;
	SYSTEMTIME s_time;
	char str[LEN_FULL_MSG]=" ";
	char strHor[4]= "  ";
	char strMin[4]= "  ";
	char strSec[4]= "  ";
	
	//Open or create the log file
	if(_access(m_szLogPath/*"LOG"*/, 6) == -1)
	{
		if(errno == ENOENT)
		{
			//����Ŀ¼�����ڣ� ����Ŀ¼
			if(_mkdir(m_szLogPath/*"LOG"*/)==-1)
			{
				//����Ŀ¼�����������
				if(errno==ENOENT)
				{
					return ;
				}
			}
		}
	}
	
	//LPCTSTR p=m_strLogType;
	if(!logFile.CreateLogFile(strLogType,m_szLogPath/*"LOG"*/))
		return ;
	//Get current local time
	GetLocalTime(&s_time);

	//Convert digital to string
	if(s_time.wHour>=10)
		sprintf(strHor, "%2d", s_time.wHour);
	else
		sprintf(strHor, "0%d", s_time.wHour);
	
	if(s_time.wMinute>=10)
		sprintf(strMin, "%2d", s_time.wMinute);
	else
		sprintf(strMin, "0%d", s_time.wMinute);
	
	if(s_time.wSecond>=10)
		sprintf(strSec, "%2d", s_time.wSecond);
	else
		sprintf(strSec, "0%d", s_time.wSecond);

	//Add time information ahead the message
	sprintf(str,"%2s:%2s:%2s >>> %s\n",strHor, strMin, strSec, m_strThisMsg);

	//Add a message into the log file for errors
	logFile.AddString(str);
	//Close the log file
	logFile.CloseLogFile();
}

void CWriteLogFile::LoadMessage(const char* msg) 
{
	sprintf(m_strThisMsg, "%s\0", msg);
}

void  __stdcall LOGFORFORTRAN(char *strMsg, int32_t strlen)
{
	CWriteLogFile WriteLogFile;
	WriteLogFile.WriteErrorMsg(strMsg);
}