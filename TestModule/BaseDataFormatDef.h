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
//	char strDataType[10];		//雷达数据类型
	char  strFilePath[MAX_PATH];		//文件路径及名称
	long  lJulDate;				//儒略日
	long  lSeconds;				//观测开始时间
	//short year;
	//short month;
	//short hour;
	//short minute;
	int  iSiteID;				//雷达站在本区域组网中的标号
//	int  iIndexInList;			//文件序号
//	unsigned __int64 ulDataTime;//
}BaseDataInfo;
