// PolarToGridCls.cpp: implementation of the CPolarToGridCls class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "radarmosaic.h"
#include "MosaicDataFormatDef.h"
#include "PolarToGridCls.h"
#include "CommFunctions.h"
#include "GlobalParams.h"
//#include "ThreadLog.h"
#include <math.h>
#include <omp.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#pragma pack(1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPolarToGridCls::CPolarToGridCls()
{
	m_siteCode=0;
	m_numCuts=0;
	m_pPPIs=0;
	m_GridData=0;
	strcpy_s(m_gridFileName,"");
	//strcpy_s(m_velGridFileName,"");

	memset(&( m_ParamSiteInfo),0x0, sizeof(RADARSITEINFO));
	m_paramNumX=0;
	m_paramNumY=0;
	m_paramGridLenX=0.;
	m_paramGridLenY=0.;
	m_paramNumZ=0;
	m_pHeights=0x0; //m	
	strcpy_s(m_paramProdDir,"");
	strcpy_s(m_gridFileName,"");  //����ļ�

	m_paramBeamWidth=float(0.95);
	
	m_year=0;
	m_month=0;
	m_day=0;
	m_hour=0;
	m_minute=0;
}

CPolarToGridCls::CPolarToGridCls(int32_t sitecode, int32_t numcuts, LPBYTE pPolarData)
{
	m_GridData=0;
	m_siteCode=sitecode;
	m_numCuts=numcuts;
	m_pPPIs=(PPISTRUCT*)pPolarData;	
	
	strcpy_s(m_gridFileName,"");
	//strcpy_s(m_velGridFileName,"");

	memset(&( m_ParamSiteInfo),0x0, sizeof(RADARSITEINFO));
	m_paramNumX=0;
	m_paramNumY=0;
	m_paramGridLenX=0.;
	m_paramGridLenY=0.;
	m_paramNumZ=0;
	m_pHeights=0x0; //m	
	strcpy_s(m_paramProdDir,"");
	strcpy_s(m_gridFileName,"");  //����ļ�

	m_paramBeamWidth=float(0.95);	
	m_year=0;
	m_month=0;
	m_day=0;
	m_hour=0;
	m_minute=0;
}

CPolarToGridCls::CPolarToGridCls(int32_t sitecode,char* pUniformBaseDataFile)
{
	m_siteCode=sitecode;
	m_pPPIs=0x0;
	m_GridData=0;
	//LoadUniformPolarData(pUniformBaseDataFile);
	LoadUniformPolarData(pUniformBaseDataFile, m_commScanData);
	
	strcpy_s(m_gridFileName,"");

	memset(&( m_ParamSiteInfo),0x0, sizeof(RADARSITEINFO));
	m_paramNumX=0;
	m_paramNumY=0;
	m_paramGridLenX=0.;
	m_paramGridLenY=0.;
	m_paramNumZ=0;
	m_pHeights=0x0; //m	
	strcpy_s(m_paramProdDir,"");
	strcpy_s( m_gridFileName,"");  //����ļ�

	m_paramBeamWidth=float(0.95);
	
	m_year=0;
	m_month=0;
	m_day=0;
	m_hour=0;
	m_minute=0;
}

CPolarToGridCls::~CPolarToGridCls()
{
	if(m_GridData!=0x0)
	{
		for(int32_t l=0; l<m_paramNumZ; l++)
		{
			if(m_GridData[l]==0x0) continue;

			for(int32_t i=0; i<m_paramNumY; i++)
			{
				if(m_GridData[l][i]==0) continue;

				delete [] m_GridData[l][i];
				m_GridData[l][i]=0x0;
			}
			delete [] m_GridData[l];
			m_GridData[l]=0x0;
		}
		delete []m_GridData;
		m_GridData=0x0;
	}

	if(m_pHeights!=0)
	{
		delete []m_pHeights;
		m_pHeights=0x0;
	}

	if(m_pPPIs!=0)
	{
		delete[]m_pPPIs;
		m_pPPIs=0x0;
	}
}

void CPolarToGridCls::LoadUniformPolarData(char* pUniformBaseDataFile)
{
	FILE *fp = 0;
	errno_t err=fopen_s(&fp,pUniformBaseDataFile, "rb");
	if(fp==0)
		return;
	
	fread_s(&m_numCuts,sizeof(m_numCuts),sizeof(m_numCuts),1,fp);
	fread_s(&m_numRadials,sizeof(m_numRadials),sizeof(m_numRadials),1,fp);
	fread_s(&m_numGates,sizeof(m_numGates),sizeof(m_numGates),1,fp);

	m_pPPIs=new PPISTRUCT[m_numCuts];
	if(m_pPPIs==0x0)
		return;
	for(int32_t i=0; i<m_numCuts; i++)
		fread_s(&m_pPPIs[i],sizeof(m_pPPIs[i]),SIZE_PPI,1,fp);
	fclose(fp);
	fp=0x0;

	ClearTempDatas(pUniformBaseDataFile);

}

void CPolarToGridCls::LoadUniformPolarData(char* pUniformBaseDataFile, stdUniformScanData &ScanData)
{
	FILE *fp = 0;
	errno_t err = fopen_s(&fp, pUniformBaseDataFile, "rb");
	if (fp == 0)
		return;

	fread_s(&m_numCuts, sizeof(uint16_t), sizeof(uint16_t), 1, fp);
	if(fread_s(&ScanData.header, sizeof(stdUniformScanHeader), sizeof(stdUniformScanHeader), 1, fp)<1)
	{
		fclose(fp);
		return;
	}
	ScanData.CutsData.resize(m_numCuts);
	for (int nc = 0; nc < m_numCuts; nc++)
	{
		stdUniformCutHeader *pCutHeader = &(ScanData.CutsData[nc].header);
		if (fread_s(pCutHeader, sizeof(stdUniformCutHeader), sizeof(stdUniformCutHeader), 1, fp)<1)
		{
			fclose(fp);
			return;
		}
		ScanData.CutsData[nc].azim.resize(pCutHeader->num_beam );
		ScanData.CutsData[nc].elev.resize(pCutHeader->num_beam);
		ScanData.CutsData[nc].field.resize(pCutHeader->num_beam);
		for (int na = 0; na < pCutHeader->num_beam; na++)
			ScanData.CutsData[nc].field[na].resize(pCutHeader->num_gate);
		if (fread_s(&(ScanData.CutsData[nc].elev[0]), sizeof(uint16_t)*pCutHeader->num_beam, sizeof(uint16_t)*pCutHeader->num_beam, 1, fp)<1)
		{
			fclose(fp);
			return;
		}
		if (fread_s(&(ScanData.CutsData[nc].azim[0]), sizeof(uint16_t)*pCutHeader->num_beam, sizeof(uint16_t)*pCutHeader->num_beam, 1, fp)<1)
		{
			fclose(fp);
			return;
		}
		for (int na = 0; na < pCutHeader->num_beam; na++)
		{
			if (fread_s(&(ScanData.CutsData[nc].field[na][0]), sizeof(uint16_t)*pCutHeader->num_gate, sizeof(uint16_t)*pCutHeader->num_gate, 1, fp) < 1)
			{
				fclose(fp);
				return;
			}
		}
	}
	fclose(fp);
	fp = 0x0;

	ClearTempDatas(pUniformBaseDataFile);
}

void CPolarToGridCls::ClearTempDatas(char *fileName)
{
	TCHAR wszChar[PATH_LEN] = _TEXT("");
	CharToTchar(fileName, wszChar);
//	::DeleteFile(wszChar); //mxg
}

void CPolarToGridCls::ClearTempDatas(TCHAR *fileName)
{
//	::DeleteFile(fileName); //mxg
}

void CPolarToGridCls::LoadParameters()
{
	//Site info.
	for(uint32_t i=0; i<g_iSitesInZone; i++)
	{
		if(m_siteCode==g_cRadarSiteInfo[i].SiteID)
		{
			m_ParamSiteInfo = g_cRadarSiteInfo[i];
			break;
		}
	}

	m_paramBeamWidth=float(0.95);
}

void  CPolarToGridCls::ReleaseTempDatBuffer()
{

	m_commScanData.ReleaseBuf();
}

bool CPolarToGridCls::InitGridData()
{
	//��ȡֱ���������ݴ�ſռ�,����ʼ��
 	m_GridData=new  uint8_t**[m_paramNumZ]; 
	if(m_GridData==0)
		return false;
	for(int32_t i=0; i<m_paramNumZ; i++)
	{
		m_GridData[i]=new uint8_t *[m_paramNumY];
		if(m_GridData[i]==0)
			return false;
		for(int32_t j=0; j<m_paramNumY; j++)
		{
			m_GridData[i][j]=new uint8_t[m_paramNumX];
			if(m_GridData[i][j]==0)
				return false;
			for(int32_t k = 0; k<m_paramNumX; k++)
				m_GridData[i][j][k] = CODE_NO_OBSERV;
		}
	}

	return true;
}

char* CPolarToGridCls::GetProdFileName()
{
	if(!FilePathExists(m_gridFileName))
		strcpy_s(m_gridFileName,"");
	return (m_gridFileName);
}

void CPolarToGridCls::ObsvDateTime()
{
	if(m_numCuts==NULL || m_pPPIs==0)
		return;

	m_year =  m_pPPIs->year;
	m_month=  m_pPPIs->month;
	m_day  =  m_pPPIs->day; 
	m_hour =  m_pPPIs->hour;
	m_minute= m_pPPIs->minute;
}

void CPolarToGridCls::ObsvDateTime(stdUniformCutData firstCut)
{
	if (m_numCuts == NULL )
		return;

	m_year =  firstCut.header.year;
	m_month = firstCut.header.month;
	m_day =   firstCut.header.day;
	m_hour =  firstCut.header.hour;
	m_minute= firstCut.header.minute;
}

