#include "stdafx.h"

#include <errno.h>
#include <direct.h>
#include <io.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <shellapi.h>

#include "CommVar.h"
#include "CommFunctions.h"
//#include "GlobalParams.h"

//----------------------------------------------------------------------
//��¼�ڴ�ʹ������ĺ���
#include <iostream>
#include <windows.h>
#include <psapi.h>

#pragma comment(lib,"psapi.lib")

///beginning of message

//�����־-״̬
void GetSystemTimeString(char *strTime)
{
	SYSTEMTIME s_time;
	char str[LEN_FULL_MSG]=" ";
	char strHor[4]= "  ";
	char strMin[4]= "  ";
	char strSec[4]= "  ";

	//Get current local time
	GetLocalTime(&s_time);
	//Convert digital to string
	if(s_time.wHour>=10)
		sprintf_s(strHor, "%2d", s_time.wHour);
	else
		sprintf_s(strHor, "0%d", s_time.wHour);
	
	if(s_time.wMinute>=10)
		sprintf_s(strMin, "%2d", s_time.wMinute);
	else
		sprintf_s(strMin, "0%d", s_time.wMinute);
	
	if(s_time.wSecond>=10)
		sprintf_s(strSec, "%2d", s_time.wSecond);
	else
		sprintf_s(strSec, "0%d", s_time.wSecond);

	sprintf_s(strTime,9,"%2s:%2s:%2s",strHor, strMin, strSec);
}


void ReportLog_Status(const char str[])
{
	if (strlen(str) < 2)
		return;

	CWriteLogFile Log;
	Log.WriteStatusMsg(str);//();	
}

//�����־-״̬
void ReportLog_Status(char *str)
{
	if (strlen(str) < 2)
		return;

	CWriteLogFile Log;
	Log.WriteStatusMsg(str);//();
	//if(!g_LogShowParam.bShowLog)
	//	return;
}

//�����־-����
void ReportLog_Error(const char str[])
{//
	if (strlen(str) < 2)
		return;

	CWriteLogFile Log;
	Log.WriteErrorMsg(str);//();
}
//�����־-����
void ReportLog_Error(char *str)
{	
	if (strlen(str) < 2)
		return;

	CWriteLogFile Log;
	Log.WriteErrorMsg(str);//();
}

//////////////////end of msg

//----------------------------------------------------------------------
///beginning of ZIP  UNZIP

//��ѹ��bz2�ļ���
//src:ѹ���ļ��������ļ�·��
//dst:��ʱ����
void UnZip(const char *src, char *dst)  //��ѹ������
{
	std::string str;
	std::string s1,s2;

	STARTUPINFO   si;   
	ZeroMemory(&si,   sizeof(si));   
	si.cb   =   sizeof   STARTUPINFO;   
	si.wShowWindow   =   SW_HIDE;   
	si.dwFlags   =   STARTF_USESHOWWINDOW   |   STARTF_USESTDHANDLES;   
	PROCESS_INFORMATION   pi;   

	str.assign("D:/bzip2-1.0.5-bin/bin/bunzip2.exe -k "); //����ԭʼѹ�����ݣ�ʹ�ò���-k
	s1.assign(src);
	str.append(s1);
	//str += " ";
	//str += s2;
	TCHAR wszChar[PATH_LEN] = _TEXT("");
	CharToTchar(str.c_str(),wszChar);

	bool   res   =   CreateProcess(NULL,   
		wszChar,//str.GetBuffer(0),//"C:\\Program Files\\WinRAR\\Rar.exe ****",     
		NULL,   
		NULL,   
		NULL,   
		NORMAL_PRIORITY_CLASS   |   CREATE_NO_WINDOW,   
		NULL,   
		NULL,   
		&si,   
		&pi);   
	if   (TRUE   ==   res)   
	{   
		//�ȴ�����ִ�����   
		WaitForSingleObject(pi.hProcess,   INFINITE);       
		CloseHandle(pi.hProcess);   
		CloseHandle(pi.hThread);   
	}

}

