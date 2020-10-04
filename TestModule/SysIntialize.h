#pragma once 

//检查是否所有设置的路径都有效或存在,不存在的自动创建
void CheckDirectory();
BOOL ClearDirectory(char chrDirName[]);

void ClearConfig();
BOOL ReadOneItem(void *Data, size_t size, size_t count, FILE *fp);
BOOL WriteOneItem(void *Data, size_t size, size_t count, FILE *fp);
BOOL ConfigureFileOfVelGridIO(BOOL bRead);
BOOL ConfigureFileIO(BOOL bRead);

BOOL InitializeConf(BOOL isReset);
int32_t MainFunc(void);
