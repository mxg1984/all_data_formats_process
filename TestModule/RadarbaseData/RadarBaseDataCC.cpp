#include "RadarBaseDataCC.h"

#include <cmath>
#include <cstring>
#include <cstdio>

#include "../CommVar.h"
#include "../CommFunctions.h"
#include "julian_date.h"
#include "RadarBaseDataMomentTypeDef.h"
#include "RadarBaseDataCommonPPI.h"
#include "RadarBaseDataFormat98D.h"

CRadarBaseDataCC1::CRadarBaseDataCC1(std::string strInFileName,int16_t dstOffset,int16_t dstScale)
{
	m_strDataFileName.assign(strInFileName);
	m_outputOffset = dstOffset;
	m_outputScale = dstScale;

	m_pRadarSite=0x0;
	m_pPerformParam = 0x0;
	m_pObsvParam = 0x0;
}

CRadarBaseDataCC1::~CRadarBaseDataCC1()
{
	//m_radialData.clear();
}

//获取一个仰角的PPI/SPPI 数据，或一个方位RHI数据
//cutIndex: 仰角/方位序号,从0开始计数
//fpOrigin: 开始读取的位置，SEEK_SET-文件开头，SEEK_CUR-当前位置
bool CRadarBaseDataCC1::GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData)
{
	int nCuts = 0;
	std::string scanModeName = "";
	uint16_t n = 0, m = 0, nb = 0, numaz = 0;
	uint16_t mid = 0;
	FILE* fp = 0;
	bool istat = true;
	stdCC1RadarDataHeader fileHeader;	

	float radlon, radlat, radht;
	std::string radCode, radName, radType;
	int ncode;

	errno_t err = fopen_s(&fp, m_strDataFileName.c_str(), "rb");
	if (err != 0)
		return false;

	if (fread_s(&fileHeader, SIZE_CC1_DATA_HEADER, SIZE_CC1_DATA_HEADER, 1, fp) < 1)
	{
		istat = false;
		fclose(fp);
		fp = 0;
		return false;
	}

	//各部分
	m_pRadarSite = &(fileHeader.siteInfo);
	m_pPerformParam = &(fileHeader.permformInfo);
	m_pObsvParam = &(fileHeader.obsvInfo);

	//观测要素
	CheckObsvMoments();
	bool bZ = NeedStat.bZ & m_obsvMoments.bZ;
	bool bV = NeedStat.bV & m_obsvMoments.bV;
	bool bW = NeedStat.bW & m_obsvMoments.bW;

	//站址相关
	GetRadarSitePos(radlon, radlat, radht);
	GetRadarName(radName);		EraseSpace(radName);
	GetRadarID(ncode, radCode);  EraseSpace(radCode);
	GetRadarType(radType);		EraseSpace(radType);

	//观测模式、Cuts
	nCuts = GetScanModeName(scanModeName);
	if(nCuts==0)
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
		strncpy(genScanData[mid].header.data_name, M_Z, min(DATA_TYPE_LEN, strlen(M_Z)+1));
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
		 
		//Cut start time
		stdUniformCutData newCut; 
		CalScanTimeOfCut(nc,
			newCut.header.year, newCut.header.month, newCut.header.day,
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

		std::vector<int16_t> rawRadialData;
		//Add radials in this cut for needed moments
		for (int na = 0; na < newCut.header.num_beam; na++)
		{//each radial
			
			//int16_t iel = pRawCutParam->usAngle / 10;			
			int16_t iaz = 10*na * 360. / newCut.header.num_beam;
			if (bZ)
			{
				rawRadialData.resize(GetDefaultNumBin(nc), VALUE_INVALID);

				if (fread_s(&rawRadialData[0], sizeof(int16_t)*GetDefaultNumBin(nc), sizeof(int16_t)*GetDefaultNumBin(nc), 1, fp) < 1)
				{
					istat = false;
					fclose(fp);
					fp = 0;
					return false;
				}

				mid = M_Z_ID;
				genScanData[mid].CutsData[nc].elev.push_back(iel);
				genScanData[mid].CutsData[nc].azim.push_back(iaz);

				vct_uniformRadial radialData;
				radialData.resize(genScanData[mid].CutsData[nc].header.num_gate);
				int16_t nb = 0;
				for (nb = 0; nb < min(GetValidNumBin(nc),radialData.size()); nb++)
					radialData[nb] = rawRadialData[nb]== INVALID_VALUE_CC1 ? VALUE_INVALID : int16_t(rawRadialData[nb]/10.*m_outputScale+ m_outputOffset);
				genScanData[mid].CutsData[nc].field.push_back(radialData);
			}
			else
				fseek(fp, sizeof(int16_t)*GetDefaultNumBin(nc), SEEK_CUR);

			if (bV)
			{
				rawRadialData.resize(GetDefaultNumBin(nc), VALUE_INVALID);

				if (fread(&rawRadialData[0], sizeof(int16_t)*GetDefaultNumBin(nc), 1, fp) < 1)
				{
					istat = false;
					fclose(fp);
					fp = 0;
					return false;
				}
				mid = M_V_ID;
				genScanData[mid].CutsData[nc].elev.push_back(iel);
				genScanData[mid].CutsData[nc].azim.push_back(iaz);

				vct_uniformRadial radialData;
				radialData.resize(genScanData[mid].CutsData[nc].header.num_gate);
				int16_t nb = 0;
				for (nb = 0; nb < min(GetValidNumBin(nc), radialData.size()); nb++)
					radialData[nb] = rawRadialData[nb] == INVALID_VALUE_CC1 ? VALUE_INVALID : int16_t(rawRadialData[nb] / 10.*m_outputScale + m_outputOffset);
				genScanData[mid].CutsData[nc].field.push_back(radialData);
			}
			else
				fseek(fp, sizeof(int16_t)*GetDefaultNumBin(nc), SEEK_CUR);

			if (bW)
			{
				rawRadialData.resize(GetDefaultNumBin(nc), VALUE_INVALID);

				if (fread_s(&rawRadialData[0], sizeof(int16_t)*GetDefaultNumBin(nc), sizeof(int16_t)*GetDefaultNumBin(nc), 1, fp) < 1)
				{
					istat = false;
					fclose(fp);
					fp = 0;
					return false;
				}

				mid = M_W_ID;
				genScanData[mid].CutsData[nc].elev.push_back(iel);
				genScanData[mid].CutsData[nc].azim.push_back(iaz);

				vct_uniformRadial radialData;
				radialData.resize(genScanData[mid].CutsData[nc].header.num_gate);
				int16_t nb = 0;
				for (nb = 0; nb < min(GetValidNumBin(nc), radialData.size()); nb++)
					radialData[nb] = rawRadialData[nb] == INVALID_VALUE_CC1 ? VALUE_INVALID : int16_t(rawRadialData[nb] / 10.*m_outputScale + m_outputOffset);
				genScanData[mid].CutsData[nc].field.push_back(radialData);
			}
			else
				fseek(fp, sizeof(int16_t)*GetDefaultNumBin(nc), SEEK_CUR);
		}//radials	
	}//Cuts

	fclose(fp);
	fp = 0x0;
	return(istat);
}

