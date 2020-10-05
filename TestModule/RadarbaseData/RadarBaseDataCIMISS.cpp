#include "RadarBaseDataFormatCIMISS.h"

#include "../CommVar.h"
#include "../CommFunctions.h"
#include "julian_date.h"
#include "RadarBaseDataMomentTypeDef.h"
#include "RadarBaseDataInputDef.h"
#include "RadarBaseDataCIMISS.h"

stdCimissMomentCuts::stdCimissMomentCuts()
{
	Needed = false;
	Available = false;
	CutsData.clear();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//
CCIMISSRadarDataCls::CCIMISSRadarDataCls(std::string strInFileName, int16_t dstOffset, int16_t dstScale)//:CRadarBaseDataWsr98D(strInFileName, dstOffset, dstScale)
{
	m_strDataFileName.assign(strInFileName);
	m_outputOffset = dstOffset;
	m_outputScale = dstScale;

	m_pDataFile = 0x0;
	memset(&m_CommonBlock, sizeof(m_CommonBlock), 0x0);
	//m_MomentsData.clear();

	m_bdRadarInfo.Reset();
	
	//Read Common  block from file
	GetCommonBlock();
}

CCIMISSRadarDataCls::~CCIMISSRadarDataCls()
{
	if(m_pDataFile != 0x0)
	{	
		fclose(m_pDataFile);	
		m_pDataFile = 0x0;	
	}

	//ClearMomentsCuts();
}
//
void CCIMISSRadarDataCls::GetCommonBlock()
{
	if (m_pDataFile == 0x0)
	{
		if (0 != fopen_s(&m_pDataFile, m_strDataFileName.c_str(), "rb"))
			return;
	}
	fseek(m_pDataFile, 0, SEEK_SET);//文件开始处

	int header_size = sizeof(m_CommonBlock.GenricHeader)+ sizeof(m_CommonBlock.SiteConfig)+ sizeof(m_CommonBlock.ScanTaskConfig);
	fread(&(m_CommonBlock.GenricHeader), header_size, 1, m_pDataFile);

	int cut_size = sizeof(stdCimissCutConfig);
	m_CommonBlock.CutConfig.clear();
	for (uint32_t i = 0; i < m_CommonBlock.ScanTaskConfig.CutNumber; i++)
	{
		stdCimissCutConfig aCut;
		fread(&aCut, cut_size, 1, m_pDataFile);
		m_CommonBlock.CutConfig.push_back(aCut);
	}
}

//获取一个仰角的PPI/SPPI 数据，或一个方位RHI数据
//cutIndex: 仰角/方位序号,从0开始计数
//fpOrigin: 开始读取的位置，SEEK_SET-文件开头，SEEK_CUR-当前位置
bool CCIMISSRadarDataCls::GetObsvData(stdMomentFlag NeedStat, map_GeneralScanDatas &genScanData)
{
	bool istat = false;
	uint16_t n = 0, m = 0, nb = 0, numaz = 0;	
	uint16_t srcCutId = 0, uniCutId = 0;

	uint16_t numbin ;
	int32_t srcOff, srcScale,srcType;
	uint16_t midlocal = 0, miduniform = 0;

	std::vector<uint8_t> srcRadialData1;
	std::vector<int16_t> srcRadialData2;
	//--------------------------------------------------------------------//

	if (m_pDataFile == 0) 
		GetCommonBlock();//此前未读过文件，从头开始读取数据

	if (m_pDataFile == 0)
		return false; //读取失败.

	ConfirmMomentsNeeded( NeedStat);
	ConfirmMomentsObserved();

	//站址相关
	GetRadarSite(m_bdRadarInfo.strRadarName, m_bdRadarInfo.strRadarCode, m_bdRadarInfo.strRadarType, 
		m_bdRadarInfo.fRadarLon, m_bdRadarInfo.fRadarLat, m_bdRadarInfo.fRadarHt);
	int nCuts = GetScanModeName(m_bdRadarInfo.strScanType);
	
	if (nCuts == 0)
		return false;

	//A radial header
	bool bGoodScan = true;
	bool bGoodCut = false;
	while (bGoodScan && !feof(m_pDataFile))
	{		
		stdCimissGenRadialHeader genRadialHeader;//统一径向头
		if (fread(&genRadialHeader, SIZE_GEN_RADIAL_HEADER, 1, m_pDataFile) <1)
			break;

		//az and el of this radial
		int16_t iel = int16_t(genRadialHeader.Elevation * 10); //0.1deg
		int16_t iaz = int16_t(genRadialHeader.Azimuth * 10);

		if (IsCutStart(genRadialHeader.RadialState))
		{
			bGoodCut = true; //标记一个正常的Cut

			ConfirmMomentsObserved(srcCutId);//确定当前CUT的观测要素

			//Cut start time
			stdUniformCutData newCut;			
			GetRadialDateTime(genRadialHeader, newCut.header.year, newCut.header.day, newCut.header.month, newCut.header.hour, newCut.header.minute, newCut.header.second);

			std::map<uint16_t, bool>::iterator itm;
			for (itm = m_bMomentNeeded.begin(); itm != m_bMomentNeeded.end(); itm++)
			{
				if(itm->second && m_bMomentObsved[itm->first])
				{ 
					newCut.header.gateWidth = GetBinWidth(itm->first, srcCutId);   //0.1m
					newCut.header.nyq_vel = int16_t(GetMaxVel(srcCutId) * 10);
					newCut.header.elev_angle = int16_t(GetCutAngle(srcCutId) * 10);

					MomentTypeLocalToUniform(itm->first, miduniform);				
					//Add a cut in needed moments vScan 
					genScanData[miduniform].CutsData.push_back(newCut);
					genScanData[miduniform].Available = true;
					genScanData[miduniform].Needed = true;
				}
			}
		}
		if (!bGoodCut)
			continue;

		//Number of moments ,read each moment and check if it is needed
		for (uint32_t m1 = 0; m1 < genRadialHeader.MomentNumber; m1++)
		{//each moment in a radial
			stdCimissMomentRadialHeader srcRadialHeader;
			if (fread(&srcRadialHeader, SIZE_MOM_RADIAL_HEADER, 1, m_pDataFile) < 1)
			{
				bGoodScan = false;
				break;
			}

			numbin = GetNumberOfBins(srcRadialHeader);// srcRadialHeader.AllBinLength / srcRadialHeader.OneBinLength;
			srcOff = srcRadialHeader.Offset;
			srcScale = srcRadialHeader.Scale;
			srcType = srcRadialHeader.DataType;
			
			if (!m_bMomentObsved[srcType] || !m_bMomentNeeded[srcType])
			{
				fseek(m_pDataFile, srcRadialHeader.AllBinLength, SEEK_CUR);
				continue; //this moment is not needed skip
			}
			MomentTypeLocalToUniform(srcType, miduniform);//转为统一格式的 moment id
		
			//azimuth and elevation
			uniCutId = uint16_t(genScanData[miduniform].CutsData.size() - 1);
			if (uniCutId >= 0)
			{
				genScanData[miduniform].CutsData[uniCutId].elev.push_back(iel);
				genScanData[miduniform].CutsData[uniCutId].azim.push_back(iaz);
			}
			//radial data
			if (srcRadialHeader.OneBinLength == 1)
			{
				srcRadialData1.resize(numbin);
				if (fread(&(srcRadialData1[0]), srcRadialHeader.AllBinLength, 1, m_pDataFile) < 1)
				{
					bGoodScan = false;
					break;
				}
							
				vct_uniformRadial radialData;				
				radialData.resize(numbin, VALUE_INVALID);
				int16_t nb = 0;
				for (nb = 0; nb < numbin; nb++)
				{
					if (srcRadialData1[nb] > 1)
						radialData[nb] = int16_t((srcRadialData1[nb] - srcOff)*1. / srcScale *m_outputScale + m_outputOffset);
				}
				genScanData[miduniform].CutsData[uniCutId].field.push_back(radialData);
			}
			else if (srcRadialHeader.OneBinLength == 2)
			{
				srcRadialData2.resize(numbin);
				if (fread(&(srcRadialData2[0]), srcRadialHeader.AllBinLength, 1, m_pDataFile) < 1)
				{
					bGoodScan = false;
					break;
				}

				vct_uniformRadial radialData;
				radialData.resize(numbin, VALUE_INVALID);
				int16_t nb = 0;
				for (nb = 0; nb <numbin; nb++)
					radialData[nb] = int16_t((srcRadialData2[nb] - srcOff)*1. / srcScale *m_outputScale + m_outputOffset);
				genScanData[miduniform].CutsData[uniCutId].field.push_back(radialData);
			}
		}//each moment in a radial

		if (!bGoodScan)
			break;

		//Do something at end of a cut
		if (IsCutEnd(genRadialHeader.RadialState))
		{				
			bGoodCut = false;
			srcCutId++;  //index of elevation cuts
		}
		if (IsScanEnd(genRadialHeader.RadialState))
		{			
			break;
		}
	};// get all radials in file
	fclose(m_pDataFile);
	m_pDataFile = 0x0;

	//Additional paramers, such as site information....
	SetRadarParamInUniformData(genScanData);

	istat = true;
	return istat;
}

void CCIMISSRadarDataCls::SetRadarParamInUniformData(map_GeneralScanDatas &genScanData)
{
	map_GeneralScanDatas::iterator itm;
	for (itm = genScanData.begin(); itm != genScanData.end(); itm++)
	{
		if (!itm->second.Available || !itm->second.Needed)
			continue;
		int nCuts = itm->second.CutsData.size();
		if (nCuts < 0)
			continue;
		
		std::string strTemp = "";
		int mid = 0;
		std::string strDataNme = "";

		if (itm->first == M_Z_ID)	strDataNme = M_Z;
		else if (itm->first == M_Zc_ID)	strDataNme = M_Zc;
		else if (itm->first == M_V_ID)	strDataNme = M_V;
		else if (itm->first == M_Vc_ID)	strDataNme = M_Vc;
		else if (itm->first == M_W_ID)	strDataNme = M_W;
		else if (itm->first == M_Wc_ID)	strDataNme = M_Wc;
		else if (itm->first == M_ZDR_ID)	strDataNme = M_ZDR;
		else if (itm->first == M_ZDRc_ID)	strDataNme = M_ZDRc;
		else if (itm->first == M_PDP_ID)	strDataNme = M_PDP;
		else if (itm->first == M_KDP_ID)	strDataNme = M_KDP;
		else if (itm->first == M_RHV_ID)	strDataNme = M_RHV;

		//additional parameters of this scan

		strTemp = strDataNme.substr(0, min(strDataNme.length(), DATA_TYPE_LEN - 1));
		strncpy(itm->second.header.data_name, M_Z, min(DATA_TYPE_LEN, strlen(M_Z) + 1));

		strTemp = m_bdRadarInfo.strScanType.substr(0, min(m_bdRadarInfo.strScanType.length(), SCAN_TYPE_LEN - 1));
		strncpy(itm->second.header.obsv_type, strTemp.c_str(), strTemp.length() + 1);

		strTemp = m_bdRadarInfo.strRadarName.substr(0, min(m_bdRadarInfo.strRadarName.length(), SITE_NAME_LEN - 1));
		strncpy(itm->second.header.radar_name, strTemp.c_str(), strTemp.length() + 1);

		strTemp = m_bdRadarInfo.strRadarCode.substr(0, min(m_bdRadarInfo.strRadarCode.length(), SITE_CODE_LEN - 1));
		strncpy(itm->second.header.radar_code, strTemp.c_str(), strTemp.length() + 1);

		strTemp = m_bdRadarInfo.strRadarType.substr(0, min(m_bdRadarInfo.strRadarType.length(), SITE_TYPE_LEN - 1));
		strncpy(itm->second.header.radar_type, strTemp.c_str(), strTemp.length() + 1);

		itm->second.header.radlon = uint32_t(m_bdRadarInfo.fRadarLon * 1000);
		itm->second.header.radlat = uint32_t(m_bdRadarInfo.fRadarLat * 1000);
		itm->second.header.radhgt = uint16_t(m_bdRadarInfo.fRadarHt);

		itm->second.header.factor = m_outputScale;
		itm->second.header.offset = m_outputOffset;
		itm->second.header.ndata = VALUE_INVALID;

		//additional parameters of all cuts
		for (int n = 0; n < nCuts; n++)
		{
			strTemp = strDataNme.substr(0, min(strDataNme.length(), DATA_TYPE_LEN - 1));
			strncpy(itm->second.CutsData[n].header.data_name, strTemp.c_str(), strTemp.length() + 1);

			strTemp = m_bdRadarInfo.strScanType.substr(0,min(m_bdRadarInfo.strScanType.length(), SCAN_TYPE_LEN-1));
			strncpy(itm->second.CutsData[n].header.obsv_type, strTemp.c_str(), strTemp.length() + 1);

			strTemp = m_bdRadarInfo.strRadarName.substr(0, min(m_bdRadarInfo.strRadarName.length(), SITE_NAME_LEN - 1));
			strncpy(itm->second.CutsData[n].header.radar_name, strTemp.c_str(), strTemp.length() + 1);

			strTemp = m_bdRadarInfo.strRadarCode.substr(0, min(m_bdRadarInfo.strRadarCode.length(), SITE_CODE_LEN - 1));
			strncpy(itm->second.CutsData[n].header.radar_code, strTemp.c_str(), strTemp.length() + 1);

			strTemp = m_bdRadarInfo.strRadarType.substr(0, min(m_bdRadarInfo.strRadarType.length(), SITE_TYPE_LEN - 1));
			strncpy(itm->second.CutsData[n].header.radar_type, strTemp.c_str(), strTemp.length() + 1);

			itm->second.CutsData[n].header.num_beam = uint16_t(itm->second.CutsData[n].azim.size());//
			if (itm->second.CutsData[n].field.size()>0)
				itm->second.CutsData[n].header.num_gate = uint16_t(itm->second.CutsData[n].field[0].size());
		}		
	}
}

//
bool CCIMISSRadarDataCls::IsPPI(int16_t &nCuts)//PPI(S)
{
	bool bFlag = false;
	nCuts = 0;

	switch (m_CommonBlock.ScanTaskConfig.ScanType)
	{
	case 0://体扫
		bFlag = true;
		nCuts = m_CommonBlock.ScanTaskConfig.CutNumber;
		break;
	case 1://单PPI
		bFlag = true;
		nCuts = m_CommonBlock.ScanTaskConfig.CutNumber;
		break;
	default:
		break;
	}
	return bFlag;
}

//
bool CCIMISSRadarDataCls::IsSPPI(int16_t &nCuts)//Sector PPI(S)
{
	bool bFlag = false;
	nCuts = 0;

	switch (m_CommonBlock.ScanTaskConfig.ScanType)
	{
	case 3://单层扇扫
		bFlag = true;
		nCuts = m_CommonBlock.ScanTaskConfig.CutNumber;
		break;
	case 4://扇体扫
		bFlag = true;
		nCuts = m_CommonBlock.ScanTaskConfig.CutNumber;
		break;
	default:
		break;
	}
	return bFlag;
}

//
bool CCIMISSRadarDataCls::IsRHI(int16_t &nAzs)
{
	bool bFlag = false;
	nAzs = 0;

	switch (m_CommonBlock.ScanTaskConfig.ScanType)
	{
	case 2://单层RHI
		bFlag = true;
		nAzs = m_CommonBlock.ScanTaskConfig.CutNumber;
		break;
	case 5://多层RHI
		bFlag = true;
		nAzs = m_CommonBlock.ScanTaskConfig.CutNumber;
		break;
	default:
		break;
	}
	return bFlag;
}

//提取径向中某个距离库的值，输出到value
//nb: index of bin in a radial, counted from 0
float CCIMISSRadarDataCls::GetBinValue(uint16_t code, int32_t offset, int32_t scale, float ndata)
{
	float value=0;
	int32_t t = 0;	
	t = t | code;
	if (t == RADAR_NODATA || t == RADAR_FOLDED || t == RADAR_NOECHO)
		value = ndata;
	else
		value = float((t*1. - offset) / scale);
	return value;
}

float CCIMISSRadarDataCls::GetBinValue(uint8_t code, int32_t offset, int32_t scale, float ndata)
{	
	float value = 0;
	int32_t t = 0;	
	t = t | code;
	if (t <= 1)
		value = ndata;
	else
		value = float((t*1. - offset) / scale);
	return value;
}

void CCIMISSRadarDataCls::GetRadialDateTime(stdCimissGenRadialHeader radialHeader, uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second)
{
	year = day = month = hour = minute = second = 0;

	TransDateTime(radialHeader.Seconds, year, day, month, hour, minute, second);
}

void CCIMISSRadarDataCls::GetScanStartDateTime(uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second)
{	
	if (m_CommonBlock.CutConfig.size() == 0)
	{
		year = day = month = hour = minute = second = 0;	
		return;
	}	
	TransDateTime(m_CommonBlock.ScanTaskConfig.ScanStartTime, year, day, month, hour, minute, second);
}

uint16_t CCIMISSRadarDataCls:: GetNumberOfBins(stdCimissMomentRadialHeader radialMomentHeader)
{
	return radialMomentHeader.AllBinLength / radialMomentHeader.OneBinLength;
}

void CCIMISSRadarDataCls::GetRadarSite(std::string &sname, std::string &scode, std::string &stype, float &lon, float &lat, float &ht)
{
	//radar type
	switch (m_CommonBlock.SiteConfig.RadarType)
	{
	case 1:
		stype.assign("SA");
		break;
	case 2:
		stype.assign("SB");
		break;
	case 3:
		stype.assign("SC");
		break;
	case 33:
		stype.assign("CA");
		break;
	case 34:
		stype.assign("CB");
		break;
	case 35:
		stype.assign("CC");
		break;
	case 36:
		stype.assign("CCJ");
	case 37:
		stype.assign("CD");
		break;
	case 65:
		stype.assign("XA");
		break;
	default:
		stype.assign("");
		break;
	}
	//code
	scode.assign(m_CommonBlock.SiteConfig.SiteCode);
	//name
	sname.assign(m_CommonBlock.SiteConfig.SiteName);
	//lon  lat ht
	lon = m_CommonBlock.SiteConfig.Longitude;
	lat = m_CommonBlock.SiteConfig.Latitude;
	ht = (float)m_CommonBlock.SiteConfig.RadarHeight;	

	//去掉头尾空格
	EraseSpace(sname);
	EraseSpace(scode);
	EraseSpace(stype);
}

stdCimissScanTaskConfig* CCIMISSRadarDataCls::GetScanConfig()
{
	return &m_CommonBlock.ScanTaskConfig;
}

stdCimissCutConfig* CCIMISSRadarDataCls::GetCutConfig(uint16_t cutid)
{
	return &(m_CommonBlock.CutConfig[cutid]);
}

uint16_t CCIMISSRadarDataCls::GetBinWidth(uint16_t momid,uint16_t cutid) //meters of each bin
{
	if (momid == BIT_Index_V || momid == BIT_Index_Vc || momid == BIT_Index_W || momid == BIT_Index_Wc)
		return GetCutConfig(cutid)->DopplerResolution;
	else
		return GetCutConfig(cutid)->LogResolution;
}

void CCIMISSRadarDataCls::GetBeamWidth(float &hbw, float &vbw) //dugree horizental width
{
	hbw = 0.;
	vbw = 0;
	
	hbw = m_CommonBlock.SiteConfig.BeamWidthH;
	vbw = m_CommonBlock.SiteConfig.BeamWidthV;
}

//
int16_t CCIMISSRadarDataCls::MomentIdByType(uint16_t srcMomentType)
{
	int16_t id = -1;
	for (int i = 0; i < cMaxMoment; i++)
	{
		if (CIMISS_M_TYPE_ID[i] != srcMomentType)
			continue;

		id = i;
		break;
	}
	return id;
}
//Value of Id is in CIMISS_M_ID
uint16_t CCIMISSRadarDataCls::GetMomentTypeId(const char* szMomentName)
{ 
	uint16_t id = 0;
	std::string strMoment = "";
	strMoment.assign(szMomentName);
	if (strMoment.find(M_Zc) != std::string::npos && strMoment.length() == strlen(M_Zc))
	{
		id = CIMISS_M_ID_Zc;//
	}
	else if (strMoment.find(M_Vc) != std::string::npos && strMoment.length() == strlen(M_Vc))
	{
		id=CIMISS_M_ID_Vc;
	}
	else if (strMoment.find(M_Wc) != std::string::npos && strMoment.length() == strlen(M_Wc))
	{
		id=CIMISS_M_ID_Wc;
	}
	else if (strMoment.find(M_ZDRc) != std::string::npos && strMoment.length() == strlen(M_ZDRc))
	{
		id=CIMISS_M_ID_ZDRc;
	}
	else if (strMoment.find(M_Z) != std::string::npos && strMoment.length() == strlen(M_Z))
	{
		id = CIMISS_M_ID_Z;//
	}
	else if (strMoment.find(M_V) != std::string::npos && strMoment.length() == strlen(M_V))
	{
		id = CIMISS_M_ID_V;
	}
	else if (strMoment.find(M_W) != std::string::npos && strMoment.length() == strlen(M_W))
	{
		id = CIMISS_M_ID_W;
	}
	else if (strMoment.find(M_ZDR) != std::string::npos && strMoment.length() == strlen(M_ZDR))
	{
		id = CIMISS_M_ID_ZDR;
	}
	else if (strMoment.find(M_PDP) != std::string::npos && strMoment.length() == strlen(M_PDP))
	{
		id = CIMISS_M_ID_PDP;
	}
	else if (strMoment.find(M_KDP) != std::string::npos && strMoment.length() == strlen(M_KDP))
	{
		id = CIMISS_M_ID_KDP;
	}
	else if (strMoment.find(M_RHV) != std::string::npos && strMoment.length() == strlen(M_RHV))
	{
		id = CIMISS_M_ID_CC;
	}
	else if (strMoment.find(M_LDR) != std::string::npos && strMoment.length() == strlen(M_LDR))
	{
		id = CIMISS_M_ID_LDR;
	}

	return MomentBitIndex(id);
}

void CCIMISSRadarDataCls::TransDateTime(uint32_t JulSeconds, uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second)
{
	uint16_t JulDate;
	uint32_t Seconds;

	JulDate = JulSeconds / SEC_IN_DAY;
	Seconds = JulSeconds - JulDate*SEC_IN_DAY;

	TransDate(JulDate, year, day, month);
	TransTime(Seconds, hour, minute, second);
}

void CCIMISSRadarDataCls::TransDate(uint16_t JulDate, uint16_t &year, uint16_t &day, uint16_t &month)
{
	CDate date(JulDate);
	// Get  start date
	year = date.GetYear();
	month = date.GetMonth();
	day = date.GetDate();
}

void CCIMISSRadarDataCls::TransTime(uint32_t Seconds, uint16_t &hour, uint16_t &minute, uint16_t &second)
{
	hour = Seconds / SEC_IN_HOR;
	minute = (Seconds - hour *SEC_IN_HOR) / SEC_IN_MIN;
	second = Seconds - hour * SEC_IN_HOR -	minute * SEC_IN_MIN;
}

int16_t CCIMISSRadarDataCls::MomentBitIndex(short m)
{
	if (m < cMaxMoment)
		return CIMISS_M_TYPE_ID[m];
	else
		return -1;
}

bool CCIMISSRadarDataCls::IsCutStart(uint16_t RadialStat)
{
	if (RadialStat == EL_FIRST || RadialStat == VS_FIRST || RadialStat == RHI_FIRST)
		return true;
	else
		return false;
}

bool CCIMISSRadarDataCls::IsCutEnd(uint16_t RadialStat)
{
	if (RadialStat == EL_LAST || RadialStat == VS_LAST || RadialStat == RHI_LAST)
		return true;
	else
		return false;
}


bool CCIMISSRadarDataCls::IsScanStart(uint16_t RadialStat)
{
	if (RadialStat == VS_FIRST || RadialStat == RHI_FIRST)
		return true;
	else
		return false;
}

bool CCIMISSRadarDataCls::IsScanEnd(uint16_t RadialStat)
{
	if ( RadialStat == VS_LAST || RadialStat == RHI_LAST)
		return true;
	else
		return false;
}

float CCIMISSRadarDataCls::GetMaxVel(uint16_t  cutIndex)
{
	return m_CommonBlock.CutConfig[cutIndex].NyquistSpeed;
}

float CCIMISSRadarDataCls::GetCutAngle(uint16_t  cutIndex)
{
	float fAngle = 0;
	if (m_CommonBlock.ScanTaskConfig.ScanType == 2 || m_CommonBlock.ScanTaskConfig.ScanType == 5)//RHI
		fAngle = m_CommonBlock.CutConfig[cutIndex].Azimuth;
	else 
		fAngle = m_CommonBlock.CutConfig[cutIndex].Elevation;

	return fAngle;
}

//Function:判断当前PPI中有什么数据
void CCIMISSRadarDataCls::ConfirmMomentsObserved()
{
	//Initializatition
	m_obsvMoments.Reset();

	for (uint32_t n = 0; n < m_CommonBlock.ScanTaskConfig.CutNumber; n++)
	{//需要且可以观测的要素数
		ConfirmMomentsObserved(n);
	}

	//顺序不能变
	for (short i = 0; i < cMaxMoment; i++)
	{
		if (!m_bMomentObsved[CIMISS_M_TYPE_ID[i]])
			continue;
		
		switch (i)
		{
		case (CIMISS_M_ID_Z) :
			m_obsvMoments.bZ = true;
			break;
		case (CIMISS_M_ID_Zc):
			m_obsvMoments.bZc = true;
			break;
		case (CIMISS_M_ID_V) :
			m_obsvMoments.bV = true;
			break;
		case (CIMISS_M_ID_Vc) :
			m_obsvMoments.bVc = true;
			break;
		case (CIMISS_M_ID_W) :
			m_obsvMoments.bW = true;
			break;
		case (CIMISS_M_ID_Wc) :
			m_obsvMoments.bWc = true;
			break;
		case (CIMISS_M_ID_ZDR) :
			m_obsvMoments.bZDR = true;
			break;
		case (CIMISS_M_ID_ZDRc) :
			m_obsvMoments.bZDRc = true;
			break;
		case (CIMISS_M_ID_PDP) :
			m_obsvMoments.bPDP = true;
			break;
		case (CIMISS_M_ID_KDP) :
			m_obsvMoments.bKDP = true;
			break;
		case (CIMISS_M_ID_CC) :
			m_obsvMoments.bRHV = true;
			break;
		case (CIMISS_M_ID_LDR) :
			m_obsvMoments.bLDR = true;
			break;
		default:
			break;
		};
	}
}

//确定需要读出且能从数据中获得的要素
void CCIMISSRadarDataCls::ConfirmMomentsObserved(uint16_t cutIndex) //仰角/方位序号 从0开始计数
{
	if (m_CommonBlock.ScanTaskConfig.CutNumber <= cutIndex)
		return;
	for (int i = 0; i < cMaxMoment; i++)
	{
		int nid = CIMISS_M_TYPE_ID[i];
		m_bMomentObsved[nid] = false;
		if ((CIMISS_M_BIT_ID[i] & m_CommonBlock.CutConfig[cutIndex].MomentMask) != 0)
			m_bMomentObsved[nid] = true;
	}
}

//确定需要读出且能从数据中获得的要素
void CCIMISSRadarDataCls::ConfirmMomentsNeeded(stdMomentFlag NeedStat) //仰角/方位序号 从0开始计数
{
	for (int i = 0; i < cMaxMoment; i++)
		m_bMomentNeeded[CIMISS_M_TYPE_ID[i]] = false;

	//顺序不能变
	uint16_t nid=0;
	for (short i = 0; i < cMaxMoment; i++)
	{
		switch (i)
		{
		case (CIMISS_M_ID_Z) :
			nid =CIMISS_M_ID_Z;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bZ;
			break;
		case (CIMISS_M_ID_Zc) :
			nid = CIMISS_M_ID_Zc;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bZc;
			break;
		case (CIMISS_M_ID_V) :
			nid = CIMISS_M_ID_V;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bV;
			break;
		case (CIMISS_M_ID_Vc) :
			nid = CIMISS_M_ID_Vc;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bVc;
			break;
		case (CIMISS_M_ID_W) :
			nid = CIMISS_M_ID_W;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bW;
			break;
		case (CIMISS_M_ID_Wc) :
			nid = CIMISS_M_ID_Wc;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bWc;
			break;
		case (CIMISS_M_ID_ZDR) : 
			nid = CIMISS_M_ID_ZDR;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bZDR;
			break;
		case (CIMISS_M_ID_ZDRc) :
			nid = CIMISS_M_ID_ZDRc;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bZDRc;
			break;
		case (CIMISS_M_ID_PDP) :
			nid = CIMISS_M_ID_PDP;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bPDP;
			break;
		case (CIMISS_M_ID_KDP) :
			nid = CIMISS_M_ID_KDP;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bKDP;
			break;
		case (CIMISS_M_ID_CC) : 
			nid = CIMISS_M_ID_CC;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bRHV;
			break;
		case (CIMISS_M_ID_LDR) :
			nid = CIMISS_M_ID_LDR;
			m_bMomentNeeded[CIMISS_M_TYPE_ID[nid]] = NeedStat.bLDR = true;
			break;
		default:
			break;
		};
	}
}
int CCIMISSRadarDataCls::GetScanModeName(std::string &modeName)
{
	int16_t nCut=0;
	modeName.assign(m_CommonBlock.ScanTaskConfig.Name);
	if (!IsPPI(nCut))
		if (!IsRHI(nCut))
			IsSPPI(nCut);

	return (int(nCut));
}

void CCIMISSRadarDataCls::MomentIdLocalToUniform(uint16_t localMomentId, uint16_t &uniformMoemntId)
{
	switch (localMomentId)
	{
	case CIMISS_M_ID_Z:
		uniformMoemntId = M_Z_ID;
		break;
	case CIMISS_M_ID_Zc:
		uniformMoemntId = M_Zc_ID;
		break;
	case CIMISS_M_ID_V:
		uniformMoemntId = M_V_ID;
		break;
	case CIMISS_M_ID_Vc:
		uniformMoemntId = M_Vc_ID;
		break;
	case CIMISS_M_ID_W:
		uniformMoemntId = M_W_ID;
		break;
	case CIMISS_M_ID_Wc:
		uniformMoemntId = M_Wc_ID;
		break;
	case CIMISS_M_ID_ZDR:
		uniformMoemntId = M_ZDR_ID;
		break;
	case CIMISS_M_ID_ZDRc:
		uniformMoemntId = M_ZDRc_ID;
		break;
	case CIMISS_M_ID_LDR:
		uniformMoemntId = M_LDR_ID;
		break;
	case CIMISS_M_ID_CC:
		uniformMoemntId = M_RHV_ID;
		break;
	case CIMISS_M_ID_PDP:
		uniformMoemntId = M_PDP_ID;
		break;
	case CIMISS_M_ID_KDP:
		uniformMoemntId = M_KDP_ID;
		break;
	default:
		uniformMoemntId = 0;
		break;
	};
}

void CCIMISSRadarDataCls::MomentTypeLocalToUniform(uint16_t localMomentType, uint16_t &uniformMoemntId)
{
	int i = 0;
	for ( i = 0; i < cMaxMoment; i++)
	{
		if (localMomentType == CIMISS_M_TYPE_ID[i])
		{
			switch (i)
			{
			case CIMISS_M_ID_Z:
				uniformMoemntId = M_Z_ID;
				break;
			case CIMISS_M_ID_Zc:
				uniformMoemntId = M_Zc_ID;
				break;
			case CIMISS_M_ID_V:
				uniformMoemntId = M_V_ID;
				break;
			case CIMISS_M_ID_Vc:
				uniformMoemntId = M_Vc_ID;
				break;
			case CIMISS_M_ID_W:
				uniformMoemntId = M_W_ID;
				break;
			case CIMISS_M_ID_Wc:
				uniformMoemntId = M_Wc_ID;
				break;
			case CIMISS_M_ID_ZDR:
				uniformMoemntId = M_ZDR_ID;
				break;
			case CIMISS_M_ID_ZDRc:
				uniformMoemntId = M_ZDRc_ID;
				break;
			case CIMISS_M_ID_LDR:
				uniformMoemntId = M_LDR_ID;
				break;
			case CIMISS_M_ID_CC:
				uniformMoemntId = M_RHV_ID;
				break;
			case CIMISS_M_ID_PDP:
				uniformMoemntId = M_PDP_ID;
				break;
			case CIMISS_M_ID_KDP:
				uniformMoemntId = M_KDP_ID;
				break;
			default:
				uniformMoemntId = 0;
				break;
			};
			break;
		}
	}
}

void CCIMISSRadarDataCls::MomentIdUniformToLocal(uint16_t uniformMoemntId, uint16_t &localMomentId)
{
	switch (uniformMoemntId)
	{
	case M_Z_ID:
		localMomentId = CIMISS_M_ID_Z;
		break;
	case M_Zc_ID:
		localMomentId = CIMISS_M_ID_Zc;
		break;
	case M_V_ID:
		localMomentId = CIMISS_M_ID_V;
		break;
	case M_Vc_ID:
		localMomentId = CIMISS_M_ID_Vc;
		break;
	case  M_W_ID:
		localMomentId =CIMISS_M_ID_W;
		break;
	case M_Wc_ID:
		localMomentId = CIMISS_M_ID_Wc;
		break;
	case M_ZDR_ID:
		localMomentId = CIMISS_M_ID_ZDR;
		break;
	case M_ZDRc_ID:
		localMomentId = CIMISS_M_ID_ZDRc;
		break;
	case M_LDR_ID:
		localMomentId = CIMISS_M_ID_LDR;
		break;
	case M_RHV_ID:
		localMomentId = CIMISS_M_ID_CC;
		break;
	case M_PDP_ID:
		localMomentId = CIMISS_M_ID_PDP;
		break;
	case M_KDP_ID:
		localMomentId = CIMISS_M_ID_KDP;
		break;
	default:
		localMomentId = 0;
		break;
	};
}