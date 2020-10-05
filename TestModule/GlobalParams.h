#pragma once
#include "stdAfx.h"

#include <string>
#include "UniformDataFormatDef.h"

////////////////////////////////////////////////////////////////////////////////////////

extern struct LOGMSGPARAM   g_LogShowParam;
extern struct DERIVEDPRODSELECTION g_DerivedProdSelect;
extern struct DERIVEDPRODSELECTION_ADD g_AddDerivedProdSelect; //�������ӵĲ�Ʒ
extern struct DATADIRECTORY g_DataDir;
extern struct RADARSITEINFO g_cRadarSiteInfo[TOTAL_SITES];
extern char   g_strRadarDataPath[TOTAL_SITES][PATH_LEN];	//Path of base data for each site
extern char g_strZoneName[SITE_NAME_LEN];
extern uint32_t  g_iSitesInZone;									//���������õ�վ�����
//ǿ���������
extern int8_t g_bUserDefineLevels;							//�Ƿ����û��Զ���߶�,�����,��ָ���ļ��༭�߶Ȳ� 2017.10
extern uint32_t  g_iNumHeight;									//�߶Ȳ���
extern uint32_t  g_iHeights[MAX_HGT_LEVELS];						//����߶�
extern struct HORIZRESPARAM g_HorizRes;						//ˮƽ�ֱ���
//�ٶ��������
extern int8_t g_bUserDefineVelLevels;							//�Ƿ����û��Զ���߶�,�����,��ָ���ļ��༭�߶Ȳ�
extern uint32_t  g_iNumVelHeight;								//�߶Ȳ���
extern uint32_t  g_iVelHeights[MAX_HGT_LEVELS];					//����߶�
extern struct POLARTOGRIDPARAM g_PolarToGrdParam;			//ˮƽ�ֱ��� �͸��뾶(km ���߸����)
extern uint16_t g_iIntervalTime;						//Minutes	ɨ���輸���ӵ�ʱ�䣬��ˣ�������ʱ����
extern uint16_t g_iMatchingTime;						//Minutes�� ����ɨ��ʱ�䲻ͬ��������һ��ƥ��ʱ��
extern uint16_t g_iDelayTime;							//Minutes. �������ݴ�����ӳ�ʱ��
extern int16_t g_iDeleteOldBaseData;							//ʵʱģʽ�£��Ƿ�ɾ���Ѿ���������״�����0: ��; 1:��
extern int16_t g_iQualityControl;                             //�Ƿ������������? 0: ��; 1:��
extern int16_t g_iSaveSingleCAPPI;							//�Ƿ���е�վ������ݱ���. 0:������; 1:����
extern int16_t g_iSaveNetCDFData;								//�Ƿ񱣴�NetCDF��ʽ�Ĳ�Ʒ����(*.nc)
extern int16_t g_iSaveQcData;									//�Ƿ񱣴��������ƺ�Ļ�����
extern char  g_strQcDataPath[TOTAL_SITES][PATH_LEN];		//�������ƺ�����ݱ���·
extern int16_t g_iProductRemainDays;							//��Ʒ��������
extern int16_t g_iOptionsGridData;							//�����������ѡ��
extern char  g_strAppTmpFolder[PATH_LEN];					//Temporary files are stored in this folder
extern int16_t g_iDllFree;

extern struct SITEPOSINMOSAIC g_SiteRangeInMosaic[TOTAL_SITES];		//��վ�����������е�λ��
extern struct MOSAICGRIDPARAM g_MosaicCommParam;					//�����������