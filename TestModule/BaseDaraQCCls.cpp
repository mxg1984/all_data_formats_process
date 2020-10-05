// BaseDaraQCCls.cpp: implementation of the CBaseDataIOandQCCls class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <memory>
#include<string>

#include "CommFunctions.h"
#include "GlobalParams.h"
#include "BaseDaraQCCls.h"

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
	int32_t i;
	char unQcFileName[PATH_LEN] = "";//unZipFileName[PATH_LEN];
	char QcFileName[PATH_LEN] = "";
	FILE *fuzp = NULL;
	char strYear[5] = "    ";
	char strMonth[4] = "   ";
	char strDay[4] = "   ";
	char strHour[4] = "   ";
	char strMinute[4] = "   ";
	int32_t  nCuts;		//Number of elevation cuts
	int32_t  nData = 3;			//Tyoe of data : REF, VEL, SPW
	char szRadarType[SITE_TYPE_LEN] = "";
	//----------------------------------------------------------------

	//firstly, load global parameter
	LoadParameter();
	
	//����ԭʼ��ɨ���ݶ�ȡ����������ѡ��
	nCuts = 0;
	//ΪQC׼��������������ļ�
	if (!PrepareObsvDataFileForReading(m_srcBaseDataFileName.c_str(), unQcFileName))
		return -1;

	//������ QC
	//*
	if (g_iQualityControl == 1) //�������������Ƶ�ģ��
	{
		if (g_iSaveQcData == 1)
		{
			//�Ƿ񱣴��������ƻ�����
			for (i = strlen(unQcFileName); i > 0; i--)
			{
				if (unQcFileName[i] == '\\')break;
			}
			CreateDir(g_strQcDataPath[m_siteIndex]);
			sprintf_s(QcFileName, PATH_LEN,"%s%s", g_strQcDataPath[m_siteIndex], &unQcFileName[i + 1]);
		}
		;//	PREPROCBASEDATAQC(m_siteInfo, m_pRefPPI, m_pVelPPI, m_pSpwPPI, &nCuts, unQcFileName, PATH_LEN, QcFileName, PATH_LEN);	
	}
	else//����û���������Ƶ�ģ��
		;//	PREPROCBASEDATANOQC(m_siteInfo, m_pRefPPI, m_pVelPPI, m_pSpwPPI, &nCuts, unQcFileName, PATH_LEN);	
	//*/
	//RADARSITEINFO * sT = new RADARSITEINFO;
	//delete sT;
	std::unique_ptr<CRadarDataInputCls> pBaseData(new CRadarDataInputCls(m_siteCode, unQcFileName));// , szRadarType);
	if (pBaseData == nullptr) {
		return -1;
	}

	pBaseData->LoadScanData();

	//ɾ����ʱ��ɨ�ļ�
	TCHAR wszChar[PATH_LEN * 2] = _TEXT("");
	CharToTchar(unQcFileName, wszChar);
	//DeleteFile(wszChar); //MxG

	if (m_paramSaveRef)
	{
		m_strRefDataName = pBaseData->GetMomentScanFile(M_Z);
		if (m_strRefDataName.empty())
		{
			char strMsg[LEN_MSG] = "";
			sprintf_s(strMsg, "Error in QC function, number of el. cuts <9. File: %s", unQcFileName);
			ReportLog_Error(strMsg);

			m_numCuts = 0;

			//delete pBaseData;
			//pBaseData = 0;

			return -1;
		}
	}
	if(m_paramSaveVel)
	{
		m_strVelDataName = pBaseData->GetMomentScanFile(M_V);
		if (m_strVelDataName.empty())
		{
			char strMsg[LEN_MSG] = "";
			sprintf_s(strMsg, "Error in QC function, number of el. cuts <9. File: %s", unQcFileName);
			ReportLog_Error(strMsg);

			m_numCuts = 0;

			//delete pBaseData;
			//pBaseData = 0;

			return -1;
		}
	}

	//delete pBaseData;
	//pBaseData = 0;

	return 1;
}

