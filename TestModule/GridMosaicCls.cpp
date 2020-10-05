#include "GridMosaicCls.h"

#include <experimental/filesystem>

#include "CommFunctions.h"
#include "GlobalParams.h"

#include "CommVar.h"
#include <math.h>
#include <io.h>
#include <omp.h>

CGridMosaicCls::CGridMosaicCls(int16_t numFile, std::vector<std::string>  &dataFileName, stdDateTime *pMosaicDateTime)
{
	m_mosaicDateTime = *pMosaicDateTime;

	if (LoadSiteGridFileAndHeader(numFile, dataFileName) == 0)
		return;

	m_paramMinX = 0.;
	m_paramMinY = 0.;
	m_paramMaxX = 0.;
	m_paramMaxY = 0.;
	m_paramNumX = 0;
	m_paramNumY = 0;
	m_paramGridLenX = 0.;
	m_paramGridLenY = 0.;
	m_paramNumZ = 0;
	m_pHeights = 0x0; //m	

	m_paramAlgThrRng = 100;//km
}

CGridMosaicCls::~CGridMosaicCls()
{
	//
	for (int ns = 0; ns < m_pFiles.size(); ns++)
	{
		if (m_pFiles[ns] == 0) continue;
		fclose(m_pFiles[ns]);
		m_pFiles[ns] = 0;
	}

	ClearTempDatas();//

	m_siteGirdFiles.clear();
	std::vector<BaseDataInfo>().swap(m_siteGirdFiles);
	m_siteGridHeader.clear();
	std::vector<MOSAICDATAHEADER>().swap(m_siteGridHeader);
	m_siteInfo.clear();
	std::vector<RADARSITEINFO>().swap(m_siteInfo);
	m_siteGridPosInMosaic.clear();
	std::vector<SITEPOSINMOSAIC>().swap(m_siteGridPosInMosaic);

	if (m_mosGridData != 0x0)
	{
		for (int32_t l = 0; l < m_paramNumZ; l++)
		{
			if (m_mosGridData[l] == 0x0) continue;

			for (int32_t i = 0; i < m_paramNumY; i++)
			{
				if (m_mosGridData[l][i] == 0) continue;

				delete[] m_mosGridData[l][i];
				m_mosGridData[l][i] = 0x0;
			}
			delete[] m_mosGridData[l];
			m_mosGridData[l] = 0x0;
		}
		delete[]m_mosGridData;
		m_mosGridData = 0x0;
	}

	if (m_pHeights != 0)
	{
		delete[]m_pHeights;
		m_pHeights = 0x0;
	}

	ClearGridPosInSites();
}

int32_t CGridMosaicCls::Run()
{
	if (!LoadParameters())
	{
		ReportLog_Error("CGridMosaicCls--Failed to load parameters!");
		return -1;
	}

	InitMosGrid();
	InitGridPosInSites();
	//*
	if (DoMultiRadarsMosaic() < 0)
	{
		ReportLog_Error("CGridMosaicCls--Failed to mosaic!");
		return -1;
	}//*/
	ClearGridPosInSites();

	if (!SaveGridData())
	{
		ReportLog_Error("CGridMosaicCls--Failed to output mosaic gird data file!");
		return -1;
	}
	return 1;
}

