#ifndef _WSR_CD_SC_BD_H
#define _WSR_CD_SC_BD_H

#pragma once

#include <string> 
#include <vector>
#include <map>

#define  cMaxMoment_CDSC  3 //要素总数量//
const uint32_t MOMENT_TYPE_ID_CDSC[cMaxMoment_CDSC]{ M_Z_ID, M_V_ID, M_W_ID };

////////////////////////////////////////////////////////////////////////////////////
//
class CRadarBaseDataCDSC
{
public:
	CRadarBaseDataCDSC();
	CRadarBaseDataCDSC(std::string strInFileName, int16_t dstOffset=0, int16_t dstScale=10);
	~CRadarBaseDataCDSC();

	virtual bool GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData);

private:
	int16_t m_outputOffset;
	int16_t m_outputScale;
	string m_strDataFileName;

	stdMomentFlag m_obsvMoments;
	
	stdSCRadarSite* m_pRadarSite;
	stdSCRadarPerformanceParam* m_pScPerformParam;
	stdSCRadarObservationParam* m_pScObsvParam;

	void CalScanTimeOfCut( uint16_t cutIdx, uint16_t &year, uint16_t &month, uint16_t &day, uint16_t &hour, uint16_t &minute, uint16_t &second);
	short GetBinValueZ(uint8_t src_code, int16_t dst_offset, int16_t dst_sale);
	short GetBinValueV(uint8_t src_code, float maxV, int16_t dst_offset, int16_t dst_sale);
	short GetBinValueW(uint8_t src_code, float maxV,int16_t dst_offset, int16_t dst_sale);

	int GetScanModeName(string &modeName);
	void CheckObsvMoments();
	void GetRadarID(int &ID,string &RadarCode);//
	void GetRadarName(string &RadarName);//
	void GetRadarType(string &type);
	void GetRadarSitePos(float &fLon, float &fLat, float &fHgt);
	uint16_t GetNumBeam( int CutIdx);	//获取一个仰角上的RADIAL数
	uint16_t GetDefaultNumBin(int cutidx);//获取径向上的库数-取决于格式的定义
	uint16_t GetValidNumBin(int CutIdx);//获取径向上的有效库数-与实际探测距离有关	
	uint16_t GetBinWidth(int CutIdx);//Get Surviliance mode bin size
	float GetElevation(int CutIdx);
	float GetMaxVel(int CutIdx);
};

#endif