#pragma once
/*
#include "ProcessSARadarData.h"
#include "ProcessCDRadarData.h"
#include "ProcessCARadarData.h"
#include "ProcessCC1RadarData.h"
#include "ProcessCC2RadarData.h"
#include "ProcessSAFOrCdScRadarData.h"
#include "ProcessSA88DRadarData.h"
*/
typedef struct _BASEDATAINFO
{
//	char strDataType[10];		//�״���������
	char  strFilePath[MAX_PATH];		//�ļ�·��������
	long  lJulDate;				//������
	long  lSeconds;				//�۲⿪ʼʱ��
	//short year;
	//short month;
	//short hour;
	//short minute;
	int  iSiteID;				//�״�վ�ڱ����������еı��
//	int  iIndexInList;			//�ļ����
//	unsigned __int64 ulDataTime;//
}BaseDataInfo;
