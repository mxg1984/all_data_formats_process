#pragma once

#include <string> 
#include <vector>
#include <map>

#define  cMaxMoment_98D  3 //要素总数量//
const uint32_t MOMENT_TYPE_ID_98D[cMaxMoment_98D]{ M_Z_ID, M_V_ID, M_W_ID };

////////////////////////////////////////////////////////////////////////////////////
//
class CRadarBaseDataWsr98D
{
public:
	CRadarBaseDataWsr98D();
	CRadarBaseDataWsr98D(std::string strInFileName, int16_t dstOffset=0, int16_t dstScale=10);
	~CRadarBaseDataWsr98D();

	int16_t m_outputOffset;
	int16_t m_outputScale; 
	std::string m_strDataFileName;

	bool IsBaseData(uint16_t RadarStat);
	bool IsCutStart(uint16_t RadialStat);
	bool IsCutEnd(uint16_t RadialStat);	
	bool IsScanStart(uint16_t RadialStat);
	bool IsScanEnd(uint16_t RadialStat);

	void TransDateTime(uint32_t JulSeconds, uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second);
	void TransDate(uint16_t JulDate, uint16_t &year, uint16_t &day, uint16_t &month);
	void TransTime(uint32_t Seconds, uint16_t &hour, uint16_t &minute, uint16_t &second);

	float GetBinValue(uint8_t code, int16_t offset, int16_t scale, float ndata);
	short GetBinValue(uint8_t src_code, int16_t src_offset, int16_t src_scale, int16_t dst_offset, int16_t dst_sale);

	void ConfirmMomentsObserved(stdRadialHeader98D *pRadialHeader, bool &bZ, bool &bV, bool &bW);
};

class CRadarBaseDataWsr98DS : public  CRadarBaseDataWsr98D
{
public:
	CRadarBaseDataWsr98DS(std::string strInFileName, int16_t dstOffset = 0, int16_t dstScale = 10);//: CRadarBaseDataWsr98D(strInFileName, dstOffset, dstScale) {};
	~CRadarBaseDataWsr98DS();

	bool GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData);

private:
	bool RefBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins);
	bool VelBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins);
	bool SpwBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins);
};

class CRadarBaseDataWsr98DC : public  CRadarBaseDataWsr98D
{
public:
	CRadarBaseDataWsr98DC(std::string strInFileName, int16_t dstOffset = 0, int16_t dstScale = 10);// : CRadarBaseDataWsr98D(strInFileName, dstOffset, dstScale) {};
	~CRadarBaseDataWsr98DC();

	bool GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData);

private:
	bool RefBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins);
	bool VelBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins);
	bool SpwBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins);
};


class CRadarBaseDataCDSC2 : public  CRadarBaseDataWsr98D
{
public:
	CRadarBaseDataCDSC2(std::string strInFileName, int16_t dstOffset = 0, int16_t dstScale = 10);// : CRadarBaseDataWsr98D(strInFileName, dstOffset, dstScale) {};
	~CRadarBaseDataCDSC2();
	
	bool GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData);

private:
	bool RefBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins);
	bool VelBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins);
	bool SpwBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins);
};