//--------
//ѹ���ļ�Ϊbz2��
//src:ѹ���ļ��������ļ�·��
//dst:��ʱ����
void Zip2Prod(char *src)  //ѹ������
{
	char str1[]="bzip2 -k "; //����ԭʼ�����ļ���ʹ�ò���-k
	char str2[300]="";
	//CString s1,s2;

	STARTUPINFO   si;   
	ZeroMemory(&si,   sizeof(si));   
	si.cb   =   sizeof   STARTUPINFO;   
	si.wShowWindow   =   SW_HIDE;   
	si.dwFlags   =   STARTF_USESHOWWINDOW   |   STARTF_USESTDHANDLES;   
	PROCESS_INFORMATION   pi;   

	//s1.Format("%s",src);
	//s2.Format("%s",dst);
	//str += s1;
	//str += " ";
	//str += s2;
	sprintf_s(str2,"%s %s",str1,src);
	TCHAR wszChar[PATH_LEN] = _TEXT("");
	CharToTchar(str2, wszChar);
	bool   res   =   CreateProcess(NULL,   
		wszChar,//str.GetBuffer(0),//"C:\\Program Files\\WinRAR\\Rar.exe ****",     
		NULL,   
		NULL,   
		NULL,   
		NORMAL_PRIORITY_CLASS   |   CREATE_NO_WINDOW,   
		NULL,   
		NULL,   
		&si,   
		&pi);   
	if   (TRUE   ==   res)   
	{   
		//�ȴ�����ִ�����   
		WaitForSingleObject(pi.hProcess,   INFINITE);       
		CloseHandle(pi.hProcess);   
		CloseHandle(pi.hThread);   
	}

} 


bool FilePathExists(const char* szFilePathName)
{
	if( (_access( szFilePathName, 0 )) != -1 )
	{	
		return true;
		//printf( "File test.txt exists\n" );	/* Check for write permission */
		//if( (_access( "test.txt", 2 )) != -1 )		
		//	printf( "File test.txt has write permission\n" );	
		//if( (_access( "test.txt", 4 )) != -1 )		
		//	printf( "File test.txt has read permission\n" );	
		//if( (_access( "test.txt", 6 )) != -1 )		
		//	printf( "File test.txt has read and write permission\n" );
	}else
	{	
		//printf("File test.txt does not exists\n");
		return false;
	}
}
//----------------------------------------------------------------------
//////////////////////////////////////////////////////////////////
//������    CreateDir(char *strPath, bool bCreate)
//
//����:		���ĳһ·���Ƿ���ڣ���������ڣ�������·��
//
//˵��:     1- �ж�Ŀ¼�Ƿ����
//          2- �жϲ����ڵ�Ŀ¼�Ƿ���Ҫ����
//			3- ������Ҫ��Ŀ¼�� ���ڳ���ʱ������־��¼
//
//����:		char *strPath -- �ļ���
//			bool bCreate  -- �Ƿ���Ҫ����Ŀ¼
//
//����:		bool, ��һ��Ӧ�ô��ڵ�Ŀ¼û���ҵ�ʱ, ������Ŀ¼����ʧ��ʱ, ����FALSE, ��������ʱ, ����TRUE.
//
//����:		Whyan
//
//����:     2007.04
//
bool CreateDir(const char *strPath, bool bCreate)
{
	//char strMsg[LEN_LOG_MSG];
	CWriteLogFile WriteLogFile;
	if(_access(strPath, 6) == -1)
	{
		if(errno == ENOENT)
		{
			//����Ŀ¼�����ڣ��ж��Ƿ���Ҫ����Ŀ¼
			if(bCreate)
			{//��Ҫ����Ŀ¼
				if(_mkdir(strPath)==-1)
				{
					//����Ŀ¼�����������
					if(errno==ENOENT)
					{
						char strMsg[LEN_MSG]="";
						sprintf_s(strMsg,"Fail creating directory(%s), path not found.",strPath);
						ReportLog_Error(strMsg);
						return false;
					}
				}
			}
			else
			{//ֻ�Ǽ��Ŀ¼�Ƿ���ڣ��������ڣ��������
				char strMsg[LEN_MSG]="";
				sprintf_s(strMsg,"Path(%s) not found.",strPath);
				ReportLog_Error(strMsg);
				return false;
			}
		}
	}

	return true;
}

