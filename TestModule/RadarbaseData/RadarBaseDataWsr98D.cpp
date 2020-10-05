#include "stdafx.h"
#include "../CommVar.h"
#include "julian_date.h"
#include "RadarBaseDataMomentTypeDef.h"
#include "RadarBaseDataCommonPPI.h"
#include "RadarBaseDataInputDef.h"
#include "RadarBaseDataFormat98D.h"
#include "RadarBaseDataWsr98D.h"
#include "RadarBaseDataFormatSC2.h"

using namespace std;
CRadarBaseDataWsr98D::CRadarBaseDataWsr98D()
{}
CRadarBaseDataWsr98D::CRadarBaseDataWsr98D(std::string strInFileName,int16_t dstOffset,int16_t dstScale)
{
	m_strDataFileName.assign(strInFileName);
	m_outputOffset = dstOffset;
	m_outputScale = dstScale;
}

CRadarBaseDataWsr98D::~CRadarBaseDataWsr98D()
{
	//m_radialData.clear();
}

float CRadarBaseDataWsr98D::GetBinValue(uint8_t code, int16_t offset, int16_t scale, float ndata)
{
	float value = 0;
	int32_t t = 0;
	t = t | code;
	if (t ==0)
		value = VALUE_INVALID;
	else if(t==1)
		value = VALUE_RANFOLD;
	else
		value = float((t*1. - offset) / scale);
	return value;
}

int16_t CRadarBaseDataWsr98D::GetBinValue(uint8_t src_code, int16_t src_offset, int16_t src_scale, int16_t dst_offset, int16_t dst_scale)
{
	int16_t value = 0;
	float   fv = 0.;
	int32_t t = 0;
	t = t | src_code;
	if (t == 0)
		value = VALUE_INVALID;
	else if (t == 1)
		value = VALUE_RANFOLD;
	else
	{
		fv = float((t*1. - src_offset) / src_scale);
		value =int16_t(dst_offset + fv*dst_scale);
	}
	return value;
}

//确定需要读出且能从数据中获得的要素
void CRadarBaseDataWsr98D::ConfirmMomentsObserved(stdRadialHeader98D *pRadialHeader, bool &bZ, bool &bV, bool &bW)
{
	bZ = bV = bW = false;

	if (pRadialHeader->PtrOfArcReflectivity > 0)
		bZ = true;
	if (pRadialHeader->PtrOfArcVelocity > 0)
		bV = true;
	if (pRadialHeader->PtrOfArcWidth > 0)
		bW = true;
	return;
}

void CRadarBaseDataWsr98D::TransDateTime(uint32_t JulSeconds, uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second)
{
	uint16_t JulDate;
	uint32_t Seconds;

	JulDate = JulSeconds / sec_in_day;
	Seconds = JulSeconds - JulDate*sec_in_day;

	TransDate(JulDate, year, day, month);
	TransTime(Seconds, hour, minute, second);
}

void CRadarBaseDataWsr98D::TransDate(uint16_t JulDate, uint16_t &year, uint16_t &month, uint16_t &day)
{
	CDate date(JulDate);
	// Get  start date
	year = date.GetYear();
	month = date.GetMonth();
	day = date.GetDate();
}

void CRadarBaseDataWsr98D::TransTime(uint32_t Seconds, uint16_t &hour, uint16_t &minute, uint16_t &second)
{
	hour = Seconds / sec_in_hor;
	minute = (Seconds - hour * sec_in_hor) / sec_in_min;
	second = Seconds - hour * sec_in_hor - minute * sec_in_min;
}

bool CRadarBaseDataWsr98D::IsBaseData(uint16_t RadarStat)
{
	if (RadarStat == 1)
		return true;
	else
		return false;
}

bool CRadarBaseDataWsr98D::IsCutStart(uint16_t RadialStat)
{
	if (RadialStat == stat_elv_beg || RadialStat == stat_vol_beg)
		return true;
	else
		return false;
}

bool CRadarBaseDataWsr98D::IsCutEnd(uint16_t RadialStat)
{
	if (RadialStat == stat_elv_end || RadialStat == stat_vol_end)
		return true;
	else
		return false;
}