int16_t CRadarBaseDataCC1::GetBinValueZ(uint8_t src_code,int16_t dst_offset, int16_t dst_scale)
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
		fv = float((t*1. - 66) / 2.);
		value = int16_t(dst_offset + fv*dst_scale);
	}
	return value;
}

int16_t CRadarBaseDataCC1::GetBinValueV(uint8_t src_code, float maxV, int16_t dst_offset, int16_t dst_scale)
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
		fv = float((t*1. - 128.) / 128.)*maxV;
		value = int16_t(dst_offset + fv*dst_scale);
	}
	return value;
}

int16_t CRadarBaseDataCC1::GetBinValueW(uint8_t src_code, float maxV, int16_t dst_offset, int16_t dst_scale)
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
		fv = float((t*1.) / 256.*maxV);
		value = int16_t(dst_offset + fv*dst_scale);
	}
	return value;
}

//cutIdx--the first cut index is 0
//根据扫描开始和结束时间，计算某个cut的观测开始时间,
//计算的结果为   世界时 
void CRadarBaseDataCC1::CalScanTimeOfCut( uint16_t cutIdx, uint16_t &year, uint16_t &month, uint16_t &day, uint16_t &hour, uint16_t &minute, uint16_t &second)
{
	if (m_pObsvParam == 0)
		return;

	int32_t cjDate = 0,cSecond = 0;

	CDate sDate(m_pObsvParam->ucSYear1*100 + m_pObsvParam->ucSYear2, m_pObsvParam->ucSMonth, m_pObsvParam->ucSDay );
	int32_t sjDate = sDate.GetJulianDate();
	int32_t sSecond = m_pObsvParam->ucSHour*SEC_IN_HOR + m_pObsvParam->ucSMinute*SEC_IN_MIN + m_pObsvParam->ucSSecond;
	sSecond -= 8 * SEC_IN_HOR; //转为世界时
	if (sSecond < 0)
	{
		sSecond += HOR_IN_DAY*SEC_IN_HOR;
		sjDate -= 1;
	}
	CDate eDate(m_pObsvParam->ucEYear1*100+ m_pObsvParam->ucEYear2, m_pObsvParam->ucEMonth, m_pObsvParam->ucEDay );
	int32_t ejDate = eDate.GetJulianDate();
	int32_t eSecond = m_pObsvParam->ucEHour*SEC_IN_HOR + m_pObsvParam->ucEMinute*SEC_IN_MIN + m_pObsvParam->ucESecond ;
	eSecond -= 8 * SEC_IN_HOR; //转为世界时
	if (eSecond < 0)
	{
		eSecond += HOR_IN_DAY*SEC_IN_HOR;
		ejDate -= 1;
	}

	cjDate = sjDate;
	cSecond = sSecond;
	if (m_pObsvParam->ucScanMode > 100)
	{		
		int16_t nc = m_pObsvParam->ucScanMode - 100;
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

void CRadarBaseDataCC1::ConfirmMomentsObserved(uint16_t dataTypeCode, bool &bZ, bool &bV, bool &bW)
{
	bZ = bV = bW = false;

	if (dataTypeCode == 0x405a)
		bZ = true;
	else if (dataTypeCode == 0x406a)
		bV = true;
	else if (dataTypeCode == 0x407a)
		bW = true;
	else if (dataTypeCode == 0x408a)
		bZ = bV = bW = true;
}

int CRadarBaseDataCC1::GetScanModeName(std::string &modeName)
{
	int nCut = 0;
	modeName.assign("");

	if (m_pObsvParam->ucScanMode == 1)
	{
		nCut = 1;
		modeName.assign("RHI");
	}
	else if (m_pObsvParam->ucScanMode == 10)
	{
		nCut = 1;
		modeName.assign("PPI");
	}
	else if (m_pObsvParam->ucScanMode > 100)
	{
		nCut = m_pObsvParam->ucScanMode - 100;
		modeName.assign("VCP");
	}

	return nCut;
}


//Function:判断当前PPI中有什么数据
void CRadarBaseDataCC1::CheckObsvMoments()
{
	//Initializatition
	m_obsvMoments.Reset();
	m_obsvMoments.bZ = true;
	m_obsvMoments.bZc = true;
	m_obsvMoments.bV = true;
	m_obsvMoments.bW = true;
}

//Function:获取雷达ID
void CRadarBaseDataCC1::GetRadarID(int &ID, std::string &RadarCode)
{
	int nSiteID;
	nSiteID = atoi(m_pRadarSite->cStationNumber);
	RadarCode.assign(m_pRadarSite->cStationNumber);
}

//Function:获取雷达站名
void CRadarBaseDataCC1::GetRadarName(std::string &RadarName)
{
	RadarName.assign(m_pRadarSite->cStation);
}

void CRadarBaseDataCC1::GetRadarType(std::string &type)
{
	type.assign(m_pRadarSite->cRadarType);
}

//Function:获取雷达的位置高度信息
void CRadarBaseDataCC1::GetRadarSitePos(float &fLon, float &fLat, float &fHgt)
{
	if (m_pRadarSite == 0)
		return;

	int deg, cent;
	char strDeg[4] = "", strCent[4] = "";
	int k, m;

	//雷达站高度
	fHgt = m_pRadarSite->lHeight / 1000; //m

										  //这两个数值暂时没有填写， 改用其它方式获取雷达精度、纬度
	fLon = m_pRadarSite->lLongitudeValue / 3600/1000.;  //deg
	fLat = m_pRadarSite->lLatitudeValue / 3600 / 1000.;  //deg

	if (fLon > 70. && fLon<135. && fLat>5. && fLat < 55.)
		return;

	//否则:
	//从字符串中获取雷达站纬度 
	k = 0;
	//获取度的数值
	for (m = 0; m<strlen(m_pRadarSite->cLongitude); m++)
	{
		if (m_pRadarSite->cLongitude[m] >= '0' && m_pRadarSite->cLongitude[m] <= '9')
		{
			strDeg[k] = m_pRadarSite->cLongitude[m];
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
	for (; m<strlen(m_pRadarSite->cLongitude); m++)
	{
		if (m_pRadarSite->cLongitude[m] >= '0' && m_pRadarSite->cLongitude[m] <= '9')
		{
			strCent[k] = m_pRadarSite->cLongitude[m];
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
	for (m = 0; m<strlen(m_pRadarSite->cLatitude); m++)
	{
		if (m_pRadarSite->cLatitude[m] >= '0' && m_pRadarSite->cLatitude[m] <= '9')
		{
			strDeg[k] = m_pRadarSite->cLatitude[m];
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
	for (; m<strlen(m_pRadarSite->cLatitude); m++)
	{
		if (m_pRadarSite->cLatitude[m] >= '0' && m_pRadarSite->cLatitude[m] <= '9')
		{
			strCent[k] = m_pRadarSite->cLatitude[m];
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
uint16_t CRadarBaseDataCC1::GetNumBeam(int CutIdx)
{
	return (m_pObsvParam->cutParam[CutIdx].usRecordNumber);
}

//Function:获取径向上的库数
//获取数据结构中定义的库数
//Parameter: CutIdx, Elevation index >=0
uint16_t CRadarBaseDataCC1::GetDefaultNumBin(int cutidx)
{
	if (m_pObsvParam == 0)
		return 0;

	return uint16_t(m_pObsvParam->cutParam[cutidx].usBinNumber);
}

//获取扫描实际用到的库数
uint16_t CRadarBaseDataCC1::GetValidNumBin(int CutIdx)
{
	if (m_pObsvParam == 0)
		return 0; 

	uint16_t num = GetDefaultNumBin(CutIdx);

	int32_t maxL = m_pObsvParam->cutParam[CutIdx].usMaxL*10;  //m
	int32_t binW = m_pObsvParam->cutParam[CutIdx].usBindWidth * 2.;//km

	num = uint16_t(maxL*1. / binW + 0.5);
	if (num > GetDefaultNumBin(CutIdx))
		num = GetDefaultNumBin(CutIdx);
	return num;
}

//Function:获取径向上的采样分辨率
//Parameter: CutIdx, Elevation index >=0
uint16_t CRadarBaseDataCC1::GetBinWidth(int CutIdx)//Get Surviliance mode bin size
{
	if (m_pObsvParam == 0)
		return 0;
	return uint16_t(m_pObsvParam->cutParam[CutIdx].usBindWidth * 2); //m
}

float CRadarBaseDataCC1::GetMaxVel(int CutIdx)
{
	return float(m_pObsvParam->cutParam[CutIdx].usMaxV*0.01);//Max. velocity (m/s)
}

float CRadarBaseDataCC1::GetElevation(int CutIdx)
{
	return float(m_pObsvParam->cutParam[CutIdx].usAngle / 100.);
}