// GridMosaicCls.cpp: implementation of the CGridMosaicCls class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "radarmosaic.h"
#include "GridMosaicCls.h"
#include "CommFunctions.h"
#include "GlobalParams.h"
//#include "ThreadLog.h"
#include "CommVar.h"
#include <math.h>
#include <io.h>
#include <omp.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//#pragma warning (disable:  C4786)
//#pragma pack(1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridMosaicCls::CGridMosaicCls(int16_t numFile, vector<string>  &dataFileName, stdDateTime *pMosaicDateTime)
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
	strcpy_s(m_paramProdDir, 2, "");
	strcpy_s(m_mosGridFileName, 2, "");  //����ļ�

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
	vector<BaseDataInfo>().swap(m_siteGirdFiles);
	m_siteGridHeader.clear();
	vector<MOSAICDATAHEADER>().swap(m_siteGridHeader);
	m_siteInfo.clear();
	vector<RADARSITEINFO>().swap(m_siteInfo);
	m_siteGridPosInMosaic.clear();
	vector<SITEPOSINMOSAIC>().swap(m_siteGridPosInMosaic);

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

	//��������ļ���
	strcpy_s(m_paramProdDir, strlen(g_DataDir.strProdDataDir) + 1, g_DataDir.strProdDataDir);
	CreateDir(m_paramProdDir);
	strcat_s(m_paramProdDir, MOSAIC_SUBFOLDER);

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
	//��ȡֱ���������ݴ�ſռ�,����ʼ��
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

	//�����ڵ�վ��λ�ò���
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
		//��վ��������(���״�վ)�����������е�����λ��
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

				//������һ���ڵ�վ�����е�λ��
				stdPosInSiteGrid apos;
				apos.sIdx = ns;
				apos.xPos = nx - m_siteGridPosInMosaic[ns].iMinLonGrid;//��վ�е�����λ��
				apos.yPos = ny - m_siteGridPosInMosaic[ns].iMinLatGrid;
				double fxR = abs(nx - sLonGrid)*m_paramGridLenX / KM_TO_DEG; //���״��x.y����
				double fyR = abs(ny - sLatGrid)*m_paramGridLenY / KM_TO_DEG;
				apos.sRng = (float)sqrt(fxR * fxR + fyR * fyR);//  
				apos.sHt = 0;
				m_mosGridPosInSite[ny][nx].push_back(apos);
			}
		}
	}
}

