#include "stdafx.h"
#include "RadarBaseDataInputCls.h"
#include "../GlobalParams.h"
#include "../CommFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
//Class CRadarDataInputCls
CRadarDataInputCls::CRadarDataInputCls(uint32_t scode, const char* szFileName)
{
	m_strInFileName.assign(szFileName);
	m_siteCode = scode;
	m_strFormatV.assign("");
	m_uniformOffset=0;
	m_uniformFactor=10;

	//加载配置参数
	LoadParameter();	
}

CRadarDataInputCls::~CRadarDataInputCls()
{

}

void CRadarDataInputCls::LoadParameter()
{
	//Site info.
	for (uint32_t i = 0; i < g_iSitesInZone; i++)
	{
		if (m_siteCode == g_cRadarSiteInfo[i].SiteID)
		{
			m_ParamSiteInfo = g_cRadarSiteInfo[i];
			break;
		}
	}
	
	if (g_iOptionsGridData == GRIDDATA_OPTION_ALL)
	{
		m_paranmNeededFlag.bZ = true;
		m_paranmNeededFlag.bV = true;

		AddNeededMoment(M_Z);
		AddNeededMoment(M_V);
	}
	else if (g_iOptionsGridData == GRIDDATA_OPTION_REF)
	{		
		m_paranmNeededFlag.bZ = true;
		AddNeededMoment(M_Z);
	}
	else if (g_iOptionsGridData == GRIDDATA_OPTION_VEL)
	{
		m_paranmNeededFlag.bV = true;
		AddNeededMoment(M_V);
	}
	else
		;

	//临时数据输出路径
	if (m_paranmNeededFlag.bZ || m_paranmNeededFlag.bV)
	{
		m_paramOutputPath.assign(g_DataDir.strTemDataDir);
		m_paramOutputPath.append(TEMP_FOLDER);
		CreateDir(m_paramOutputPath.c_str());////
		m_paramOutputPath.append(SZ_SLASH);
		m_paramOutputPath.append(UNIFORM_POLAR_FOLDER);// "UNIFORMBD");
		CreateDir(m_paramOutputPath.c_str());////
		m_paramOutputPath.append(SZ_SLASH);
		m_paramOutputPath.append(m_ParamSiteInfo.SiteName);		
	}
}