void CBaseDataIOandQCCls::LoadParameter()
{
	//ȷ���״�վ��Ϣ
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
	else
		;
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

bool CBaseDataIOandQCCls::FileIsZipped(const char *srcFileName, char *dstFileName)
{
	std::string strSrcName = "", strDstName = "";
	strSrcName.assign(srcFileName);

	int bn = strSrcName.find(".bz2");//findStr((char*)srcFileName, ".bz2");
	if (bn==std::string::npos)
		bn = strSrcName.find(".BZ2");//findStr((char*)srcFileName, ".BZ2");
	
	if (bn == std::string::npos)
	{
		sprintf_s(dstFileName, strlen(srcFileName) + 1, "%s", srcFileName);
		return false;
	}
	else
	{		
		strDstName = strSrcName.substr(0, bn);
		sprintf_s(dstFileName, strDstName.length()+1 , "%s", strDstName.c_str());
		return true;
	}
	
	return false;
}

bool CBaseDataIOandQCCls::PrepareObsvDataFileForReading(const char* szSrcFileName, char* szDstFileName)
{
	int32_t bn=-1,bn1=-1;
	char szCopyFileName[PATH_LEN]=""; //���������ص���ʱѹ���ļ�
	char szUnzipFileName[PATH_LEN]="";
	TCHAR wszUnzipFileName[PATH_LEN] = _TEXT("");
	TCHAR wszSrcFileName[PATH_LEN] = _TEXT("");
	TCHAR wszCopyFileName[PATH_LEN] = _TEXT("");//���������ص���ʱѹ���ļ�
												//��ʼ��
	strcpy_s(szDstFileName, 2, "");

	CharToTchar(szSrcFileName, wszSrcFileName);

	//�������ݵ�������ʱ����Ŀ¼
	sprintf_s(szCopyFileName, "%s%s\\", g_DataDir.strTemDataDir,TEMP_FOLDER);			
	if(!CreateDir(szCopyFileName))
		return false;//Create directory\int32_t i
	int32_t i = 0;
	for( i=strlen(szSrcFileName); i>0; i--)
	{
		if(szSrcFileName[i]=='\\' || szSrcFileName[i] == '/')
			break;
	}
	strcat_s(szCopyFileName,&szSrcFileName[i+1]);
	CharToTchar(szCopyFileName, wszCopyFileName);
	::CopyFile(wszSrcFileName, wszCopyFileName,TRUE);		
	TcharToChar(wszCopyFileName, szCopyFileName);
	if(FileIsZipped(szCopyFileName, szDstFileName))
	{//��ѹ���ļ�(ֻ���bz2�ļ�)��������ѹ������
		UnZip(szCopyFileName,NULL);	
		//bn1=findStr(szCopyFileName,".bz2");
		//::StrCpyN(wszUnzipFileName,wszCopyFileName,bn1+1);	
		
		Sleep(5);
		if(!FilePathExists(szDstFileName))
		{
			//��ѹ���ɹ�
			char strMsg[LEN_MSG]="";
			sprintf_s(strMsg,"Faile to unzip base data file %s", szCopyFileName);
			ReportLog_Error(strMsg);

			return false;
		}
		
		DeleteFile(wszCopyFileName); //ɾ����ʱѹ���ļ�,ֻ������ѹ����ļ�
	}
	
	return true;
}

//��str�ַ����в������ַ���substr,���ҵ��������Ӵ��Ŀ�ʼλ��,����,����-1
int32_t CBaseDataIOandQCCls::findStr(char *str, char *substr)
{
	int32_t n;
	int32_t m,l; //��ʼƥ���λ��   H:\Data1\ShenZhenAvg\Z_RADR_I_Z9010_20100610064800_O_DOR_SA_CAP.bin
	char *p,*r;
	n=0;
	m=0;
	l=-1;
	while(*str)
	{
		p=str;
		r=substr;
		while(*r)
			if(*r==*p)
			{
				r++;p++;
				if(l==-1) 
					l=m;
			}			
			else
			{
				if(l!=-1 && n==0) 
					l=-1;
				break;
			}
			if(*r=='\0')
				n++;
			str++;
			m++;
	}
//	return n;
	return l;
}