bool CreateDir(TCHAR *strPath, bool bCreate)
{
	//char strMsg[LEN_LOG_MSG];
	CWriteLogFile WriteLogFile;
	if (_taccess(strPath, 6) == -1)
	{
		if (errno == ENOENT)
		{
			//����Ŀ¼�����ڣ��ж��Ƿ���Ҫ����Ŀ¼
			if (bCreate)
			{//��Ҫ����Ŀ¼
				if (_tmkdir(strPath) == -1)
				{
					//����Ŀ¼�����������
					if (errno == ENOENT)
					{
						TCHAR strMsg[LEN_MSG] = _TEXT("");
						_tprintf_s(strMsg, "Fail creating directory(%s), path not found.", strPath);

						char szMsg[LEN_MSG] = "";
						TcharToChar(strMsg, szMsg);
						ReportLog_Error(szMsg);
						return false;
					}
				}
			}
			else
			{//ֻ�Ǽ��Ŀ¼�Ƿ���ڣ��������ڣ��������
				TCHAR strMsg[LEN_MSG] = _TEXT("");
				_tprintf_s(strMsg, "Path(%s) not found", strPath);
				char szMsg[LEN_MSG] = "";
				TcharToChar(strMsg, szMsg);
				ReportLog_Error(szMsg);
				return false;
			}
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////
//����:    BOOL BrowseDirectory(char *Path)
//
//����:		����ļ���
//
//˵��:     
//
//����:		char *Path -- �ļ���
//
//����:		BOOL, �ҵ��ļ���,����TRUE; ����,����FALSE
//
//����:		Whyan
//
//����:     2006
//
#include <iostream>
#include <cstring>
//Reference : https://blog.csdn.net/qq_18297675/java/article/details/51313121
bool BrowseDirectory(CString &strPath, HWND hWnd )
{//Get a full path (without file name) 
	TCHAR szBrowsePath[PATH_LEN] = _TEXT("");

	BROWSEINFO bi = { 0 };  //��Ϊ�Ѿ���ʼ��Ϊ0.������Щ������ظ���ֵ��
	bi.hwndOwner = hWnd;
	bi.pszDisplayName = szBrowsePath;
	bi.lpszTitle = TEXT("Please select a folder:");

	LPITEMIDLIST lpiml = { 0 };
	lpiml = SHBrowseForFolder(&bi); //���û��ѡ��Ŀ¼���򷵻�NULL
	if (lpiml && SHGetPathFromIDList(lpiml, szBrowsePath))//��lpiml �л�ȡ·����Ϣ
	{
		size_t tt = _tcsclen(szBrowsePath);
		strPath.Format(_TEXT("%s"), szBrowsePath);
		return true;
	}
	else
	{
		strPath = _T("");
		return false;
	}
}

//�򱣴�������ļ������ļ���д��������ļ���
bool FileNameWrite()
{
	return TRUE;
}

//�ӱ���������ļ������ļ��ж����������ļ���
bool FileNameRead()
{	
	return TRUE;
}

//�������ļ��ж�������
bool SettingFileRead()
{
	return false;
}

//�������ļ���д�����
bool SettingFileWrite()
{
	return TRUE;
}

/*
//�������ļ��ж�������
bool SettingFileRead()
{
	FILE *fp = NULL;

	char strTemp[32]="                               ";

	fp = fopen_s(strSettingFile,"r");
	if(fp==NULL)
		return FALSE;

	fscanf(fp, "%s        %s\n", LOCALS_DATADIRS.strInputDir, strTemp);
	fscanf(fp, "%s        %s\n", LOCALS_DATADIRS.strOutputDir,strTemp);
	fscanf(fp, "%s        %s\n", LOCALS_DATADIRS.strSoundDir,strTemp);

	fscanf(fp, "%6d,%6d               %s\n",&LOCAL_PARAMS.cCentX, &LOCAL_PARAMS.cCentY,strTemp);
	fscanf(fp, "%6d,%6d,%6d        %s\n", &LOCAL_PARAMS.iResX, &LOCAL_PARAMS.iResY, &LOCAL_PARAMS.iResZ,strTemp);
	fscanf(fp, "%6d                      %s\n", &(LOCAL_PARAMS.cLevels),strTemp);
	fscanf(fp, "%6d                      %s\n", &LOCAL_PARAMS.iMode,strTemp);

	fclose(fp);

	return TRUE;
}

//�������ļ���д�����
bool SettingFileWrite()
{
	FILE *fp = NULL;

	fp = fopen_s(strSettingFile,"w");
	if(fp==NULL)
		return FALSE;

	fprintf(fp, "%s        %s\n", LOCALS_DATADIRS.strInputDir,"!input_dir");
	fprintf(fp, "%s        %s\n", LOCALS_DATADIRS.strOutputDir,"!output_dir");
	fprintf(fp, "%s        %s\n", LOCALS_DATADIRS.strSoundDir,"!sounding_data_directory");

	fprintf(fp, "%6d,%6d               %s\n",LOCAL_PARAMS.cCentX, LOCAL_PARAMS.cCentY,"!x0,y0");
	fprintf(fp, "%6d,%6d,%6d        %s\n", LOCAL_PARAMS.iResX, LOCAL_PARAMS.iResY, LOCAL_PARAMS.iResZ, "!dx,dy,dz");
	fprintf(fp, "%6d                      %s\n", LOCAL_PARAMS.cLevels,"!iteration_nmb");
	fprintf(fp, "%6d                      %s\n", LOCAL_PARAMS.iMode,"!moist");

	fclose(fp);

	return TRUE;
}
*/

//////////////////////////////////////////////////////////////////
//����:		void DeleteDirectory(char strDir[], int32_t iRemainDays)
//
//����:		�����ļ���
//
//˵��:     1- �ж�Ŀ¼�µ��ļ��Ƿ���Ҫ����,����������Ϊ-1ʱ,����Ҫ����
//          2- ���Ŀ¼�Ƿ�Ϸ�
//          3- ����Ŀ¼
//          4- �ж�Ŀ¼�µ��ļ��Ƿ񳬳���������,��ɾ���������޵��ļ�
//
//����:		char strDir[] -- �ļ���
//          int32_t iRemainDays -- ��������(��), -1, ��ʾ������
//			int32_t iRemainMinutes--����������(����), -1 ��ʾ������
//
//����:		��
//
//����:		Whyan- download source code from internet and modified it for local use.
//
//����:     2007.07.03
//

//////////////////////////////////////////////////////////////////
//����:		void DeleteDirectory(char strDir[], int32_t iRemainDays)
//
//����:		�����ļ���
//
//˵��:     1- �ж�Ŀ¼�µ��ļ��Ƿ���Ҫ����,����������Ϊ-1ʱ,����Ҫ����
//          2- ���Ŀ¼�Ƿ�Ϸ�
//          3- ����Ŀ¼
//          4- �ж�Ŀ¼�µ��ļ��Ƿ񳬳���������,��ɾ���������޵��ļ�
//
//����:		char strDir[] -- �ļ���
//          int32_t iRemainDays -- ��������(��), -1, ��ʾ������
//			int	iRemainHours-- ��������(Сʱ) >=0; iRemainMinutes-����,>=0
//
//����:		��
//
//����:		Whyan- download source code from internet and modified it for local use.
//
//����:     2007.07.03
//
void DeleteDirectory(char strDir[], int32_t iRemainDays, uint32_t iRemainHours,uint32_t iRemainMinutes)
{
	//�����������==1�� �򲻶Բ�Ʒ��������
	if(iRemainDays == -1) return;

	//���·���Ƿ���ȷ
	if(strlen(strDir)==0)
		return;
	
	// ����ɾ���ļ������ļ���
	CFileFind ff;
	char FileFilter[PATH_LEN];
	TCHAR wFileFilter[PATH_LEN] = TEXT("");

	if(strDir[strlen(strDir)-1]!='\\')
		sprintf_s(FileFilter, "%s\\%s", strDir, "*.*");
	else
		sprintf_s(FileFilter, "%s%s", strDir, "*.*");
	CharToTchar(FileFilter, wFileFilter);

	bool bFound = ff.FindFile(wFileFilter, 0);
	while(bFound)
	{
		bFound = ff.FindNextFile();
		if(ff.GetFileName()=="."||ff.GetFileName()=="..")
			continue;

		// ȥ���ļ�(��)ֻ��������
		SetFileAttributes(ff.GetFilePath(), FILE_ATTRIBUTE_NORMAL);
		if(ff.IsDirectory())
		{
			char subDir[PATH_LEN]="";
			char szChar[PATH_LEN] = "";
			TcharToChar(ff.GetFilePath().GetBuffer(), szChar);
			ff.GetFilePath().ReleaseBuffer();
			sprintf_s(subDir,strlen(szChar)+1, "%s", szChar);
			// �ݹ�, �������ļ���
			DeleteDirectory(subDir, iRemainDays);
		}
		else
		{
			
			CTime ct = CTime::GetCurrentTime();
			CTimeSpan ts( iRemainDays,iRemainHours,iRemainMinutes,0);		
			CTime timeEnd = ct-ts;

			CTime timeCreate;
			ff.GetLastWriteTime( timeCreate );
			//ɾ�������������޵��ļ�
			if( timeCreate<timeEnd ) 
				CFile::Remove( ff.GetFilePath() );			
		}
	}
	ff.Close();
	
	// Ȼ��ɾ�����ļ���
	//RemoveDirectory(strDir);//����ɾ���ļ���
}

void TimetToSystemTime( time_t t, LPSYSTEMTIME pst )
{
   FILETIME ft; 
   LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
   ft.dwLowDateTime = (DWORD) ll;
   ft.dwHighDateTime = (DWORD)(ll >> 32);

   FileTimeToSystemTime( &ft, pst );
}

void SystemTimeToTimet( SYSTEMTIME st, time_t *pt )
{
   FILETIME ft;
   SystemTimeToFileTime( &st, &ft );

   LONGLONG ll;

   ULARGE_INTEGER ui;
   ui.LowPart = ft.dwLowDateTime;
   ui.HighPart = ft.dwHighDateTime;

   ll = (ft.dwHighDateTime << 32) + ft.dwLowDateTime;

   *pt = (DWORD)((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}


void DeleteClipFile()
{
	return;
}

// ���ý��̵�����
DWORD GetProcessidFromName(char* name)
{
	PROCESSENTRY32 pe;
	DWORD id=0;
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize=sizeof(PROCESSENTRY32);
	if(!Process32First(hSnapshot,&pe))
		return 0;
	while(1)
	{
		pe.dwSize=sizeof(PROCESSENTRY32);
		if(Process32Next(hSnapshot,&pe)==FALSE)
			break;

		TCHAR wszNam[100] = _TEXT("");
		CharToTchar(name, wszNam);
		if (_tcscmp(pe.szExeFile, wszNam) == 0)
		{
			id=pe.th32ProcessID;
			
			break;
		}
		
	}
	CloseHandle(hSnapshot);
	return id;
}


// ���ý��̵�����
DWORD GetProcessidFromName(TCHAR *name)
{
	PROCESSENTRY32 pe;
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
		return 0;
	while (1)
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE)
			break;

		if (_tcscmp(pe.szExeFile, name) == 0)
		{
			id = pe.th32ProcessID;

			break;
		}

	}
	CloseHandle(hSnapshot);
	return id;
}

//���������ڵ����пؼ�,������Ϊ��Ч������Ч//2012.04.28
void InvalidDlgItems(CWnd* pWnd, bool bFlag)
{
	//return;
	CWnd* pThisWnd = pWnd->GetWindow(GW_CHILD);   
    TCHAR  szClassName[PATH_LEN]=_TEXT(" ");
	while(pThisWnd) 
	{   
		GetClassName(pThisWnd->GetSafeHwnd(), szClassName,PATH_LEN);//�õ��ؼ�����������Ҫ��Edit��Button��Static�ȵ�

		if(bFlag)
			pThisWnd->EnableWindow(bFlag);
		else
		{		
			//int32_t ii=strcmp(cClassName,"Edit");
			if( lstrcmpi(szClassName,_T("EDIT"))==0)  //e	
				((CEdit*)pThisWnd)->EnableWindow(FALSE); //SetReadOnly(TRUE);
			else if( lstrcmpi(szClassName,_T("BUTTON"))==0)
				((CButton*)pThisWnd)->EnableWindow(FALSE);
			else //if(lstrcmpi(cClassName,_T("STATIC"))!=0)
				pThisWnd->EnableWindow(TRUE);
		}
		
		pThisWnd = pThisWnd->GetWindow(GW_HWNDNEXT);
	}
	pWnd->UpdateData(FALSE);
}

// ��һ��ָ���ؼ���Ϊ��Ч������Ч//2012.04.28
void InvalidDlgItem(CWnd* pWnd, int32_t ID, bool bFlag)
{
	EnableWindow(GetDlgItem(pWnd->m_hWnd,ID),bFlag);	
	pWnd->UpdateData(FALSE);
}


void ResetDlgFont(CWnd* pWnd, CFont* pFont)
{
	CWnd* pThisWnd = pWnd->GetWindow(GW_CHILD);   
    char cClassName[255]=" ";
	while(pThisWnd) 
	{   
		//GetClassName(pThisWnd->GetSafeHwnd(),cClassName,255);//�õ��ؼ�����������Ҫ��Edit��Button��Static�ȵ�
		pThisWnd->SetFont(pFont);//
	};

	pWnd->UpdateData(FALSE);
}

void TcharToChar(const TCHAR * tchar, char * _char)
{
	int32_t iLength;
	//��ȡ�ֽڳ���   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

void CharToTchar(const char * _char, TCHAR * tchar)
{
	int32_t iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}

//reference: https://blog.csdn.net/CaptainHailong/java/article/details/88917732
void TcharToString(TCHAR* input, std::string& output)
{
	int32_t length = WideCharToMultiByte(CP_ACP, 0, input, -1, NULL, 0, NULL, NULL);
	char ch[] = "";
	WideCharToMultiByte(CP_ACP, 0, input, -1, ch, length, NULL, NULL);
	output = std::string(ch);
}

//reference :https://blog.csdn.net/itas109/java/article/details/52193513
void StringToTchar(std::string  input, TCHAR* output)
{
#ifdef UNICODE
	CharToTchar(input.c_str(),output);
	//_stprintf_s(output, MAX_PATH, _T("%S"), input.c_str());//%S���ַ�
#else
	_stprintf_s(output, MAX_PATH, _T("%s"), input.c_str());//%s���ַ�
#endif
}

//���ͷβ�ո�
void EraseSpace(std::string &s)
{
	if (s.empty())
		return;
	//ch�ɻ��������ַ�
	const char ch = ' ';
	s.erase(s.find_last_not_of(" ") + 1);
	s.erase(0, s.find_first_not_of(" "));
}

#include "tlhelp32.h"

//ˢ��������ͼ��
//Reference: https://blog.csdn.net/dpsying/article/details/20139651
void RefreshTaskbarIcon()
{
	//����������
	HWND hShellTrayWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
	//�������ұ�����ͼ��+ʱ����
	HWND hTrayNotifyWnd = ::FindWindowEx(hShellTrayWnd, 0, _T("TrayNotifyWnd"), NULL);
	//��ͬϵͳ�����п���û�����
	HWND hSysPager = ::FindWindowEx(hTrayNotifyWnd, 0, _T("SysPager"), NULL);
	//����ͼ�괰��
	HWND hToolbarWindow32;
	if (hSysPager)
	{
		hToolbarWindow32 = ::FindWindowEx(hSysPager, 0, _T("ToolbarWindow32"), NULL);
	}
	else
	{
		hToolbarWindow32 = ::FindWindowEx(hTrayNotifyWnd, 0, _T("ToolbarWindow32"), NULL);
	}
	if (hToolbarWindow32)
	{
		RECT r;
		::GetWindowRect(hToolbarWindow32, &r);
		int width = r.right - r.left;
		int height = r.bottom - r.top;
		//���������м������ MOUSEMOVEһ�飬����ͼ��״̬�ᱻ����
		for (int x = 1; x<width; x++)
		{
			::SendMessage(hToolbarWindow32, WM_MOUSEMOVE, 0, MAKELPARAM(x, height / 2));
		}
	}
}

//������Ч����ͼ�� 
//Reference http://www.cppblog.com/woaidongmao/archive/2011/08/27/154486.html
void SearchAndClearIdleTrayIcon()
{
	bool bNullBeDel = FALSE;
	do
	{
		DeleteTrayNullIcon(bNullBeDel);
	} while (bNullBeDel);
}

int DeleteTrayNullIcon(bool &bNullBeDel)
{
	HWND  hNotify1;
	int iMethod = 0;
	bNullBeDel = FALSE;
	HWND  hStatus = ::FindWindow(_T("Shell_TrayWnd"), NULL);  //�õ����������
	if (hStatus == NULL)
	{
		//AfxMessageBox ( "Get Shell_TrayWnd error!" );
		return -1;
	}
	HWND  hNotify = FindWindowEx(hStatus, NULL, _T("TrayNotifyWnd"), NULL); //���½�����
	if (hNotify == NULL)
	{
		//AfxMessageBox ( "Get TrayNotifyWnd error!" );
		return -1;
	}
goto1:
	if (iMethod<1)
	{
		hNotify1 = FindWindowEx(hNotify, NULL, _T("SysPager"), NULL);
		if (hNotify == NULL)
		{
			//AfxMessageBox ( "Get SysPager error!" );
			return -1;
		}
		if (hNotify1 == NULL)
		{
			hNotify1 = hNotify;
		}
	}
	else
	{
		hNotify1 = hNotify;
	}

	HWND  hNotify1_0 = FindWindowEx(hNotify1, NULL, _T("ToolBarWindow32"), NULL);//���½�����(������ʱ��)
	if (hNotify1_0 == NULL)
	{
		//AfxMessageBox ( "Get ToolBarWindow32 error!" );
		return -1;
	}
	//-------------------�����ǵõ����������½�һ��ط��ľ��
	DWORD  pid = 0;
	GetWindowThreadProcessId(hNotify1_0, &pid);
	if (pid == NULL)
	{
		//AfxMessageBox ( "Get pid error!" );
		return -1;
	}

	HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_ALL_ACCESS, true, pid);
	if (hProcess == NULL)
	{
		//AfxMessageBox ( "Get hd error!" );
		return -1;
	}
	::SendMessage(hNotify1_0, WM_PAINT, NULL, NULL);
	CRect rect;
	::GetWindowRect(hNotify1_0, &rect);
	::InvalidateRect(hNotify1_0, &rect, FALSE);

	int  iNum = ::SendMessage(hNotify1_0, TB_BUTTONCOUNT, NULL, NULL);  //��ȡ��������ͼ�����
	if (iNum<1 && iMethod == 0)
	{
		iMethod = 2;
		goto goto1;
	}

	unsigned long n = 0;
	TBBUTTON  *pButton = new TBBUTTON;
	CString  strInfo = _T("");
	wchar_t  name[256] = { 0 };
	TBBUTTON  BButton;
	unsigned   long    whd, proid;
	CString x;

	for (int i = 0; i<iNum; i++)
	{
		::SendMessage(hNotify1_0, TB_GETBUTTON, i, (LPARAM)(&BButton));
		ReadProcessMemory(hProcess, &BButton, pButton, sizeof(TBBUTTON), &n);
		if (pButton->iString != 0xffffffff)
		{
			try
			{
				ReadProcessMemory(hProcess, (void *)pButton->iString, name, 255, &n);
			}
			catch (...)
			{
			}
			//strInfo.Format(_T("%d : %s "), i + 1, CString(name));
			//TRACE(strInfo);
		}

		try
		{
			whd = 0;
			ReadProcessMemory(hProcess, (void *)pButton->dwData, &whd, 4, &n);
		}
		catch (...)
		{
		}
		proid = NULL;
		GetWindowThreadProcessId((HWND)whd, &proid);
		if (proid == NULL)
		{
			bNullBeDel = TRUE;
			::SendMessage(hNotify1_0, TB_DELETEBUTTON, i, 0);
		}
	}
	delete pButton;

	return 0;
}