bool CGridMosaicCls::LoadParameters()
{
	m_paramAlgThrRng = 50;//

	m_paramGridLenX = g_MosaicCommParam.iLonRes / 10000.;
	m_paramGridLenY = g_MosaicCommParam.iLatRes / 10000.;
	m_paramNumX = g_MosaicCommParam.iLonNumGrids;
	m_paramNumY = g_MosaicCommParam.iLatNumGrids;
	m_paramMinX = g_MosaicCommParam.iStartLon / 1000.;
	m_paramMinY = g_MosaicCommParam.iStartLat / 1000.;
	m_paramMaxX = m_paramMinX + m_paramNumX * m_paramGridLenX;
	m_paramMaxY = m_paramMinY + m_paramNumY * m_paramGridLenY;

	m_paramNumZ = g_MosaicCommParam.iHgtNumLevels;
	m_pHeights = new uint16_t[m_paramNumZ]; //m	
	if (m_pHeights == 0x0)
	{
		ReportLog_Error("Failed to allocate memory for single-3D data");
		return false;
	}
	for (int32_t i = 0; i < m_paramNumZ; i++)
		m_pHeights[i] = g_MosaicCommParam.iHeights[i];

	//输出数据文件夹
	m_paramProdDir.assign(g_DataDir.strProdDataDir);
	std::experimental::filesystem::create_directories(m_paramProdDir);

	m_paramProdDir +=MOSAIC_SUBFOLDER;

	//g_SiteRangeInMosaic
	m_siteGridPosInMosaic.resize(m_numValidSite);
	for (int ns = 0; ns < m_numValidSite; ns++)
	{
		int32_t id = m_siteInfo[ns].SiteID;
		for (int nsg = 0; nsg < g_iSitesInZone; nsg++)
		{
			if (m_siteInfo[ns].SiteID == g_cRadarSiteInfo[nsg].SiteID)
			{
				m_siteGridPosInMosaic[ns] = g_SiteRangeInMosaic[nsg];
				break;
			}
		}
	}
	return true;
}

void CGridMosaicCls::InitMosGrid()
{
	//获取直角坐标数据存放空间,并初始化
	m_mosGridData = new  uint8_t**[m_paramNumZ];
	if (m_mosGridData == 0)
	{
		ReportLog_Error("Failed to allocate memory for single-3D data");
		return;
	}
	for (int nz = 0; nz < m_paramNumZ; nz++)
	{
		m_mosGridData[nz] = new uint8_t *[m_paramNumY];
		if (m_mosGridData[nz] == 0)
		{
			ReportLog_Error("Failed to allocate memory for single-3D data");
			return;
		}
		for (int ny = 0; ny < m_paramNumY; ny++)
		{
			m_mosGridData[nz][ny] = new uint8_t[m_paramNumX];
			if (m_mosGridData[nz][ny] == 0)
			{
				ReportLog_Error("Failed to allocate memory for single-3D data");
				return;
			}
			for (int nx = 0; nx < m_paramNumX; nx++)
				m_mosGridData[nz][ny][nx] = CODE_NO_OBSERV;
		}
	}
}

void CGridMosaicCls::InitGridPosInSites()
{
	if (m_numValidSite == 0 || m_paramNumY == 0 || m_paramNumX == 0 || m_pHeights == 0)
		return;

	int nx, ny, ns;

	//坐标在单站中位置参数
	m_mosGridPosInSite.resize(m_paramNumY);
	for (ny = 0; ny < m_paramNumY; ny++)
	{
		m_mosGridPosInSite[ny].resize(m_paramNumX);
		for (nx = 0; nx < m_paramNumX; nx++)
		{
			m_mosGridPosInSite[ny][nx].resize(0);
		}
	}

	//Init
	for (ns = 0; ns < m_numValidSite; ns++)
	{
		//单站网格中心(即雷达站)在组网网格中的坐标位置
		int sLonGrid = (m_siteGridPosInMosaic[ns].iMaxLonGrid + m_siteGridPosInMosaic[ns].iMinLonGrid) / 2;
		int sLatGrid = (m_siteGridPosInMosaic[ns].iMaxLatGrid + m_siteGridPosInMosaic[ns].iMinLatGrid) / 2;

		for (ny = 0; ny < m_paramNumY; ny++)
		{
			if (ny < m_siteGridPosInMosaic[ns].iMinLatGrid || ny >= m_siteGridPosInMosaic[ns].iMaxLatGrid)
				continue;
			for (int nx = 0; nx < m_paramNumX; nx++)
			{
				if (nx < m_siteGridPosInMosaic[ns].iMinLonGrid || nx >= m_siteGridPosInMosaic[ns].iMaxLonGrid)
					continue;

				//计算这一点在单站网格中的位置
				stdPosInSiteGrid apos;
				apos.sIdx = ns;
				apos.xPos = nx - m_siteGridPosInMosaic[ns].iMinLonGrid;//单站中的坐标位置
				apos.yPos = ny - m_siteGridPosInMosaic[ns].iMinLatGrid;
				double fxR = abs(nx - sLonGrid)*m_paramGridLenX / KM_TO_DEG; //距雷达的x.y距离
				double fyR = abs(ny - sLatGrid)*m_paramGridLenY / KM_TO_DEG;
				apos.sRng = (float)sqrt(fxR * fxR + fyR * fyR);//  
				apos.sHt = 0;
				m_mosGridPosInSite[ny][nx].push_back(apos);
			}
		}
	}
}

