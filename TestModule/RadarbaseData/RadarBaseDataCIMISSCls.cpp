#include "stdafx.h"
#include "RadarBaseDataCIMISSCls.h"
#include "RadarBaseDataMomentTypeDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////
//
stdCimissMomentCuts::stdCimissMomentCuts()
{
	Needed = false;
	Available = false;
	CutsData.clear();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//
CCIMISSRadarDataCls::CCIMISSRadarDataCls()
{
	m_strDataFileName.assign("");
	m_pDataFile = 0x0;
	m_MomentsData.clear();
	memset(&m_CommonBlock, sizeof(m_CommonBlock), 0x0);		
}

CCIMISSRadarDataCls::CCIMISSRadarDataCls(string strInFileName)
{
	m_strDataFileName.assign(strInFileName);
	m_pDataFile = 0x0;
	memset(&m_CommonBlock, sizeof(m_CommonBlock), 0x0);
	m_MomentsData.clear();
	
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

	ClearMomentsCuts();
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
void CCIMISSRadarDataCls::GetObsvData(const bool &bZ, const bool &bV, const bool &bW, const bool &bZDR, 
	const bool &bZc, const bool &bVc, const bool &bWc, const bool &bZDRc,
	const bool &bLDR, const bool &bCC, const bool &bPHIDP, const bool &bKDP, const bool &bHCL, const bool &bDBT,
	const bool &bSQI, const bool &bCPA, const bool &bCP, const bool &bCF, const bool &bSNRH)
{
	map<uint16_t,vct_momentCut> aCutMoments;
	//vct_momentCut::iterator itr;

	uint32_t n=0, m=0;	
	//
	if (m_pDataFile == 0) 
		GetCommonBlock();//此前未读过文件，从头开始读取数据

	if (m_pDataFile == 0)
		return; //读取失败

	if (m_CommonBlock.GenricHeader.GenericType != 1)
		return; //非基数据

	ConfirmMomentsNeeded(bZ, bZc, bV, bVc, bW, bWc, bZDR, bZDRc,
		bLDR, bCC, bPHIDP, bKDP, bHCL, bDBT, bSQI, bCPA, bCP, bCF, bSNRH);
	
	//Confirm available and needed moments
	short numdata = 0;
	for (n = 0; n < m_CommonBlock.ScanTaskConfig.CutNumber; n++)
	{//需要且可以观测的要素数
		short t = ConfirmMomentsToGet(n); 
			if (t > numdata)
				numdata = t;
	}
	if (numdata == 0)
		return;		

	//ini cuts of each  moment	
	map<uint16_t, stdCimissMomentCuts>::iterator itrm;
	for (itrm = m_MomentsData.begin(); itrm != m_MomentsData.end(); itrm++)//elements
	{
		if (itrm->second.Available && itrm->second.Needed)
			itrm->second.CutsData.resize(m_CommonBlock.ScanTaskConfig.CutNumber);//some cuts may have no some moments
	}
	
	//A radial header
	map<uint16_t, bool> mapIsDataSkip; //临时标记当前Cut要素读取与否
	bool bGoodScan = true;
	bool bGoodCut = false;
	while (bGoodScan && !feof(m_pDataFile))
	{
		stdCimissGenRadialHeader genRadialHeader;//统一径向头
		if (fread(&genRadialHeader, SIZE_GEN_RADIAL_HEADER, 1, m_pDataFile) <1)
			break;

		if (IsCutStart(genRadialHeader.RadialState))
		{
			bGoodCut = true; //标记一个正常的Cut
			
			for (short m = 0; m < cMaxMoment; m++)
				mapIsDataSkip[MomentBitIndex(m)] = true;
			for (itrm = m_MomentsData.begin(); itrm != m_MomentsData.end(); itrm++)//elements
			{
				if (itrm->second.Available && itrm->second.Needed)
				{
					mapIsDataSkip[itrm->first] = false;
					aCutMoments[itrm->first].clear();
				}
			}
		}
		if (!bGoodCut)
			continue;

		//Number of moments ,read each moment and check if it is needed
		for (uint32_t m1 = 0; m1 < genRadialHeader.MomentNumber; m1++)
		{//each moment in a radial
			stdCimissMomentRadialHeader momRadialHeader;
			if (fread(&momRadialHeader, SIZE_MOM_RADIAL_HEADER, 1, m_pDataFile) < 1)
			{
				bGoodScan = false;
				break;
			}			
			//need this moment, read		
			stdCimissMomentRadial momRadial;
			momRadial.genHeader = genRadialHeader;
			momRadial.Radial.header = momRadialHeader;
			momRadial.Radial.Data.resize(momRadialHeader.AllBinLength);
			if (fread(&(momRadial.Radial.Data[0]), momRadialHeader.AllBinLength, 1, m_pDataFile)<1)
			{
				bGoodScan = false;
				break;
			}
			
			if (!mapIsDataSkip[momRadialHeader.DataType]) //Skip block of radial data for this moment
			{
				vct_momentCut* pcut = &(*(std::next(m_MomentsData[momRadialHeader.DataType].CutsData.begin(), genRadialHeader.ElevationNumber - 1)));
				pcut->push_back(momRadial);
			}
				//aCutMoments[momRadialHeader.DataType].push_back(momRadial);// a new radial
		}//each moment in a radial

		if (!bGoodScan)
			break;

		//Do something at end of a cut
		if (IsCutEnd(genRadialHeader.RadialState))
		{				
			bGoodCut = false;
			mapIsDataSkip.clear();
		}
	};// get all radials in file
	fclose(m_pDataFile);
	m_pDataFile = 0x0;
}

//确定需要读出的要素
void CCIMISSRadarDataCls::ConfirmMomentsNeeded
(	bool bZ,		//反射率 (滤波后reflectivity)
	bool bZc,		//反射率-订正后
	bool bV,		//径向速度
	bool bVc,		//径向速度-订正后
	bool bW,		//谱宽
	bool bWc,		//谱宽-订正后
	bool bZDR,		//差分反射率
	bool bZDRc,		//差分反射率-订正后
	bool bLDR,		//退偏振比
	bool bCC,		//协相关系数
	bool bPHIDP,	//差分相移
	bool bKDP,		//差分相移率
	bool bHCL,		//双偏振相态分类
	bool bDBT,		//滤波前反射率（Total reflectivity)
	bool bSQI,		//信号质量指数
	bool bCPA,		//杂波相位一致性
	bool bCP,		//杂波可能性
	bool bCF,		//杂波标志
	bool bSNRH)		//水平通道信噪比*/
{
	short i = 0;
	//顺序不能变
	bool bFlag[]={ bZ,bZc,bV,bVc,bW,bWc,bZDR,bZDRc,bLDR,bCC,bPHIDP,bKDP,bHCL,bDBT,bSQI,bCPA,bCP,bCF,bSNRH }	;
	for (i = 0; i < cMaxMoment; i++)
	{
		if (!bFlag[i]) continue;
		m_MomentsData[MomentBitIndex(i)].Needed = true;
	}
}

//确定需要读出且能从数据中获得的要素
void CCIMISSRadarDataCls::ConfirmMomentsObserved(uint16_t cutIndex) //仰角/方位序号 从0开始计数
{
	if (m_CommonBlock.ScanTaskConfig.CutNumber <= cutIndex)
		return;
	for (int i = 0; i < cMaxMoment; i++)
	{
		if ((CIMISS_M_BIT_ID[i] & m_CommonBlock.CutConfig[cutIndex].MomentMask) != 0)
			m_MomentsData[MomentBitIndex(i)].Available = true;
	}
}

//确定需要读出且能从数据中获得的要素
//返回要素数量
int CCIMISSRadarDataCls::ConfirmMomentsToGet(uint16_t cutIndex) //仰角/方位序号 从0开始计数
{
	int numGet = 0;
	int i = 0;

	if(m_CommonBlock.ScanTaskConfig.CutNumber<=cutIndex) 
		return numGet;

	//观测数据中可获取的要素
	ConfirmMomentsObserved(cutIndex);

	map<uint16_t, stdCimissMomentCuts>::iterator itrm;
	for (itrm = m_MomentsData.begin(); itrm != m_MomentsData.end(); itrm++)//elements
	{
		if (itrm->second.Available && itrm->second.Needed)
			numGet++;
	}

	return (numGet);
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

void CCIMISSRadarDataCls::ClearMomentsCuts()
{
	map<uint16_t, stdCimissMomentCuts>::iterator itrm;
	for (itrm = m_MomentsData.begin(); itrm != m_MomentsData.end(); itrm++)//elements
	{
		vct_momentCuts::iterator itrp;
		for (itrp = itrm->second.CutsData.begin(); itrp != itrm->second.CutsData.end(); itrp++)//ppis
		{
			vct_momentCut::iterator  itrr;
			for (itrr = itrp->begin(); itrr != itrp->end(); itrr++)//radials
			{
				itrr->Radial.Data.clear();
				vector<uint8_t>().swap(itrr->Radial.Data);
			}
			itrp->clear();
			list<stdCimissMomentRadial>().swap(*itrp);
		}
		itrm->second.CutsData.clear();
		list<vct_momentCut>().swap(itrm->second.CutsData);
	}
	m_MomentsData.clear();

	m_CommonBlock.CutConfig.clear();
	vector<stdCimissCutConfig>().swap(m_CommonBlock.CutConfig);
}

vct_momentCut* CCIMISSRadarDataCls::GetMomentCut(uint16_t momid, uint16_t cutidx)
{
	map<uint16_t, stdCimissMomentCuts>::iterator itrm;
	for (itrm = m_MomentsData.begin(); itrm != m_MomentsData.end(); itrm++)//elements
	{
		if (itrm->first != momid)//MomentBitIndex(momid)) 
			continue;
		if (itrm->second.CutsData.size() <= cutidx)
			return 0;

		return &(*(std::next((itrm->second.CutsData).begin(),cutidx)));
	}
	return 0;
}

uint16_t CCIMISSRadarDataCls::GetCutNum(uint16_t momid)
{
	map<uint16_t, stdCimissMomentCuts>::iterator itrm;
	for (itrm = m_MomentsData.begin(); itrm != m_MomentsData.end(); itrm++)//elements
	{
		if (itrm->first != momid)//MomentBitIndex( momid))
			continue;
		return uint16_t(itrm->second.CutsData.size());
	}

	return 0;
}

void CCIMISSRadarDataCls::GetCutStartDateTime(uint16_t cutid, uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second)
{
	year = day = month = hour = minute = second = 0;
	if (m_CommonBlock.CutConfig.size() <= cutid) {
		return;
	}

	map<uint16_t, stdCimissMomentCuts>::iterator itrv = m_MomentsData.begin();
	for (; itrv != m_MomentsData.end(); itrv++)
	{ 
		if (itrv->second.CutsData.size() == 0)
			continue;

		vct_momentCuts::iterator itrc = std::next(itrv->second.CutsData.begin(), cutid);
		if (itrc->size() == 0)
			continue;
		vct_momentCut::iterator itrr = itrc->begin();
		TransDateTime(itrr->genHeader.Seconds, year, day, month, hour, minute, second);
		break;
	}
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


void CCIMISSRadarDataCls::GetRadarSite(string &sname, string &scode, string &stype, float &lon, float &lat, float &ht)
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
}


void CCIMISSRadarDataCls::GetRadarSite(char* sname, char* scode, char* stype, float &lon, float &lat, float &ht)
{
	//radar type
	switch (m_CommonBlock.SiteConfig.RadarType)
	{
	case 1:
		strcpy(stype,"SA");
		break;
	case 2:
		strcpy(stype, "SB");
		break;
	case 3:
		strcpy(stype, "SC");
		break;
	case 33:
		strcpy(stype, "CA");
		break;
	case 34:
		strcpy(stype, "CB");
		break;
	case 35:
		strcpy(stype, "CC");
		break;
	case 36:
		strcpy(stype, "CCJ");
	case 37:
		strcpy(stype, "CD");
		break;
	case 4:
		strcpy(stype, "SAD");
		break;
	case 43:
		strcpy(stype, "CAD");
		break;
	case 65:
		strcpy(stype, "XA");
		break;
	default:
		strcpy(stype, "");
		break;
	}
	//code
	strcpy(scode,m_CommonBlock.SiteConfig.SiteCode);
	//name
	strcpy(sname,m_CommonBlock.SiteConfig.SiteName);
	//lon  lat ht
	lon = m_CommonBlock.SiteConfig.Longitude;
	lat = m_CommonBlock.SiteConfig.Latitude;
	ht = (float)m_CommonBlock.SiteConfig.RadarHeight;
}


stdCimissScanTaskConfig* CCIMISSRadarDataCls::GetScanConfig()
{
	return &m_CommonBlock.ScanTaskConfig;
}

stdCimissCutConfig* CCIMISSRadarDataCls::GetCutConfig(uint16_t cutid)
{
	return &(m_CommonBlock.CutConfig[cutid]);
}

uint16_t CCIMISSRadarDataCls::GetNumberOfRadials(uint16_t momid, uint16_t cutid)
{
	uint16_t nr = 0;
	map<uint16_t, stdCimissMomentCuts>::iterator itm;
	for (itm = m_MomentsData.begin(); itm != m_MomentsData.end(); itm++)
	{
		if (itm->first != momid)//MomentBitIndex(momid))
			continue;
		if (itm->second.CutsData.size() <= cutid) 
			continue;

		vct_momentCuts::iterator itc = std::next(itm->second.CutsData.begin(), cutid);
		vct_momentCut::iterator itr;
		for (itr = itc->begin(); itr != itc->end(); itr++)
		{
			if (itr->Radial.Data.size() > 0)
				nr++;
		}
		break; //skip other cuts and moments
	}
	return nr;
}

#include <math.h>
uint16_t CCIMISSRadarDataCls::GetNumberOfBins(uint16_t momid, uint16_t cutid)
{
	uint16_t nb = 0;
	map<uint16_t, stdCimissMomentCuts>::iterator itm;
	for (itm = m_MomentsData.begin(); itm != m_MomentsData.end(); itm++)
	{
		if (itm->first != momid)//MomentBitIndex(momid))
			continue;
		if (itm->second.CutsData.size() <= cutid)
			continue;

		vct_momentCuts::iterator itc = std::next(itm->second.CutsData.begin(), cutid);
		vct_momentCut::iterator itr;
		for (itr = itc->begin(); itr != itc->end(); itr++)
			nb = (nb<itr->Radial.Data.size()) ? uint16_t(itr->Radial.Data.size()):nb;
		
		break; //skip other cuts and moments
	}
	return nb;
}

uint16_t CCIMISSRadarDataCls::GetLengthOfBins(uint16_t momid, uint16_t cutid) //meters
{
	uint16_t binlen = 0;//meteters
	map<uint16_t, stdCimissMomentCuts>::iterator itm;
	for (itm = m_MomentsData.begin(); itm != m_MomentsData.end(); itm++)
	{
		if (itm->first != momid)//MomentBitIndex(momid))
			continue;
		if (itm->second.CutsData.size() <= cutid)
			continue;

		vct_momentCuts::iterator itc = std::next(itm->second.CutsData.begin(), cutid);
		vct_momentCut::iterator itr;
		for (itr = itc->begin(); itr != itc->end(); itr++)
		{
			if (!(itr->Radial.Data.size() > 0))
				continue;
			//=bytes of all bins/bytes of a bin
			binlen = itr->Radial.header.AllBinLength / itr->Radial.header.OneBinLength;
			break;
		}
		break;
	}
	return binlen;
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

//Value of Id is in CIMISS_M_ID
uint16_t CCIMISSRadarDataCls::GetMomentTypeId(const char* szMomentName)
{ 
	uint16_t id = 0;
	string strMoment = "";
	strMoment.assign(szMomentName);
	if (strMoment.find(M_Zc) != string::npos && strMoment.length() == strlen(M_Zc))
	{
		id = CIMISS_M_ID_Zc;//
	}
	else if (strMoment.find(M_Vc) != string::npos && strMoment.length() == strlen(M_Vc))
	{
		id=CIMISS_M_ID_Vc;
	}
	else if (strMoment.find(M_Wc) != string::npos && strMoment.length() == strlen(M_Wc))
	{
		id=CIMISS_M_ID_Wc;
	}
	else if (strMoment.find(M_ZDRc) != string::npos && strMoment.length() == strlen(M_ZDRc))
	{
		id=CIMISS_M_ID_ZDRc;
	}
	else if (strMoment.find(M_Z) != string::npos && strMoment.length() == strlen(M_Z))
	{
		id = CIMISS_M_ID_Z;//
	}
	else if (strMoment.find(M_V) != string::npos && strMoment.length() == strlen(M_V))
	{
		id = CIMISS_M_ID_V;
	}
	else if (strMoment.find(M_W) != string::npos && strMoment.length() == strlen(M_W))
	{
		id = CIMISS_M_ID_W;
	}
	else if (strMoment.find(M_ZDR) != string::npos && strMoment.length() == strlen(M_ZDR))
	{
		id = CIMISS_M_ID_ZDR;
	}
	else if (strMoment.find(M_PDP) != string::npos && strMoment.length() == strlen(M_PDP))
	{
		id = CIMISS_M_ID_PDP;
	}
	else if (strMoment.find(M_KDP) != string::npos && strMoment.length() == strlen(M_KDP))
	{
		id = CIMISS_M_ID_KDP;
	}
	else if (strMoment.find(M_RHV) != string::npos && strMoment.length() == strlen(M_RHV))
	{
		id = CIMISS_M_ID_CC;
	}
	else if (strMoment.find(M_LDR) != string::npos && strMoment.length() == strlen(M_LDR))
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