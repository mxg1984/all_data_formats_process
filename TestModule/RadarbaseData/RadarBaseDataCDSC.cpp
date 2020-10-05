#include "RadarBaseDataFormatCDSC.h"

#include "../CommVar.h"
#include "../CommFunctions.h"
#include "julian_date.h"
#include "RadarBaseDataMomentTypeDef.h"
#include "RadarBaseDataCommonPPI.h"
#include "RadarBaseDataInputDef.h"
#include "RadarBaseDataFormat98D.h"
#include "RadarBaseDataCDSC.h"

CRadarBaseDataCDSC::CRadarBaseDataCDSC()
{
}
CRadarBaseDataCDSC::CRadarBaseDataCDSC(std::string strInFileName,int16_t dstOffset,int16_t dstScale)
{
	m_strDataFileName.assign(strInFileName);
	m_outputOffset = dstOffset;
	m_outputScale = dstScale;

	m_pRadarSite   = 0x0;
	m_pScPerformParam = 0x0;
	m_pScObsvParam   = 0x0;
}

CRadarBaseDataCDSC::~CRadarBaseDataCDSC()
{
	//m_radialData.clear();
}

#include <string.h>
#include <stdio.h>
//获取一个仰角的PPI/SPPI 数据，或一个方位RHI数据
//cutIndex: 仰角/方位序号,从0开始计数
//fpOrigin: 开始读取的位置，SEEK_SET-文件开头，SEEK_CUR-当前位置
bool CRadarBaseDataCDSC::GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData)
{
	FILE* fp = 0;
	int nCuts = 0;
	std::string scanModeName = "";
	uint16_t n = 0, m = 0, nb = 0, numaz = 0;
	uint16_t mid = 0;
	
	bool istat = true;
	stdSCDataHeader fileHeader;	
	
	float radlon, radlat,radht;
	std::string radCode, radName, radType;
	int ncode;
	//-------------------------------------------------------------//

	errno_t err = fopen_s(&fp, m_strDataFileName.c_str(), "rb");
	if (err != 0)
		return false;

	if (fread_s(&fileHeader, SIZE_SC_DATA_HEADER/*SC_RADAR_DATA_HEADER_LEN*/, SIZE_SC_DATA_HEADER, 1, fp) < 1)
	{
		istat = false;
		fclose(fp);
		fp = 0;
		return false;
	}

	//各部分
	m_pRadarSite = &(fileHeader.SiteInfo);
	m_pScPerformParam = &(fileHeader.PerformanceInfo);
	m_pScObsvParam = &(fileHeader.ObservationInfo);

	//观测要素
	CheckObsvMoments();
	bool bZ = NeedStat.bZ & m_obsvMoments.bZ;
	bool bV = NeedStat.bV & m_obsvMoments.bV;
	bool bW = NeedStat.bW & m_obsvMoments.bW;

	//站址相关
	GetRadarSitePos(radlon, radlat, radht);
	GetRadarName(radName);		EraseSpace(radName);
	GetRadarID(ncode,radCode);  EraseSpace(radCode);
	GetRadarType(radType);		EraseSpace(radType);

	//观测模式、Cuts
	nCuts = GetScanModeName(scanModeName);
	if (nCuts == 0)
	{
		istat = false;
		fclose(fp);
		fp = 0;
		return false;
	}
	
	//Set scan header
	if (bZ)
	{
		mid = M_Z_ID;
		genScanData[mid].Available = true;
		strncpy(genScanData[mid].header.data_name, M_Z, min(DATA_TYPE_LEN, strlen(M_Z) + 1));
		strncpy(genScanData[mid].header.obsv_type, scanModeName.c_str(), min(SCAN_TYPE_LEN, scanModeName.length() + 1));
		strncpy(genScanData[mid].header.radar_name, radName.c_str(), min(SITE_NAME_LEN, radName.length() + 1));
		strncpy(genScanData[mid].header.radar_code, radCode.c_str(), min(SITE_CODE_LEN, radCode.length() + 1));// ->radartype);
		strncpy(genScanData[mid].header.radar_type, radType.c_str(), min(SITE_TYPE_LEN, radType.length() + 1));// ->radartype);
		genScanData[mid].header.radlon = uint32_t(radlon*1000);
		genScanData[mid].header.radlat = uint32_t(radlat*1000 );
		genScanData[mid].header.radhgt = uint16_t(radht);

		genScanData[mid].header.factor = m_outputScale;
		genScanData[mid].header.offset = m_outputOffset;
		genScanData[mid].header.ndata = VALUE_INVALID;
		genScanData[mid].CutsData.resize(0);
	}
	if (bV)
	{
		mid = M_V_ID;
		genScanData[mid].Available = true;
		strncpy(genScanData[mid].header.data_name, M_V, min(DATA_TYPE_LEN, strlen(M_V) + 1));
		strncpy(genScanData[mid].header.obsv_type, scanModeName.c_str(), min(SCAN_TYPE_LEN, scanModeName.length() + 1));
		strncpy(genScanData[mid].header.radar_name, radName.c_str(), min(SITE_NAME_LEN, radName.length() + 1));
		strncpy(genScanData[mid].header.radar_code, radCode.c_str(), min(SITE_CODE_LEN, radCode.length() + 1));// ->radartype);
		strncpy(genScanData[mid].header.radar_type, radType.c_str(), min(SITE_TYPE_LEN, radType.length() + 1));// ->radartype);
		genScanData[mid].header.radlon = uint32_t(radlon * 1000);
		genScanData[mid].header.radlat = uint32_t(radlat * 1000);
		genScanData[mid].header.radhgt = uint16_t(radht);

		genScanData[mid].header.factor = m_outputScale;
		genScanData[mid].header.offset = m_outputOffset;
		genScanData[mid].header.ndata = VALUE_INVALID;
		genScanData[mid].CutsData.resize(0);
	}
	if (bW)
	{
		mid = M_W_ID;
		genScanData[mid].Available = true;
		strncpy(genScanData[mid].header.data_name, M_W, min(DATA_TYPE_LEN, strlen(M_W) + 1));
		strncpy(genScanData[mid].header.obsv_type, scanModeName.c_str(), min(SCAN_TYPE_LEN, scanModeName.length() + 1));
		strncpy(genScanData[mid].header.radar_name, radName.c_str(), min(SITE_NAME_LEN, radName.length() + 1));
		strncpy(genScanData[mid].header.radar_code, radCode.c_str(), min(SITE_CODE_LEN, radCode.length() + 1));// ->radartype);
		strncpy(genScanData[mid].header.radar_type, radType.c_str(), min(SITE_TYPE_LEN, radType.length() + 1));// ->radartype);
		genScanData[mid].header.radlon = uint32_t(radlon * 1000);
		genScanData[mid].header.radlat = uint32_t(radlat * 1000);
		genScanData[mid].header.radhgt = uint16_t(radht);

		genScanData[mid].header.factor = m_outputScale;
		genScanData[mid].header.offset = m_outputOffset;
		genScanData[mid].header.ndata = VALUE_INVALID;
		genScanData[mid].CutsData.resize(0);
	}

	for (int nc = 0; nc < nCuts; nc++)
	{
		if (feof(fp)) 
			break;

		stdSCCutParam *pRawCut = &(fileHeader.ObservationInfo.LayerParam[nc]);
					 
		//Cut start time
		stdUniformCutData newCut; 
		CalScanTimeOfCut( nc,newCut.header.year, newCut.header.month, newCut.header.day,
			                 newCut.header.hour, newCut.header.minute, newCut.header.second);

		strncpy(newCut.header.obsv_type, scanModeName.c_str(), min(SCAN_TYPE_LEN, scanModeName.length() + 1));
		strncpy(newCut.header.radar_name, radName.c_str(), min(SITE_NAME_LEN, radName.length() + 1));
		strncpy(newCut.header.radar_code, radCode.c_str(), min(SITE_CODE_LEN, radCode.length() + 1));// ->radartype);
		strncpy(newCut.header.radar_type, radType.c_str(), min(SITE_TYPE_LEN, radType.length() + 1));// ->radartype);
		newCut.header.radlon = uint32_t(radlon * 1000);
		newCut.header.radlat = uint32_t(radlat * 1000);
		newCut.header.radhgt = uint16_t(radht);
			
		newCut.header.num_beam = GetNumBeam(nc);
		newCut.header.num_gate = GetValidNumBin(nc);		
		newCut.header.gateWidth = GetBinWidth(nc);   //0.1m
		newCut.header.nyq_vel = int16_t(GetMaxVel(nc) * 10);
		newCut.header.elev_angle = int16_t(GetElevation(nc) * 10);		
		
		//Add a cut in needed moments vScan 
		if (bZ)
		{
			mid = M_Z_ID;
			strncpy(newCut.header.data_name, M_Z, strlen(M_Z) + 1);
			genScanData[mid].CutsData.push_back(newCut);
		}
		if (bV)
		{
			mid = M_V_ID;
			strncpy(newCut.header.data_name, M_V, strlen(M_V) + 1);
			genScanData[mid].CutsData.push_back(newCut);
		}
		if (bW)
		{
			mid = M_W_ID;
			strncpy(newCut.header.data_name, M_W, strlen(M_W) + 1);
			genScanData[mid].CutsData.push_back(newCut);
		}
		
		int16_t iel = int16_t(GetElevation(nc) * 10);

		//Add radials in this cut for needed moments
		for (int na = 0; na < GetNumBeam(nc); na++)
		{//each radial
			stdRVP7RadialData rawRadial;
			if (fread_s(&rawRadial, sizeof(rawRadial), sizeof(rawRadial), 1, fp) < 1)
			{
				istat = false;
				fclose(fp);
				fp = 0;
				return false;
			}
			
			uint16_t iaz = 10*int16_t(5*(rawRadial.startaz+rawRadial.endaz) * 360. / 65536);// *90. * 10 / 32768);
			
			if (bZ)
			{
				mid = M_Z_ID;
				genScanData[mid].CutsData[nc].elev.push_back(iel);
				genScanData[mid].CutsData[nc].azim.push_back(iaz);

				vct_uniformRadial radialData;
				radialData.resize(GetDefaultNumBin(nc), VALUE_INVALID);
				int16_t nb = 0;
				for (nb = 0; nb <min(RECORD_LEN, GetValidNumBin(nc)); nb++)
					radialData[nb] = GetBinValueZ(rawRadial.RawData[nb].m_dbz, m_outputOffset, m_outputScale);
				genScanData[mid].CutsData[nc].field.push_back(radialData);
			}
			if (bV)
			{
				mid = M_V_ID;
				genScanData[mid].CutsData[nc].elev.push_back(iel);
				genScanData[mid].CutsData[nc].azim.push_back(iaz);

				vct_uniformRadial radialData;
				radialData.resize(genScanData[mid].CutsData[nc].header.num_gate, VALUE_INVALID);
				int16_t nb = 0;
				for (nb = 0; nb <min(RECORD_LEN, GetDefaultNumBin(nc)); nb++)
					radialData[nb] = GetBinValueV(rawRadial.RawData[nb].m_vel, pRawCut->MaxV/100., m_outputOffset, m_outputScale);
				genScanData[mid].CutsData[nc].field.push_back(radialData);
			}
			if (bW)
			{
				mid = M_W_ID;
				genScanData[mid].CutsData[nc].elev.push_back(iel);
				genScanData[mid].CutsData[nc].azim.push_back(iaz);

				vct_uniformRadial radialData;
				radialData.resize(genScanData[mid].CutsData[nc].header.num_gate, VALUE_INVALID);
				int16_t nb = 0;
				for (nb = 0; nb < min(RECORD_LEN, GetDefaultNumBin(nc)); nb++)
					radialData[nb] = GetBinValueW(rawRadial.RawData[nb].m_vel, pRawCut->MaxV/100., m_outputOffset, m_outputScale);
				genScanData[mid].CutsData[nc].field.push_back(radialData);
			}
		}//radials		
	}
	if (fp != 0)
		fclose(fp);// (fp);
	fp = 0x0;
	return(istat);
}