//设置需要的要素
//szMomentType -- 要素名
//bQC --是否需要的是QC 后的要素
void CRadarDataInputCls::AddNeededMoment(const char* szMomentType, bool bQc)
{ 
	string strMoment = "";
	strMoment.assign(szMomentType);

	stdGeneralInputScanData aScan;
	aScan.Needed = true;

	aScan.header.offset = m_uniformOffset;
	aScan.header.factor = m_uniformFactor;
	aScan.header.ndata = VALUE_INVALID;

	if (strMoment.find(M_Z) != string::npos && strMoment.length() == strlen(M_Z))
	{
		if (bQc)
		{
			m_paranmNeededFlag.bZc = true;
			m_genScanDatas[M_Zc_ID] = aScan;
		}
		else
		{
			m_paranmNeededFlag.bZ = true;
			m_genScanDatas[M_Z_ID] = aScan;
		}
	}
	if (strMoment.find(M_V) != string::npos && strMoment.length() == strlen(M_V))
	{
		if (bQc)
		{
			m_paranmNeededFlag.bVc = true;
			m_genScanDatas[M_Vc_ID] = aScan;
		}
		else
		{
			m_paranmNeededFlag.bV = true;
			m_genScanDatas[M_V_ID] = aScan;
		}
	}
	if (strMoment.find(M_W) != string::npos && strMoment.length() == strlen(M_W))
	{
		if (bQc)
		{
			m_paranmNeededFlag.bWc = true;
			m_genScanDatas[M_Wc_ID] = aScan;
		}
		else
		{
			m_paranmNeededFlag.bW = true;
			m_genScanDatas[M_W_ID] = aScan;
		}
	}
	if (strMoment.find(M_ZDR) != string::npos && strMoment.length() == strlen(M_ZDR))
	{
		if (bQc)
		{
			m_paranmNeededFlag.bZDRc = true;
			m_genScanDatas[M_ZDRc_ID] = aScan;
		}
		else
		{
			m_paranmNeededFlag.bZDR = true;
			m_genScanDatas[M_ZDR_ID] = aScan;
		}
	}
	if (strMoment.find(M_PDP) != string::npos && strMoment.length() == strlen(M_PDP))
	{
		m_paranmNeededFlag.bPDP = true;
		m_genScanDatas[M_PDP_ID] = aScan;
	}
	if (strMoment.find(M_KDP) != string::npos && strMoment.length() == strlen(M_KDP))
	{
		m_paranmNeededFlag.bKDP = true;
		m_genScanDatas[M_KDP_ID] = aScan;
	}
	if (strMoment.find(M_RHV) != string::npos && strMoment.length() == strlen(M_RHV))
	{
		m_paranmNeededFlag.bRHV = true;
		m_genScanDatas[M_RHV_ID] = aScan;
	}
	if (strMoment.find(M_LDR) != string::npos && strMoment.length() == strlen(M_LDR))
	{
		m_paranmNeededFlag.bLDR = true;
		m_genScanDatas[M_LDR_ID] = aScan;
	}
}
//
void CRadarDataInputCls:: LoadScanData()
{
	//Radar type "SA" "SB"....
	char szType[SITE_TYPE_LEN] = "";
	DetermineRadarType(m_strInFileName.c_str(), m_ParamSiteInfo,szType);
	m_strFormatV.assign(szType);

	//根读对应类型的数据
	if (m_strFormatV.find(FM_RD_SA) != string::npos || m_strFormatV.find(FM_RD_SB) != string::npos)
	{
		CRadarBaseDataWsr98DS  Met98DSRadarData(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		Met98DSRadarData.GetObsvData(m_paranmNeededFlag, m_genScanDatas);
	}
	else if (m_strFormatV.find(FM_RD_CA) != string::npos || m_strFormatV.find(FM_RD_CB) != string::npos)
	{
		CRadarBaseDataWsr98DC   Met98DCRadarData(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		Met98DCRadarData.GetObsvData(m_paranmNeededFlag, m_genScanDatas);
	}
	else if (m_strFormatV.find(FM_RD_SC2) != string::npos && m_strFormatV.length() == strlen(FM_RD_SC2)||
		m_strFormatV.find(FM_RD_CD2) != string::npos && m_strFormatV.length() == strlen(FM_RD_CD2))
	{
		CRadarBaseDataCDSC2 MetSCRadarData(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		MetSCRadarData.GetObsvData(m_paranmNeededFlag, m_genScanDatas);
	}
	else if (m_strFormatV.find(FM_RD_SC) != string::npos && m_strFormatV.length() == strlen(FM_RD_SC) ||
		m_strFormatV.find(FM_RD_CD) != string::npos  && m_strFormatV.length() == strlen(FM_RD_CD))
	{
		CRadarBaseDataCDSC   LocalSCRadarData(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		LocalSCRadarData.GetObsvData(m_paranmNeededFlag, m_genScanDatas);
	}
	else if (m_strFormatV.find(FM_RD_CC2) != string::npos && m_strFormatV.length() == strlen(FM_RD_CC2))
	{
		;
	}/*
	else if (m_strFormatV.find(FM_RD_CC) != string::npos && m_strFormatV.length() == strlen(FM_RD_CC))
	{
		m_pLoacalCC1RadarData = new CRadarBaseDataCC1(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		if (m_pLoacalCC1RadarData)
			m_pLoacalCC1RadarData->GetObsvData(m_paranmNeededFlag, m_genScanDatas);

		if (m_pLoacalCC1RadarData)
		{
			delete m_pLoacalCC1RadarData;
			m_pLoacalCC1RadarData = 0x0;  
		}
	}
	else if (m_strFormatV.find(FM_RD_CIMISS) != string::npos) 
	{
		m_pCimissRadarData = new CCIMISSRadarDataCls(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		m_pCimissRadarData->GetObsvData(m_paranmNeededFlag, m_genScanDatas);
		if (m_pCimissRadarData)
		{
			delete m_pCimissRadarData;
			m_pCimissRadarData = 0x0;
		}
	}*//*
	else if (m_strFormatV.find(FM_RD_CC2) != string::npos && m_strFormatV.length() == strlen(FM_RD_CC2))
		;//CC2.0
	else if (m_strFormatV.find(FM_RD_88D) != string::npos && m_strFormatV.length() == strlen(FM_RD_88D))
		;//88D-SA Shanghai*/

	//完善其它信息
	map_GeneralScanDatas::iterator itm;
	for (itm = m_genScanDatas.begin(); itm != m_genScanDatas.end(); itm++)
	{
		AddSiteInfoInData(&(itm->second), m_ParamSiteInfo);
	}
	return;
}

void CRadarDataInputCls::LoadCutData(float el, float az)
{
	if (m_strFormatV.find(FM_RD_SA) != string::npos || m_strFormatV.find(FM_RD_SB) != string::npos)
	{
		;//m_pMet98DSRadarData = new CRadarBaseDataWsr98DS(m_strInFileName);//SA SB
		//m_pMet98DRadarData->GetObsvData(m_genScanDatas, m_genScanDatas);
	}
	return;
}

stdUniformScanData* CRadarDataInputCls::GetMomentScanData(const char* szMomentType)//, stdCommonScanData *pOutScanData)
{
	uint16_t id = CommonMomentTypeToID((char*)szMomentType);

	return(GetMomentScanData(id));
}

stdUniformScanData* CRadarDataInputCls::GetMomentScanData(uint16_t nMomentTypeId)//, stdCommonScanData *pOutScanData)
{ 
	map_GeneralScanDatas::iterator itm;
	for (itm = m_genScanDatas.begin(); itm != m_genScanDatas.end(); itm++)
	{
		if (itm->first != nMomentTypeId)
			continue;

		return &(itm->second);
	}

	return 0x0;
}

string CRadarDataInputCls::GetMomentScanFile(const char* szMomentType)//, stdCommonScanData *pOutScanData)
{
	m_strUniformFieNameZ = "";
	m_strUniformFieNameV = "";
	uint16_t id = CommonMomentTypeToID((char*)szMomentType);

	stdUniformScanData* pvsScan=GetMomentScanData(id);
	if (id == M_Z_ID)
	{
		m_strUniformFieNameZ = SaveUniformData(*pvsScan, (char*)szMomentType, m_paramOutputPath.c_str(), m_strUniformFieNameZ);
		return m_strUniformFieNameZ;
	}
	if (id == M_V_ID)
	{
		m_strUniformFieNameV = SaveUniformData(*pvsScan, (char*)szMomentType, m_paramOutputPath.c_str(), m_strUniformFieNameV);
		return m_strUniformFieNameV;
	}
	return m_strUniformFieNameZ; //=""
}

string CRadarDataInputCls::GetMomentScanFile(uint16_t nMomentTypeId)//, stdCommonScanData *pOutScanData)
{
	m_strUniformFieNameZ = "";
	m_strUniformFieNameV = "";
	map_GeneralScanDatas::iterator itm;
	for (itm = m_genScanDatas.begin(); itm != m_genScanDatas.end(); itm++)
	{
		if (itm->first != nMomentTypeId)
			continue;

		if(nMomentTypeId==M_Z_ID)
		{
			m_strUniformFieNameZ = SaveUniformData(itm->second, (char*)M_Z, m_paramOutputPath.c_str(), m_strUniformFieNameZ);
			return m_strUniformFieNameZ;
		}
		if (nMomentTypeId == M_V_ID)
		{
			m_strUniformFieNameV = SaveUniformData(itm->second, (char*)M_V, m_paramOutputPath.c_str(), m_strUniformFieNameV);
			return m_strUniformFieNameV;
		}
	}
	return m_strUniformFieNameZ; //=""
}

void CRadarDataInputCls::DetermineRadarType(const char* szDataFileName, RADARSITEINFO siteInfo, char *szType)
{
	//m_siteInfo
	strcpy_s(szType, strlen(siteInfo.SiteType)+1,siteInfo.SiteType);

	string strFileName = "";
	strFileName.assign(szDataFileName);

	//以下类型又有两类格式，通过文件内容区分
	string strFormatKey = "";
	strFormatKey.assign(szType);
	if (strFormatKey.find(FM_RD_CD) != string::npos) //两类格式
	{
		FILE* fp = 0;
		errno_t err = fopen_s(&fp, strFileName.c_str(), "rb");
		if (err == 0)
		{
			stdRadialHeader98D header;
			fread(&header, sizeof(header), 1, fp);
			if(header.RadarStatus==1)  //CD2 SC2
				sprintf_s(szType, strlen(FM_RD_CD2) + 1, "%s", FM_RD_CD2);
			else
				sprintf_s(szType, strlen(FM_RD_CD) + 1, "%s", FM_RD_CD);
			fclose(fp);
		}		
	}
	else if (strFileName.find(FM_RD_SC) != string::npos)
	{
		FILE* fp = 0;
		errno_t err = fopen_s(&fp, strFileName.c_str(), "rb");
		if (err == 0)
		{
			stdRadialHeader98D header;
			fread(&header, sizeof(header), 1, fp);
			if (header.RadarStatus == 1)  //CD2 SC2
				sprintf_s(szType, strlen(FM_RD_SC2) + 1, "%s", FM_RD_SC2);
			else
				sprintf_s(szType, strlen(FM_RD_SC) + 1, "%s", FM_RD_SC);
			fclose(fp);
		}
	}
	else if (strFileName.find(FM_RD_CC) != string::npos)
	{
		FILE* fp = 0;
		errno_t err = fopen_s(&fp, strFileName.c_str(), "rb");
		if (err == 0)
		{
			char cFileType[16]="";
			string strFileType = "";
			fread(&cFileType, sizeof(char)*16, 1, fp);
			strFileType.assign(cFileType);
			if (strFileType.find("CINRAD")!=string::npos)  //CD2 SC2
				sprintf_s(szType, strlen(FM_RD_CC) + 1, "%s", FM_RD_CC);
			else
				sprintf_s(szType, strlen(FM_RD_CC2) + 1, "%s", FM_RD_CC2);
			fclose(fp);
		}
	}
}

void CRadarDataInputCls::AddSiteInfoInData(stdUniformScanData *pvsScan, RADARSITEINFO siteInfo)
{
	uint32_t code = 0;
	char szcode[SITE_CODE_LEN] = "";

	pvsScan->header.radlon = siteInfo.SiteLon;
	pvsScan->header.radlat = siteInfo.SiteLat;
	pvsScan->header.radhgt = siteInfo.SiteHgt;
	sprintf_s(pvsScan->header.radar_name, strlen(siteInfo.SiteName) + 1, siteInfo.SiteName);
	sprintf_s(pvsScan->header.radar_type, strlen(siteInfo.SiteType) + 1, siteInfo.SiteType);
	sprintf_s(pvsScan->header.obsv_type, 4, "VCP");
	code = siteInfo.SiteID;
	if (code < 1000)
		code += 9000;
	_itoa_s(siteInfo.SiteID, szcode, SITE_CODE_LEN, 10);
	sprintf_s(pvsScan->header.radar_code, strlen(szcode) + 1, szcode);

	for (size_t i = 0; i < pvsScan->CutsData.size(); i++)
	{
		if (strlen(pvsScan->CutsData[i].header.radar_name)>1)
			continue;

		sprintf_s(pvsScan->CutsData[i].header.radar_name, strlen(siteInfo.SiteName) + 1, siteInfo.SiteName);
		sprintf_s(pvsScan->CutsData[i].header.radar_type, strlen(siteInfo.SiteType) + 1, siteInfo.SiteType);
		pvsScan->CutsData[i].header.radlon = siteInfo.SiteLon;
		pvsScan->CutsData[i].header.radlat = siteInfo.SiteLat;
		pvsScan->CutsData[i].header.radhgt = siteInfo.SiteHgt;
	}
}

string  CRadarDataInputCls::SaveUniformData(stdUniformScanData &vsData, char *pExtType, const char *dstPath,string &strDestFileName)
{
	strDestFileName.assign("");

	size_t nc = vsData.CutsData.size();
	size_t na = vsData.CutsData[0].header.num_beam;
	//size_t nb = 0;	

	size_t nCut = vsData.CutsData.size();
	if (nCut<4)
		return strDestFileName;

	//创建目录
	CreateDir(dstPath);////

	stdUniformCutData *pCutData = &(vsData.CutsData[0]);
	stdUniformCutHeader *pCutHeader = &(vsData.CutsData[0].header);
	for (nc = 0; nc < vsData.CutsData.size(); nc++)
	{
		pCutData = &(vsData.CutsData[nc]);
		pCutHeader = &(vsData.CutsData[nc].header);

		if (pCutData->azim.size() != pCutData->elev.size() ||
			pCutData->azim.size() != pCutData->field.size() ||
			pCutData->field.size() != pCutHeader->num_beam)
			return strDestFileName;

		for (na = 0; na < pCutHeader->num_beam; na++)
		{
			if (pCutData->field[na].size() != pCutHeader->num_gate)
				return strDestFileName;
		}
	}

	//Set file name
	char strFileName[256] = "";
	char strYear[6] = "", strMonth[4] = "", strDay[4] = "", strHour[4] = "", strMinute[4] = "";
	_itoa_s(pCutHeader->year, strYear, 6, 10);
	_itoa_s(pCutHeader->month + 100, strMonth, 4, 10);
	_itoa_s(pCutHeader->day + 100, strDay, 4, 10);
	_itoa_s(pCutHeader->hour + 100, strHour, 4, 10);
	_itoa_s(pCutHeader->minute + 100, strMinute, 4, 10);
	strcpy_s(strFileName, dstPath);
	if(strFileName[strlen(strFileName)-1]!='/' && strFileName[strlen(strFileName) - 1] != '\\')
		strcat_s(strFileName, SZ_SLASH);
	strcat_s(strFileName, strYear);
	strcat_s(strFileName, strMonth + 1);
	strcat_s(strFileName, strDay + 1);
	strcat_s(strFileName, strHour + 1);
	strcat_s(strFileName, strMinute + 1);
	strcat_s(strFileName, ".");
	strcat_s(strFileName, pExtType);
	strcat_s(strFileName, ".bin");

	FILE *fp = 0;
	errno_t err = 0;
	err = fopen_s(&fp, strFileName, "wb");
	if (!fp)
		return strDestFileName;//(false);
			   //Write data header into the file
	uint16_t shortCut = uint16_t(nCut);
	if (fwrite(&shortCut, sizeof(uint16_t), 1, fp)<1)
	{
		fclose(fp);
		return strDestFileName;
	}
	if (fwrite(&vsData.header, sizeof(stdUniformScanHeader), 1, fp)<1)
	{
		fclose(fp);
		return strDestFileName;
	}
	for (nc = 0; nc < vsData.CutsData.size(); nc++)
	{
		pCutData = &(vsData.CutsData[nc]);
		pCutHeader = &(vsData.CutsData[nc].header);
		//header
		if (fwrite(pCutHeader, sizeof(stdUniformCutHeader), 1, fp)<1)
		{
			fclose(fp);
			return strDestFileName;
		}
		if (fwrite(&(pCutData->elev[0]), sizeof(uint16_t)*pCutHeader->num_beam, 1, fp) < 1)
		{
			fclose(fp);
			return strDestFileName;
		}
		if (fwrite(&(pCutData->azim[0]), sizeof(uint16_t)*pCutHeader->num_beam, 1, fp) < 1)
		{
			fclose(fp);
			return strDestFileName;
		}

		for (na = 0; na < pCutHeader->num_beam; na++)
		{
			if (fwrite(&(pCutData->field[na][0]), sizeof(uint16_t)*pCutHeader->num_gate, 1, fp) < 1)
			{
				fclose(fp);
			}
		}
	}//number of cuts

	fclose(fp);
	fp = 0;

	strDestFileName.assign(strFileName);
	return strDestFileName;
	//strcpy_s(dstDataFile, strlen(strFileName) + 1, strFileName);
}

/*
void CRadarDataInputCls::GetMomentCuts(const char* szMomentType, stdCommonScanData &outScanData)
{
	
	uint16_t momid = 0;
	string strMoment;
	strMoment.assign(szMomentType);

	if (m_pCimissRadarData)
	{
		GetCimisMoment(szMomentType, outScanData);
	}	//CIMISS DATA end
	else if (m_pMet98DRadarData)
	{
		GetMet98DMoment(szMomentType, outScanData);
	}
	
}*/
/*//#pragma warning(disable:4996)
void CRadarDataInputCls::GetCimisMoment(const char* szMomentType, stdCommScan &outScanData)
{
	if (m_pCimissRadarData == 0x0)
		return;
	uint16_t momId = m_pCimissRadarData->GetMomentTypeId(szMomentType);
	uint16_t nCut = m_pCimissRadarData->GetCutNum(momId);

	stdScanTaskConfig* pScanConfig = m_pCimissRadarData->GetScanConfig();

	float flon, flat, fht;
	char sname[SITE_NAME_LEN]="", sid[SITE_NAME_LEN]="", stype[SITE_TYPE_LEN]="";
	m_pCimissRadarData->GetRadarSite(sname, sid, stype, flon, flat, fht);
	//cuts	
	//outScanData.scanData.resize(nCut);
	for (uint16_t n = 0; n < nCut; n++)
	{
		if (m_pCimissRadarData->GetMomentCut(momId, n)->size()  == 0)
			continue;//cut n of this moment is empty 

		stdCommonCutData aNewCut;
		strcpy(aNewCut.header.radar_name, sname);
		strcpy(aNewCut.header.radar_type, stype);
		strcpy(aNewCut.header.data_name, szMomentType);
		aNewCut.header.radlon = uint32_t(flon * 1000);
		aNewCut.header.radlat = uint32_t(flat * 1000);
		aNewCut.header.radhgt = uint16_t(fht);
		//
		stdCutConfig* pCutConfig = m_pCimissRadarData->GetCutConfig(n);
		aNewCut.header.nyq_vel = uint32_t(pCutConfig->NyquistSpeed*10);
		if(pScanConfig->ScanType==2 || pScanConfig->ScanType == 5)//RHI RHIS
			aNewCut.header.elev_angle = uint32_t(pCutConfig->Azimuth*10);
		else
			aNewCut.header.elev_angle = uint32_t(pCutConfig->Elevation*10);
		m_pCimissRadarData->GetCutStartDateTime(n, aNewCut.header.year, aNewCut.header.month, aNewCut.header.day, aNewCut.header.hour, aNewCut.header.minute, aNewCut.header.second);
		aNewCut.header.fstgatdis = pCutConfig->StartRange * 10;
		aNewCut.header.num_neededFlag.beam = m_pCimissRadarData->GetNumberOfRadials(momId, n);

		//short maxbin, stepbin;
		//stepbin = 1;
		//maxbin = m_pCimissRadarData->GetNumberOfBins(momId, n);
		///if (maxbin > ngate){
		//	stepbin = 2;maxbin = maxbin / stepbin;}
		aNewCut.header.num_gate  = m_pCimissRadarData->GetNumberOfBins(momId, n);
		aNewCut.header.gateWidth = m_pCimissRadarData->GetBinWidth(momId, n);// *stepbin;
		
		aNewCut.azim.resize(aNewCut.header.num_neededFlag.beam);
		aNewCut.elev.resize(aNewCut.header.num_neededFlag.beam);
		aNewCut.field.resize(aNewCut.header.num_neededFlag.beam);
		
		//cut data		
		vct_momentCut::iterator itrc = m_pCimissRadarData->GetMomentCut(momId, n)->begin();
		vct_momentCut::iterator itrc1 = m_pCimissRadarData->GetMomentCut(momId, n)->end();
		short na = 0, nr = 0;	
		short bytes_abin = 0, num_neededFlag.bins = 0;
		for (; itrc != itrc1; itrc++) //radials
		{//az			
			//
			aNewCut.elev[na] = uint32_t(itrc->genHeader.Elevation*10);
			aNewCut.azim[na] = uint32_t(itrc->genHeader.Azimuth*10);
			//
			aNewCut.field[na].resize(aNewCut.header.num_gate);

			bytes_abin = itrc->Radial.header.OneBinLength;
			num_neededFlag.bins = short(itrc->Radial.Data.size()/ bytes_abin);
			if (bytes_abin == 1)
			{
				uint8_t* pRadial = &(itrc->Radial.Data[0]);
				for (nr = 0; nr < num_neededFlag.bins; nr++)
				{
					float value = m_pCimissRadarData->GetBinValue(pRadial[nr], itrc->Radial.header.Offset, itrc->Radial.header.Scale, VALUE_INVALID / 10);
					aNewCut.field[na][nr] = int16_t(value * 10);  //*10
				}
				for (; nr < aNewCut.header.num_gate; nr++) {//bin
					aNewCut.field[na][nr] = VALUE_INVALID;
				}
			}
			else
			{
				uint16_t* pRadial = (uint16_t*)&(itrc->Radial.Data[0]);
				for (nr = 0; nr < num_neededFlag.bins; nr++)
				{
					float value = m_pCimissRadarData->GetBinValue(pRadial[nr], itrc->Radial.header.Offset, itrc->Radial.header.Scale, VALUE_INVALID / 10);
					aNewCut.field[na][nr] = int16_t(value * 10);  //*10
				}
				for (; nr < aNewCut.header.num_gate; nr++) {//bin
					aNewCut.field[na][nr] = VALUE_INVALID;
				}
			}
			na++;
		}//radials

		outScanData.DataCuts.push_back(aNewCut);
	}
}
*/

