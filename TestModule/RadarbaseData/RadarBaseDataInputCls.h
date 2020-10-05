#pragma once

#include "RadarBaseDataMomentTypeDef.h"
#include "RadarBaseDataFormatTypeDef.h"
#include "RadarBaseDataCommonPPI.h"
#include "RadarBaseDataInputDef.h"
#include "RadarBaseDataFormatCIMISS.h"
#include "RadarBaseDataCIMISS.h"
#include "RadarBaseDataFormat98D.h"
#include "RadarBaseDataWsr98D.h"
#include "RadarBaseDataFormatCDSC.h"
#include "RadarBaseDataCDSC.h"
#include "RadarBaseDataFormatCC.h"
#include "RadarBaseDataCC.h"

class CRadarDataInputCls
{
public:
	CRadarDataInputCls(uint32_t scode, const char* szFileName);// (const char* szFileName, const char* szVersionFlag);
	~CRadarDataInputCls() = default;

	void AddNeededMoment(const char* szMomentType, bool bQc=false);
	void LoadScanData();
	void LoadCutData(float el, float az);

	stdUniformScanData* GetMomentScanData(const char* szMomentType);// , stdCommonScanData *pOutScanData);
	stdUniformScanData* GetMomentScanData(uint16_t nMomentTypeId);// , stdCommonScanData *pOutScanData);
	string  GetMomentScanFile(const char* szMomentType);// , stdCommonScanData *pOutScanData);
	string  GetMomentScanFile(uint16_t nMomentTypeId);//
private:
	string m_strFormatV;
	string m_strInFileName;
	uint32_t m_siteCode;

	int16_t m_uniformOffset;
	int16_t m_uniformFactor;

	string m_paramOutputPath;
	struct RADARSITEINFO m_ParamSiteInfo; //≈‰÷√≤Œ ˝
	stdMomentFlag m_paranmNeededFlag;

	//Flag moments needed or not	
	map_GeneralScanDatas m_genScanDatas;
	string m_strUniformFieNameV;
	string m_strUniformFieNameZ;
	/*
	CCIMISSRadarDataCls* m_pCimissRadarData;
	CRadarBaseDataWsr98DS* m_pMet98DSRadarData;
	CRadarBaseDataWsr98DC* m_pMet98DCRadarData;
	CRadarBaseDataCDSC2* m_pMetSCRadarData;
	CRadarBaseDataCDSC* m_pLocalSCRadarData;
	CRadarBaseDataCC1* m_pLoacalCC1RadarData;
	*/
	void LoadParameter();
	void DetermineRadarType(const char* szDataFileName, RADARSITEINFO siteInfo, char *szType);//
	void AddSiteInfoInData(stdUniformScanData *pvsScan, RADARSITEINFO siteInfo);
	string SaveUniformData(stdUniformScanData &vsData, char *pExtType, const char *dstPath, string &strDestFileName);
};

