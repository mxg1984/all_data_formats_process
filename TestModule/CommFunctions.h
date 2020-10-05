
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

DWORD GetProcessidFromName(char *name);//�����ⲿ����
DWORD GetProcessidFromName(TCHAR *name);

void InvalidDlgItems(CWnd* pWnd, BOOL bFlag);////���������ڵ����пؼ�,������Ϊ��Ч������Ч//2012.04.28
void InvalidDlgItem(CWnd* pWnd, int32_t ID, BOOL bFlag);
void ResetDlgFont(CWnd* pWnd, CFont* pFont);
void RefreshTaskbarIcon();  //ˢ��������ͼ��
int DeleteTrayNullIcon(BOOL &bNullBeDel);//����������ͼ��

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

//TCHAR  �� char ��ת
void TcharToChar(const TCHAR * tchar, char * _char);
void CharToTchar(const char * _char, TCHAR * tchar);
void TcharToString(TCHAR* input, string& output);
void StringToTchar(string  input, TCHAR* output);
//string TcharToString(TCHAR* str);

void EraseSpace(string &s); //���ͷβ���ַ�


void showMemoryInfo(char *module = 0x0);
#include <assert.h>
void memoryExit();
// 
#include "WriteLogFile.h"

#endif