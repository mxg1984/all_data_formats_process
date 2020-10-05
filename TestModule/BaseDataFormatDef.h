#pragma once

#include <cstdio>

typedef struct _BASEDATAINFO
{
	char  strFilePath[FILENAME_MAX];		//文件路径及名称
	uint64_t  lJulDate;				//儒略日
	uint64_t  lSeconds;				//观测开始时间
	uint32_t  iSiteID;				//雷达站在本区域组网中的标号
}BaseDataInfo;
