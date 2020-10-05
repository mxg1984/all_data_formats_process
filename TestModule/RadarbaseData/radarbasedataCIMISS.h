#pragma once
//
#include "stdio.h"
#include <stdint.h>
#include <string> 
#include <vector>
#include <list>
#include <map>

#include "RadarBaseDataFormatCIMISS.h"
#include "RadarBaseDataCommonPPI.h"

//-------------------------------------------------------//
//
struct stdCimissMomentRadial
{
	stdCimissGenRadialHeader genHeader;
	stdCimissOneMomentRadial Radial;
};
typedef std::list<stdCimissMomentRadial> vct_momentCut;	//单要素一个CUT的所有radials
typedef std::list<vct_momentCut> vct_momentCuts;
struct stdCimissMomentCuts
{
	bool Needed;	//需提出来
	bool Available; //可获得
	vct_momentCuts CutsData;//所有仰角的数据
	stdCimissMomentCuts();
};
//typedef map<uint16_t, stdCimissMomentCuts> map_Cimiss_MomentsCuts;

#define  cMaxMoment  19			//要素总数量//
typedef enum CIMISS_MOMENT_ID
{	
	CIMISS_M_ID_Z=0,
	CIMISS_M_ID_Zc,
	CIMISS_M_ID_V,
	CIMISS_M_ID_Vc,
	CIMISS_M_ID_W,
	CIMISS_M_ID_Wc,
	CIMISS_M_ID_ZDR,
	CIMISS_M_ID_ZDRc,
	CIMISS_M_ID_LDR,
	CIMISS_M_ID_CC,
	CIMISS_M_ID_PDP,
	CIMISS_M_ID_KDP,
	CIMISS_M_ID_HCL,
	CIMISS_M_ID_DBT,
	CIMISS_M_ID_CP,
	CIMISS_M_ID_CF,
	CIMISS_M_ID_SQI,
	CIMISS_M_ID_CPA,
	CIMISS_M_ID_SNR
}MomentID;//
const int64_t CIMISS_M_BIT_ID[cMaxMoment]//与MOMENT_ID顺序对应好
{
	BIT_DBZ,BIT_Zc,BIT_V,BIT_Vc,BIT_W,BIT_Wc,BIT_ZDR,BIT_ZDRc,
	BIT_LDR,BIT_CC,BIT_PDP,BIT_KDP,BIT_HCL,
	BIT_DBT,BIT_CP,BIT_CF,BIT_SQI,BIT_CPA,BIT_SNR
};//
//data type index
const uint32_t CIMISS_M_TYPE_ID[cMaxMoment]
{	
	BIT_Index_DBZ,
	BIT_Index_Zc,
	BIT_Index_V,
	BIT_Index_Vc,
	BIT_Index_W,
	BIT_Index_Wc,
	BIT_Index_ZDR,
	BIT_Index_ZDRc,
	BIT_Index_LDR,
	BIT_Index_CC,
	BIT_Index_PDP,
	BIT_Index_KDP,
	BIT_Index_HCL,
	BIT_Index_DBT,	
	BIT_Index_CP,	
	BIT_Index_CF,
	BIT_Index_SQI,
	BIT_Index_CPA,
	BIT_Index_SNR
};
//
////////////////////////////////////////////////////////////////////////////////////
//
class CCIMISSRadarDataCls
{
public:
	CCIMISSRadarDataCls(std::string strInFileName, int16_t dstOffset, int16_t dstScale);
	~CCIMISSRadarDataCls();
	
	bool GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData);//获取一个仰角的PPI/SPPI 数据，或一个方位RHI数据

private:
	int16_t m_outputOffset;
	int16_t m_outputScale;
	std::string m_strDataFileName;
	FILE* m_pDataFile;

	stdBaseDataRadarInfo m_bdRadarInfo; //基数据相关的的雷达参数

	stdMomentFlag m_obsvMoments;
	std::map<uint16_t, bool> m_bMomentObsved;// [cMaxMoment];
	std::map<uint16_t, bool> m_bMomentNeeded;// [cMaxMoment];

	int32_t  m_MomentCutId[cMaxMoment];		//不同要素的仰角序号 1开始

	int32_t  m_CurrentCutId;//当前仰角序号 1开始---不论要素
	//map<uint16_t, stdCimissMomentCuts>	m_MomentsData; // 各要素多层数据
	stdCimissCommonBlock	m_CommonBlock; // 公共数据块

	bool IsPPI(int16_t &bnCuts);	//是否为单/多层RHI扫描
	bool IsSPPI(int16_t &nCuts);	//是否为单/多层扇扫
	bool IsRHI(int16_t &nAzs);	//是否为单/多方位RHI扫描

	void GetCommonBlock();
	//int ConfirmMomentsToGet(uint16_t  cutIndex);
	void ConfirmMomentsObserved(uint16_t  cutIndex);
	void ConfirmMomentsNeeded(stdMomentFlag NeedStat);
	void ConfirmMomentsObserved(); //All cuts
	void TransDateTime(uint32_t JulSeconds, uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second);
	void TransDate(uint16_t JulDate, uint16_t &year, uint16_t &day, uint16_t &month);
	void TransTime(uint32_t Seconds, uint16_t &hour, uint16_t &minute, uint16_t &second);

	bool IsCutStart(uint16_t RadialStat);
	bool IsCutEnd(uint16_t RadialStat);
	bool  IsScanStart(uint16_t RadialStat);
	bool  IsScanEnd(uint16_t RadialStat);

	int16_t MomentBitIndex(short m);
	uint16_t GetMomentTypeId(const char* szMomentName); //根据要素名给出本类所对应的要素ID
	int16_t MomentIdByType(uint16_t srcMomentType);

	float GetBinValue(uint8_t code, int32_t offset, int32_t scale, float ndata); //Decode 
	float GetBinValue(uint16_t code, int32_t offset, int32_t scale, float nata);
	uint16_t GetBinWidth(uint16_t momid, uint16_t cutid);
	void GetBeamWidth(float &hbw, float &vbw);
	uint16_t GetNumberOfBins(stdCimissMomentRadialHeader radialMomentHeader);

	stdCimissCutConfig* GetCutConfig(uint16_t cutid);
	stdCimissScanTaskConfig* GetScanConfig();
	void GetRadarSite(std::string &sname, std::string &scode, std::string &stype, float &lon, float &lat, float &ht);
	//void GetRadarSite(char* sname, char* scode, char* stype, float &lon, float &lat, float &ht);
	//uint16_t GetCutNum(uint16_t momid);
	//vct_momentCut* GetMomentCut(uint16_t momid, uint16_t cutidx);
	void GetRadialDateTime(stdCimissGenRadialHeader radialHeader, uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second);
	void GetScanStartDateTime(uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second);
	int GetScanModeName(std::string &modeName);

	void MomentTypeLocalToUniform(uint16_t localMomentType,uint16_t &uniformMoemntId);
	void MomentIdLocalToUniform(uint16_t localMomentId, uint16_t &uniformMoemntId);
	void MomentIdUniformToLocal(uint16_t uniformMoemntId, uint16_t &localMomentId);

	float GetMaxVel(uint16_t  cutIndex);
	float GetCutAngle(uint16_t  cutIndex);


	//设置统一格式中的站点
	void SetRadarParamInUniformData(map_GeneralScanDatas &genScanData);//
};