//
int16_t CGridMosaicCls::LoadSiteGridFileAndHeader(int16_t numFile, std::vector<std::string>  &pFilesNameList)
{
	m_numValidSite = 0;
	m_siteGirdFiles.resize(0);
	//m_fpSiteGrid.resize(0);
	m_siteInfo.resize(0);
	m_siteGridHeader.resize(0);

	for (int32_t i = 0; i < numFile; i++)
	{
		BaseDataInfo bBaseData;
		//	char tempFile[PATH_LEN]="";
		strncpy(bBaseData.strFilePath, pFilesNameList[i].c_str(), pFilesNameList[i].length() + 1);
		if (_access(bBaseData.strFilePath, 4) == -1)
			continue;

		FILE *fp = 0;
		errno_t err = 0;
		err = fopen_s(&fp, bBaseData.strFilePath, "rb");
		if (err != 0)
			continue;

		MOSAICDATAHEADER aHeader;			//单站数据文件头		
		if (fread(&aHeader, sizeof(aHeader), 1, fp) < 1)
		{
			fclose(fp);
			continue;
		}

		RADARSITEINFO aSiteInfo;
		if (fread(&aSiteInfo, SIZE_RADAR_INFO, 1, fp) < 1)
		{
			fclose(fp);
			continue;
		}
		fclose(fp);
		fp = 0x0;

		m_siteGridHeader.push_back(aHeader);
		m_siteInfo.push_back(aSiteInfo);
		m_siteGirdFiles.push_back(bBaseData);

		m_numValidSite++;
	}
	return (m_numValidSite);
}

