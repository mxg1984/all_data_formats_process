#ifndef __MosaicDataFormateDef_H
#define __MosaicDataFormateDef_H


//
#include "CommVar.h"
#pragma pack(1)

//三维雷达拼图数据头
struct MOSAICDATAHEADER			// 112 Bytes (108字节 ?)
{
	uint16_t iVersion;	//格式版本号 1，2，- 2字节
	uint16_t iTemp[7];	//保留 -14字节
	char           strZoneName[SITE_NAME_LEN];//区域名称 - 20字节
	char           strDataType[DATA_TYPE_LEN];//数据名  -4 Bytes
	uint16_t iNumRadars;	//区域内雷达数 - 2字节
	uint16_t iStYear;		//观测开始时间-年 2字节
	uint16_t iStMonth;	//观测开始时间-月 2字节
	uint16_t iStDay;		//观测开始时间-日 2字节
	uint16_t iStHour;		//观测开始时间-时 2字节
	uint16_t iStMinute;	//观测开始时间-分 2字节
	int32_t		   iFstLon;		//网格开始经度 (度*1000) - 4字节
	int32_t		   iFstLat;		//网格开始纬度 (度*1000) - 4字节
	int32_t	       iLstLon;		//网格结束经度 (度*1000) - 4字节
	int32_t           iLstLat;		//网格结束纬度 (度*1000) - 4字节
	uint16_t iFstHgt;		//网格开始高度 (米) - 2字节
	uint16_t iLstHgt;		//网格结束高度 (米) - 2字节
	uint16_t iLonGrids;	//经向格点数 - 2字节
	uint16_t iLatGrids;	//纬向格点数 - 2字节
	uint16_t iLevels;		//高度层数   - 2字节
	short		   temp1;		//在iLevels后面凑齐4字节整数倍结尾  !!!!!!
	uint32_t   iLonRes;		//经向格点分辨率 (度*10000) - 4字节
	uint32_t   iLatRes;		//纬向格点分辨率 (度*10000) - 4字节
	short		   iHgtRes;		//高度分辨率(米; -1表示高度间距不均匀) - 2字节
	int16_t          temp2;		//在iHgtRes后面凑齐4字节整数倍结尾!!!!!!!
	//???/这个位置在写的时候似乎多出了4个字节 (FFFF0000)
	uint32_t   dwRadInfoOffset;	//雷达站信息数据开始位置(相对于头末尾)  - 4字节
	uint32_t   dwLevHgtOffset;  //各层高度数据开始位置(相对于头末尾)    - 4字节
	uint32_t   dwObvDataOffset;	//数据(反射率\速度等)开始位置(相对于头末尾) - 4字节
	int16_t          temp[4];		//保留 -8字节
};
//const int32_t SIZE_MOSAIC_HEADER= sizeof(MOSAICDATAHEADER);
//SIZE_RADAR_INFO

#endif