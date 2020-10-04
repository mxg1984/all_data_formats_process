// WriteLogFile1.h: interface for the CWriteLogFile class.
//
#include "LogFile.h"
#include "ErrorStatusMsg.h"
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WRITELOGFILE1_H__0753DBE2_29C8_4552_9AA3_3C18C751EAB1__INCLUDED_)
#define AFX_WRITELOGFILE1_H__0753DBE2_29C8_4552_9AA3_3C18C751EAB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

int32_t  const LEN_MSG=800; //MSG内容的长度
int32_t  const LEN_FULL_MSG=LEN_MSG+100; //一行写入LOG的MSG的长度

class CWriteLogFile
{
public:
	//Construct
	CWriteLogFile();
	CWriteLogFile(char* szUpperLogFolder); //指定路径
	//Deconstruct
	virtual ~CWriteLogFile();
	
	void ClearLogFiles();
	void ClearLogFiles(char* szLogPath);

	//两种方式记录消息
	void WriteStatusMsg( const char* msg);	//比较简单的消息调带参数的WriteStatusMsg函数直接给出
	void WriteErrorMsg(const char* msg);	//比较简单的消息直接给出	
	void WriteStatusMsg();//比较复杂的消息,则先格式化输出到成员变量中，再调不带参数的WriteStatusMsg()	
	void WriteErrorMsg();

	char m_strThisMsg[LEN_FULL_MSG];

private:
	//char m_strMsg[];
	CString m_strLogType;
	void AddOneMsg(char *strLogType);
	void LoadMessage( const char*  msg);	
	//CString m_str1Msg;	
	char m_szLogPath[MAX_PATH];
	//void SendLogMsgToMainWnd(char* strLogType, const char *msg);	
};

extern "C" {void __stdcall LOGFORFORTRAN(char *strMsg,int32_t strlen);}


#endif // !defined(AFX_WRITELOGFILE1_H__0753DBE2_29C8_4552_9AA3_3C18C751EAB1__INCLUDED_)
