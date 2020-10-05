//CommVar.h
#ifndef _COMMVAR
#define _COMMVAR
//
#include "MyUserMessage.h"
#include "VersionControlParam.h"
#include <vector>
#include <tchar.h>
using namespace std;

//#pragma pack(1)
/////////////////////////////////////////////////////////////////////////
#ifdef _WIN32   //windows OS
const char SZ_SLASH[] = "\\";
const TCHAR WSZ_SLASH[] = _TEXT("\\");
const char CH_SLASH = '\\';
const TCHAR WCH_SLASH = '\\';
#else 
const char SZ_SLASH[] = "/";
const TCHAR WSZ_SLASH[] = _TEXT("/");
const char CH_SLASH = '/';
const TCHAR WCH_SLASH = '/';
#endif
//
//备选雷达与可选中雷达数定义
//
const int32_t TOTAL_SITES = 300;					//可供选择的雷达列表数量
//
//#ifdef TRIAL_VERSION
const int32_t MAX_SITES_TRIAL = 20;					//区域内最多雷达站数-非正式用户
//#else
const int32_t MAX_SITES_FORMAL = 100;					//区域内最多雷达站数-正式用户
//#endif
//
const int16_t PATH_LEN = 256;				//完整数据路径长度

int32_t const NAME_LEN = 24;
//
const int16_t  STAT_FISRT_LEVEL =  0; //第一层
const int16_t  STAT_MID_LEVEL   =  1; //中间层
const int16_t  STAT_LAST_LEVEL  =  2; //最后一层
//
const int32_t    C_iFirstMosaicParamIdx = 0;
const int32_t    C_iLastMosaicParamIdx = 5;
//
const char INVALID_STRING[] = "----";	//Code for invalid string
//

//
//
//#define VALUE_INVALID_BYTE  0    //无效数值, 用于 CHAR 型数据
//#define VALUE_FOLD_BYTE		1  //表示距离模糊 用于 CHAR 型数据
const unsigned char CODE_INVALID = 0;		//No data
const unsigned char CODE_RANFOLD = 1;		//range fold
const unsigned char CODE_NO_OBSERV = 2;	//no observation
const unsigned char CODE_CSRED = 3;      //Censored data

//#define CODE_MISSING	255		//Missing data
const char  CONF_FOLDER[] = "Conf";
const char TEMP_FOLDER[] = "TempData";
const char  LOGS_FOLDER[] = "LOG";
const char  NOWCST_TMP_FOLDER[] = "NOWCASTDATA";	//临时保存前一个时次单层组网数据的文件所在的目录
const char  SGL_MSC_FOLDER[] = "SMOSAICDATA";	//笛卡儿坐标单站雷达数据临时保存的位置
const char  SGL_MSCV_FOLDER[] = "SMOSAICDATA_V";//笛卡儿坐标单站雷达数据临时保存的位置
const char  UNIFORM_POLAR_FOLDER[] = "UNIFORMBD";//统一格式的雷达坐标基础数据
const char  MOSAIC_EXT_FILE[] = ".dat";		//	".MOS"		//笛卡儿坐标单站雷达数据临时保存的文件名
const char  MOSAIC_SUBFOLDER[] = "mref";		//"MosaicData";	//组网数据保存的最底层目录
const char  TRACK_SUBFOLDER[] = "fore";		//临近预报数据-回波预报产品目录
const char  WIND_SUBFOLDER[] = "wind";		//临近预报-回波移向、移速产品
//
const char NETCDF_EXT_FILE[] = ".nc";			//file ext. name for NetCDF Format product data
//
const char QC_SUBFOLDER[] = "QCBaseData";	//Folder for QC processed data to be saved in
const char BD_SUBFOLDER[] = "BaseData";//	//Folder for radar base data
const char CR_SUBFOLDER[] = "CR";		//Folder for CR product
const char ET_SUBFOLDER[] = "ET";		//Folder for ET product
const char VIL_SUBFOLDER[] = "VIL";		//Folder for VIL product
const char QPR_SUBFOLDER[] = "QPR";		//降水率产品目录
const char QPE_SUBFOLDER[] = "QPE";		//降水累积产品目录
const char CT_SUBFOLDER[] = "CT";		//云类型识别产品目录
//
const char SETTINGFILEOLD[] = "Config.dat";			//旧版本的参数文件
const char SETTINGFILE[] = "Config2.dat";			//参数文件
const char SETTINGFILE_BCK[] = "Config2_back.dat";	//备份的参数文件
const char SETTINGVELGRIDFILE[] = "Config_velgrid.dat";	//参数文
//
const int16_t MSEC_IN_SEC = 1000;
const int32_t SEC_IN_DAY = 86400;
const int32_t SEC_IN_HOR = 3600;
const int16_t SEC_IN_MIN = 60;
const int16_t MIN_IN_DAY = 1440;
const int16_t MIN_IN_HOR = 60;
const int16_t HOR_IN_DAY = 24;
const double  KM_TO_DEG = 0.01;   //中纬度,1公里大约相当于0.01度
//
const int16_t OLD_RAD_TYPS = 7;
const int16_t RAD_TYPS = 16;
const float kg_to_g = 1000.;
const float km_to_m = 1000.;
const int16_t  hw_to_byte = 2;	// Half word to byte
const int16_t  int_to_byte = 4;
const int16_t  double_to_byte = 8;	// Double type to byte
//
enum GRIDDATA_OPTION { GRIDDATA_OPTION_NONE=0, GRIDDATA_OPTION_ALL=1, GRIDDATA_OPTION_REF=2, GRIDDATA_OPTION_VEL=3 };