#include <omp.h>
int CGridMosaicCls::DoMultiRadarsMosaic()
{
	int nx, ny, nz, ns, ns1;
	int32_t res = -1; //

	//单站参数
	char	chOneFileName[PATH_LEN] = "  ";
	int16_t iDataStat = 0;  //Flag indecating which levels data		
	bool bND_Flag = false, bFold_flag = false, b_NB_FLag = false, b_CLT_flag = false;
	double	sumWeight = 0., afWeight = 0.;
	float	sumValue = 0.;
	int		overlapCAPPINum = 0;
	bool    bWithinRadarRange = false; //
	stdPosInSiteGrid apos;
	uint8_t Vp;
	//
	typedef std::vector<uint8_t> vct_byteCodeLine;
	typedef std::vector<vct_byteCodeLine> vct_byteCodeGrid;
	typedef std::vector<vct_byteCodeGrid> vct_byteSitesGrid; //所有站的单层数据
	vct_byteSitesGrid iLevelOfSites;

	//--------------------------------------------------------------------------------------
	//return; //Test 本行测试，用于测试时跳过以下实际执行部分

	if (m_numValidSite == 0)
		return res;
	if (m_mosGridData == 0x0)
		return res;

	//Allocate memory block for site information
	m_pFiles.resize(m_numValidSite, 0x0);
	for (ns = 0; ns < m_numValidSite; ns++)
	{
		//Skip to position of obsv. data in the grid file
		errno_t err = 0;
		err = fopen_s(&m_pFiles[ns], m_siteGirdFiles[ns].strFilePath, "rb");
		if (err != 0)
		{
			//Write Log 
			ReportLog_Error("Mosaic: failed to open single-3D data");
			return res;
		}
		fseek(m_pFiles[ns], sizeof(MOSAICDATAHEADER) + m_siteGridHeader[ns].dwObvDataOffset, SEEK_SET); //skip header
	}

	////申请内存空间保存从文件中读出的单站3D数据
	iLevelOfSites.resize(m_numValidSite);
	//#pragma omp parallel for //private
	for (ns = 0; ns < iLevelOfSites.size(); ns++)
	{
		iLevelOfSites[ns].resize(m_siteGridHeader[ns].iLatGrids);
		//#pragma omp parallel for
		for (ny = 0; ny < m_siteGridHeader[ns].iLatGrids; ny++)
			iLevelOfSites[ns][ny].resize(m_siteGridHeader[ns].iLonGrids, CODE_CSRED);
	}

	//计算组网数据并写入文件中
	for (nz = 0; nz < m_paramNumZ; nz++)
	{//高度循环

		//获取这层的单站雷达数据
//#pragma omp parallel for
		for (ns = 0; ns < m_numValidSite; ns++)
		{
			for (ny = 0; ny < m_siteGridHeader[ns].iLatGrids; ny++)
			{
				if (fread(&iLevelOfSites[ns][ny][0], m_siteGridHeader[ns].iLonGrids * sizeof(uint8_t), 1, m_pFiles[ns]) < 1)
				{
					ReportLog_Error("Mosaic: failed to open single-3D data");
					return res;
				}
			}
		}

		//水平组网开始
//#pragma omp parallel for
		for (ny = 0; ny < m_paramNumY; ny++)
		{
			//radLat = DataHeader.iFstLat/1000. + j*DataHeader.iLatRes/10000.;
//#pragma omp parallel for //private(k)		
			for (nx = 0; nx < m_paramNumX; nx++)
			{
				//radLon = DataHeader.iFstLon/1000. + k*DataHeader.iLonRes/10000.;
				sumWeight = 0.;
				afWeight = 0.;
				sumValue = 0.;
				overlapCAPPINum = 0;
				bWithinRadarRange = false; //
				bND_Flag = bFold_flag = b_NB_FLag = b_CLT_flag = FALSE;

				for (ns1 = 0; ns1 < m_mosGridPosInSite[ny][nx].size(); ns1++)
				{
					apos = m_mosGridPosInSite[ny][nx][ns1];
					Vp = iLevelOfSites[apos.sIdx][apos.yPos][apos.xPos];
					if (Vp != CODE_NO_OBSERV)
						bWithinRadarRange = true;

					if (Vp > CODE_CSRED)//CODE_NO_OBSERV)
					{																																									  //根据斜距计算权重
						afWeight = exp(-1.*apos.sRng * apos.sRng / (m_paramAlgThrRng*m_paramAlgThrRng));
						//这点的数值在组网坐标中的贡献
						sumValue += float(afWeight*(Vp / 2. - 33));   //from code to dbz							
						sumWeight += afWeight;

						//有单站数据对组网坐标中的这一点作出贡献，计数器累加
						overlapCAPPINum++;
					} //bND_Flag, bFold_flag, b_NB_FLag
					else if (Vp == CODE_INVALID) bND_Flag = true;
					else if (Vp == CODE_RANFOLD) bFold_flag = true;
					else if (Vp == CODE_NO_OBSERV) b_NB_FLag = true;
					else if (Vp == CODE_CSRED) b_CLT_flag = true;/**/
				}
				//Calculate value for this combined 3D grid point				
				if (overlapCAPPINum > 0)
				{//至少有一个站有有效数据可供这一点使用					
					m_mosGridData[nz][ny][nx] = uint8_t((sumValue / sumWeight)*2. + 66. + .5);  //dBZ to code
				}
				else if (bWithinRadarRange) //Within radar observation range, but no valid data
				{
					if (b_CLT_flag)
						m_mosGridData[nz][ny][nx] = CODE_CSRED;
					else if (bFold_flag)
						m_mosGridData[nz][ny][nx] = CODE_RANFOLD;
					else
						//没有数据可用，填写无效值
						m_mosGridData[nz][ny][nx] = CODE_INVALID;
				}
				else
					m_mosGridData[nz][ny][nx] = CODE_NO_OBSERV;
			}//X
		}//Y
	}//Z

	for (ns = 0; ns < m_pFiles.size(); ns++)
	{
		if (m_pFiles[ns] != 0x0)
		{
			fclose(m_pFiles[ns]);
			m_pFiles[ns] = 0x0;
		}
	}
	for (ns = 0; ns < iLevelOfSites.size(); ns++)
	{
		for (ny = 0; ny < iLevelOfSites[ns].size(); ny++)
		{
			iLevelOfSites[ns][ny].clear();
			vct_byteCodeLine().swap(iLevelOfSites[ns][ny]);
		}
		iLevelOfSites[ns].clear();
		vct_byteCodeGrid().swap(iLevelOfSites[ns]);
	}
	iLevelOfSites.clear();
	vct_byteSitesGrid().swap(iLevelOfSites);

	res = 1;
	return res;
}

//#include "SaveAsNetCDF.h"
bool CGridMosaicCls::SaveGridData()
{
	std::experimental::filesystem::create_directories(m_paramProdDir);

	//保存笛卡儿坐标数据- 
	MOSAICDATAHEADER DataHeader;
	ZeroMemory(&DataHeader, sizeof(DataHeader));
	DataHeader.iVersion = 1;
	strcpy(DataHeader.strZoneName, g_strZoneName);//, strlen(g_strZoneName)+1);
	//strcpy(DataHeader.strZoneName, m_ParamSiteInfo.SiteName);
	DataHeader.iNumRadars = m_numValidSite;
	strcpy(DataHeader.strDataType, "Z");
	DataHeader.iStYear = m_mosaicDateTime.year;
	DataHeader.iStMonth = m_mosaicDateTime.mon;
	DataHeader.iStDay = m_mosaicDateTime.day;
	DataHeader.iStHour = m_mosaicDateTime.hor;
	DataHeader.iStMinute = m_mosaicDateTime.min;
	DataHeader.iFstLon = int32_t(m_paramMinX * 1000);
	DataHeader.iFstLat = int32_t(m_paramMinY * 1000);
	DataHeader.iLstLon = int32_t(m_paramMaxX * 1000);
	DataHeader.iLstLat = int32_t(m_paramMaxY * 1000);
	DataHeader.iFstHgt = m_pHeights[0];
	DataHeader.iLstHgt = m_pHeights[m_paramNumZ - 1];
	DataHeader.iLonGrids = m_paramNumX;
	DataHeader.iLatGrids = m_paramNumY;
	DataHeader.iLevels = m_paramNumZ;
	DataHeader.iLonRes = uint32_t(m_paramGridLenX * 10000 + .5);  //.g_MosaicCommParam.iLonRes;
	DataHeader.iLatRes = uint32_t(m_paramGridLenY * 10000 + .5);  //g_MosaicCommParam.iLatRes;

	//设置垂直分辨率
	//如果垂直分辨率相等,则直接写入垂直分辨率
	//如果垂直分辨率不相等,则该处设为-1
	int32_t ns, ny, nx, nz = 1;
	do
	{
		if (m_pHeights[nz + 1] - m_pHeights[nz] != m_pHeights[nz] - m_pHeights[nz - 1])
		{
			DataHeader.iHgtRes = -1;
			break;
		}
		nz++;
	} while (nz < m_paramNumZ - 1);
	if (DataHeader.iHgtRes != -1) DataHeader.iHgtRes = m_pHeights[0];

	//设置雷达站信息, 层高度, 以及观测或结果数据开始位置(以数据头末尾为准)
	DataHeader.dwRadInfoOffset = 0;
	DataHeader.dwLevHgtOffset = DataHeader.dwRadInfoOffset + DataHeader.iNumRadars * sizeof(RADARSITEINFO);
	DataHeader.dwObvDataOffset = DataHeader.dwLevHgtOffset + sizeof(uint16_t)*DataHeader.iLevels;

	//Set file name
	std::string strYear = std::to_string(DataHeader.iStYear);
	std::string strMonth = std::to_string(DataHeader.iStMonth + 100);
	std::string strDay = std::to_string(DataHeader.iStDay + 100);
	std::string strHour = std::to_string(DataHeader.iStHour + 100);
	std::string strMinute = std::to_string(DataHeader.iStMinute + 100);

	m_mosGridFileName = m_paramProdDir;
	m_mosGridFileName += std::experimental::filesystem::path::preferred_separator;
	m_mosGridFileName += strYear;
	m_mosGridFileName.append(strMonth.data() + 1);
	m_mosGridFileName.append(strDay.data() + 1);
	m_mosGridFileName.append(strHour.data() + 1);
	m_mosGridFileName.append(strMinute.data() + 1);
	m_mosGridFileName.append(".");
	m_mosGridFileName.append(DataHeader.strDataType);
	m_mosGridFileName.append(MOSAIC_EXT_FILE);

	FILE *fp = 0;
	errno_t err = fopen_s(&fp, m_mosGridFileName.c_str(), "wb");
	if (err != 0)
		return (false);

	//Write data header into the file
	if (fwrite(&DataHeader, sizeof(DataHeader), 1, fp) < 1)
	{
		fclose(fp);
		return false;
	}

	for (int ns = 0; ns < m_numValidSite; ns++)
		if (fwrite(&(m_siteInfo[ns]), sizeof(RADARSITEINFO), 1, fp) < 1)
		{
			fclose(fp);
			return false;
		}
	if (fwrite(m_pHeights, sizeof(uint16_t), m_paramNumZ, fp) < (size_t)m_paramNumZ)
	{
		fclose(fp);
		return false;
	}
	for (nz = 0; nz < m_paramNumZ; nz++)
	{
		//Write data at thei height into the file
		for (ny = 0; ny < m_paramNumY; ny++)
			if (fwrite(m_mosGridData[nz][ny], sizeof(uint8_t), m_paramNumX, fp) < (size_t)m_paramNumX)
			{
				fclose(fp);
				return false;
			}
	}
	fclose(fp);
	fp = 0x0;


	//判断是否需要生成NetCDF格式的产品数据(*.nc)
	if (g_iSaveNetCDFData != 1)
		return true;

	std::string chNetCdfFileName=  m_paramProdDir;
	chNetCdfFileName += std::experimental::filesystem::path::preferred_separator;

	chNetCdfFileName += strYear;
	chNetCdfFileName.append(strMonth.data() + 1);
	chNetCdfFileName.append(strDay.data() + 1);
	chNetCdfFileName.append(strHour.data() + 1);
	chNetCdfFileName.append(strMinute.data() + 1);
	chNetCdfFileName += ".";
	chNetCdfFileName.append(DataHeader.strDataType);
	chNetCdfFileName.append(NETCDF_EXT_FILE);

	//MyNetCDF LocalMyNetCdf(m_mosGridFileName, chNetCdfFileName);
	//LocalMyNetCdf.ConvertToNcfile();

	return true;
}

