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
//��ѡ�״����ѡ���״�������
//
const int32_t TOTAL_SITES = 300;					//�ɹ�ѡ����״��б�����
//
//#ifdef TRIAL_VERSION
const int32_t MAX_SITES_TRIAL = 20;					//����������״�վ��-����ʽ�û�
//#else
const int32_t MAX_SITES_FORMAL = 100;					//����������״�վ��-��ʽ�û�
//#endif
//
const int16_t PATH_LEN = 256;				//��������·������

int32_t const NAME_LEN = 24;
//
const int16_t  STAT_FISRT_LEVEL =  0; //��һ��
const int16_t  STAT_MID_LEVEL   =  1; //�м��
const int16_t  STAT_LAST_LEVEL  =  2; //���һ��
//
const int32_t    C_iFirstMosaicParamIdx = 0;
const int32_t    C_iLastMosaicParamIdx = 5;
//
const char INVALID_STRING[] = "----";	//Code for invalid string
//

//
//
//#define VALUE_INVALID_BYTE  0    //��Ч��ֵ, ���� CHAR ������
//#define VALUE_FOLD_BYTE		1  //��ʾ����ģ�� ���� CHAR ������
const unsigned char CODE_INVALID = 0;		//No data
const unsigned char CODE_RANFOLD = 1;		//range fold
const unsigned char CODE_NO_OBSERV = 2;	//no observation
const unsigned char CODE_CSRED = 3;      //Censored data

//#define CODE_MISSING	255		//Missing data
const char  CONF_FOLDER[] = "Conf";
const char TEMP_FOLDER[] = "TempData";
const char  LOGS_FOLDER[] = "LOG";
const char  NOWCST_TMP_FOLDER[] = "NOWCASTDATA";	//��ʱ����ǰһ��ʱ�ε����������ݵ��ļ����ڵ�Ŀ¼
const char  SGL_MSC_FOLDER[] = "SMOSAICDATA";	//�ѿ������굥վ�״�������ʱ�����λ��
const char  SGL_MSCV_FOLDER[] = "SMOSAICDATA_V";//�ѿ������굥վ�״�������ʱ�����λ��
const char  UNIFORM_POLAR_FOLDER[] = "UNIFORMBD";//ͳһ��ʽ���״������������
const char  MOSAIC_EXT_FILE[] = ".dat";		//	".MOS"		//�ѿ������굥վ�״�������ʱ������ļ���
const char  MOSAIC_SUBFOLDER[] = "mref";		//"MosaicData";	//�������ݱ������ײ�Ŀ¼
const char  TRACK_SUBFOLDER[] = "fore";		//�ٽ�Ԥ������-�ز�Ԥ����ƷĿ¼
const char  WIND_SUBFOLDER[] = "wind";		//�ٽ�Ԥ��-�ز��������ٲ�Ʒ
//
const char NETCDF_EXT_FILE[] = ".nc";			//file ext. name for NetCDF Format product data
//
const char QC_SUBFOLDER[] = "QCBaseData";	//Folder for QC processed data to be saved in
const char BD_SUBFOLDER[] = "BaseData";//	//Folder for radar base data
const char CR_SUBFOLDER[] = "CR";		//Folder for CR product
const char ET_SUBFOLDER[] = "ET";		//Folder for ET product
const char VIL_SUBFOLDER[] = "VIL";		//Folder for VIL product
const char QPR_SUBFOLDER[] = "QPR";		//��ˮ�ʲ�ƷĿ¼
const char QPE_SUBFOLDER[] = "QPE";		//��ˮ�ۻ���ƷĿ¼
const char CT_SUBFOLDER[] = "CT";		//������ʶ���ƷĿ¼
//
const char SETTINGFILEOLD[] = "Config.dat";			//�ɰ汾�Ĳ����ļ�
const char SETTINGFILE[] = "Config2.dat";			//�����ļ�
const char SETTINGFILE_BCK[] = "Config2_back.dat";	//���ݵĲ����ļ�
const char SETTINGVELGRIDFILE[] = "Config_velgrid.dat";	//������
//
const int16_t MSEC_IN_SEC = 1000;
const int32_t SEC_IN_DAY = 86400;
const int32_t SEC_IN_HOR = 3600;
const int16_t SEC_IN_MIN = 60;
const int16_t MIN_IN_DAY = 1440;
const int16_t MIN_IN_HOR = 60;
const int16_t HOR_IN_DAY = 24;
const double  KM_TO_DEG = 0.01;   //��γ��,1�����Լ�൱��0.01��
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

const int16_t MAX_HGT_LEVELS = 220;			//������ֱ�����������
const int16_t MAX_HGT_LEVELS_VEL = 100;		//������ֱ�����������--�ٶȸ�㻯

//--------------------------------------------------------------
const int16_t SITE_NAME_LEN = 20;		//վ�����Ƴ���(�ַ�)
const int16_t SITE_TYPE_LEN = 12;		//�״����ݸ�ʽ����������(�ַ�)
const int16_t DATA_TYPE_LEN = 4;		//�״����������ַ�������(��REF��VEL..)
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
//	int16_t   DelayedMin;			//���紫����ɵĻ�������ʱ���֣�
};
const int32_t SIZE_RADAR_INFO=sizeof(RADARSITEINFO);