const int16_t MAX_HGT_LEVELS = 220;			//组网垂直方向的最多层数
const int16_t MAX_HGT_LEVELS_VEL = 100;		//组网垂直方向的最多层数--速度格点化

//--------------------------------------------------------------
const int16_t SITE_NAME_LEN = 20;		//站点名称长度(字符)
const int16_t SITE_TYPE_LEN = 12;		//雷达数据格式类型名长度(字符)
const int16_t DATA_TYPE_LEN = 4;		//雷达数据类型字符串长度(如REF，VEL..)
const uint16_t SCAN_TYPE_LEN = 6;
const uint16_t SITE_CODE_LEN = 6;

//Radar Site Info. structure
struct RADARSITEINFO				// 52 Bytes
{
	int32_t	SiteID;					//Site index
	char	SiteName[SITE_NAME_LEN];//Site Name, the number of characters is not more than 20
	char	SiteType[SITE_TYPE_LEN];//Site type, whitch indicates data format type <=12 chars
	int32_t  	SiteLon;			//Longtitude of the site(Degree*1000)
	int32_t  	SiteLat;			//Latitude of the site(Degree*1000)
	int32_t 	SiteHgt;			//Height(m) above sea level of the antenner
	int32_t     ObsRange;           //Observation range(km)//deg
//	char    DataPathName[PATH_LEN];	//Base data path name
//	int16_t   DelayedMin;			//网络传输造成的基数据延时（分）
};
const int32_t SIZE_RADAR_INFO=sizeof(RADARSITEINFO);

//---------------------------------------------------------------------
struct _tagDataDirs
{
	char strInputDir[PATH_LEN];	//输入数据路径
	char strOutputDir[PATH_LEN];//输出数据路径
	char strSoundDir[PATH_LEN];	//探空数据路径
};

struct _tagParams
{
	short	cCentX;		//反演中心位置到雷达的距离（X-km）
	short	cCentY;		//反演中心位置到雷达的距离（Y-km）
	int32_t	iResX;		//X方向的分辨率（米）
	int32_t	iResY;		//Y方向的分辨率（米）
	int32_t	iResZ;		//Z方向的分辨率（米）
	short	cCircles;	//迭代次数
	int32_t	iMode;		//0：干模式， 1：湿模式
};

struct _tagRunParams
{
	int16_t iRunCode;				//控制程序是否运行
	char  strThisFile[NAME_LEN];//前一个体扫的基数据文件名
	char  strLastFile[NAME_LEN];//当前体扫基数据文件名
};


//雷达站类型
const char OldRadarType[OLD_RAD_TYPS][SITE_TYPE_LEN]=
{
	"SA/SB",			//Index:0
	"CA/CB",			//Index:1
	"CD/SC",			//Index:2
	"CC1.0",			//Index:3
	"CC2.0",				//Index:4
	"CD/SC2",  //5
	"88DSA"    //6
};

const char RadarType[RAD_TYPS][SITE_TYPE_LEN]=
{
	"SA",			//Index:0
	"SB",			//Index:1
	"CA",			//Index:2
	"CB",			//Index:3
	"CD",			//Index:4
	"SC",			//Index:5
	"CD2",			//Index:6
	"SC2",			//Index:7
	"CC1.0",		//Index:8
	"CC2.0",		//Index:9
	"CCJ",			//Index:10
	"88DSA",		//Index:11
	"SA/SB",		//Index:12 //从这个开始的类型兼容老版本的站号
	"CA/CB",		//Index:13
	"CD/SC",		//Index:14
	"CD/SC2"		//Index:15
};
//



struct COMMONTIME
{
	int16_t year;
	char month;
	char day;
	char hor;
	char min;
	char sec;
};

struct STARTDATEANDTIME
{
	int32_t JulDate;
	int32_t UtcMin;
};

//以下定义数据路径,输入和输出
struct DATADIRECTORY
{	
	char strTemDataDir[PATH_LEN];	//临时数据文件位置
	char strRainGageDir[PATH_LEN];	//雨量计数据位置
	char strProdDataDir[PATH_LEN];	//产品存储位置
	char strRadarDataDir[PATH_LEN];	//雷达数据总路径 //当雷达数据都存储在同一个目录下,根据站号区分
	int16_t iRadarSubflderType; //雷达数据分站分别存储在不同子目录的类型选择 1分站号 2分站名 3不分子目录  4自定义子目录
};