int16_t CRadarBaseDataCDSC::GetBinValueZ(uint8_t src_code,int16_t dst_offset, int16_t dst_scale)
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
		fv = float((t*1. - 64) / 2.);
		value = int16_t(dst_offset + fv*dst_scale);
	}
	return value;
}

int16_t CRadarBaseDataCDSC::GetBinValueV(uint8_t src_code, float maxV, int16_t dst_offset, int16_t dst_scale)
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
		fv = float((t*1. - 128.)*maxV / 128.);
		value = int16_t(dst_offset + fv*dst_scale);
	}
	return value;
}

int16_t CRadarBaseDataCDSC::GetBinValueW(uint8_t src_code, float maxV, int16_t dst_offset, int16_t dst_scale)
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
		fv = float((t*1.)*maxV / 256.);
		value = int16_t(dst_offset + fv*dst_scale);
	}
	return value;
}

//cutIdx--the first cut index is 0
//根据扫描开始和结束时间，计算某个cut的观测开始时间,
//计算的结果为   世界时 
void CRadarBaseDataCDSC::CalScanTimeOfCut(uint16_t cutIdx, uint16_t &year, uint16_t &month, uint16_t &day, uint16_t &hour, uint16_t &minute, uint16_t &second)
{
	if (m_pScObsvParam == 0)
		return;

	int32_t cjDate = 0,cSecond = 0;

	CDate sDate(m_pScObsvParam->syear + 2000, m_pScObsvParam->smonth, m_pScObsvParam->sday);
	int32_t sjDate = sDate.GetJulianDate();
	int32_t sSecond = m_pScObsvParam->shour*SEC_IN_HOR + m_pScObsvParam->sminute*SEC_IN_MIN + m_pScObsvParam->ssecond;
	sSecond -= 8 * SEC_IN_HOR; //转为世界时
	if (sSecond < 0)
	{
		sSecond += HOR_IN_DAY*SEC_IN_HOR;
		sjDate -= 1;
	}
	CDate eDate(m_pScObsvParam->eyear + 2000, m_pScObsvParam->emonth, m_pScObsvParam->eday);
	int32_t ejDate = eDate.GetJulianDate();
	int32_t eSecond = m_pScObsvParam->ehour*SEC_IN_HOR + m_pScObsvParam->eminute*SEC_IN_MIN + m_pScObsvParam->esecond;
	eSecond -= 8 * SEC_IN_HOR; //转为世界时
	if (eSecond < 0)
	{
		eSecond += HOR_IN_DAY*SEC_IN_HOR;
		ejDate -= 1;
	}

	cjDate = sjDate;
	cSecond = sSecond;

	if (m_pScObsvParam->stype > 100)
	{		
		int16_t nc = m_pScObsvParam->stype - 100;
		int16_t step_sec = ((ejDate - sjDate)*SEC_IN_DAY + (eSecond - sSecond)) / nc;
		cSecond += step_sec*cutIdx;
		if (cSecond >= SEC_IN_DAY)
		{
			cSecond -= SEC_IN_DAY;
			cjDate += 1;
		}
	}

	CDate cDate(cjDate);
	year = cDate.GetYear();
	month = cDate.GetMonth();
	day = cDate.GetDate();
	hour = cSecond / SEC_IN_HOR;
	minute = (cSecond - hour*SEC_IN_HOR) / SEC_IN_MIN;
	second = cSecond - hour*SEC_IN_HOR - minute*SEC_IN_MIN;
}

int CRadarBaseDataCDSC::GetScanModeName(std::string &modeName)
{
	int nCut = 0;
	modeName.assign("");

	if (m_pScObsvParam->stype == 1)
	{
		nCut = 1;
		modeName.assign("RHI");
	}
	else if (m_pScObsvParam->stype == 10)
	{
		nCut = 1;
		modeName.assign("PPI");
	}
	else if (m_pScObsvParam->stype > 100)
	{
		nCut = m_pScObsvParam->stype - 100;
		modeName.assign("VCP");
	}

	return nCut;
}

//Function:判断当前PPI中有什么数据
void CRadarBaseDataCDSC::CheckObsvMoments()
{
	//Initializatition
	m_obsvMoments.Reset();
	m_obsvMoments.bZ = true;
	m_obsvMoments.bZc = true;
	m_obsvMoments.bV = true;
	m_obsvMoments.bW = true;
}

//Function:获取雷达ID
void CRadarBaseDataCDSC::GetRadarID(int &ID, std::string &RadarCode)
{
	int nSiteID;
	nSiteID = atoi(m_pRadarSite->stationnumber);
	RadarCode.assign(m_pRadarSite->stationnumber);
}

//Function:获取雷达站名
void CRadarBaseDataCDSC::GetRadarName(std::string &RadarName)
{
	RadarName.assign(m_pRadarSite->station);
}

void CRadarBaseDataCDSC::GetRadarType(std::string &type)
{
	type.assign(m_pRadarSite->radartype);
}

//Function:获取雷达的位置高度信息
void CRadarBaseDataCDSC::GetRadarSitePos(float &fLon, float &fLat, float &fHgt)
{
	if (m_pRadarSite == 0)
		return;

	int deg, cent;
	char strDeg[4] = "", strCent[4] = "";
	int k, m;

	//雷达站高度
	fHgt = m_pRadarSite->height / 1000; //m

	 //这两个数值暂时没有填写， 改用其它方式获取雷达精度、纬度
	fLon = m_pRadarSite->longitudevalue/100.;  //deg
	fLat = m_pRadarSite->lantitudevalue/100.;  //deg

	if (fLon > 70. && fLon<135. && fLat>5. && fLat < 55.)
		return;

	//否则:
	//从字符串中获取雷达站纬度 
	k = 0;
	//获取度的数值
	for (m = 0; m<strlen(m_pRadarSite->longitude); m++)
	{
		if (m_pRadarSite->longitude[m] >= '0' && m_pRadarSite->longitude[m] <= '9')
		{
			strDeg[k] = m_pRadarSite->longitude[m];
			k++;
		}
		else
		{
			if (k>0)
				break;
		}
	}

	k = 0;
	//获取分的数值
	for (; m<strlen(m_pRadarSite->longitude); m++)
	{
		if (m_pRadarSite->longitude[m] >= '0' && m_pRadarSite->longitude[m] <= '9')
		{
			strCent[k] = m_pRadarSite->longitude[m];
			k++;
		}
		else
		{
			if (k>0)
				break;
		}
	}
	//纬度的浮点表示方式
	deg = atoi(strDeg);
	cent = atoi(strCent);
	fLon = deg + cent / 60.;

	//从字符串中获取雷达站经度
	k = 0;
	//获取度的数值
	for (m = 0; m<strlen(m_pRadarSite->latitude); m++)
	{
		if (m_pRadarSite->latitude[m] >= '0' && m_pRadarSite->latitude[m] <= '9')
		{
			strDeg[k] = m_pRadarSite->latitude[m];
			k++;
		}
		else
		{
			if (k>0)
				break;
		}
	}

	k = 0;
	//获取分的数值
	for (; m<strlen(m_pRadarSite->latitude); m++)
	{
		if (m_pRadarSite->latitude[m] >= '0' && m_pRadarSite->latitude[m] <= '9')
		{
			strCent[k] = m_pRadarSite->latitude[m];
			k++;
		}
		else
		{
			if (k>0)
				break;
		}
	}
	//经度的浮点表示方式
	deg = atoi(strDeg);
	cent = atoi(strCent);
	fLat = deg + cent / 60.;
}

//Function: 获取PPI上的径向数
//Parameter: CutIdx:Elevation index . >=0
uint16_t CRadarBaseDataCDSC::GetNumBeam(int CutIdx)
{
	return (m_pScObsvParam->LayerParam[CutIdx].recordnumber);
}

//Function:获取径向上的库数
//获取数据结构中定义的库数
//Parameter: CutIdx, Elevation index >=0
uint16_t CRadarBaseDataCDSC::GetDefaultNumBin(int cutidx)
{
	if (m_pScObsvParam == 0)
		return 0;

	if (m_pScObsvParam->LayerParam[cutidx].binnumber > 1000)
		return uint16_t(m_pScObsvParam->LayerParam[cutidx].binnumber / 4);
	else
		return uint16_t(m_pScObsvParam->LayerParam[cutidx].binnumber);
}

//获取扫描实际用到的库数
uint16_t CRadarBaseDataCDSC::GetValidNumBin(int CutIdx)
{
	if (m_pScObsvParam == 0)
		return 0;

	float maxL = m_pScObsvParam->LayerParam[CutIdx].MaxL / 100.;  //km
	float binW = m_pScObsvParam->LayerParam[CutIdx].binWidth / 10000.;//km
	return (uint16_t(maxL / binW + 0.5));
}

//Function:获取径向上的采样分辨率
//Parameter: CutIdx, Elevation index >=0
uint16_t CRadarBaseDataCDSC::GetBinWidth(int CutIdx)//Get Surviliance mode bin size
{
	if (m_pScObsvParam == 0)
		return 0;
	return uint16_t(m_pScObsvParam->LayerParam[CutIdx].binWidth*0.1); //m
}

float CRadarBaseDataCDSC::GetMaxVel(int CutIdx)
{
	return float(m_pScObsvParam->LayerParam[CutIdx].MaxV*0.01);//Max. velocity (m/s)
}

float CRadarBaseDataCDSC::GetElevation(int CutIdx)
{
	return float(m_pScObsvParam->LayerParam[CutIdx].Swangles / 100.);
}