//
#include "stdafx.h"
#include "CommVar.h"
#include "GlobalParams.h"
#include <windows.h>

//���ﶨ����ȫ�ֱ���

// ---------- ���浽�ļ��е�ȫ�ֱ��� -----------//
struct DERIVEDPRODSELECTION g_DerivedProdSelect={false, false, false, false, false};  //If derived products should be generated
struct DERIVEDPRODSELECTION_ADD g_AddDerivedProdSelect= //10 ��Ҫ��
{
	false,	false,	false,	false,	false,
	false,	false,	false,	false,	false,
	false,	false,	false,	false,	false
};

struct LOGMSGPARAM   g_LogShowParam={0, 30, 5};				//��־��ʾ����
struct DATADIRECTORY g_DataDir={"D:\\", "D:\\", "D:\\", "D:\\", 0};
struct RADARSITEINFO g_cRadarSiteInfo[TOTAL_SITES];
char   g_strRadarDataPath[TOTAL_SITES][PATH_LEN];				//Path of base data for each site
char g_strZoneName[SITE_NAME_LEN]=" ";
uint32_t  g_iSitesInZone=0;										//���������õ�վ�����

int8_t g_bUserDefineLevels=false;								//�Ƿ����û��Զ���߶�,�����,��ָ���ļ��༭�߶Ȳ�
uint32_t  g_iNumHeight=0;										//�߶Ȳ���
uint32_t  g_iHeights[MAX_HGT_LEVELS];							//����߶�
struct HORIZRESPARAM g_HorizRes={0.0, 0.0};					//ˮƽ�ֱ���

int8_t g_bUserDefineVelLevels=false;							//�Ƿ����û��Զ���߶�,�����,��ָ���ļ��༭�߶Ȳ�
uint32_t  g_iNumVelHeight=0;										//�߶Ȳ���
uint32_t  g_iVelHeights[MAX_HGT_LEVELS];							//����߶�
struct POLARTOGRIDPARAM g_PolarToGrdParam={0.0, 0};				//ˮƽ�ֱ��� �͸��뾶(km ���߸����)

uint16_t g_iIntervalTime=0;							//Minutes	ɨ���輸���ӵ�ʱ�䣬��ˣ�������ʱ����
uint16_t g_iMatchingTime=0;							//Minutes�� ����ɨ��ʱ�䲻ͬ��������һ��ƥ��ʱ��
uint16_t g_iDelayTime=0;								//Minutes. �������ݴ�����ӳ�ʱ��
int16_t g_iDeleteOldBaseData = false;                         //ʵʱģʽ�£��Ƿ�ɾ���Ѿ���������״�����0: ��; 1:��
int16_t g_iQualityControl = false;                            //�Ƿ������������? 0: ��; 1:��
int16_t g_iSaveSingleCAPPI= false;							//�Ƿ���е�վ������ݱ���. 0:������; 1:����
int16_t g_iSaveNetCDFData = false;							//�Ƿ񱣴�NetCDF��ʽ�Ĳ�Ʒ����
int16_t g_iSaveQcData = false;								//�Ƿ񱣴��������ƺ�Ļ�����
char  g_strQcDataPath[TOTAL_SITES][PATH_LEN];					//�������ƺ�����ݱ���·��
int16_t g_iProductRemainDays = 7;								//��Ʒ��������
int16_t g_iOptionsGridData=2;									//����վ��ά�����������ѡ�0:��������; 1:������; 2: ���ǿ������; 3:����ٶ�����
int16_t g_iDllFree=false;
char  g_strAppTmpFolder[PATH_LEN]=" ";						//Temporary files are stored in this folder

struct SITEPOSINMOSAIC g_SiteRangeInMosaic[TOTAL_SITES];	//��վ�����������е�λ��
struct MOSAICGRIDPARAM g_MosaicCommParam;					//�����������


