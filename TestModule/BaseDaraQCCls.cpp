#include "BaseDaraQCCls.h"

#include <experimental/filesystem>
#include <memory>
#include <thread>

#include "CommFunctions.h"
#include "GlobalParams.h"

CBaseDataIOandQCCls::CBaseDataIOandQCCls(int32_t sitecode, const std::string& srcfile)
	:m_srcBaseDataFileName(srcfile)
{	
	m_siteCode=sitecode;
}

CBaseDataIOandQCCls::~CBaseDataIOandQCCls()
{	
}

int32_t CBaseDataIOandQCCls::Run()
{
	std::string unQcFileName("");

	//firstly, load global parameter
	LoadParameter();
	
	//为QC准备待处理的数据文件
	if (!PrepareObsvDataFileForReading(m_srcBaseDataFileName, &unQcFileName))
		return -1;

	std::unique_ptr<CRadarDataInputCls> pBaseData(new CRadarDataInputCls(m_siteCode, unQcFileName));// , szRadarType);
	if (pBaseData == nullptr) {
		return -1;
	}

	pBaseData->LoadScanData();

	//删除临时体扫文件
	std::experimental::filesystem::remove(unQcFileName);

	if (m_paramSaveRef)
	{
		m_strRefDataName = pBaseData->GetMomentScanFile(M_Z);
		if (m_strRefDataName.empty())
		{
			//char strMsg[LEN_MSG] = "";
			//sprintf_s(strMsg, "Error in QC function, number of el. cuts <9. File: %s", unQcFileName);
			//ReportLog_Error(strMsg);

			m_numCuts = 0;

			return -1;
		}
	}
	if(m_paramSaveVel)
	{
		m_strVelDataName = pBaseData->GetMomentScanFile(M_V);
		if (m_strVelDataName.empty())
		{
			//char strMsg[LEN_MSG] = "";
			//sprintf_s(strMsg, "Error in QC function, number of el. cuts <9. File: %s", unQcFileName);
			//ReportLog_Error(strMsg);

			m_numCuts = 0;

			return -1;
		}
	}

	return 1;
}

void CBaseDataIOandQCCls::LoadParameter()
{
	//确定雷达站信息
	for(uint32_t k=0; k<g_iSitesInZone; k++)
	{//Find the correct site inforamtion from the site list
		if(m_siteCode==g_cRadarSiteInfo[k].SiteID)
		{
			m_siteInfo = g_cRadarSiteInfo[k];
			m_siteIndex=k;
			break;
		}
	}

	m_paramSaveRef=false;
	m_paramSaveVel=false;
	if(g_iOptionsGridData==GRIDDATA_OPTION_ALL)
	{
		m_paramSaveRef=true;
		m_paramSaveVel=true;
	}
	else if(g_iOptionsGridData==GRIDDATA_OPTION_REF)
		m_paramSaveRef=true;
	else if(g_iOptionsGridData==GRIDDATA_OPTION_VEL)
		m_paramSaveVel=true;
}

const char* CBaseDataIOandQCCls::GetRefFileName()
{
	if(!FilePathExists(m_strRefDataName.c_str()))
		m_strRefDataName="";

	return (m_strRefDataName.c_str());
}

const char* CBaseDataIOandQCCls::GetVelFileName()
{
	if(!FilePathExists(m_strVelDataName.c_str()))
		m_strVelDataName="";
	
	return (m_strVelDataName.c_str());
}

bool CBaseDataIOandQCCls::FileIsZipped(const std::string& strSrcName, std::string *const strDstName)
{
	auto extension = std::experimental::filesystem::path(strSrcName).extension().string();
	if((extension == ".bz2") || (extension == ".BZ2"))
	{
		*strDstName = std::experimental::filesystem::path(strSrcName).replace_extension("").string();
		return true;
	}

    return false;
}

bool CBaseDataIOandQCCls::PrepareObsvDataFileForReading(const std::string& szSrcFileName, 
	std::string* const szDstFileName)
{
	std::string szCopyFileName=""; //拷贝到本地的临时压缩文件
	std::string szUnzipFileName="";

	//拷贝数据到本地临时数据目录
	szCopyFileName = g_DataDir.strTemDataDir ;
	szCopyFileName += TEMP_FOLDER;
	szCopyFileName += std::experimental::filesystem::path::preferred_separator;

	if (!std::experimental::filesystem::exists(szCopyFileName) && 
		!std::experimental::filesystem::create_directories(szCopyFileName)) {
		return false;
	}

	std::string filename = std::experimental::filesystem::path(szSrcFileName).filename().string();
	szCopyFileName += filename;
	std::experimental::filesystem::copy_file(szSrcFileName, szCopyFileName);

	if(FileIsZipped(szCopyFileName, szDstFileName))
	{//是压缩文件(只针对bz2文件)，启动解压缩功能
		UnZip(szCopyFileName.c_str(),NULL);	
		
		std::this_thread::sleep_for(std::chrono::seconds(5));
		if(!std::experimental::filesystem::exists(*szDstFileName))
		{
			//解压不成功
			return false;
		}
		
		std::experimental::filesystem::remove(szCopyFileName); //删除临时压缩文件,只保留解压后的文件
	}
	
	return true;
}