float CPolarToGridCls::DecodeUniformPolarValue(int16_t src_value, int16_t factor, int16_t offset)
{
	float dst_value = 0.;

	if (src_value <= VALUE_INVALID)
		return (-99.);
	else
		return (float) (float(src_value - offset)*1. / factor);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Class
CPolarToGridOfRefCls::CPolarToGridOfRefCls()
{
}

CPolarToGridOfRefCls::~CPolarToGridOfRefCls()
{

}

int32_t CPolarToGridOfRefCls::Run1()//(stdCommScan vsScan)
{
	//��ɨ���ݿ�, ����������Ϊ��, ������ɨ���ݲ���ȷ
	if(m_numCuts==0 || m_commScanData.CutsData.size()==0)
	{
		ReportLog_Error("CPolarToGridOfRefCls-- No input volume scan data!");
		return (-1);
	}
	
	LoadParameters();

	if(!InitGridData())
	{
		ReportLog_Error("CPolarToGridOfRefCls--Failed to allocate memory for single-3D data!");
		return -1;
	}

	VolumnScanTo3DGrid(m_commScanData,m_numCuts,m_GridData,
		m_paramNumX,m_paramGridLenX,m_paramNumY,m_paramGridLenY,m_paramNumZ,m_pHeights,
		float(m_ParamSiteInfo.SiteLon/1000.),float(m_ParamSiteInfo.SiteLat/1000.),uint16_t(m_ParamSiteInfo.SiteHgt));

	ObsvDateTime(m_commScanData.CutsData[0]);

	if(!SaveGridData())
	{
		ReportLog_Error("CPolarToGridOfRefCls--Failed to output grid data file!");
		return -1;
	}

	return 1;
}

void  CPolarToGridOfRefCls::LoadParameters()
{
	//General parameter
	CPolarToGridCls::LoadParameters();
	
	//set local resolution of mosaic data
	m_paramGridLenX = double(g_MosaicCommParam.iLonRes / 10000.);//Degree
	m_paramGridLenY = double(g_MosaicCommParam.iLatRes / 10000.);//Degree

	//Parameter for dbz
	//Set grid number(x,y,z) of single site cartisain ordination. data
	m_paramNumX = int(m_ParamSiteInfo.ObsRange*KM_TO_DEG*2/ m_paramGridLenX +1);//SINGLE_LON_NUMS;	
	m_paramNumY = int(m_ParamSiteInfo.ObsRange*KM_TO_DEG*2/ m_paramGridLenY +1);//SINGLE_LAT_NUMS;
	m_paramNumZ = g_MosaicCommParam.iHgtNumLevels;	
	m_pHeights=new uint16_t[m_paramNumZ];
	if(m_pHeights!=0)
		for(int32_t i=0; i<m_paramNumZ; i++)
			m_pHeights[i]=g_MosaicCommParam.iHeights[i];//meters
	
	//m_paramProdDir
	//����������ļ���
	strcpy_s(m_paramProdDir, g_DataDir.strTemDataDir);
	strcat_s(m_paramProdDir,TEMP_FOLDER);
	CreateDir(m_paramProdDir);

	strcat_s(m_paramProdDir, SZ_SLASH);
	strcat_s(m_paramProdDir,SGL_MSC_FOLDER);  //�����DBZ
	//strcat_s(m_paramProdDir,SGL_MSCV_FOLDER);  //������ٶ�
	CreateDir(m_paramProdDir);

	strcat_s(m_paramProdDir, SZ_SLASH);
	strcat_s(m_paramProdDir, m_ParamSiteInfo.SiteName);	
}

void CPolarToGridOfRefCls::VolumnScanTo3DGrid(stdUniformScanData &vsData, int16_t nCuts, uint8_t***  &outData,
	int16_t x_Num, double x_Dlt, int16_t y_Num, double y_Dlt, int16_t z_Num, uint16_t* Height,
	double radLon, double radLat, uint16_t radHgt)
{
	float fRadHt = float(radHgt / 1000.);
	unsigned char const ucMinRefOnTop = unsigned char(2 * 2.) + 66; //5DBZ
	float fMaxElvDiff = m_paramBeamWidth / 2;  //�������������ٽ�����֮�������ֵ

	vector<float> fAvgEl;// = NULL;			//һ��PPI��ƽ������
	typedef vector<int16_t> vct_azPolar;
	vector<vct_azPolar> azIndex;// = NULL;
	int16_t curAzIdx = -1;//��ͬPPI�ϣ�ÿ����λ��(0,1,...359��)��Ӧ������

	//double fAzPI = 0.,fAz = 0.;	
	//double belta = 0., sina = 0., s = 0.;
	//float x = 0., y = 0., z = 0.;	//���Ƿ�Χ
	float minEl = 99, maxEl = -99;//��ɨ��������С����

	//-------------------------------------------------------------------------//
	//------------------- e x e c t u a b l e   c o d e  ----------------------//

	//Ϊ��λ�ǽ�������
	azIndex.resize(nCuts);
	fAvgEl.resize(nCuts);
	for (int i = 0; i < nCuts; i++)
	{
		azIndex[i].resize(360);
		//��ʼ����λ������ֵ:-1
		memset(&azIndex[i][0], -1, sizeof(int16_t) * 360);
		//��ʼ����PPI��ƽ������:0.0
		fAvgEl[i] = 0.;
		int16_t numAzUsed = 0;
		for (int32_t k = 0; k<vsData.CutsData[i].header.num_beam; k++)
		{
			int32_t idx = int(vsData.CutsData[i].azim[k] / 10.);	//ת�����͵ķ�λ�� ��0-359�ȣ�  !Whyan 2006.04.28
			if (idx >= 360) idx -= 360;
			else if (idx<0)
				continue;

			//���ͷ�λ�Ƕ�Ӧ������
			if (k>0 && azIndex[i][idx] != -1) idx++;
			if (idx >= 360) idx -= 360;

			azIndex[i][idx] = k;

			//if (nCuts == 9)
			//{
			///if(abs(strct_Data[i].elev[k]-VCP_21[i])<5)
			//{
			numAzUsed++;
			fAvgEl[i] += float(vsData.CutsData[i].elev[k] / 10.);
			//}//
			//}
			//else
			//	goto ERRCODE1;
		}

		//Calculate average elevation of this cut
		if (numAzUsed>1) //��ƽ������
			fAvgEl[i] = fAvgEl[i] / numAzUsed;//

		if (minEl > fAvgEl[i]) minEl = fAvgEl[i];
		if (maxEl < fAvgEl[i]) maxEl = fAvgEl[i];
	}//cuts

	//��ʼ���״����굽�ѿ�������Ĳ�ֵ����
	//FILE *fp = 0;
	//if(m_ParamSiteInfo.SiteID==10)
	//	errno_t tt= fopen_s(&fp,"check.txt", "w");

	for (int j = 0; j < y_Num; j++)
	{
		double y = radLat + (j - (y_Num + 1) / 2)*y_Dlt; //�ӵ�γ���γ

		for (int k = 0; k < x_Num; k++)
		{
			//����õ㾭��(degree)
			double x = radLon + (k - (x_Num + 1) / 2)*x_Dlt; //����
			//���ˮƽ�����״�̫��,���������
			if (fabs(x - radLon) < 0.00001 && fabs(y - radLat) < 0.00001)continue;
			//s��Բ����//km
			double s = earth_R*acos(sin(radLat*PI0)*sin(y*PI0) +
				cos(radLat*PI0)*cos(y*PI0)*cos(x*PI0 - radLon*PI0));
			//(r����,��)�����з�λ�ǵ�SINֵ
			double sina = cos(y*PI0)*sin(x*PI0 - radLon*PI0) / sin(s / earth_R);
			if (fabs(sina) > 1)
			{
				//whyan added 2 lines for 270degrees and 90 degrees are skipped
				sina = int(sina * 500) / 500.;
				if (fabs(sina) > 1)
					continue;
			}

			//(1): fAz  (r����,��)�����еķ�λ�� 
			double fAzPI = 0.;
			if (y >= radLat)
			{
				if (x >= radLon)
					fAzPI = asin(sina);
				else
					fAzPI = 2 * PAI + asin(sina);
			}
			else
				fAzPI = PAI - asin(sina);
			float fAz = float(fAzPI / PI0);		//�ɻ���ת��Ϊ��

			//------ Ѱ������ķ�λ�� ---------	
			//��1��Ϊ��λ�ķ�λ		
			curAzIdx = int(fAz); // (0-359 deg)
			if (curAzIdx >= 360)
				curAzIdx -= 360;

			//TRACE("y=%5.1f, x=%5.1f, az=%3.0f   ", y, x, fAz);
			double belta = s / earth_Rm;

//#pragma omp parallel for// private(i,z,ii)//fEl,upElIdx,lowElIdx,lowWeight,upWeight)
			for (int i = 0; i < z_Num; i++)
			{
				int16_t  lowV = 0, upV = 0;
				uint32_t fSR;
				double fElPI = 0.;
				float  fSRange = 0., fEl = 0.;
				int		lowElIdx = 0, upElIdx = 0, lowRIdx = 0, upRIdx = 0;
				float	lowWeight = 0., upWeight = 0.;
				float	fDefBetwLow = 0.;
				double ratio;
				double fz1, fz2;
				BOOL    b_ND_Flag = false, bFold_flag = false, b_NB_FLag = false, b_CSD_Flag = false;
				//b_ND_Flag=false, bFold_flag=false, b_NB_FLag = false, b_CSD_Flag = false;

				//��ʼ������
				upElIdx = -1;
				lowElIdx = -1;

				upRIdx = -1;
				lowRIdx = -1;

				lowWeight = 0;
				upWeight = 0;

				//(2):  fElvPhi  (r����,��)�����е�����
				float z = float(Height[i] / 1000.);// ����õ�߶�(KM)
				fElPI = atan((cos(belta) - earth_Rm / (earth_Rm + z - fRadHt)) / sin(belta));
				fEl = float(fElPI / PI0);//�ɻ���ת��Ϊ��
				
				  //������������״���ɨ��Χ֮��,����,������������
				if (fEl < minEl - fMaxElvDiff)
					continue;
				if (fEl > maxEl + fMaxElvDiff)
				{
					//����ɨ���Ƿ�Χ���ڵ�ä��������Ϊ��Ч��ֵ
					outData[i][j][k] = CODE_INVALID;
					continue;
				}

				//(3):  fSRange (r����,��)�����е�б��Slant range (km)
				fSRange = float(sin(belta)*(earth_Rm + z - fRadHt) / cos(fElPI));
				float tmpMaxElvDiff = fMaxElvDiff;// float(min(float(asin(.5 / fSRange) / PI0), 0.01));

				//����õ�(r����,��)���״������е�λ��			

				//1 ȷ���˸�����״���������Ƿ�Χ lowElIdx: �²����ǣ� upElIdx: �ϲ�����,
				//2 ȷ���˸�����״������е�б��--���������λ��б��
				int ii = 0;
				for ( ii = 0; ii < nCuts - 1; ii++)
				{					
					//�����������������ɨ����������֮�䣬�������������ڵ�����
					if (fAvgEl[ii]<fEl && fAvgEl[ii + 1]>fEl)
					{
						//����ⳤ�Ƿ��� �д����������������һ��
						if (vsData.CutsData[ii].header.gateWidth == 0 ||
							vsData.CutsData[ii + 1].header.gateWidth <= 0) break;

						ratio = cos(fEl*PI0) / cos(fAvgEl[ii] * PI0);
						fSR = int(fSRange * 1000 * ratio);
						lowRIdx = int(fSR / vsData.CutsData[ii].header.gateWidth + .5);

						fSR = int(fSRange * 1000 * cos(fElPI) / cos(fAvgEl[ii + 1] * PI0));
						upRIdx = int(fSR / vsData.CutsData[ii + 1].header.gateWidth + .5);

						//����Чб�෶Χ��
						if (lowRIdx < vsData.CutsData[ii].header.num_gate && azIndex[ii][curAzIdx] != -1) lowElIdx = ii;
						if (upRIdx < vsData.CutsData[ii + 1].header.num_gate  && azIndex[ii + 1][curAzIdx] != -1) upElIdx = ii + 1;

						//ֻ�е����Ǿ��뷶Χ��Чʱ�� �жϵѿ����������Ƿ񳬳������ǵĲ�����ȷ�Χ
						//������Χ����Ĳ�������
						outData[i][j][k] = CODE_INVALID;

						if (lowElIdx != -1 && upElIdx != -1)
						{
							lowWeight = float((fAvgEl[upElIdx] - fEl) / (fAvgEl[upElIdx] - fAvgEl[lowElIdx]));
							upWeight = float((fEl - fAvgEl[lowElIdx]) / (fAvgEl[upElIdx] - fAvgEl[lowElIdx]));
						}
						else if (upElIdx != -1)
						{
							lowWeight = 0;
							upWeight = 1;// float((fEl - fAvgEl[lowElIdx]) / (fAvgEl[upElIdx] - fAvgEl[lowElIdx]));
						}
						else if (lowElIdx != -1)
						{
							lowWeight = 1;
							upWeight = 0;
						}

						break;
					}
					//(����)λ�ڵ�������
					else if (fabs(fEl - fAvgEl[ii]) <= tmpMaxElvDiff)
					{
						//����ⳤ�Ƿ��� �д����������������һ��
						if (vsData.CutsData[ii].header.gateWidth == 0)
						{
							ReportLog_Error("PolarToDescartes: Invalid beam width");
							break;
						}

						ratio = cos(fEl*PI0) / cos(fAvgEl[ii] * PI0);
						fSR = uint32_t(fSRange * 1000 * ratio);
						lowRIdx = int(fSR / vsData.CutsData[ii].header.gateWidth + .5);

						if (lowRIdx < vsData.CutsData[ii].header.num_gate && azIndex[ii][curAzIdx] != -1) lowElIdx = ii;

						//���ô�ֱ�ڲ巽��
						lowWeight = 1;
						upWeight = 0;
						break;
					}
					//(����)λ�ڸ�������
					else if (fabs(fAvgEl[ii + 1] - fEl) <= tmpMaxElvDiff)//0.45)
					{
						//����ⳤ�Ƿ��� �д����������������һ��
						if (vsData.CutsData[ii + 1].header.gateWidth == 0)
						{
							ReportLog_Error("PolarToDescartes: Invalid beam width");
							break;
						}
						ratio = cos(fElPI) / cos(fAvgEl[ii + 1] * PI0);
						fSR = uint32_t(fSRange * 1000 * ratio);
						upRIdx = int(fSR / vsData.CutsData[ii + 1].header.gateWidth + .5);

						if (upRIdx < vsData.CutsData[ii + 1].header.num_gate  && azIndex[ii + 1][curAzIdx] != -1) upElIdx = ii + 1;

						//���ô�ֱ�ڲ巽��
						lowWeight = 0;
						upWeight = 1;
						break;
					}
				}//�����������

				 //�������������֮��Ĳ�
				fDefBetwLow = fEl - fAvgEl[ii];

				if (lowElIdx != -1 && upElIdx != -1)//����������Ч������ͬһ�߶ȵĶ�άˮƽ��ֵ�ʹ�ֱ���Բ�ֵ
				{
					//��ȡ�ڽ����������ϣ���ͬб��λ�õ�ֵ
					upV = vsData.CutsData[upElIdx].field[azIndex[upElIdx][curAzIdx]][upRIdx];
					lowV = vsData.CutsData[lowElIdx].field[azIndex[lowElIdx][curAzIdx]][lowRIdx];

					//����Ŀ�����ֵ
					if ((upV > VALUE_INVALID) && (lowV > VALUE_INVALID))
					{	//���ô�ֱ�ڲ巽��
						fz1 = pow(10., (upV - 0) / 100.);// - vsData.header.offset) / vsData.header.factor/10.);
						fz2 = pow(10., (lowV - 0) / 100.);// - vsData.header.offset) / vsData.header.factor / 10.);
						outData[i][j][k] = uint8_t(10 * log10(upWeight*fz1 + lowWeight*fz2) * 2 + 66. + .5);   //NVI���� /��ֱ�ڲ�//
																													 //outData[i][j][k]= unsigned char((upWeight*upV+lowWeight*lowV)/10.*2 + 66. + .5);   //NVI���� /��ֱ�ڲ�
					}
					else if (lowV > VALUE_INVALID)// && fDefBetwLow<=fMaxElvDiff)
					{	//ȡ�Ͳ����ǵ�ֵ
						//fz1=pow(10.,upV/100.);
						fz2 = pow(10., (lowV - 0) / 100.);// - vsData.header.offset) / vsData.header.factor / 10.);
						outData[i][j][k] = uint8_t(10 * log10(lowWeight*fz2) * 2 + 66. + .5);
					}
					else if (upV > VALUE_INVALID)
					{	//ȡ�߲����ǵ�ֵ
						fz1 = pow(10., (upV - 0) / 100.);// -vsData.header.offset) / vsData.header.factor / 10.);
						outData[i][j][k] = uint8_t(10 * log10(upWeight*fz1) * 2 + 66. + .5);
					}
					else //bND_Flag, bFold_flag, b_NB_FLag
					{	//�ж���ʲô������ֵ
						if (upV == VALUE_INVALID || lowV == VALUE_INVALID)
							b_ND_Flag = true;
						if (upV == VALUE_RANFOLD || lowV == VALUE_RANFOLD)
							bFold_flag = true;
						if (upV <= VALUE_CLUTTER || lowV <= VALUE_CLUTTER)
							b_CSD_Flag = true;

						if (!b_ND_Flag && bFold_flag && !b_CSD_Flag)		//��Ϊ����ģ��
							outData[i][j][k] = CODE_RANFOLD;
						else if (!b_ND_Flag && !bFold_flag && b_CSD_Flag) //��Ϊ������������
							outData[i][j][k] = CODE_CSRED;
						else											//���������Ϊ��������
							outData[i][j][k] = CODE_INVALID;
					}
				}
				else if (lowElIdx != -1) //�Ͳ�б����Ч�����õͲ����ǵ���ֵ
				{
					lowV = vsData.CutsData[lowElIdx].field[azIndex[lowElIdx][curAzIdx]][lowRIdx];
					if (lowV > VALUE_INVALID)//&& fDefBetwLow<=fMaxElvDiff)
					{
						fz2 = pow(10., (lowV - 0) / 100.);// vsData.header.offset) / vsData.header.factor / 10.);
						outData[i][j][k] = uint8_t(10 * log10(lowWeight*fz2) * 2 + 66. + .5);
					}
					else if (lowV == VALUE_INVALID)
						outData[i][j][k] = CODE_INVALID;
					else if (lowV == VALUE_RANFOLD)
						outData[i][j][k] = CODE_RANFOLD;
					else if (lowV <= VALUE_CLUTTER)
						outData[i][j][k] = CODE_CSRED; //������������
				}
				else if (upElIdx != -1) //�߲�б����Ч�����ø߲����ǵ���ֵ
				{
					upV = vsData.CutsData[upElIdx].field[azIndex[upElIdx][curAzIdx]][upRIdx]; //dBZ*10
					if (upV > VALUE_INVALID)
					{
						fz1 = pow(10., (upV - 0) / 100.);// - vsData.header.offset) / vsData.header.factor / 10.);
						outData[i][j][k] = uint8_t(10 * log10(upWeight*fz1) * 2 + 66. + .5);
					}
					//if(upV > VALUE_INVALID) 
					//	outData[i][j][k] = unsigned char((upV*upWeight)/10.*2 + 66. + .5);
					else if (upV == VALUE_INVALID)
						outData[i][j][k] = CODE_INVALID;
					else if (upV == VALUE_RANFOLD)
						outData[i][j][k] = CODE_RANFOLD;
					else if (upV <= VALUE_CLUTTER)
						outData[i][j][k] = CODE_CSRED; //������������
				}
			}// end for i(0-num_Z)
		} //end for k(0-num_X)
	}// end for j(0-num_Y)

	 //�ͷŷ�λ���������桢���ǻ���
	for (size_t i = 0; i < azIndex.size(); i++)
	{
		azIndex[i].clear();
		vct_azPolar().swap(azIndex[i]);
	}
	azIndex.clear();
	vector<vct_azPolar>().swap(azIndex);
	fAvgEl.clear();
	vector<float>().swap(fAvgEl);
}

bool CPolarToGridOfRefCls::SaveGridData()
{
	CreateDir(m_paramProdDir);	//Create directory
	//CProcessIO ProcessIO;
	//����ѿ�����������- 
	MOSAICDATAHEADER DataHeader;
	ZeroMemory(&DataHeader, sizeof(DataHeader));
	DataHeader.iVersion = 1;
	strcpy_s(DataHeader.strZoneName, m_ParamSiteInfo.SiteName);
	DataHeader.iNumRadars = 1;
	strcpy_s(DataHeader.strDataType,"Z");
	DataHeader.iStYear = m_year;
	DataHeader.iStMonth= m_month;
	DataHeader.iStDay  = m_day; 
	DataHeader.iStHour = m_hour;
	DataHeader.iStMinute = m_minute;
	DataHeader.iFstLon = int32_t((m_ParamSiteInfo.SiteLon/1000.-((m_paramNumX-1)/2)*m_paramGridLenX)*1000);
	DataHeader.iFstLat = int32_t((m_ParamSiteInfo.SiteLat/1000.-((m_paramNumY-1)/2)*m_paramGridLenY)*1000);
	DataHeader.iLstLon = int32_t((m_ParamSiteInfo.SiteLon/1000.+((m_paramNumX-1)/2)*m_paramGridLenX)*1000);
	DataHeader.iLstLat = int32_t((m_ParamSiteInfo.SiteLat/1000.+((m_paramNumY-1)/2)*m_paramGridLenY)*1000);
	DataHeader.iFstHgt = m_pHeights[0];
	DataHeader.iLstHgt = m_pHeights[m_paramNumZ-1];
	DataHeader.iLonGrids=m_paramNumX;
	DataHeader.iLatGrids=m_paramNumY;
	DataHeader.iLevels  =m_paramNumZ;
	DataHeader.iLonRes  =uint32_t(m_paramGridLenX*10000+.5) ;  //.g_MosaicCommParam.iLonRes;
	DataHeader.iLatRes  =uint32_t(m_paramGridLenY*10000+.5) ;  //g_MosaicCommParam.iLatRes;

	//���ô�ֱ�ֱ���
	//�����ֱ�ֱ������,��ֱ��д�봹ֱ�ֱ���
	//�����ֱ�ֱ��ʲ����,��ô���Ϊ-1
	int32_t j,i=1;
	do
	{
		if(m_pHeights[i+1]-m_pHeights[i] != m_pHeights[i]-m_pHeights[i-1])
		{
			DataHeader.iHgtRes  =-1;
			break;
		}
		i++;
	}while(i<m_paramNumZ-1);
	if(DataHeader.iHgtRes!=-1) DataHeader.iHgtRes = m_pHeights[0];

	//�����״�վ��Ϣ, ��߶�, �Լ��۲�������ݿ�ʼλ��(������ͷĩβΪ׼)
	DataHeader.dwRadInfoOffset = 0;
	DataHeader.dwLevHgtOffset  = DataHeader.dwRadInfoOffset+DataHeader.iNumRadars*SIZE_RADAR_INFO;//sizeof(RADARSITEINFO);
	DataHeader.dwObvDataOffset = DataHeader.dwLevHgtOffset+sizeof(uint16_t)*DataHeader.iLevels;

	char szCode[6] = "";
	if (m_ParamSiteInfo.SiteID < 1000)
		sprintf_s(szCode, "%d", m_ParamSiteInfo.SiteID + 9000);
	else
		sprintf_s(szCode, "%s", m_ParamSiteInfo.SiteID);

	//Set file name
	char strYear[6]="",strMonth[4]="",strDay[4]="",strHour[4]="",strMinute[4]="";
	_itoa_s(m_year, strYear, 10);
	_itoa_s(m_month+100, strMonth, 10);
	_itoa_s(m_day+100, strDay, 10);
	_itoa_s(m_hour+100, strHour, 10);
	_itoa_s(m_minute+100, strMinute, 10);
	strcpy_s(m_gridFileName, m_paramProdDir);
	strcat_s(m_gridFileName,SZ_SLASH);
	strcat_s(m_gridFileName,strYear);
	strcat_s(m_gridFileName,strMonth+1);
	strcat_s(m_gridFileName,strDay+1);
	strcat_s(m_gridFileName,strHour+1);
	strcat_s(m_gridFileName,strMinute+1);
	strcat_s(m_gridFileName,".");
	strcat_s(m_gridFileName, szCode);
	strcat_s(m_gridFileName, ".");
	strcat_s(m_gridFileName,DataHeader.strDataType);
	strcat_s(m_gridFileName,MOSAIC_EXT_FILE );

	FILE *fp = 0;
	errno_t err=fopen_s(&fp,m_gridFileName, "wb");
	if(err!=0)
		return (false);
	//Write data header into the file
	if(fwrite(&DataHeader,sizeof(MOSAICDATAHEADER),1,fp)<1)
	{
		fclose(fp);
		return false;
	}
	if(fwrite(&m_ParamSiteInfo,SIZE_RADAR_INFO,1,fp)<1)
	{
		fclose(fp);
		return false;
	}
	if(fwrite(m_pHeights,sizeof(uint16_t)*m_paramNumZ,1,fp)<1)
	{
		fclose(fp);
		return false;
	}
	for(i=0; i<m_paramNumZ; i++)
	{
		//Write data at thei height into the file
		for(j=0; j<m_paramNumY; j++)
			if(fwrite(m_GridData[i][j],sizeof(uint8_t)*m_paramNumX,1,fp)<1)
			{
				fclose(fp);
				return false;
			}
	}
	fclose(fp);
	fp=0x0;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
//Class 
CPolarToGridOfDopplerCls::CPolarToGridOfDopplerCls()
{
}

CPolarToGridOfDopplerCls::~CPolarToGridOfDopplerCls()
{

}

int32_t CPolarToGridOfDopplerCls::Run1()// (stdCommScan vsScan)
{
	//��ɨ���ݿ�, ����������Ϊ��, ������ɨ���ݲ���ȷ
	if(m_numCuts==0 || m_commScanData.CutsData.size()==0x0)
	{
		ReportLog_Error("CPolarToGridOfRefCls-- No input volume scan data!");
		return (-1);
	}
	
	LoadParameters();

	if(!InitGridData())
	{
		ReportLog_Error("CPolarToGridOfRefCls--Failed to allocate memory for single-3D data!");
		return -1;
	}

	VolumnScanTo3DGrid(m_commScanData,m_numCuts,m_GridData,
		m_paramNumX,m_paramGridLenX,m_paramNumY,m_paramGridLenY,m_paramNumZ,m_pHeights,
		double(m_ParamSiteInfo.SiteLon/1000.), double(m_ParamSiteInfo.SiteLat/1000.),uint16_t(m_ParamSiteInfo.SiteHgt));

	ObsvDateTime(m_commScanData.CutsData[0]);
	m_commScanData.ReleaseBuf();

	if(!SaveGridData())
	{
		ReportLog_Error("CPolarToGridOfRefCls--Failed to output grid data file!");
		return -1;
	}

	return 1;
}

void  CPolarToGridOfDopplerCls::LoadParameters()
{
	CPolarToGridCls::LoadParameters();//General parameter
	
	//Parameter for vel
	//Set grid number(x,y,z) of single site cartisain ordination. data
	m_paramNumX = min(short(m_ParamSiteInfo.ObsRange*KM_TO_DEG/(g_MosaicCommParam.iLonRes/10000.)*2+1),
		       short(g_PolarToGrdParam.iRng*KM_TO_DEG/g_PolarToGrdParam.fRes)*2+1);
	m_paramNumY = min(short(m_ParamSiteInfo.ObsRange*KM_TO_DEG/(g_MosaicCommParam.iLatRes/10000.)*2+1),
		      short(g_PolarToGrdParam.iRng*KM_TO_DEG/g_PolarToGrdParam.fRes)*2+1);
	m_paramNumZ = g_iNumVelHeight;
	m_pHeights=new uint16_t[m_paramNumZ];
	if(m_pHeights!=0)//m
	{
		for(int32_t i=0; i<m_paramNumZ; i++)
			m_pHeights[i]=g_iVelHeights[i];
	}

	//set local resolution of mosaic data
	m_paramGridLenX = g_PolarToGrdParam.fRes;//Degree
	m_paramGridLenY = g_PolarToGrdParam.fRes;//Degree
	
	//m_paramProdDir
	//Write site information into the file	
	//��վ����������ļ���
	strcpy_s(m_paramProdDir, g_DataDir.strTemDataDir);
	strcat_s(m_paramProdDir,TEMP_FOLDER);
	CreateDir(m_paramProdDir);

	strcat_s(m_paramProdDir,SZ_SLASH);
	strcat_s(m_paramProdDir,SGL_MSCV_FOLDER);  //������ٶ�
	CreateDir(m_paramProdDir);

	strcat_s(m_paramProdDir, SZ_SLASH);
	strcat_s(m_paramProdDir, m_ParamSiteInfo.SiteName);
	CreateDir(m_paramProdDir);	//Create directory
}

bool CPolarToGridOfDopplerCls::SaveGridData()
{
	CreateDir(m_paramProdDir);	//Create directory
	//����ѿ�����������- 
	MOSAICDATAHEADER DataHeader;
	ZeroMemory(&DataHeader, sizeof(DataHeader));
	DataHeader.iVersion = 1;
	strcpy_s(DataHeader.strZoneName, m_ParamSiteInfo.SiteName);
	DataHeader.iNumRadars = 1;
	strcpy_s(DataHeader.strDataType,"V");
	DataHeader.iStYear = m_year;
	DataHeader.iStMonth= m_month;
	DataHeader.iStDay  = m_day; 
	DataHeader.iStHour = m_hour;
	DataHeader.iStMinute = m_minute;
	DataHeader.iFstLon = int32_t((m_ParamSiteInfo.SiteLon/1000.-((m_paramNumX-1)/2)*m_paramGridLenX)*1000);
	DataHeader.iFstLat = int32_t((m_ParamSiteInfo.SiteLat/1000.-((m_paramNumY-1)/2)*m_paramGridLenY)*1000);
	DataHeader.iLstLon = int32_t((m_ParamSiteInfo.SiteLon/1000.+((m_paramNumX-1)/2)*m_paramGridLenX)*1000);
	DataHeader.iLstLat = int32_t((m_ParamSiteInfo.SiteLat/1000.+((m_paramNumY-1)/2)*m_paramGridLenY)*1000);
	DataHeader.iFstHgt = m_pHeights[0];
	DataHeader.iLstHgt = m_pHeights[m_paramNumZ-1];
	DataHeader.iLonGrids=m_paramNumX;
	DataHeader.iLatGrids=m_paramNumY;
	DataHeader.iLevels  =m_paramNumZ;
	DataHeader.iLonRes  =uint32_t(m_paramGridLenX*10000+.5) ;  //.g_MosaicCommParam.iLonRes;
	DataHeader.iLatRes  =uint32_t(m_paramGridLenY*10000+.5) ;  //g_MosaicCommParam.iLatRes;

	//���ô�ֱ�ֱ���
	//�����ֱ�ֱ������,��ֱ��д�봹ֱ�ֱ���
	//�����ֱ�ֱ��ʲ����,��ô���Ϊ-1
	int32_t j,i=1;
	do
	{
		if(m_pHeights[i+1]-m_pHeights[i] != m_pHeights[i]-m_pHeights[i-1])
		{
			DataHeader.iHgtRes  =-1;
			break;
		}
		i++;
	}while(i<m_paramNumZ-1);
	if(DataHeader.iHgtRes!=-1) DataHeader.iHgtRes = m_pHeights[0];

	//�����״�վ��Ϣ, ��߶�, �Լ��۲�������ݿ�ʼλ��(������ͷĩβΪ׼)
	DataHeader.dwRadInfoOffset = 0;
	DataHeader.dwLevHgtOffset  = DataHeader.dwRadInfoOffset+DataHeader.iNumRadars*SIZE_RADAR_INFO;
	DataHeader.dwObvDataOffset = DataHeader.dwLevHgtOffset+sizeof(uint16_t)*DataHeader.iLevels;

	char szCode[6] = "";
	if (m_ParamSiteInfo.SiteID < 1000)
		sprintf_s(szCode, "%d", m_ParamSiteInfo.SiteID + 9000);
	else
		sprintf_s(szCode, "%s", m_ParamSiteInfo.SiteID);

	//Set file name
	char strYear[6] = "", strMonth[4] = "", strDay[4] = "", strHour[4] = "", strMinute[4] = "";
	_itoa_s(m_year, strYear, 10);
	_itoa_s(m_month + 100, strMonth, 10);
	_itoa_s(m_day + 100, strDay, 10);
	_itoa_s(m_hour + 100, strHour, 10);
	_itoa_s(m_minute + 100, strMinute, 10);
	strcpy_s(m_gridFileName, m_paramProdDir);
	strcat_s(m_gridFileName, SZ_SLASH);
	strcat_s(m_gridFileName, strYear);
	strcat_s(m_gridFileName, strMonth + 1);
	strcat_s(m_gridFileName, strDay + 1);
	strcat_s(m_gridFileName, strHour + 1);
	strcat_s(m_gridFileName, strMinute + 1);
	strcat_s(m_gridFileName, ".");
	strcat_s(m_gridFileName, szCode);
	strcat_s(m_gridFileName, ".");
	strcat_s(m_gridFileName, DataHeader.strDataType);
	strcat_s(m_gridFileName, MOSAIC_EXT_FILE);

	FILE *fp = 0;
	errno_t err=fopen_s(&fp,m_gridFileName, "wb");
	if(err!=0)
		return (false);
	//Write data header into the file
	if(fwrite(&DataHeader,sizeof(MOSAICDATAHEADER),1,fp)<1)
	{
		fclose(fp);
		return false;
	}
	if(fwrite(&m_ParamSiteInfo,SIZE_RADAR_INFO,1,fp)<1)
	{
		fclose(fp);
		return false;
	}
	if(fwrite(m_pHeights,sizeof(uint16_t)*m_paramNumZ,1,fp)<1)
	{
		fclose(fp);
		return false;
	}
	for(i=0; i<m_paramNumZ; i++)
	{
		//Write data at thei height into the file
		for(j=0; j<m_paramNumY; j++)
			if(fwrite(m_GridData[i][j],sizeof(unsigned char)*m_paramNumX,1,fp)<1)
			{
				fclose(fp);
				return false;
			}
	}
	fclose(fp);
	fp=0x0;
	return true;
}

void CPolarToGridOfDopplerCls::VolumnScanTo3DGrid(stdUniformScanData &vsData, int16_t nCuts, uint8_t***  &outData,
	int16_t x_Num, double x_Dlt, int16_t y_Num, double y_Dlt, int16_t z_Num, uint16_t* Height,
	double radLon, double radLat, uint16_t radHgt)
{
	float fRadHt = float(radHgt / 1000.);
	unsigned char const ucMinRefOnTop = unsigned char(2 * 2.) + 66; //5DBZ
	float fMaxElvDiff = m_paramBeamWidth / 2;  //�������������ٽ�����֮�������ֵ

	typedef vector<int16_t> vct_azPolar;
	vector<vct_azPolar> azIndex;// = NULL;
	vector<float> fAvgEl;// = NULL;			//һ��PPI��ƽ������
	vector<double> cos_el;// [20];
	int16_t curAzIdx = -1;//��ͬPPI�ϣ�ÿ����λ��(0,1,...359��)��Ӧ������

	float minEl = 99, maxEl = -99;//��ɨ��������С����

	//float ratio;
	//uint32_t fSR;
	//double fz1, fz2;
	
	//
	//-------------------------------------------------------------------------//
	//------------------- e x e c t u a b l e   c o d e  ----------------------//
	//
	//Ϊ��λ�ǽ�������
	azIndex.resize(nCuts);
	fAvgEl.resize(nCuts);
	cos_el.resize(nCuts);
	for (int i = 0; i < nCuts; i++)
	{
		azIndex[i].resize(360);
		//��ʼ����λ������ֵ:-1
		memset(&azIndex[i][0], -1, sizeof(int16_t) * 360);
		//��ʼ����PPI��ƽ������:0.0
		fAvgEl[i] = 0.;
		int16_t numAzUsed = 0;
		for (int32_t k = 0; k<vsData.CutsData[i].header.num_beam; k++)
		{
			int32_t idx = int(vsData.CutsData[i].azim[k] / 10.);	//ת�����͵ķ�λ�� ��0-359�ȣ�  !Whyan 2006.04.28
			if (idx >= 360) idx -= 360;
			else if (idx<0)
				continue;

			//���ͷ�λ�Ƕ�Ӧ������
			if (k>0 && azIndex[i][idx] != -1) idx++;
			if (idx >= 360) idx -= 360;

			azIndex[i][idx] = k;

			numAzUsed++;
			fAvgEl[i] += float(vsData.CutsData[i].elev[k] / 10.);
		}

		//Calculate average elevation of this cut
		if (numAzUsed>1) //��ƽ������
			fAvgEl[i] = fAvgEl[i] / numAzUsed;//

		if (minEl > fAvgEl[i]) minEl = fAvgEl[i];
		if (maxEl < fAvgEl[i]) maxEl = fAvgEl[i];

		cos_el[i] = cos(fAvgEl[i] * PI0);
	}

	//��ʼ���״����굽�ѿ�������Ĳ�ֵ����
	for (int j = 0; j < y_Num; j++)
	{
		double y = radLat + (j - (y_Num + 1) / 2)*y_Dlt; //�ӵ�γ���γ

		for (int k = 0; k < x_Num; k++)
		{
			//����õ㾭��(degree)
			double x = radLon + (k - (x_Num + 1) / 2)*x_Dlt; //����
															 //���ˮƽ�����״�̫��,���������
			if (fabs(x - radLon) < 0.00001 && fabs(y - radLat) < 0.00001)continue;
			//s��Բ����//km
			double s = earth_R*acos(sin(radLat*PI0)*sin(y*PI0) +
				cos(radLat*PI0)*cos(y*PI0)*cos(x*PI0 - radLon*PI0));
			//(r����,��)�����з�λ�ǵ�SINֵ
			double sina = cos(y*PI0)*sin(x*PI0 - radLon*PI0) / sin(s / earth_R);
			if (fabs(sina) > 1)
			{
				//whyan added 2 lines for 270degrees and 90 degrees are skipped
				sina = int(sina * 500) / 500.;
				if (fabs(sina) > 1)
					continue;
			}

			//(1): fAz  (r����,��)�����еķ�λ�� 
			double fAzPI = 0.;
			if (y >= radLat)
			{
				if (x >= radLon)
					fAzPI = asin(sina);
				else
					fAzPI = 2 * PAI + asin(sina);
			}
			else
				fAzPI = PAI - asin(sina);
			float fAz = float(fAzPI / PI0);		//�ɻ���ת��Ϊ��

												//------ Ѱ������ķ�λ�� ---------	
												//��1��Ϊ��λ�ķ�λ		
			curAzIdx = int(fAz); // (0-359 deg)
			if (curAzIdx >= 360)
				curAzIdx -= 360;

			//TRACE("y=%5.1f, x=%5.1f, az=%3.0f   ", y, x, fAz);
			double belta = s / earth_Rm;

//#pragma omp parallel for// private(i,z,ii)//fEl,upElIdx,lowElIdx,lowWeight,upWeight)
			for (int i = 0; i < z_Num; i++)
			{
				int16_t  lowV = 0, upV = 0;
				uint32_t fSR;
				double fElPI = 0.;
				float  fSRange = 0., fEl = 0.;
				int		lowElIdx = 0, upElIdx = 0, lowRIdx = 0, upRIdx = 0;
				float	lowWeight = 0., upWeight = 0.;
				float	fDefBetwLow = 0.;
				double ratio;
				double fz1, fz2;
				BOOL    b_ND_Flag = false, bFold_flag = false, b_NB_FLag = false, b_CSD_Flag = false;
				//b_ND_Flag=false, bFold_flag=false, b_NB_FLag = false, b_CSD_Flag = false;

				//��ʼ������
				upElIdx = -1;
				lowElIdx = -1;
				upRIdx = -1;
				lowRIdx = -1;

				lowWeight = 0;
				upWeight = 0;

				//(2):  fElvPhi  (r����,��)�����е�����
				float z = float(Height[i] / 1000.);// ����õ�߶�(KM)
				fElPI = atan((cos(belta) - earth_Rm / (earth_Rm + z - fRadHt)) / sin(belta));
				fEl = float(fElPI / PI0);//�ɻ���ת��Ϊ��

				//������������״���ɨ��Χ֮��,����,������������
				if (fEl < minEl - fMaxElvDiff)
					continue;
				if (fEl > maxEl + fMaxElvDiff)
				{
					//����ɨ���Ƿ�Χ���ڵ�ä��������Ϊ��Ч��ֵ
					outData[i][j][k] = CODE_INVALID;
					continue;
				}

				//(3):  fSRange (r����,��)�����е�б��Slant range (km)
				fSRange = float(sin(belta)*(earth_Rm + z - fRadHt) / cos(fElPI));
				float tmpMaxElvDiff = fMaxElvDiff;// float(min(float(asin(.5 / fSRange) / PI0), 0.01));
				
				//����õ�(r����,��)���״������е�λ��

				//1 ȷ���˸�����״���������Ƿ�Χ lowElIdx: �²����ǣ� upElIdx: �ϲ�����,
				//2 ȷ���˸�����״������е�б��--���������λ��б��
				int ii = 0;
				for (ii = 0; ii < nCuts - 1; ii++)
				{					
					//�����������������ɨ����������֮�䣬�������������ڵ�����
					if (fAvgEl[ii]<fEl && fAvgEl[ii + 1]>fEl)
					{
						//����ⳤ�Ƿ��� �д����������������һ��
						if (vsData.CutsData[ii].header.gateWidth == 0 ||
							vsData.CutsData[ii + 1].header.gateWidth <= 0) break;

						ratio = cos(fEl*PI0) / cos(fAvgEl[ii] * PI0);
						int32_t fSR = int(fSRange * 1000 * ratio);
						lowRIdx = int(fSR / vsData.CutsData[ii].header.gateWidth + .5);

						fSR = int(fSRange * 1000 * cos(fElPI) / cos(fAvgEl[ii + 1] * PI0));
						upRIdx = int(fSR / vsData.CutsData[ii + 1].header.gateWidth + .5);

						//����Чб�෶Χ��
						if (lowRIdx < vsData.CutsData[ii].header.num_gate && azIndex[ii][curAzIdx] != -1) lowElIdx = ii;
						if (upRIdx < vsData.CutsData[ii + 1].header.num_gate  && azIndex[ii + 1][curAzIdx] != -1) upElIdx = ii + 1;

						//ֻ�е����Ǿ��뷶Χ��Чʱ�� �жϵѿ����������Ƿ񳬳������ǵĲ�����ȷ�Χ
						//������Χ����Ĳ�������
						outData[i][j][k] = CODE_INVALID;

						if (lowElIdx != -1 && upElIdx != -1)
						{
							lowWeight = float((fAvgEl[upElIdx] - fEl) / (fAvgEl[upElIdx] - fAvgEl[lowElIdx]));
							upWeight = float((fEl - fAvgEl[lowElIdx]) / (fAvgEl[upElIdx] - fAvgEl[lowElIdx]));
						}
						else if (upElIdx != -1)
						{
							lowWeight = 0;
							upWeight = 1;// float((fEl - fAvgEl[lowElIdx]) / (fAvgEl[upElIdx] - fAvgEl[lowElIdx]));
						}
						else if (lowElIdx != -1)
						{
							lowWeight = 1;
							upWeight = 0;
						}
						break;
					}
					//(����)λ�ڵ�������
					else if (fabs(fEl - fAvgEl[ii]) <= tmpMaxElvDiff)
					{
						//����ⳤ�Ƿ��� �д����������������һ��
						if (vsData.CutsData[ii].header.gateWidth == 0)
						{
							ReportLog_Error("PolarToDescartes: Invalid beam width");
							break;
						}

						ratio = cos(fEl*PI0) / cos(fAvgEl[ii] * PI0);
						fSR = int(fSRange * 1000 * ratio);
						lowRIdx = int(fSR / vsData.CutsData[ii].header.gateWidth + .5);

						if (lowRIdx < vsData.CutsData[ii].header.num_gate && azIndex[ii][curAzIdx] != -1) lowElIdx = ii;

						//���ô�ֱ�ڲ巽��
						lowWeight = 1;
						upWeight = 0;
						break;
					}
					//(����)λ�ڸ�������
					else if (fabs(fAvgEl[ii + 1] - fEl) <= tmpMaxElvDiff)//0.45)
					{
						//����ⳤ�Ƿ��� �д����������������һ��
						if (vsData.CutsData[ii + 1].header.gateWidth == 0)
						{
							ReportLog_Error("PolarToDescartes: Invalid beam width");
							break;
						}

						ratio = cos(fElPI) / cos(fAvgEl[ii + 1] * PI0);
						int32_t fSR = int(fSRange * 1000 * ratio);
						upRIdx = int(fSR / vsData.CutsData[ii + 1].header.gateWidth + .5);

						if (upRIdx < vsData.CutsData[ii + 1].header.num_gate  && azIndex[ii + 1][curAzIdx] != -1) upElIdx = ii + 1;

						//���ô�ֱ�ڲ巽��
						lowWeight = 0;
						upWeight = 1;
						break;
					}
				}//�����������

				 //�������������֮��Ĳ�
				fDefBetwLow = fEl - fAvgEl[ii];

				if (lowElIdx != -1 && upElIdx != -1)//����������Ч������ͬһ�߶ȵĶ�άˮƽ��ֵ�ʹ�ֱ���Բ�ֵ
				{
					//��ȡ�ڽ����������ϣ���ͬб��λ�õ�ֵ
					upV = vsData.CutsData[upElIdx].field[azIndex[upElIdx][curAzIdx]][upRIdx];
					lowV = vsData.CutsData[lowElIdx].field[azIndex[lowElIdx][curAzIdx]][lowRIdx];

					//����Ŀ�����ֵ
					if ((upV>VALUE_INVALID) && (lowV>VALUE_INVALID))
					{	//���ô�ֱ�ڲ巽��
						fz1 = upV / 10. / cos_el[upElIdx];
						fz2 = lowV / 10. / cos_el[lowElIdx];
						outData[i][j][k] = uint8_t((upWeight*fz1 + lowWeight*fz2) * 2 + 129);   //NVI���� /��ֱ�ڲ�//
					}
					else if (lowV > VALUE_INVALID)// && fDefBetwLow<=fMaxElvDiff)
					{	//ȡ�Ͳ����ǵ�ֵ
						fz2 = lowV / 10. / cos_el[lowElIdx];
						outData[i][j][k] = uint8_t(fz2 * 2 + 129);
					}
					else if (upV > VALUE_INVALID)
					{	//ȡ�߲����ǵ�ֵ
						fz1 = upV / 10. / cos_el[upElIdx];
						outData[i][j][k] = uint8_t(fz1 * 2 + 129);
					}
					else //bND_Flag, bFold_flag, b_NB_FLag
					{	//�ж���ʲô������ֵ
						if (upV == VALUE_INVALID || lowV == VALUE_INVALID)
							b_ND_Flag = true;
						if (upV == VALUE_RANFOLD || lowV == VALUE_RANFOLD)
							bFold_flag = true;
						if (upV <= VALUE_CLUTTER || lowV <= VALUE_CLUTTER)
							b_CSD_Flag = true;

						if (!b_ND_Flag && bFold_flag && !b_CSD_Flag)		//��Ϊ����ģ��
							outData[i][j][k] = CODE_RANFOLD;
						else if (!b_ND_Flag && !bFold_flag && b_CSD_Flag) //��Ϊ������������
							outData[i][j][k] = CODE_CSRED;
						else											//���������Ϊ��������
							outData[i][j][k] = CODE_INVALID;
					}
				}
				else if (lowElIdx != -1) //�Ͳ�б����Ч�����õͲ����ǵ���ֵ
				{
					lowV = vsData.CutsData[lowElIdx].field[azIndex[lowElIdx][curAzIdx]][lowRIdx];
					if (lowV > VALUE_INVALID)//&& fDefBetwLow<=fMaxElvDiff)
					{
						fz2 = lowV / 10. / cos_el[lowElIdx];
						//outData[i][j][k]= unsigned char(lowWeight*fz2*2 + 64.5);
						outData[i][j][k] = uint8_t(fz2 * 2 + 129);
					}
					else if (lowV == VALUE_INVALID)
						outData[i][j][k] = CODE_INVALID;
					else if (lowV == VALUE_RANFOLD)
						outData[i][j][k] = CODE_RANFOLD;
					else if (lowV <= VALUE_CLUTTER)
						outData[i][j][k] = CODE_CSRED; //������������
				}
				else if (upElIdx != -1) //�߲�б����Ч�����ø߲����ǵ���ֵ
				{
					upV = vsData.CutsData[upElIdx].field[azIndex[upElIdx][curAzIdx]][upRIdx]; //dBZ*10
					if (upV > VALUE_INVALID)
					{
						fz1 = upV / 10. / cos_el[upElIdx];
						//outData[i][j][k]= unsigned char(upWeight*fz1*2 + 64.5);
						outData[i][j][k] = uint8_t(fz1 * 2 + 129);
					}
					else if (upV == VALUE_INVALID)
						outData[i][j][k] = CODE_INVALID;
					else if (upV == VALUE_RANFOLD)
						outData[i][j][k] = CODE_RANFOLD;
					else if (upV <= VALUE_CLUTTER)
						outData[i][j][k] = CODE_CSRED; //������������
				}
				if (g_MosaicCommParam.iHeights[i] != 5000)
					continue;
			}// end for i(0-num_Z)
		} //end for k(0-num_X)
	}// end for j(0-num_Y)

	 //�ͷŷ�λ���������桢���ǻ���
	for (size_t i = 0; i < azIndex.size(); i++)
	{
		azIndex[i].clear();
		vct_azPolar().swap(azIndex[i]);
	}
	azIndex.clear();
	vector<vct_azPolar>().swap(azIndex);
	fAvgEl.clear();
	vector<float>().swap(fAvgEl);
}