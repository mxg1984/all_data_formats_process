#pragma once
#include "stdAfx.h"

#include <string>
#include "UniformDataFormatDef.h"

////////////////////////////////////////////////////////////////////////////////////////

extern struct LOGMSGPARAM   g_LogShowParam;
extern struct DERIVEDPRODSELECTION g_DerivedProdSelect;
extern struct DERIVEDPRODSELECTION_ADD g_AddDerivedProdSelect; //后来增加的产品
extern struct DATADIRECTORY g_DataDir;
extern struct RADARSITEINFO g_cRadarSiteInfo[TOTAL_SITES];
extern char   g_strRadarDataPath[TOTAL_SITES][PATH_LEN];	//Path of base data for each site
extern char g_strZoneName[SITE_NAME_LEN];
extern uint32_t  g_iSitesInZone;									//区域中设置的站点个数
//强度网格参数
extern int8_t g_bUserDefineLevels;							//是否由用户自定义高度,如果是,在指定文件编辑高度层 2017.10
extern uint32_t  g_iNumHeight;									//高度层数
extern uint32_t  g_iHeights[MAX_HGT_LEVELS];						//各层高度
extern struct HORIZRESPARAM g_HorizRes;						//水平分辨率
//速度网格参数
extern int8_t g_bUserDefineVelLevels;							//是否由用户自定义高度,如果是,在指定文件编辑高度层
extern uint32_t  g_iNumVelHeight;								//高度层数
extern uint32_t  g_iVelHeights[MAX_HGT_LEVELS];					//各层高度
extern struct POLARTOGRIDPARAM g_PolarToGrdParam;			//水平分辨率 和格点半径(km 或者格点数)
extern uint16_t g_iIntervalTime;						//Minutes	扫描需几分钟的时间，因此，组网有时间间隔
extern uint16_t g_iMatchingTime;						//Minutes。 考虑扫描时间不同步，允许一段匹配时间
extern uint16_t g_iDelayTime;							//Minutes. 考虑数据传输的延迟时间
extern int16_t g_iDeleteOldBaseData;							//实时模式下，是否删除已经处理过的雷达数据0: 否; 1:是
extern int16_t g_iQualityControl;                             //是否进行质量控制? 0: 否; 1:是
extern int16_t g_iSaveSingleCAPPI;							//是否进行单站格点数据保存. 0:不保存; 1:保存
extern int16_t g_iSaveNetCDFData;								//是否保存NetCDF格式的产品数据(*.nc)
extern int16_t g_iSaveQcData;									//是否保存质量控制后的基数据
extern char  g_strQcDataPath[TOTAL_SITES][PATH_LEN];		//质量控制后基数据保存路
extern int16_t g_iProductRemainDays;							//产品保留天数
extern int16_t g_iOptionsGridData;							//格点数据类型选项
extern char  g_strAppTmpFolder[PATH_LEN];					//Temporary files are stored in this folder
extern int16_t g_iDllFree;

extern struct SITEPOSINMOSAIC g_SiteRangeInMosaic[TOTAL_SITES];		//单站在组网网格中的位置
extern struct MOSAICGRIDPARAM g_MosaicCommParam;					//组网网格参数