#pragma once

#include <cstdio>

typedef struct _BASEDATAINFO
{
	char  strFilePath[FILENAME_MAX];		//�ļ�·��������
	uint64_t  lJulDate;				//������
	uint64_t  lSeconds;				//�۲⿪ʼʱ��
	uint32_t  iSiteID;				//�״�վ�ڱ����������еı��
}BaseDataInfo;
