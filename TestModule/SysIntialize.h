#pragma once 

//����Ƿ��������õ�·������Ч�����,�����ڵ��Զ�����
void CheckDirectory();
BOOL ClearDirectory(char chrDirName[]);

void ClearConfig();
BOOL ReadOneItem(void *Data, size_t size, size_t count, FILE *fp);
BOOL WriteOneItem(void *Data, size_t size, size_t count, FILE *fp);
BOOL ConfigureFileOfVelGridIO(BOOL bRead);
BOOL ConfigureFileIO(BOOL bRead);

BOOL InitializeConf(BOOL isReset);
int32_t MainFunc(void);