bool CRadarBaseDataWsr98D::IsScanStart(uint16_t RadialStat)
{
	if (RadialStat == stat_vol_beg)
		return true;
	else
		return false;
}
bool CRadarBaseDataWsr98D::IsScanEnd(uint16_t RadialStat)
{
	if (RadialStat == stat_vol_end)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
CRadarBaseDataWsr98DS::CRadarBaseDataWsr98DS(string strInFileName, int16_t dstOffset, int16_t dstScale)//:CRadarBaseDataWsr98D(strInFileName, dstOffset, dstScale)
{
	//CRadarBaseDataWsr98D::CRadarBaseDataWsr98D(strInFileName, dstOffset, dstScale);
	m_strDataFileName.assign(strInFileName);
	m_outputOffset = dstOffset;
	m_outputScale = dstScale;
}

CRadarBaseDataWsr98DS::~CRadarBaseDataWsr98DS()
{
}

//获取一个仰角的PPI/SPPI 数据，或一个方位RHI数据
//cutIndex: 仰角/方位序号,从0开始计数
//fpOrigin: 开始读取的位置，SEEK_SET-文件开头，SEEK_CUR-当前位置
bool CRadarBaseDataWsr98DS::GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData)
{
	uint16_t n = 0, m = 0, nb = 0, numaz = 0;
	uint16_t mid = 0;
	FILE* m_pFile = 0;
	bool istat = true;

	errno_t err = fopen_s(&m_pFile, m_strDataFileName.c_str(), "rb");
	if (err != 0)
		return false;

	bool bGoodScan = true;
	bool bGoodCut = false;
	bool bObsvZ = false, bObsvV = false, bObsvW = false;
	bool bZ = false, bV = false, bW = false;
	size_t ncR = 0, ncV = 0, ncW = 0;
	while (bGoodScan && !feof(m_pFile))
	{
		stdRadial98DS metRadial;//统一径向头
		if (fread(&metRadial, SIZE_RADIAL_98DS, 1, m_pFile) < 1)
		{
			istat = false;
			break;
		}

		if (!IsBaseData(metRadial.RadarStatus))
			continue;

		if (IsCutStart(metRadial.RadialStatus))
		{
			bGoodCut = true; //标记一个正常的Cut
			numaz = 0;

			//moment in this radial
			ConfirmMomentsObserved((stdRadialHeader98D*)&metRadial, bObsvZ, bObsvV, bObsvW);
			bZ = bObsvZ & NeedStat.bZ;
			bV = bObsvV & NeedStat.bV;
			bW = bObsvW & NeedStat.bW;

			stdUniformCutData newCut;
			TransDate(metRadial.JulianDate, newCut.header.year, newCut.header.month, newCut.header.day);
			TransTime(metRadial.mSeconds / 1000, newCut.header.hour, newCut.header.minute, newCut.header.second);

			if (bZ)
			{
				strcpy_s(newCut.header.data_name, strlen(M_Z) + 1, M_Z);
				newCut.header.gateWidth = metRadial.GateSizeOfReflectivity;
				newCut.header.num_gate = metRadial.GatesNumberOfReflectivity;
				mid = M_Z_ID;
				genScanData[mid].Available = true;
				genScanData[mid].CutsData.push_back(newCut);
				ncR = genScanData[mid].CutsData.size() - 1;

				if (IsScanStart(metRadial.RadialStatus))
				{
					strcpy_s(genScanData[mid].header.data_name, strlen(M_Z) + 1, M_Z);
					genScanData[mid].header.factor = m_outputScale;
					genScanData[mid].header.offset = m_outputOffset;
					genScanData[mid].header.ndata = VALUE_INVALID;
				}
			}
			if (bV)
			{
				strcpy_s(newCut.header.data_name, strlen(M_V) + 1, M_V);
				newCut.header.gateWidth = metRadial.GateSizeOfDoppler;
				newCut.header.num_gate = metRadial.GatesNumberOfDoppler;
				newCut.header.nyq_vel = metRadial.Nyquist;
				mid = M_V_ID;
				genScanData[mid].Available = true;
				genScanData[mid].CutsData.push_back(newCut);
				ncV = genScanData[mid].CutsData.size() - 1;

				if (IsScanStart(metRadial.RadialStatus))
				{
					strcpy_s(genScanData[mid].header.data_name, strlen(M_Z) + 1, M_Z);
					genScanData[mid].header.factor = m_outputScale;
					genScanData[mid].header.offset = m_outputOffset;
					genScanData[mid].header.ndata = VALUE_INVALID;
				}
			}
			if (bW)
			{
				strcpy_s(newCut.header.data_name, strlen(M_W) + 1, M_W);
				newCut.header.gateWidth = metRadial.GateSizeOfDoppler;
				newCut.header.num_gate = metRadial.GatesNumberOfDoppler;
				newCut.header.nyq_vel = metRadial.Nyquist;
				mid = M_W_ID;
				genScanData[mid].Available = true;
				genScanData[mid].CutsData.push_back(newCut);
				ncW = genScanData[mid].CutsData.size() - 1;

				if (IsScanStart(metRadial.RadialStatus))
				{
					strcpy_s(genScanData[mid].header.data_name, strlen(M_Z) + 1, M_Z);
					genScanData[mid].header.factor = m_outputScale;
					genScanData[mid].header.offset = m_outputOffset;
					genScanData[mid].header.ndata = VALUE_INVALID;
				}
			}
		}

		if (!bGoodCut)
			continue;

		double fel = get_angle(metRadial.El);
		double faz = get_angle(metRadial.Az);
		double value = 0;
		int16_t fstb = 0, lstb = 0, numbin = 0, i = 0;
		size_t nc = 0;
		if (bZ && ncR >= 0)
		{
			mid = M_Z_ID;
			nc = ncR;
			genScanData[mid].CutsData[nc].elev.push_back(int16_t(fel * 10));
			genScanData[mid].CutsData[nc].azim.push_back(int16_t(faz * 10));
			if (!RefBinParamsInRadial(&metRadial, fstb, lstb, numbin))
				break;
			size_t t = genScanData[mid].CutsData[nc].azim.size();
			if (t > MAX_RADS)
			{
				istat = false;
				break;
			}

			vct_uniformRadial radialData;
			radialData.resize(numbin);
			nb = 0;
			for (int16_t i = fstb - 1; i < lstb; i++)
			{
				radialData[nb] = GetBinValue(metRadial.data[i], 66, 2, m_outputOffset, m_outputScale);
				nb++;
			}
			for (; nb < numbin; nb++)
				radialData[nb] = VALUE_INVALID;
			genScanData[mid].CutsData[nc].field.push_back(radialData);
		}
		if (bV && ncV >= 0)
		{
			mid = M_V_ID;
			nc = ncV;
			genScanData[mid].CutsData[nc].elev.push_back(int16_t(fel * 10));
			genScanData[mid].CutsData[nc].azim.push_back(int16_t(faz * 10));
			if (!VelBinParamsInRadial(&metRadial, fstb, lstb, numbin))
			{
				istat = false;
				break;
			}
			if (genScanData[mid].CutsData[nc].azim.size() > MAX_RADS)
			{
				istat = false;
				break;
			}

			vct_uniformRadial radialData;
			radialData.resize(numbin);
			for (int16_t i = fstb - 1, nb = 0; i < lstb; i++, nb++)
				radialData[nb] = GetBinValue(metRadial.data[nb], 129, metRadial.ResolutionOfVelocity, m_outputOffset, m_outputScale);
			for (; nb < numbin; nb++)
				radialData[nb] = VALUE_INVALID;
			genScanData[mid].CutsData[nc].field.push_back(radialData);
		}
		if (bW  && ncW >= 0)
		{
			mid = M_W_ID;
			nc = ncW;
			nc = genScanData[mid].CutsData.size() - 1;
			genScanData[mid].CutsData[nc].elev.push_back(int16_t(fel * 10));
			genScanData[mid].CutsData[nc].azim.push_back(int16_t(faz * 10));
			if (!SpwBinParamsInRadial(&metRadial, fstb, lstb, numbin))
			{
				istat = false;
				break;
			}
			if (genScanData[mid].CutsData[nc].azim.size() > MAX_RADS)
			{
				istat = false;
				break;
			}

			vct_uniformRadial radialData;
			radialData.resize(numbin);
			for (int16_t i = fstb - 1, nb = 0; i < lstb; i++, nb++)
				radialData[nb] = GetBinValue(metRadial.data[nb], 129, 2, m_outputOffset, m_outputScale);
			for (; nb < numbin; nb++)
				radialData[nb] = VALUE_INVALID;
			genScanData[mid].CutsData[nc].field.push_back(radialData);
		}

		//Do something at end of a cut
		if (IsCutEnd(metRadial.RadialStatus))
		{
			//if (!bGoodCut)
			//	break; //没找到CUT开始标记的前提下有结束标记

			bGoodCut = false;

			float avgEl = 0;
			if (bZ && ncR >= 0)
			{
				mid = M_Z_ID;
				nc = ncR;
				avgEl = 0.;
				genScanData[mid].CutsData[nc].header.num_beam = uint16_t(genScanData[mid].CutsData[nc].elev.size());
				for (n = 0; n < genScanData[mid].CutsData[nc].header.num_beam; n++)
					avgEl += float(genScanData[mid].CutsData[nc].elev[n] / 10.);
				if (genScanData[mid].CutsData[nc].header.num_beam>0)
					avgEl /= genScanData[mid].CutsData[nc].header.num_beam;
				genScanData[mid].CutsData[nc].header.elev_angle = int16_t(avgEl * 10);
			}
			if (bV && ncV >= 0)
			{
				mid = M_V_ID;
				nc = ncV;
				avgEl = 0.;
				genScanData[mid].CutsData[nc].header.num_beam = uint16_t(genScanData[mid].CutsData[nc].elev.size());
				for (n = 0; n < genScanData[mid].CutsData[nc].header.num_beam; n++)
					avgEl /= genScanData[mid].CutsData[nc].header.num_beam;
				genScanData[mid].CutsData[nc].header.elev_angle = int16_t(avgEl * 10);
			}
			if (bW && ncW >= 0)
			{
				mid = M_W_ID;
				nc = ncW;
				avgEl = 0.;
				genScanData[mid].CutsData[nc].header.num_beam = uint16_t(genScanData[mid].CutsData[nc].elev.size());
				for (n = 0; n < genScanData[mid].CutsData[nc].header.num_beam; n++)
					avgEl /= genScanData[mid].CutsData[nc].header.num_beam;
				genScanData[mid].CutsData[nc].header.elev_angle = int16_t(avgEl * 10);
			}

			if (IsScanEnd(metRadial.RadialStatus))
			{
				istat = true;
				break;
			}
		}//end of a cut
	}// get all radials in file

	fclose(m_pFile);
	m_pFile = 0x0;
	return(istat);
}