//---------------------------------------------------------------------
struct _tagDataDirs
{
	char strInputDir[PATH_LEN];	//��������·��
	char strOutputDir[PATH_LEN];//�������·��
	char strSoundDir[PATH_LEN];	//̽������·��
};

struct _tagParams
{
	short	cCentX;		//��������λ�õ��״�ľ��루X-km��
	short	cCentY;		//��������λ�õ��״�ľ��루Y-km��
	int32_t	iResX;		//X����ķֱ��ʣ��ף�
	int32_t	iResY;		//Y����ķֱ��ʣ��ף�
	int32_t	iResZ;		//Z����ķֱ��ʣ��ף�
	short	cCircles;	//��������
	int32_t	iMode;		//0����ģʽ�� 1��ʪģʽ
};

struct _tagRunParams
{
	int16_t iRunCode;				//���Ƴ����Ƿ�����
	char  strThisFile[NAME_LEN];//ǰһ����ɨ�Ļ������ļ���
	char  strLastFile[NAME_LEN];//��ǰ��ɨ�������ļ���
};


//�״�վ����
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
	"SA/SB",		//Index:12 //�������ʼ�����ͼ����ϰ汾��վ��
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

//���¶�������·��,��������
struct DATADIRECTORY
{	
	char strTemDataDir[PATH_LEN];	//��ʱ�����ļ�λ��
	char strRainGageDir[PATH_LEN];	//����������λ��
	char strProdDataDir[PATH_LEN];	//��Ʒ�洢λ��
	char strRadarDataDir[PATH_LEN];	//�״�������·�� //���״����ݶ��洢��ͬһ��Ŀ¼��,����վ������
	int16_t iRadarSubflderType; //�״����ݷ�վ�ֱ�洢�ڲ�ͬ��Ŀ¼������ѡ�� 1��վ�� 2��վ�� 3������Ŀ¼  4�Զ�����Ŀ¼
};

//�����������
struct HORIZRESPARAM
{
	double fLonRes;//��
	double fLatRes;
};

//������-->��㻯����
struct POLARTOGRIDPARAM
{
	double fRes;//��
	int32_t iRng; //�뾶 ǧ��
};

//������������Ҫ�õ��ĵ�վ��������������λ��
struct SITEPOSINMOSAIC
{
	int32_t iMinLonGrid;		//��վ���������������е���С������λ��
	int32_t iMaxLonGrid;		//��վ���������������е��������λ��
	int32_t iMinLatGrid;		//��վ���������������е���Сγ����λ��
	int32_t iMaxLatGrid;		//��վ���������������е����γ����λ��
};

//��¼վ����һ�������ڵĴ������
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

//�����߳���Ҫ�Ĳ���
struct MOSAICPTHREADCOMM
{
	int32_t iRunCounter;
	int32_t iDataIndex;		//��ʾ��ʱ�εڼ���վ�Ѿ��������
	int32_t iNumData;			//��ʾ��ʱ��һ���м���վ����
	int8_t bMosaicTimeReached;//����ʱ���Ѿ��������������֤
	COMMONTIME commTime;	//��ʱ�ε�ʱ��
	vector<SITERUNNINGINFO> sitesRunningInfo; //һ��������վ����Ǵ������
	//char **strFileNames;	//���汾ʱ��ȫ���������ļ�
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
//ʣ�಻�õĹ�������չӦ�á�������Ӳ�Ʒ��������ʣ��ģ����ṹ��Ԫ�ص�
//�������ܱ䣬����ᵼ��ǰ��汾�������ļ������ݣ�15��Ԫ��
struct DERIVEDPRODSELECTION_ADD
{
	int8_t GenQPEPRod;	//��ˮ��Ʒ��������ˮǿ�Ⱥͽ�ˮ�ۻ���
	int8_t GenCtProd;     //������ʶ���Ʒ
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

//#���¶���ƴͼ���Ĳ��� ��ֱ��,�ֱ���,�������ò�����
struct MOSAICGRIDPARAM_SETTING
{
	HORIZRESPARAM horizentalRes;		//ˮƽ�ֱ���
	int16_t bUserDefineLevels;			//�û��Զ���߶Ȳ�
	int16_t numLevels;					//�߶Ȳ���
	uint16_t Heights[MAX_HGT_LEVELS]; //����߶ȣ�meters
};

//������������Ķ���
struct MOSAICGRIDPARAM
{
	int32_t iStartLon;		//������ʼ����Degree*1000
	int32_t iStartLat;		//������ʼγ��Degree*1000
	int32_t iStartHgt;		//��ʼ�߶� (��)
	uint16_t iLonNumGrids;	//��������
	uint16_t iLatNumGrids;	//γ������
	uint16_t iHgtNumLevels;	//��ֱ�����
	uint32_t iLonRes;			//����ֱ���Degree*10000
	uint32_t iLatRes;			//γ��ֱ���Degree*10000
	uint16_t iHeights[MAX_HGT_LEVELS];	//��ֱ����ĸ߶� meter
};


//��־��Ϣ��ʾ����
struct LOGMSGPARAM
{
	int8_t bShowLog;
	uint16_t iMaxStatusItem; //�����ʾ״̬������,����Ļ��¼��LOG�ļ�
	uint16_t iMaxErrorItem;  //�����ʾ���������,����Ļ��¼��LOG�ļ�
};


#endif