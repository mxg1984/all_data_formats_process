//
#include "stdafx.h"
#include "CommVar.h"
#include "GlobalParams.h"
#include <windows.h>

//这里定义了全局变量

// ---------- 保存到文件中的全局变量 -----------//
struct DERIVEDPRODSELECTION g_DerivedProdSelect={false, false, false, false, false};  //If derived products should be generated
struct DERIVEDPRODSELECTION_ADD g_AddDerivedProdSelect= //10 个要素
{
	false,	false,	false,	false,	false,
	false,	false,	false,	false,	false,
	false,	false,	false,	false,	false
};

struct LOGMSGPARAM   g_LogShowParam={0, 30, 5};				//日志显示参数
struct DATADIRECTORY g_DataDir={"D:\\", "D:\\", "D:\\", "D:\\", 0};
struct RADARSITEINFO g_cRadarSiteInfo[TOTAL_SITES];
char   g_strRadarDataPath[TOTAL_SITES][PATH_LEN];				//Path of base data for each site
char g_strZoneName[SITE_NAME_LEN]=" ";
uint32_t  g_iSitesInZone=0;										//区域中设置的站点个数

int8_t g_bUserDefineLevels=false;								//是否由用户自定义高度,如果是,在指定文件编辑高度层
uint32_t  g_iNumHeight=0;										//高度层数
uint32_t  g_iHeights[MAX_HGT_LEVELS];							//各层高度
struct HORIZRESPARAM g_HorizRes={0.0, 0.0};					//水平分辨率

int8_t g_bUserDefineVelLevels=false;							//是否由用户自定义高度,如果是,在指定文件编辑高度层
uint32_t  g_iNumVelHeight=0;										//高度层数
uint32_t  g_iVelHeights[MAX_HGT_LEVELS];							//各层高度
struct POLARTOGRIDPARAM g_PolarToGrdParam={0.0, 0};				//水平分辨率 和格点半径(km 或者格点数)

uint16_t g_iIntervalTime=0;							//Minutes	扫描需几分钟的时间，因此，组网有时间间隔
uint16_t g_iMatchingTime=0;							//Minutes。 考虑扫描时间不同步，允许一段匹配时间
uint16_t g_iDelayTime=0;								//Minutes. 考虑数据传输的延迟时间
int16_t g_iDeleteOldBaseData = false;                         //实时模式下，是否删除已经处理过的雷达数据0: 否; 1:是
int16_t g_iQualityControl = false;                            //是否进行质量控制? 0: 否; 1:是
int16_t g_iSaveSingleCAPPI= false;							//是否进行单站格点数据保存. 0:不保存; 1:保存
int16_t g_iSaveNetCDFData = false;							//是否保存NetCDF格式的产品数据
int16_t g_iSaveQcData = false;								//是否保存质量控制后的基数据
char  g_strQcDataPath[TOTAL_SITES][PATH_LEN];					//质量控制后基数据保存路径
int16_t g_iProductRemainDays = 7;								//产品保留天数
int16_t g_iOptionsGridData=2;									//处理单站三维格点数据类型选项，0:都不处理; 1:都处理; 2: 格点强度数据; 3:格点速度数据
int16_t g_iDllFree=false;
char  g_strAppTmpFolder[PATH_LEN]=" ";						//Temporary files are stored in this folder

struct SITEPOSINMOSAIC g_SiteRangeInMosaic[TOTAL_SITES];	//单站在组网网格中的位置
struct MOSAICGRIDPARAM g_MosaicCommParam;					//组网网格参数