//if bin parameter is incorrect, return false
bool CRadarBaseDataWsr98DS::RefBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins)
{
	int16_t  stpos;  //bin index start from 1 

	fstBin = pRadialHeader->RangeToFirstGateOfReflectivity / pRadialHeader->GateSizeOfReflectivity;
	if (fstBin < 0)
		fstBin = -fstBin ;
	fstBin += 1;
	defaultBins = pRadialHeader->GatesNumberOfReflectivity;
	if (defaultBins > SURV_BINS_S)
		defaultBins = SURV_BINS_S;
	lstBin = defaultBins;

	stpos = pRadialHeader->PtrOfArcReflectivity - 100;//! 本要素数据在Radial中的开始位置
	if (stpos + lstBin > SURV_BINS_S)
		return false;
	else if (stpos + fstBin < 1)
		return false;
	else
		return true;
}

bool CRadarBaseDataWsr98DS::VelBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins)
{
	int16_t  stpos;  //bin index start from 1 

	fstBin = pRadialHeader->RangeToFirstGateOfDoppler / pRadialHeader->GateSizeOfDoppler;
	if (fstBin < 0)
		fstBin = -fstBin;
	fstBin += 1;
	defaultBins = pRadialHeader->GatesNumberOfDoppler;
	if (defaultBins > DOPL_BINS_S)
		defaultBins = DOPL_BINS_S;
	lstBin = defaultBins;	
	stpos = pRadialHeader->PtrOfArcVelocity - 100;//! 本要素数据在Radial中的开始位置
	if (stpos + lstBin > SURV_BINS_S + DOPL_BINS_S)
		return false;
	else if (stpos + fstBin < 1)
		return false;
	else
		return true;
}
bool CRadarBaseDataWsr98DS::SpwBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins)
{
	int16_t  stpos;  //bin index start from 1 

	fstBin = pRadialHeader->RangeToFirstGateOfDoppler / pRadialHeader->GateSizeOfDoppler;
	if (fstBin < 0)
		fstBin = -fstBin;
	fstBin += 1;
	defaultBins = pRadialHeader->GatesNumberOfDoppler;
	if (defaultBins > DOPL_BINS_S)
		defaultBins = DOPL_BINS_S;
	lstBin = defaultBins;

	stpos = pRadialHeader->PtrOfArcWidth - 100;//! 本要素数据在Radial中的开始位置
	if (stpos + lstBin > SURV_BINS_S + DOPL_BINS_S * 2)
		return false;
	else if (stpos + fstBin < 1)
		return false;
	else
		return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//
CRadarBaseDataWsr98DC::CRadarBaseDataWsr98DC(string strInFileName, int16_t dstOffset, int16_t dstScale)
{	
	//CRadarBaseDataWsr98D::CRadarBaseDataWsr98D(strInFileName, dstOffset, dstScale);
	m_strDataFileName.assign(strInFileName);
	m_outputOffset = dstOffset;
	m_outputScale = dstScale;
}

CRadarBaseDataWsr98DC::~CRadarBaseDataWsr98DC()
{
}


//获取一个仰角的PPI/SPPI 数据，或一个方位RHI数据
//cutIndex: 仰角/方位序号,从0开始计数
//fpOrigin: 开始读取的位置，SEEK_SET-文件开头，SEEK_CUR-当前位置
bool CRadarBaseDataWsr98DC::GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData)
{
	uint16_t n = 0, m = 0, nb = 0, numaz = 0;
	uint16_t mid = 0;
	FILE* m_pFile = 0;
	bool istat = true;

	errno_t err = fopen_s(&m_pFile, m_strDataFileName.c_str(), "rb");
	if (err != 0)
		return false;

	bool bGoodScan = true;
	bool bGoodCut = false;
	bool bObsvZ = false, bObsvV = false, bObsvW = false;
	bool bZ = false, bV = false, bW = false;
	size_t ncR = 0, ncV = 0, ncW = 0;
	while (bGoodScan && !feof(m_pFile))
	{
		stdRadial98DC metRadial;//统一径向
		if (fread(&metRadial, SIZE_RADIAL_98DC, 1, m_pFile) < 1)
		{
			istat = false;
			break;
		}

		if (!IsBaseData(metRadial.RadarStatus))
			continue;

		if (IsCutStart(metRadial.RadialStatus))
		{
			bGoodCut = true; //标记一个正常的Cut
			numaz = 0;

			//moment in this radial
			ConfirmMomentsObserved((stdRadialHeader98D*)&metRadial, bObsvZ, bObsvV, bObsvW);
			bZ = bObsvZ & NeedStat.bZ;
			bV = bObsvV & NeedStat.bV;
			bW = bObsvW & NeedStat.bW;

			stdUniformCutData newCut;
			TransDate(metRadial.JulianDate, newCut.header.year, newCut.header.month, newCut.header.day);
			TransTime(metRadial.mSeconds / 1000, newCut.header.hour, newCut.header.minute, newCut.header.second);

			if (bZ)
			{
				strcpy_s(newCut.header.data_name, strlen(M_Z) + 1, M_Z);
				newCut.header.gateWidth = metRadial.GateSizeOfReflectivity;
				newCut.header.num_gate = metRadial.GatesNumberOfReflectivity;
				mid = M_Z_ID;
				genScanData[mid].Available = true;
				genScanData[mid].CutsData.push_back(newCut);
				ncR = genScanData[mid].CutsData.size() - 1;

				if (IsScanStart(metRadial.RadialStatus))
				{
					strcpy_s(genScanData[mid].header.data_name, strlen(M_Z) + 1, M_Z);
					genScanData[mid].header.factor = m_outputScale;
					genScanData[mid].header.offset = m_outputOffset;
					genScanData[mid].header.ndata = VALUE_INVALID;
				}
			}
			if (bV)
			{
				strcpy_s(newCut.header.data_name, strlen(M_V) + 1, M_V);
				newCut.header.gateWidth = metRadial.GateSizeOfDoppler;
				newCut.header.num_gate = metRadial.GatesNumberOfDoppler;
				mid = M_V_ID;
				genScanData[mid].Available = true;
				genScanData[mid].CutsData.push_back(newCut);
				ncV = genScanData[mid].CutsData.size() - 1;

				if (IsScanStart(metRadial.RadialStatus))
				{
					strcpy_s(genScanData[mid].header.data_name, strlen(M_Z) + 1, M_Z);
					genScanData[mid].header.factor = m_outputScale;
					genScanData[mid].header.offset = m_outputOffset;
					genScanData[mid].header.ndata = VALUE_INVALID;
				}
			}
			if (bW)
			{
				strcpy_s(newCut.header.data_name, strlen(M_W) + 1, M_W);
				newCut.header.gateWidth = metRadial.GateSizeOfDoppler;
				newCut.header.num_gate = metRadial.GatesNumberOfDoppler;
				mid = M_W_ID;
				genScanData[mid].Available = true;
				genScanData[mid].CutsData.push_back(newCut);
				ncW = genScanData[mid].CutsData.size() - 1;

				if (IsScanStart(metRadial.RadialStatus))
				{
					strcpy_s(genScanData[mid].header.data_name, strlen(M_Z) + 1, M_Z);
					genScanData[mid].header.factor = m_outputScale;
					genScanData[mid].header.offset = m_outputOffset;
					genScanData[mid].header.ndata = VALUE_INVALID;
				}
			}
		}

		if (!bGoodCut)
			continue;

		double fel = get_angle(metRadial.El);
		double faz = get_angle(metRadial.Az);
		double value = 0;
		int16_t fstb = 0, lstb = 0, numbin = 0, i = 0;
		size_t nc = 0;
		if (bZ && ncR >= 0)
		{
			mid = M_Z_ID;
			nc = ncR;
			genScanData[mid].CutsData[nc].elev.push_back(int16_t(fel * 10));
			genScanData[mid].CutsData[nc].azim.push_back(int16_t(faz * 10));
			if (!RefBinParamsInRadial(&metRadial, fstb, lstb, numbin))
				break;
			size_t t = genScanData[mid].CutsData[nc].azim.size();
			if (t > MAX_RADS)
			{
				istat = false;
				break;
			}

			vct_uniformRadial radialData;
			radialData.resize(numbin);
			nb = 0;
			for (int16_t i = fstb - 1; i < lstb; i++)
			{
				radialData[nb] = GetBinValue(metRadial.data[i], 66, 2, m_outputOffset, m_outputScale);
				nb++;
			}
			for (; nb < numbin; nb++)
				radialData[nb] = VALUE_INVALID;
			genScanData[mid].CutsData[nc].field.push_back(radialData);
		}
		if (bV && ncV >= 0)
		{
			mid = M_V_ID;
			nc = ncV;
			genScanData[mid].CutsData[nc].elev.push_back(int16_t(fel * 10));
			genScanData[mid].CutsData[nc].azim.push_back(int16_t(faz * 10));
			if (!VelBinParamsInRadial(&metRadial, fstb, lstb, numbin))
			{
				istat = false;
				break;
			}
			if (genScanData[mid].CutsData[nc].azim.size() > MAX_RADS)
			{
				istat = false;
				break;
			}

			vct_uniformRadial radialData;
			radialData.resize(numbin);
			for (int16_t i = fstb - 1, nb = 0; i < lstb; i++, nb++)
				radialData[nb] = GetBinValue(metRadial.data[nb], 129, metRadial.ResolutionOfVelocity, m_outputOffset, m_outputScale);
			for (; nb < numbin; nb++)
				radialData[nb] = VALUE_INVALID;
			genScanData[mid].CutsData[nc].field.push_back(radialData);
		}
		if (bW  && ncW >= 0)
		{
			mid = M_W_ID;
			nc = ncW;
			nc = genScanData[mid].CutsData.size() - 1;
			genScanData[mid].CutsData[nc].elev.push_back(int16_t(fel * 10));
			genScanData[mid].CutsData[nc].azim.push_back(int16_t(faz * 10));
			if (!SpwBinParamsInRadial(&metRadial, fstb, lstb, numbin))
			{
				istat = false;
				break;
			}
			if (genScanData[mid].CutsData[nc].azim.size() > MAX_RADS)
			{
				istat = false;
				break;
			}

			vct_uniformRadial radialData;
			radialData.resize(numbin);
			for (int16_t i = fstb - 1, nb = 0; i < lstb; i++, nb++)
				radialData[nb] = GetBinValue(metRadial.data[nb], 129, 2, m_outputOffset, m_outputScale);
			for (; nb < numbin; nb++)
				radialData[nb] = VALUE_INVALID;
			genScanData[mid].CutsData[nc].field.push_back(radialData);
		}

		//Do something at end of a cut
		if (IsCutEnd(metRadial.RadialStatus))
		{
			//if (!bGoodCut)
			//	break; //没找到CUT开始标记的前提下有结束标记
			bGoodCut = false;

			float avgEl = 0;
			if (bZ && ncR >= 0)
			{
				mid = M_Z_ID;
				nc = ncR;
				avgEl = 0.;
				genScanData[mid].CutsData[nc].header.num_beam = uint16_t(genScanData[mid].CutsData[nc].elev.size());
				for (n = 0; n < genScanData[mid].CutsData[nc].header.num_beam; n++)
					avgEl += float(genScanData[mid].CutsData[nc].elev[n] / 10.);
				if (genScanData[mid].CutsData[nc].header.num_beam>0)
					avgEl /= genScanData[mid].CutsData[nc].header.num_beam;
				genScanData[mid].CutsData[nc].header.elev_angle = int16_t(avgEl * 10);
			}
			if (bV && ncV >= 0)
			{
				mid = M_V_ID;
				nc = ncV;
				avgEl = 0.;
				genScanData[mid].CutsData[nc].header.num_beam = uint16_t(genScanData[mid].CutsData[nc].elev.size());
				for (n = 0; n < genScanData[mid].CutsData[nc].header.num_beam; n++)
					avgEl /= genScanData[mid].CutsData[nc].header.num_beam;
				genScanData[mid].CutsData[nc].header.elev_angle = int16_t(avgEl * 10);
			}
			if (bW && ncW >= 0)
			{
				mid = M_W_ID;
				nc = ncW;
				avgEl = 0.;
				genScanData[mid].CutsData[nc].header.num_beam = uint16_t(genScanData[mid].CutsData[nc].elev.size());
				for (n = 0; n < genScanData[mid].CutsData[nc].header.num_beam; n++)
					avgEl /= genScanData[mid].CutsData[nc].header.num_beam;
				genScanData[mid].CutsData[nc].header.elev_angle = int16_t(avgEl * 10);
			}

			if (IsScanEnd(metRadial.RadialStatus))
			{
				istat = true;
				break;
			}
		}//end of a cut
	}// get all radials in file

	fclose(m_pFile);
	m_pFile = 0x0;
	return(istat);
}

