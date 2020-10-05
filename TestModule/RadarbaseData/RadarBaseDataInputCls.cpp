#include "RadarBaseDataInputCls.h"

#include <experimental/filesystem>

#include "../GlobalParams.h"
#include "../CommFunctions.h"

////////////////////////////////////////////////////////////////////////////
//Class CRadarDataInputCls
CRadarDataInputCls::CRadarDataInputCls(uint32_t scode, const std::string& szFileName)
	:m_strInFileName(szFileName)
{
	m_siteCode = scode;
	m_strFormatV.assign("");
	m_uniformOffset=0;
	m_uniformFactor=10;

	//加载配置参数
	LoadParameter();	
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
		std::experimental::filesystem::create_directories(m_paramOutputPath);

		//CreateDir(m_paramOutputPath.c_str());////

		m_paramOutputPath += std::experimental::filesystem::path::preferred_separator;
		m_paramOutputPath.append(UNIFORM_POLAR_FOLDER);// "UNIFORMBD");
		std::experimental::filesystem::create_directories(m_paramOutputPath);
		//CreateDir(m_paramOutputPath.c_str());////

		m_paramOutputPath += std::experimental::filesystem::path::preferred_separator;
		m_paramOutputPath.append(m_ParamSiteInfo.SiteName);	
		//std::experimental::filesystem::create_directories(m_paramOutputPath);
	}
}