//组网网格参数
struct HORIZRESPARAM
{
	double fLonRes;//度
	double fLatRes;
};

//极坐标-->格点化参数
struct POLARTOGRIDPARAM
{
	double fRes;//度
	int32_t iRng; //半径 千米
};

//在组网过程中要用到的单站相对于组网网格的位置
struct SITEPOSINMOSAIC
{
	int32_t iMinLonGrid;		//单站数据在组网网格中的最小经向格点位置
	int32_t iMaxLonGrid;		//单站数据在组网网格中的最大经向格点位置
	int32_t iMinLatGrid;		//单站数据在组网网格中的最小纬向格点位置
	int32_t iMaxLatGrid;		//单站数据在组网网格中的最大纬向格点位置
};

//记录站点在一个周期内的处理进度
struct SITERUNNINGINFO
{
	int8_t bStart;    //
	int8_t bFinish;
	int32_t siteCode;
	string strBaseDataFile;
	string strUniformPolarRefFile;
	string strUniformPolarVelFile;
	string strGridRefDataFile;
	string strGridVelDataFile;
	//char szBaseDataFile[PATH_LEN];
	//char szUniformPolarRefFile[PATH_LEN];
	//char szUniformPolarVelFile[PATH_LEN];
	//char szGridRefDataFile[PATH_LEN];
	//char szGridVelDataFile[PATH_LEN];
};

//组网线程需要的参数
struct MOSAICPTHREADCOMM
{
	int32_t iRunCounter;
	int32_t iDataIndex;		//表示本时次第几个站已经处理完毕
	int32_t iNumData;			//表示本时次一共有几个站可用
	int8_t bMosaicTimeReached;//组网时间已经到达，并经过了验证
	COMMONTIME commTime;	//本时次的时间
	vector<SITERUNNINGINFO> sitesRunningInfo; //一个周期中站点书记处理情况
	//char **strFileNames;	//保存本时次全部基数据文件
	vector<string> strFileNames;
	string strMosGridDataFile;
	//char szMosGridDataFile[PATH_LEN];
};

//Product Generation Selection Param
struct DERIVEDPRODSELECTION
{
	int8_t GenForeRefProd;	//If it is true, means that forecasting echo  should be generated.
	int8_t GenEchoMovProd;
//	int8_t GenNoMosaicRefProd;
	int8_t GenCrProd;
	int8_t GenEtProd;
	int8_t GenVilProd;
};

//Product Generation Selection Param
//剩余不用的供程序扩展应用。如果增加产品，可以用剩余的，但结构中元素的
//个数不能变，否则会导致前后版本的配置文件不兼容，15个元素
struct DERIVEDPRODSELECTION_ADD
{
	int8_t GenQPEPRod;	//降水产品（包括降水强度和降水累积）
	int8_t GenCtProd;     //云类型识别产品
	int8_t Spare2;		
	int8_t Spare3;
	int8_t Spare4;
	int8_t Spare5;
	int8_t Spare6;
	int8_t Spare7;
	int8_t Spare8;
	int8_t Spare9;
	int8_t Spare10;
	int8_t Spare11;
	int8_t Spare12;
	int8_t Spare13;
	int8_t Spare14;
};

//#以下定义拼图格点的参数 垂直层,分辨率,用来设置参数的
struct MOSAICGRIDPARAM_SETTING
{
	HORIZRESPARAM horizentalRes;		//水平分辨率
	int16_t bUserDefineLevels;			//用户自定义高度层
	int16_t numLevels;					//高度层数
	uint16_t Heights[MAX_HGT_LEVELS]; //各层高度，meters
};

//组网网格参数的定义
struct MOSAICGRIDPARAM
{
	int32_t iStartLon;		//区域起始经度Degree*1000
	int32_t iStartLat;		//区域起始纬度Degree*1000
	int32_t iStartHgt;		//开始高度 (米)
	uint16_t iLonNumGrids;	//经向格点数
	uint16_t iLatNumGrids;	//纬向格点数
	uint16_t iHgtNumLevels;	//垂直格点数
	uint32_t iLonRes;			//经向分辨率Degree*10000
	uint32_t iLatRes;			//纬向分辨率Degree*10000
	uint16_t iHeights[MAX_HGT_LEVELS];	//垂直方向的高度 meter
};


//日志消息显示参数
struct LOGMSGPARAM
{
	int8_t bShowLog;
	uint16_t iMaxStatusItem; //最多显示状态的条数,更多的会记录在LOG文件
	uint16_t iMaxErrorItem;  //最多显示错误的条数,更多的会记录在LOG文件
};


#endif