//if bin parameter is incorrect, return false
bool CRadarBaseDataWsr98DC::RefBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins)
{
	int16_t  stpos;  //bin index start from 1 

	fstBin = pRadialHeader->RangeToFirstGateOfReflectivity / pRadialHeader->GateSizeOfReflectivity;
	if (fstBin < 0)
		fstBin = -fstBin;
	fstBin += 1;
	defaultBins = pRadialHeader->GatesNumberOfReflectivity;
	if (defaultBins > SURV_BINS_C)
		defaultBins = SURV_BINS_C;
	lstBin = defaultBins;

	stpos = pRadialHeader->PtrOfArcReflectivity - 100;//! 本要素数据在Radial中的开始位置
	if (stpos + lstBin > SURV_BINS_C)
		return false;
	else if (stpos + fstBin < 1)
		return false;
	else
		return true;
}
bool CRadarBaseDataWsr98DC::VelBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins)
{
	int16_t  stpos;  //bin index start from 1 

	fstBin = pRadialHeader->RangeToFirstGateOfDoppler / pRadialHeader->GateSizeOfDoppler;
	if (fstBin < 0)
		fstBin = -fstBin;
	fstBin += 1;
	defaultBins = pRadialHeader->GatesNumberOfDoppler;
	if (defaultBins > DOPL_BINS_C)
		defaultBins = DOPL_BINS_C;
	lstBin = defaultBins;

	stpos = pRadialHeader->PtrOfArcVelocity - 100;//! 本要素数据在Radial中的开始位置
	if (stpos + lstBin > SURV_BINS_C + DOPL_BINS_C)
		return false;
	else if (stpos + fstBin < 1)
		return false;
	else
		return true;
}
bool CRadarBaseDataWsr98DC::SpwBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins)
{
	int16_t  stpos;  //bin index start from 1 

	fstBin = pRadialHeader->RangeToFirstGateOfDoppler / pRadialHeader->GateSizeOfDoppler;
	if (fstBin < 0)
		fstBin = -fstBin;
	fstBin += 1;
	defaultBins = pRadialHeader->GatesNumberOfDoppler;
	if (defaultBins > DOPL_BINS_C)
		defaultBins = DOPL_BINS_C;
	lstBin = defaultBins;

	stpos = pRadialHeader->PtrOfArcWidth - 100;//! 本要素数据在Radial中的开始位置
	if (stpos + lstBin > SURV_BINS_C + DOPL_BINS_C * 2)
		return false;
	else if (stpos + fstBin < 1)
		return false;
	else
		return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
CRadarBaseDataCDSC2::CRadarBaseDataCDSC2(string strInFileName, int16_t dstOffset, int16_t dstScale)
{
	//CRadarBaseDataWsr98D::CRadarBaseDataWsr98D(strInFileName, dstOffset, dstScale);
	m_strDataFileName.assign(strInFileName);
	m_outputOffset = dstOffset;
	m_outputScale = dstScale;
}

CRadarBaseDataCDSC2::~CRadarBaseDataCDSC2()
{
}

//获取一个仰角的PPI/SPPI 数据，或一个方位RHI数据
//cutIndex: 仰角/方位序号,从0开始计数
//fpOrigin: 开始读取的位置，SEEK_SET-文件开头，SEEK_CUR-当前位置
bool CRadarBaseDataCDSC2::GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData)
{
	uint16_t n = 0, m = 0, nb = 0, numaz = 0;
	uint16_t mid = 0;
	FILE* m_pFile = 0;
	bool istat = true;

	errno_t err = fopen_s(&m_pFile, m_strDataFileName.c_str(), "rb");
	if (err != 0)
		return false;

	bool bGoodScan = true;
	bool bGoodCut = false;
	bool bObsvZ = false, bObsvV = false, bObsvW = false;
	bool bZ = false, bV = false, bW = false;
	size_t ncR = 0, ncV = 0, ncW = 0;
	while (bGoodScan && !feof(m_pFile))
	{
		stdRadialSC2 metRadial;//统一径向
		if (fread(&metRadial, SIZE_RADIAL_SC2, 1, m_pFile) < 1)
		{
			istat = false;
			break;
		}

		if (!IsBaseData(metRadial.RadarStatus))
			continue;

		if (IsCutStart(metRadial.RadialStatus))
		{
			bGoodCut = true; //标记一个正常的Cut
			numaz = 0;

			//moment in this radial
			ConfirmMomentsObserved((stdRadialHeader98D*)&metRadial, bObsvZ, bObsvV, bObsvW);
			bZ = bObsvZ & NeedStat.bZ;
			bV = bObsvV & NeedStat.bV;
			bW = bObsvW & NeedStat.bW;

			stdUniformCutData newCut;
			TransDate(metRadial.JulianDate, newCut.header.year, newCut.header.month, newCut.header.day);
			TransTime(metRadial.mSeconds / 1000, newCut.header.hour, newCut.header.minute, newCut.header.second);

			if (bZ)
			{
				strcpy_s(newCut.header.data_name, strlen(M_Z) + 1, M_Z);
				newCut.header.gateWidth = metRadial.GateSizeOfReflectivity;
				newCut.header.num_gate = metRadial.GatesNumberOfReflectivity;
				mid = M_Z_ID;
				genScanData[mid].Available = true;
				genScanData[mid].CutsData.push_back(newCut);
				ncR = genScanData[mid].CutsData.size() - 1;

				if (IsScanStart(metRadial.RadialStatus))
				{
					strcpy_s(genScanData[mid].header.data_name, strlen(M_Z) + 1, M_Z);
					genScanData[mid].header.factor = m_outputScale;
					genScanData[mid].header.offset = m_outputOffset;
					genScanData[mid].header.ndata = VALUE_INVALID;
				}
			}
			if (bV)
			{
				strcpy_s(newCut.header.data_name, strlen(M_V) + 1, M_V);
				newCut.header.gateWidth = metRadial.GateSizeOfDoppler;
				newCut.header.num_gate = metRadial.GatesNumberOfDoppler;
				mid = M_V_ID;
				genScanData[mid].Available = true;
				genScanData[mid].CutsData.push_back(newCut);
				ncV = genScanData[mid].CutsData.size() - 1;

				if (IsScanStart(metRadial.RadialStatus))
				{
					strcpy_s(genScanData[mid].header.data_name, strlen(M_Z) + 1, M_Z);
					genScanData[mid].header.factor = m_outputScale;
					genScanData[mid].header.offset = m_outputOffset;
					genScanData[mid].header.ndata = VALUE_INVALID;
				}
			}
			if (bW)
			{
				strcpy_s(newCut.header.data_name, strlen(M_W) + 1, M_W);
				newCut.header.gateWidth = metRadial.GateSizeOfDoppler;
				newCut.header.num_gate = metRadial.GatesNumberOfDoppler;
				mid = M_W_ID;
				genScanData[mid].Available = true;
				genScanData[mid].CutsData.push_back(newCut);
				ncW = genScanData[mid].CutsData.size() - 1;

				if (IsScanStart(metRadial.RadialStatus))
				{
					strcpy_s(genScanData[mid].header.data_name, strlen(M_Z) + 1, M_Z);
					genScanData[mid].header.factor = m_outputScale;
					genScanData[mid].header.offset = m_outputOffset;
					genScanData[mid].header.ndata = VALUE_INVALID;
				}
			}
		}

		if (!bGoodCut)
			continue;

		double fel = get_angle(metRadial.El);
		double faz = get_angle(metRadial.Az);
		double value = 0;
		int16_t fstb = 0, lstb = 0, numbin = 0, i = 0;
		size_t nc = 0;
		if (bZ && ncR >= 0)
		{
			mid = M_Z_ID;
			nc = ncR;
			genScanData[mid].CutsData[nc].elev.push_back(int16_t(fel * 10));
			genScanData[mid].CutsData[nc].azim.push_back(int16_t(faz * 10));
			if (!RefBinParamsInRadial(&metRadial, fstb, lstb, numbin))
				break;
			size_t t = genScanData[mid].CutsData[nc].azim.size();
			if (t > MAX_RADS)
			{
				istat = false;
				break;
			}

			vct_uniformRadial radialData;
			radialData.resize(numbin);
			nb = 0;
			for (int16_t i = fstb - 1; i < lstb; i++)
			{
				radialData[nb] = GetBinValue(metRadial.data[i], 66, 2, m_outputOffset, m_outputScale);
				nb++;
			}
			for (; nb < numbin; nb++)
				radialData[nb] = VALUE_INVALID;
			genScanData[mid].CutsData[nc].field.push_back(radialData);
		}
		if (bV && ncV >= 0)
		{
			mid = M_V_ID;
			nc = ncV;
			genScanData[mid].CutsData[nc].elev.push_back(int16_t(fel * 10));
			genScanData[mid].CutsData[nc].azim.push_back(int16_t(faz * 10));
			if (!VelBinParamsInRadial(&metRadial, fstb, lstb, numbin))
			{
				istat = false;
				break;
			}
			if (genScanData[mid].CutsData[nc].azim.size() > MAX_RADS)
			{
				istat = false;
				break;
			}

			vct_uniformRadial radialData;
			radialData.resize(numbin);
			for (int16_t i = fstb - 1, nb = 0; i < lstb; i++, nb++)
				radialData[nb] = GetBinValue(metRadial.data[nb], 129, metRadial.ResolutionOfVelocity, m_outputOffset, m_outputScale);
			for (; nb < numbin; nb++)
				radialData[nb] = VALUE_INVALID;
			genScanData[mid].CutsData[nc].field.push_back(radialData);
		}
		if (bW  && ncW >= 0)
		{
			mid = M_W_ID;
			nc = ncW;
			nc = genScanData[mid].CutsData.size() - 1;
			genScanData[mid].CutsData[nc].elev.push_back(int16_t(fel * 10));
			genScanData[mid].CutsData[nc].azim.push_back(int16_t(faz * 10));
			if (!SpwBinParamsInRadial(&metRadial, fstb, lstb, numbin))
			{
				istat = false;
				break;
			}
			if (genScanData[mid].CutsData[nc].azim.size() > MAX_RADS)
			{
				istat = false;
				break;
			}

			vct_uniformRadial radialData;
			radialData.resize(numbin);
			for (int16_t i = fstb - 1, nb = 0; i < lstb; i++, nb++)
				radialData[nb] = GetBinValue(metRadial.data[nb], 129, 2, m_outputOffset, m_outputScale);
			for (; nb < numbin; nb++)
				radialData[nb] = VALUE_INVALID;
			genScanData[mid].CutsData[nc].field.push_back(radialData);
		}

		//Do something at end of a cut
		if (IsCutEnd(metRadial.RadialStatus))
		{
			//if (!bGoodCut)
			//	break; //没找到CUT开始标记的前提下有结束标记
			bGoodCut = false;

			float avgEl = 0;
			if (bZ && ncR >= 0)
			{
				mid = M_Z_ID;
				nc = ncR;
				avgEl = 0.;
				genScanData[mid].CutsData[nc].header.num_beam = uint16_t(genScanData[mid].CutsData[nc].elev.size());
				for (n = 0; n < genScanData[mid].CutsData[nc].header.num_beam; n++)
					avgEl += float(genScanData[mid].CutsData[nc].elev[n] / 10.);
				if (genScanData[mid].CutsData[nc].header.num_beam>0)
					avgEl /= genScanData[mid].CutsData[nc].header.num_beam;
				genScanData[mid].CutsData[nc].header.elev_angle = int16_t(avgEl * 10);
			}
			if (bV && ncV >= 0)
			{
				mid = M_V_ID;
				nc = ncV;
				avgEl = 0.;
				genScanData[mid].CutsData[nc].header.num_beam = uint16_t(genScanData[mid].CutsData[nc].elev.size());
				for (n = 0; n < genScanData[mid].CutsData[nc].header.num_beam; n++)
					avgEl /= genScanData[mid].CutsData[nc].header.num_beam;
				genScanData[mid].CutsData[nc].header.elev_angle = int16_t(avgEl * 10);
			}
			if (bW && ncW >= 0)
			{
				mid = M_W_ID;
				nc = ncW;
				avgEl = 0.;
				genScanData[mid].CutsData[nc].header.num_beam = uint16_t(genScanData[mid].CutsData[nc].elev.size());
				for (n = 0; n < genScanData[mid].CutsData[nc].header.num_beam; n++)
					avgEl /= genScanData[mid].CutsData[nc].header.num_beam;
				genScanData[mid].CutsData[nc].header.elev_angle = int16_t(avgEl * 10);
			}

			if (IsScanEnd(metRadial.RadialStatus))
			{
				istat = true;
				break;
			}
		}//end of a cut
	}// get all radials in file

	fclose(m_pFile);
	m_pFile = 0x0;
	return(istat);
}

