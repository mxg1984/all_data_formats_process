#ifndef _WSR_CC1_BD_H
#define _WSR_CC1_BD_H

#pragma once

#include <string> 
#include <vector>
#include <map>

#define  cMaxMoment_CC1  3 //要素总数量//
const uint32_t MOMENT_TYPE_ID_CC1[cMaxMoment_CC1]{ M_Z_ID, M_V_ID, M_W_ID };

////////////////////////////////////////////////////////////////////////////////////
//
class CRadarBaseDataCC1
{
public:
	CRadarBaseDataCC1();
	CRadarBaseDataCC1(std::string strInFileName, int16_t dstOffset=0, int16_t dstScale=10);
	~CRadarBaseDataCC1();

	virtual bool GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData);
	
private:
	int16_t m_outputOffset;
	int16_t m_outputScale;
	string m_strDataFileName; 
	stdMomentFlag m_obsvMoments;

	stdCC1RadarSite* m_pRadarSite;
	stdCC1PerformanceParam* m_pPerformParam;
	stdCC1ObsvInfo* m_pObsvParam;

	void CalScanTimeOfCut(uint16_t cutIdx, uint16_t &year, uint16_t &month, uint16_t &day, uint16_t &hour, uint16_t &minute, uint16_t &second);
	//float GetBinValue(uint8_t code, int16_t offset, int16_t scale, float ndata);
	short GetBinValueZ(uint8_t src_code, int16_t dst_offset, int16_t dst_sale);
	short GetBinValueV(uint8_t src_code, float maxV, int16_t dst_offset, int16_t dst_sale);
	short GetBinValueW(uint8_t src_code, float maxV,int16_t dst_offset, int16_t dst_sale);

	void ConfirmMomentsObserved(uint16_t dataTypeCode, bool &bZ, bool &bV, bool &bW);
	int GetScanModeName(string &modeName);
	void CheckObsvMoments();
	void GetRadarID(int &ID, string &RadarCode);//
	void GetRadarName(string &RadarName);//
	void GetRadarType(string &type);
	void GetRadarSitePos(float &fLon, float &fLat, float &fHgt);
	uint16_t GetNumBeam(int CutIdx);	//获取一个仰角上的RADIAL数
	uint16_t GetDefaultNumBin(int cutidx);//获取径向上的库数-取决于格式的定义
	uint16_t GetValidNumBin(int CutIdx);//获取径向上的有效库数-与实际探测距离有关	
	uint16_t GetBinWidth(int CutIdx);//Get Surviliance mode bin size
	float GetElevation(int CutIdx);
	float GetMaxVel(int CutIdx);
};

#endif