//设置需要的要素
//szMomentType -- 要素名
//bQC --是否需要的是QC 后的要素
void CRadarDataInputCls::AddNeededMoment(const char* szMomentType, bool bQc)
{ 
	std::string strMoment = "";
	strMoment.assign(szMomentType);

	stdGeneralInputScanData aScan;
	aScan.Needed = true;

	aScan.header.offset = m_uniformOffset;
	aScan.header.factor = m_uniformFactor;
	aScan.header.ndata = VALUE_INVALID;

	if (strMoment.find(M_Z) != std::string::npos && strMoment.length() == strlen(M_Z))
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
	if (strMoment.find(M_V) != std::string::npos && strMoment.length() == strlen(M_V))
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
	if (strMoment.find(M_W) != std::string::npos && strMoment.length() == strlen(M_W))
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
	if (strMoment.find(M_ZDR) != std::string::npos && strMoment.length() == strlen(M_ZDR))
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
	if (strMoment.find(M_PDP) != std::string::npos && strMoment.length() == strlen(M_PDP))
	{
		m_paranmNeededFlag.bPDP = true;
		m_genScanDatas[M_PDP_ID] = aScan;
	}
	if (strMoment.find(M_KDP) != std::string::npos && strMoment.length() == strlen(M_KDP))
	{
		m_paranmNeededFlag.bKDP = true;
		m_genScanDatas[M_KDP_ID] = aScan;
	}
	if (strMoment.find(M_RHV) != std::string::npos && strMoment.length() == strlen(M_RHV))
	{
		m_paranmNeededFlag.bRHV = true;
		m_genScanDatas[M_RHV_ID] = aScan;
	}
	if (strMoment.find(M_LDR) != std::string::npos && strMoment.length() == strlen(M_LDR))
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
	if (m_strFormatV.find(FM_RD_SA) != std::string::npos || m_strFormatV.find(FM_RD_SB) != std::string::npos)
	{
		CRadarBaseDataWsr98DS  Met98DSRadarData(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		Met98DSRadarData.GetObsvData(m_paranmNeededFlag, m_genScanDatas);
	}
	else if (m_strFormatV.find(FM_RD_CA) != std::string::npos || m_strFormatV.find(FM_RD_CB) != std::string::npos)
	{
		CRadarBaseDataWsr98DC   Met98DCRadarData(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		Met98DCRadarData.GetObsvData(m_paranmNeededFlag, m_genScanDatas);
	}
	else if (m_strFormatV.find(FM_RD_SC2) != std::string::npos && m_strFormatV.length() == strlen(FM_RD_SC2)||
		m_strFormatV.find(FM_RD_CD2) != std::string::npos && m_strFormatV.length() == strlen(FM_RD_CD2))
	{
		CRadarBaseDataCDSC2 MetSCRadarData(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		MetSCRadarData.GetObsvData(m_paranmNeededFlag, m_genScanDatas);
	}
	else if (m_strFormatV.find(FM_RD_SC) != std::string::npos && m_strFormatV.length() == strlen(FM_RD_SC) ||
		m_strFormatV.find(FM_RD_CD) != std::string::npos  && m_strFormatV.length() == strlen(FM_RD_CD))
	{
		CRadarBaseDataCDSC   LocalSCRadarData(m_strInFileName, m_uniformOffset, m_uniformFactor);//SA SB
		LocalSCRadarData.GetObsvData(m_paranmNeededFlag, m_genScanDatas);
	}
	else if (m_strFormatV.find(FM_RD_CC2) != std::string::npos && m_strFormatV.length() == strlen(FM_RD_CC2))
	{
		;
	}

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
	if (m_strFormatV.find(FM_RD_SA) != std::string::npos || m_strFormatV.find(FM_RD_SB) != std::string::npos)
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

std::string CRadarDataInputCls::GetMomentScanFile(const char* szMomentType)//, stdCommonScanData *pOutScanData)
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

std::string CRadarDataInputCls::GetMomentScanFile(uint16_t nMomentTypeId)//, stdCommonScanData *pOutScanData)
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
	strncpy(szType,siteInfo.SiteType, strlen(siteInfo.SiteType) + 1);

	std::string strFileName = "";
	strFileName.assign(szDataFileName);

	//以下类型又有两类格式，通过文件内容区分
	std::string strFormatKey = "";
	strFormatKey.assign(szType);
	if (strFormatKey.find(FM_RD_CD) != std::string::npos) //两类格式
	{
		FILE* fp = 0;
		errno_t err = fopen_s(&fp, strFileName.c_str(), "rb");
		if (err == 0)
		{
			format_98d::stdRadialHeader98D header;
			fread(&header, sizeof(header), 1, fp);
			if(header.RadarStatus==1)  //CD2 SC2
				sprintf(szType,  "%s", FM_RD_CD2);
			else
				sprintf(szType, "%s", FM_RD_CD);
			fclose(fp);
		}		
	}
	else if (strFileName.find(FM_RD_SC) != std::string::npos)
	{
		FILE* fp = 0;
		errno_t err = fopen_s(&fp, strFileName.c_str(), "rb");
		if (err == 0)
		{
			format_98d::stdRadialHeader98D header;
			fread(&header, sizeof(header), 1, fp);
			if (header.RadarStatus == 1)  //CD2 SC2
				sprintf(szType, "%s", FM_RD_SC2);
			else
				sprintf(szType, "%s", FM_RD_SC);
			fclose(fp);
		}
	}
	else if (strFileName.find(FM_RD_CC) != std::string::npos)
	{
		FILE* fp = 0;
		errno_t err = fopen_s(&fp, strFileName.c_str(), "rb");
		if (err == 0)
		{
			char cFileType[16]="";
			std::string strFileType = "";
			fread(&cFileType, sizeof(char)*16, 1, fp);
			strFileType.assign(cFileType);
			if (strFileType.find("CINRAD")!=std::string::npos)  //CD2 SC2
				sprintf(szType, "%s", FM_RD_CC);
			else
				sprintf(szType, "%s", FM_RD_CC2);
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
	sprintf(pvsScan->header.radar_name, "%s", siteInfo.SiteName);
	sprintf(pvsScan->header.radar_type, "%s", siteInfo.SiteType);
	sprintf(pvsScan->header.obsv_type, "%s", "VCP");
	code = siteInfo.SiteID;
	if (code < 1000)
		code += 9000;
	_itoa_s(siteInfo.SiteID, szcode, SITE_CODE_LEN, 10);
	sprintf(pvsScan->header.radar_code, "%s", szcode);

	for (size_t i = 0; i < pvsScan->CutsData.size(); i++)
	{
		if (strlen(pvsScan->CutsData[i].header.radar_name)>1)
			continue;

		sprintf(pvsScan->CutsData[i].header.radar_name, "%s", siteInfo.SiteName);
		sprintf(pvsScan->CutsData[i].header.radar_type, "%s", siteInfo.SiteType);
		pvsScan->CutsData[i].header.radlon = siteInfo.SiteLon;
		pvsScan->CutsData[i].header.radlat = siteInfo.SiteLat;
		pvsScan->CutsData[i].header.radhgt = siteInfo.SiteHgt;
	}
}

std::string  CRadarDataInputCls::SaveUniformData(stdUniformScanData &vsData, char *pExtType, const char *dstPath,std::string &strDestFileName)
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
	std::string strFileName="";
	std::string strYear = std::to_string(pCutHeader->year);
	std::string strMonth = std::to_string(pCutHeader->month + 100);
	std::string strDay = std::to_string(pCutHeader->day + 100);
	std::string strHour = std::to_string(pCutHeader->hour + 100);
	std::string strMinute = std::to_string(pCutHeader->minute + 100);

	strFileName = dstPath;
	if (strFileName[strFileName.size() - 1] != '/' && strFileName[strFileName.size() - 1] != '\\') {
		strFileName += std::experimental::filesystem::path::preferred_separator;
	}
	strFileName += strYear;
	strFileName.append(strMonth.data() + 1);
	strFileName.append(strDay.data() + 1);
	strFileName.append(strHour.data() + 1);
	strFileName.append(strMinute.data() + 1);
	strFileName += ".";
	strFileName += pExtType;
	strFileName += ".bin";

	FILE *fp = nullptr;
	errno_t err = 0;
	err = fopen_s(&fp, strFileName.c_str(), "wb");
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
}