const char* CGridMosaicCls::GetProdFileName()
{
	if (!std::experimental::filesystem::exists(m_mosGridFileName)) {
		m_mosGridFileName ="";
	}

	return m_mosGridFileName.c_str();
}

void CGridMosaicCls::ClearTempDatas()
{
	//判断是否需要删除单站三维格点数据
	if (g_iSaveSingleCAPPI != 1)
	{
		//Delete the single 3D radar data file
		for (int32_t ns = 0; ns < m_numValidSite; ns++)
		{
			TCHAR wszChar[PATH_LEN] = _TEXT("");
			CharToTchar(m_siteGirdFiles[ns].strFilePath, wszChar);
			//::DeleteFile(wszChar);// m_siteGirdFiles[ns].strFilePath); //mxg
		}
	}
}

void CGridMosaicCls::ClearGridPosInSites()
{
	int nx, ny;
	for (ny = 0; ny < m_mosGridPosInSite.size(); ny++)
	{
		for (nx = 0; nx < m_mosGridPosInSite[ny].size(); nx++)
		{
			m_mosGridPosInSite[ny][nx].clear();
			vct_sitelyPos().swap(m_mosGridPosInSite[ny][nx]);
		}
		m_mosGridPosInSite[ny].clear();
		vct_mosAlongXPos().swap(m_mosGridPosInSite[ny]);
	}
	m_mosGridPosInSite.clear();
	vct_mosAlongXYPos().swap(m_mosGridPosInSite);
}

//#include "WindRetrievalProc.h"
//Do something for retriving 3D wind
void CGridMosaicCls::PrepareWindRetrival()
{
	//	
	//if(g_iOptionsGridData==GRIDDATA_OPTION_ALL || g_iOptionsGridData==GRIDDATA_OPTION_VEL)
	//	StartupWindRetrievialProc(tempNumRadar,strYear,strMonth,strDay,strHour,strMinute);//风场相关处理
	//if(g_iOptionsGridData!=GRIDDATA_OPTION_ALL && g_iOptionsGridData !=GRIDDATA_OPTION_REF)
	//	return;//没有数据需要组网处理 直接返回
}