//if bin parameter is incorrect, return false
bool CRadarBaseDataCDSC2::RefBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins)
{
	int16_t  stpos;  //bin index start from 1 

	fstBin = pRadialHeader->RangeToFirstGateOfReflectivity / pRadialHeader->GateSizeOfReflectivity;
	if (fstBin < 0)
		fstBin = -fstBin;
	fstBin += 1;
	defaultBins = pRadialHeader->GatesNumberOfReflectivity;
	if (defaultBins > SURV_BINS_SC2)
		defaultBins = SURV_BINS_SC2;
	lstBin = defaultBins;

	stpos = pRadialHeader->PtrOfArcReflectivity - 100;//! 本要素数据在Radial中的开始位置
	if (stpos + lstBin > SURV_BINS_SC2)
		return false;
	else if (stpos + fstBin < 1)
		return false;
	else
		return true;
}
bool CRadarBaseDataCDSC2::VelBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins)
{
	int16_t  stpos;  //bin index start from 1 

	fstBin = pRadialHeader->RangeToFirstGateOfDoppler / pRadialHeader->GateSizeOfDoppler;
	if (fstBin < 0)
		fstBin = -fstBin;
	fstBin += 1;
	defaultBins = pRadialHeader->GatesNumberOfDoppler;
	if (defaultBins > DOPL_BINS_SC2)
		defaultBins = DOPL_BINS_SC2;
	lstBin = defaultBins;

	stpos = pRadialHeader->PtrOfArcVelocity - 100;//! 本要素数据在Radial中的开始位置
	if (stpos + lstBin > SURV_BINS_C + DOPL_BINS_SC2)
		return false;
	else if (stpos + fstBin < 1)
		return false;
	else
		return true;
}
bool CRadarBaseDataCDSC2::SpwBinParamsInRadial(stdRadialHeader98D *pRadialHeader, int16_t &fstBin, int16_t &lstBin, int16_t &defaultBins)
{
	int16_t  stpos;  //bin index start from 1 

	fstBin = pRadialHeader->RangeToFirstGateOfDoppler / pRadialHeader->GateSizeOfDoppler;
	if (fstBin < 0)
		fstBin = -fstBin;
	fstBin += 1;
	defaultBins = pRadialHeader->GatesNumberOfDoppler;
	if (defaultBins > DOPL_BINS_SC2)
		defaultBins = DOPL_BINS_SC2;
	lstBin = defaultBins;

	stpos = pRadialHeader->PtrOfArcWidth - 100;//! 本要素数据在Radial中的开始位置
	if (stpos + lstBin > SURV_BINS_C + DOPL_BINS_SC2 * 2)
		return false;
	else if (stpos + fstBin < 1)
		return false;
	else
		return true;
}