//
int16_t CGridMosaicCls::LoadSiteGridFileAndHeader(int16_t numFile, vector<string>  &pFilesNameList)
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
		strcpy_s(bBaseData.strFilePath, pFilesNameList[i].length() + 1, pFilesNameList[i].c_str());
		if (_access(bBaseData.strFilePath, 4) == -1)
			continue;

		FILE *fp = 0;
		errno_t err = 0;
		err = fopen_s(&fp, bBaseData.strFilePath, "rb");
		if (err != 0)
			continue;

		MOSAICDATAHEADER aHeader;			//��վ�����ļ�ͷ		
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

	//��վ����
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
	typedef vector<uint8_t> vct_byteCodeLine;
	typedef vector<vct_byteCodeLine> vct_byteCodeGrid;
	typedef vector<vct_byteCodeGrid> vct_byteSitesGrid; //����վ�ĵ�������
	vct_byteSitesGrid iLevelOfSites;

	//--------------------------------------------------------------------------------------
	//return; //Test ���в��ԣ����ڲ���ʱ��������ʵ��ִ�в���

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

	////�����ڴ�ռ䱣����ļ��ж����ĵ�վ3D����
	iLevelOfSites.resize(m_numValidSite);
	//#pragma omp parallel for //private
	for (ns = 0; ns < iLevelOfSites.size(); ns++)
	{
		iLevelOfSites[ns].resize(m_siteGridHeader[ns].iLatGrids);
		//#pragma omp parallel for
		for (ny = 0; ny < m_siteGridHeader[ns].iLatGrids; ny++)
			iLevelOfSites[ns][ny].resize(m_siteGridHeader[ns].iLonGrids, CODE_CSRED);
	}

	//�����������ݲ�д���ļ���
	for (nz = 0; nz < m_paramNumZ; nz++)
	{//�߶�ѭ��

		//��ȡ���ĵ�վ�״�����
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

		//ˮƽ������ʼ
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
					{																																									  //����б�����Ȩ��
						afWeight = exp(-1.*apos.sRng * apos.sRng / (m_paramAlgThrRng*m_paramAlgThrRng));
						//������ֵ�����������еĹ���
						sumValue += float(afWeight*(Vp / 2. - 33));   //from code to dbz							
						sumWeight += afWeight;

						//�е�վ���ݶ����������е���һ���������ף��������ۼ�
						overlapCAPPINum++;
					} //bND_Flag, bFold_flag, b_NB_FLag
					else if (Vp == CODE_INVALID) bND_Flag = true;
					else if (Vp == CODE_RANFOLD) bFold_flag = true;
					else if (Vp == CODE_NO_OBSERV) b_NB_FLag = true;
					else if (Vp == CODE_CSRED) b_CLT_flag = true;/**/
				}
				//Calculate value for this combined 3D grid point				
				if (overlapCAPPINum > 0)
				{//������һ��վ����Ч���ݿɹ���һ��ʹ��					
					m_mosGridData[nz][ny][nx] = uint8_t((sumValue / sumWeight)*2. + 66. + .5);  //dBZ to code
				}
				else if (bWithinRadarRange) //Within radar observation range, but no valid data
				{
					if (b_CLT_flag)
						m_mosGridData[nz][ny][nx] = CODE_CSRED;
					else if (bFold_flag)
						m_mosGridData[nz][ny][nx] = CODE_RANFOLD;
					else
						//û�����ݿ��ã���д��Чֵ
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
	CreateDir(m_paramProdDir);

	//����ѿ�����������- 
	MOSAICDATAHEADER DataHeader;
	ZeroMemory(&DataHeader, sizeof(DataHeader));
	DataHeader.iVersion = 1;
	strcpy_s(DataHeader.strZoneName, g_strZoneName);//, strlen(g_strZoneName)+1);
	//strcpy(DataHeader.strZoneName, m_ParamSiteInfo.SiteName);
	DataHeader.iNumRadars = m_numValidSite;
	strcpy_s(DataHeader.strDataType, "Z");
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

	//���ô�ֱ�ֱ���
	//�����ֱ�ֱ������,��ֱ��д�봹ֱ�ֱ���
	//�����ֱ�ֱ��ʲ����,��ô���Ϊ-1
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

	//�����״�վ��Ϣ, ��߶�, �Լ��۲�������ݿ�ʼλ��(������ͷĩβΪ׼)
	DataHeader.dwRadInfoOffset = 0;
	DataHeader.dwLevHgtOffset = DataHeader.dwRadInfoOffset + DataHeader.iNumRadars * sizeof(RADARSITEINFO);
	DataHeader.dwObvDataOffset = DataHeader.dwLevHgtOffset + sizeof(uint16_t)*DataHeader.iLevels;

	//Set file name
	char strYear[6] = "", strMonth[4] = "", strDay[4] = "", strHour[4] = "", strMinute[4] = "";
	_itoa_s(DataHeader.iStYear, strYear, 10);
	_itoa_s(DataHeader.iStMonth + 100, strMonth, 10);
	_itoa_s(DataHeader.iStDay + 100, strDay, 10);
	_itoa_s(DataHeader.iStHour + 100, strHour, 10);
	_itoa_s(DataHeader.iStMinute + 100, strMinute, 10);

	strcpy_s(m_mosGridFileName, m_paramProdDir);
	strcat_s(m_mosGridFileName, SZ_SLASH);
	strcat_s(m_mosGridFileName, strYear);
	strcat_s(m_mosGridFileName, strMonth + 1);
	strcat_s(m_mosGridFileName, strDay + 1);
	strcat_s(m_mosGridFileName, strHour + 1);
	strcat_s(m_mosGridFileName, strMinute + 1);
	//strcat(m_mosGridFileName,".");
	//strcat(m_mosGridFileName,DataHeader.strDataType);	
	strcat_s(m_mosGridFileName, MOSAIC_EXT_FILE);

	FILE *fp = 0;
	errno_t err = fopen_s(&fp, m_mosGridFileName, "wb");
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


	//�ж��Ƿ���Ҫ����NetCDF��ʽ�Ĳ�Ʒ����(*.nc)
	if (g_iSaveNetCDFData != 1)
		return true;

	char chNetCdfFileName[PATH_LEN] = "";
	strcpy_s(chNetCdfFileName, m_paramProdDir);
	strcat_s(chNetCdfFileName, SZ_SLASH);
	strcat_s(chNetCdfFileName, strYear);
	strcat_s(chNetCdfFileName, strMonth + 1);
	strcat_s(chNetCdfFileName, strDay + 1);
	strcat_s(chNetCdfFileName, strHour + 1);
	strcat_s(m_mosGridFileName, strMinute + 1);
	strcat_s(chNetCdfFileName, ".");
	strcat_s(chNetCdfFileName, DataHeader.strDataType);
	strcat_s(chNetCdfFileName, NETCDF_EXT_FILE);

	//MyNetCDF LocalMyNetCdf(m_mosGridFileName, chNetCdfFileName);
	//LocalMyNetCdf.ConvertToNcfile();

	return true;
}

char* CGridMosaicCls::GetProdFileName()
{
	if (!FilePathExists(m_mosGridFileName))
		strcpy_s(m_mosGridFileName, "");

	return (m_mosGridFileName);
}

void CGridMosaicCls::ClearTempDatas()
{
	//�ж��Ƿ���Ҫɾ����վ��ά�������
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
	//	StartupWindRetrievialProc(tempNumRadar,strYear,strMonth,strDay,strHour,strMinute);//�糡��ش���
	//if(g_iOptionsGridData!=GRIDDATA_OPTION_ALL && g_iOptionsGridData !=GRIDDATA_OPTION_REF)
	//	return;//û��������Ҫ�������� ֱ�ӷ���
}

