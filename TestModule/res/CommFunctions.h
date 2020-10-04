
#ifndef _COMMFUNCTIONS_H

#define _COMMFUNCTIONS_H

#include <string>
using namespace std;


void Zip2Prod(char *src);
void UnZip(char *src, char *dst);
BOOL FilePathExists(char* szFilePathName);
BOOL FilePathExists(const char* szFilePathName);
BOOL CreateDir(const char *strPath, BOOL bCreate=true);
BOOL CreateDir(TCHAR *strPath, BOOL bCreate = true);
BOOL BrowseDirectory(CString &strPath, HWND hWnd=0);
BOOL SettingFileRead();
BOOL SettingFileWrite();
BOOL FileNameWrite();
BOOL FileNameRead();

BOOL ConfigureFileIO(BOOL bRead);
int32_t MainFunc(void);//int32_t argc, const char *argv[]);
BOOL InitializeConf(BOOL isReset=FALSE);

void DeleteDirectory(char strDir[], int32_t iRemainDays=-1, uint32_t iRemainHours=0, uint32_t iRemainMinutes=0);

void TimetToSystemTime( time_t t, LPSYSTEMTIME pst );
void SystemTimeToTimet( SYSTEMTIME st, time_t *pt );
//void OnClipsend();
void DeleteClipFile();
BOOL ClearDirectory(char chrDirName[]);

DWORD GetProcessidFromName(char *name);//调度外部进程
DWORD GetProcessidFromName(TCHAR *name);

void InvalidDlgItems(CWnd* pWnd, BOOL bFlag);////遍历窗口内的所有控件,把它置为有效或者无效//2012.04.28
void InvalidDlgItem(CWnd* pWnd, int32_t ID, BOOL bFlag);
void ResetDlgFont(CWnd* pWnd, CFont* pFont);
void RefreshTaskbarIcon();  //刷新任务栏图标
int DeleteTrayNullIcon(BOOL &bNullBeDel);//清理任务栏图标

//MSG
void ReportLog_Status(const char str[]);
void ReportLog_Error(const char str[]);
void ReportLog_Status(char *str);
void ReportLog_Error(char *str);
void SendStatusMsgToWnd();
void SendErrorMsgToWnd();
void GetSystemTimeString(char *strTime);

/////
//MSG 

//TCHAR  与 char 互转
void TcharToChar(const TCHAR * tchar, char * _char);
void CharToTchar(const char * _char, TCHAR * tchar);
void TcharToString(TCHAR* input, string& output);
void StringToTchar(string  input, TCHAR* output);
//string TcharToString(TCHAR* str);

void EraseSpace(string &s); //清除头尾空字符


void showMemoryInfo(char *module = 0x0);
#include <assert.h>
void memoryExit();
// 
#include "WriteLogFile.h"

#endif