#include "stdafx.h"
//#pragma pack(push)
//#pragma pack(1)

#include <experimental/filesystem>

#include <stdio.h>
#include < iostream>
#include < windows.h>
//#include <Shlwapi.h>
#include <direct.h>
#include <io.h>
#include "GlobalParams.h"
#include "CommFunctions.h"
#include "SysIntialize.h"
#include <direct.h>
#include "CommVar.h"

/////////////////////////////////////////////////////////////////////////

//执行程序需带一个参数, 如果程序参数是-r,执行实时运行, 否则,执行非实时运行
int32_t MainFunc(void)//int32_t argc, const char *argv[])
{
	//初始化配置信息
	if(InitializeConf()==false)
	{		
		ReportLog_Error("Initializing configure failed");
		return 0;
	}
	
	//清理临时数据空间
	char szTempDataFolder[MAX_PATH] = "";
	sprintf_s(szTempDataFolder, MAX_PATH, "%s%s", g_DataDir.strTemDataDir, TEMP_FOLDER);
	ClearDirectory(szTempDataFolder);

	//检查配置的各数据目录是否存在, 不存在的创建
	CheckDirectory();
	return 1;
}

//在程序启动的时候或者修改配置参数后，采用这个函数，从配置文件中读出设置参数，来初始化
BOOL InitializeConf(BOOL isReset)
{

	//初始化组网网格参数
	uint32_t i,iNumSites;
	float fMinLon, fMaxLon, fMinLat, fMaxLat;

	///////////////////////////////////////////////////////////////////////////////////////
	//区域名
	strcpy_s(g_strZoneName , "ZoneName");
	//雷达站数
	g_iSitesInZone = 0;
	for(i=0; i<TOTAL_SITES; i++)
	{
		//每个雷达站参数
		strcpy_s(g_cRadarSiteInfo[i].SiteName , "SiteName");
		strcpy_s(g_cRadarSiteInfo[i].SiteType , INVALID_STRING);
		g_cRadarSiteInfo[i].SiteID = 0;
		g_cRadarSiteInfo[i].SiteLon = 0;
		g_cRadarSiteInfo[i].SiteLat = 0;
		g_cRadarSiteInfo[i].SiteHgt = 0;
		g_cRadarSiteInfo[i].ObsRange = 0;
		//每个雷达站基数据位置
		strcpy_s(g_strRadarDataPath[i],"");
	}
	//数据路径
	strcpy_s(g_DataDir.strTemDataDir,"");
	strcpy_s(g_DataDir.strProdDataDir,"");
	strcpy_s(g_DataDir.strRainGageDir,"");
	//分辨率
	g_HorizRes.fLatRes = 0.01;
	g_HorizRes.fLonRes = 0.01;
	//高度层数
	g_iNumHeight = 0;
	//高度
	for(i=0; i<MAX_HGT_LEVELS; i++)
	{
		g_iHeights[i] = 0;
	}
	//间隔时间
	g_iIntervalTime = 6;
	//匹配时间
	g_iMatchingTime = 3;
	//网络延迟时间
	g_iDelayTime = 0;	
	//是否进行基数据质量控制
	g_iQualityControl = 0;
	//是否保存单站笛卡儿坐标数据
	g_iSaveSingleCAPPI = 0;
	//产品生成选择参数
	g_DerivedProdSelect.GenCrProd = false;
	g_DerivedProdSelect.GenEchoMovProd = false;
	g_DerivedProdSelect.GenEtProd = false;
	g_DerivedProdSelect.GenForeRefProd = false;
	g_DerivedProdSelect.GenVilProd = false;
	//是否保存NetCDF格式产品
	g_iSaveNetCDFData = false;
	//产品保留天数
	g_iProductRemainDays = 7;
	//是否输出质量控制后的基数据
	g_iSaveQcData=false;
	//处理单站三维格点数据类型选项
	g_iOptionsGridData=GRIDDATA_OPTION_REF;
	
	for(i=0; i<TOTAL_SITES; i++)
		//质量控制后的基数据保存位置
		memset(g_strQcDataPath[i], 0x0, PATH_LEN);


	//Initialize local variables
	iNumSites=0;
	fMinLon = 180.;
	fMaxLon = 0.;	
	fMinLat = 90.;
	fMaxLat = 0.;

	//初始化高度
	for(i=0; i<MAX_HGT_LEVELS; i++)
		g_MosaicCommParam.iHeights[i]=0;

	//读配置参数
	if (!ConfigureFileIO(TRUE))
		return false;

	//读速度格点化配置参数
	//ConfigureFileOfVelGridIO(TRUE); //这部分比较特殊，在速度格点化属性页部分单独获取和保存
	
	//计算站点最大经度,纬度和最小经度,纬度
	for(i=0; i<g_iSitesInZone; i++)
	{
		int32_t LonRng=int((g_cRadarSiteInfo[i].ObsRange*KM_TO_DEG)/(g_HorizRes.fLonRes));  //Range in grids 
		int32_t LatRng= int((g_cRadarSiteInfo[i].ObsRange*KM_TO_DEG)/(g_HorizRes.fLatRes)); //Range in grids 
		float WestOfSite = float(g_cRadarSiteInfo[i].SiteLon/1000.-LonRng*g_HorizRes.fLonRes);
		float EastOfSite= float(g_cRadarSiteInfo[i].SiteLon/1000.+LonRng*g_HorizRes.fLonRes);
		float NorthOfSite = float(g_cRadarSiteInfo[i].SiteLat/1000.+LatRng*g_HorizRes.fLatRes);
		float SouthOfSite= float(g_cRadarSiteInfo[i].SiteLat/1000.-LatRng*g_HorizRes.fLatRes);
		
		if(WestOfSite < fMinLon)
			fMinLon = WestOfSite;
		if(SouthOfSite < fMinLat)
			fMinLat = SouthOfSite;

		if(EastOfSite > fMaxLon)
			fMaxLon = EastOfSite;
		if(NorthOfSite > fMaxLat)
			fMaxLat = NorthOfSite;
	}

	//高度层数
	g_MosaicCommParam.iHgtNumLevels   = g_iNumHeight;
	//设置垂直层数和各层高度
	for(i=0; i<g_iNumHeight; i++)
		g_MosaicCommParam.iHeights[i] = g_iHeights[i];
	//开始高度
	g_MosaicCommParam.iStartHgt = g_MosaicCommParam.iHeights[0];	//米
	//水平分辨率
	g_MosaicCommParam.iLatRes   = uint32_t( g_HorizRes.fLatRes*10000.+.5);
	g_MosaicCommParam.iLonRes   = uint32_t( g_HorizRes.fLonRes*10000.+.5);

	//根据区域内站点经度,纬度覆盖范围,计算组网区域的起始经度和纬度,以及格点数

	//如果经度小于零,表示在西半球; 如果纬度小于0,表示在南半球
	g_MosaicCommParam.iStartLon = int(fMinLon*1000);
	g_MosaicCommParam.iStartLat = int(fMinLat*1000);
	//Number of grids
	g_MosaicCommParam.iLonNumGrids = int((fMaxLon-fMinLon)/g_HorizRes.fLonRes)+1;
	g_MosaicCommParam.iLatNumGrids = int((fMaxLat-fMinLat)/g_HorizRes.fLatRes)+1;
	
	//求各雷达站在组网坐标中的位置
	ZeroMemory(g_SiteRangeInMosaic, sizeof(SITEPOSINMOSAIC)*TOTAL_SITES);
	int32_t   SiteLonPos, SiteLatPos; //雷达中心相对于组网网格的坐标
	float fLon0, fLat0;//区域中心经度、纬度
	fLon0 = float(g_MosaicCommParam.iStartLon/1000.+(g_HorizRes.fLonRes)*(g_MosaicCommParam.iLonNumGrids/2));
	fLat0 = float(g_MosaicCommParam.iStartLat/1000.+(g_HorizRes.fLatRes)*(g_MosaicCommParam.iLatNumGrids/2));
	for(i=0; i<g_iSitesInZone; i++)
	{
		int32_t LonRng=int((g_cRadarSiteInfo[i].ObsRange*KM_TO_DEG)/(g_HorizRes.fLonRes)); //Range in grids 
		int32_t LatRng= int((g_cRadarSiteInfo[i].ObsRange*KM_TO_DEG)/(g_HorizRes.fLatRes)); //Range in grids 

		SiteLonPos = int((g_cRadarSiteInfo[i].SiteLon*10.-g_MosaicCommParam.iStartLon*10.)/(g_MosaicCommParam.iLonRes));
		SiteLatPos = int((g_cRadarSiteInfo[i].SiteLat*10.-g_MosaicCommParam.iStartLat*10.)/(g_MosaicCommParam.iLatRes));

		//单站格点移动到组网坐标中后，起始格点和终止格点在组网坐标的位置(X,Y)
		g_SiteRangeInMosaic[i].iMinLonGrid = max(SiteLonPos-LonRng, 0);
		g_SiteRangeInMosaic[i]/*g_cRadarSiteInfo[i].SiteID]*/.iMaxLonGrid = min(SiteLonPos+LonRng, g_MosaicCommParam.iLonNumGrids);
		g_SiteRangeInMosaic[i]/*g_cRadarSiteInfo[i].SiteID]*/.iMinLatGrid = max(SiteLatPos-LatRng, 0);
		g_SiteRangeInMosaic[i]/*g_cRadarSiteInfo[i].SiteID]*/.iMaxLatGrid = min(SiteLatPos+LatRng, g_MosaicCommParam.iLatNumGrids); 
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// int32_t ConfigureFileIO()
//
//Function: read configure parameters from the congifuration file
//
// Description: 
//	- ...
//	- ...
//
// Input Params:   
//	 BOOL bRead     --  Flag indicates reading or writing
//
// Output data elements: 
//   IF read or write file successfully, return true, els, false.
//
// Author:  Whyan
// Date:    2007.04.10
// Date:    2007.06.29  Add some new parameters
//
BOOL ConfigureFileIO(BOOL bRead)
{
	FILE *fp=0;
	errno_t err = 0;
	string strFullName="";//[PATH_LEN] = " ";
	string strFullNameOld="";//[PATH_LEN] = " ";
	uint32_t i, j;

	/////////////////////////////////////////////////////////////
	char appPath[256]="C:/Users/mengxiangang/Desktop/TestModule(20201004)";	
	//_getcwd(appPath, sizeof(appPath));

	strFullName.assign(appPath);
	strFullName += std::experimental::filesystem::path::preferred_separator;
	strFullName.append(SETTINGFILE);
	
	if(bRead)
	{//读操作		
		//Open the file,return FALSE if fiales to open it.
		//err=fopen_s(&fp,strFullName.c_str(),"rb");
		//if (err != 0) {
		//	return FALSE;
		//}

		//区域名
		strcpy(g_strZoneName, "HB");
		//if(!ReadOneItem(g_strZoneName, sizeof(g_strZoneName), 1, fp)) {
		//	fclose(fp); 
		//	ClearConfig(); 
		//	return false;
		//}
		//雷达站数
		g_iSitesInZone = 4;
		//if(!ReadOneItem(&g_iSitesInZone, sizeof(g_iSitesInZone), 1, fp)) {
		//	fclose(fp); 
		//	ClearConfig(); 
		//	return false;
		//}
		uint32_t i = 0;
		g_cRadarSiteInfo[i].SiteID = 10;
		strcpy(g_cRadarSiteInfo[i].SiteName, "北京");
		strcpy(g_cRadarSiteInfo[i].SiteType, "SA");
		g_cRadarSiteInfo[i].SiteLon = 116470;
		g_cRadarSiteInfo[i].SiteLat = 39810;
		g_cRadarSiteInfo[i].SiteHgt = 92;
		g_cRadarSiteInfo[i].ObsRange = 300;

		++i;
		g_cRadarSiteInfo[i].SiteID = 220;
		strcpy(g_cRadarSiteInfo[i].SiteName, "塘沽");
		strcpy(g_cRadarSiteInfo[i].SiteType, "SA");
		g_cRadarSiteInfo[i].SiteLon = 117720;
		g_cRadarSiteInfo[i].SiteLat = 39040;
		g_cRadarSiteInfo[i].SiteHgt = 69;
		g_cRadarSiteInfo[i].ObsRange = 300;

		++i;
		g_cRadarSiteInfo[i].SiteID = 335;
		strcpy(g_cRadarSiteInfo[i].SiteName, "秦皇岛");
		strcpy(g_cRadarSiteInfo[i].SiteType, "SA");
		g_cRadarSiteInfo[i].SiteLon = 118880;
		g_cRadarSiteInfo[i].SiteLat = 39880;
		g_cRadarSiteInfo[i].SiteHgt = 114;
		g_cRadarSiteInfo[i].ObsRange = 300;

		++i;
		g_cRadarSiteInfo[i].SiteID = 311;
		strcpy(g_cRadarSiteInfo[i].SiteName, "石家庄");
		strcpy(g_cRadarSiteInfo[i].SiteType, "SA");
		g_cRadarSiteInfo[i].SiteLon = 114710;
		g_cRadarSiteInfo[i].SiteLat = 38350;
		g_cRadarSiteInfo[i].SiteHgt = 134;
		g_cRadarSiteInfo[i].ObsRange = 300;

		i = 0;
		strcpy(g_strRadarDataPath[i++], "D:/data/20200706-huabei/Z9010/");
		strcpy(g_strRadarDataPath[i++], "D:/data/20200706-huabei/Z9220/");
		strcpy(g_strRadarDataPath[i++], "D:/data/20200706-huabei/Z9335/");
		strcpy(g_strRadarDataPath[i++], "D:/data/20200706-huabei/Z9311/");
	
		//for(i=0; i<g_iSitesInZone; i++)
		//{
		//	if(i<TOTAL_SITES)
		//	{//读取区域可容最多雷达数内的雷达站信息
		//	//每个雷达站参数
		//	if(!ReadOneItem(&g_cRadarSiteInfo[i], sizeof(g_cRadarSiteInfo[i]), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//	//每个雷达站基数据位置
		//	if(!ReadOneItem(g_strRadarDataPath[i], sizeof(char), PATH_LEN, fp)) {
		//		fclose(fp); 
		//		ClearConfig(); 
		//		return false;
		//	}
		//	}
		//	else
		//	{//舍弃超出区域可容最多雷达数的雷达站信息
		//		struct RADARSITEINFO tmpsiteinfo;
		//		char tmpPath[PATH_LEN];
		//		//每个雷达站参数
		//		if(!ReadOneItem(&tmpsiteinfo, sizeof(g_cRadarSiteInfo[i]), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//		//每个雷达站基数据位置
		//		if(!ReadOneItem(tmpPath, sizeof(char), PATH_LEN, fp)) {
		//			fclose(fp); 
		//			ClearConfig(); 
		//			return false;
		//		}
		//	}
		//}
		//数据路径
		strcpy(g_DataDir.strTemDataDir, "D:\\data\\20200706-huabei\\prod\\");
		strcpy(g_DataDir.strRainGageDir, "");
		strcpy(g_DataDir.strProdDataDir, "D:\\data\\20200706-huabei\\prod\\");
		strcpy(g_DataDir.strRadarDataDir, "D:\\data\\20200706-huabei\\");
		g_DataDir.iRadarSubflderType = 1;
		//if(!ReadOneItem(&g_DataDir, sizeof(g_DataDir), 1, fp)) {
		//	fclose(fp); 
		//	ClearConfig(); 
		//	return false;
		//}
		//分辨率
		g_HorizRes.fLatRes = 0.01;
		g_HorizRes.fLonRes = 0.01;
		//if(!ReadOneItem(&g_HorizRes, sizeof(g_HorizRes), 1, fp)) {
		//	fclose(fp); 
		//	ClearConfig(); 
		//	return false;
		//}
		//是否用户自定义高度
		//size_t t = sizeof(g_bUserDefineLevels);
		g_bUserDefineLevels = 0;
		//if(!ReadOneItem(&g_bUserDefineLevels, sizeof(g_bUserDefineLevels), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//高度层数
		g_iNumHeight = 16;
		//if(!ReadOneItem(&g_iNumHeight, sizeof(g_iNumHeight), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//高度
		j = 0;
		g_iHeights[j++] = 1000;
		g_iHeights[j++] = 1500;
		g_iHeights[j++] = 2000;
		g_iHeights[j++] = 2500;
		g_iHeights[j++] = 3000;
		g_iHeights[j++] = 3500;
		g_iHeights[j++] = 4000;
		g_iHeights[j++] = 4500;
		g_iHeights[j++] = 5000;
		g_iHeights[j++] = 5500;
		g_iHeights[j++] = 6000;
		g_iHeights[j++] = 6500;
		g_iHeights[j++] = 7000;
		g_iHeights[j++] = 8000;
		g_iHeights[j++] = 9000;
		g_iHeights[j++] = 10000;

		//for(j=0; j<g_iNumHeight; j++)
		//{
		//	if(!ReadOneItem(&g_iHeights[j], sizeof(g_iHeights[j]), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//}
		//间隔时间
		g_iIntervalTime = 6;
		//if(!ReadOneItem(&g_iIntervalTime, sizeof(g_iIntervalTime), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//匹配时间
		g_iMatchingTime = 3;
		//if(!ReadOneItem(&g_iMatchingTime, sizeof(g_iMatchingTime), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//网络延迟时间
		g_iDelayTime = 8;
		//if(!ReadOneItem(&g_iDelayTime, sizeof(g_iDelayTime), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		
		//是否进行基数据质量控制
		g_iQualityControl = 1;
		//if(!ReadOneItem(&g_iQualityControl, sizeof(g_iQualityControl), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//是否保存单站笛卡儿坐标数据
		g_iSaveSingleCAPPI = 0;
		//if(!ReadOneItem(&g_iSaveSingleCAPPI, sizeof(g_iSaveSingleCAPPI), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//产品生成选择参数
		g_DerivedProdSelect.GenForeRefProd = false;
		g_DerivedProdSelect.GenEchoMovProd = false;
		g_DerivedProdSelect.GenCrProd = false;
		g_DerivedProdSelect.GenEtProd = false;
		g_DerivedProdSelect.GenVilProd = false;

		//if(!ReadOneItem(&g_DerivedProdSelect, sizeof(g_DerivedProdSelect), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//The following parameters is added after 2007.6.28
		//是否保存NETCDF格式的产品
		g_iSaveNetCDFData = false;
		//if(!ReadOneItem(&g_iSaveNetCDFData, sizeof(g_iSaveNetCDFData), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//产品保存天数
		g_iProductRemainDays = 5;
		//if(!ReadOneItem(&g_iProductRemainDays, sizeof(g_iProductRemainDays), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//是否保存质量控制的基数据
		g_iSaveQcData = 0;
		//if(!ReadOneItem(&g_iSaveQcData, sizeof(g_iSaveQcData), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//质量控制基数据保存位置
		i = 0;
		strcpy(g_strQcDataPath[i++], "");
		strcpy(g_strQcDataPath[i++], "");
		strcpy(g_strQcDataPath[i++], "");
		strcpy(g_strQcDataPath[i++], "");
		//for(i=0; i<g_iSitesInZone; i++)
		//{
		//	if(i<TOTAL_SITES)
		//	{//读取区域可容最多雷达数内的雷达QC数据保存路径
		//		if(!ReadOneItem(g_strQcDataPath[i], sizeof(char), PATH_LEN, fp)) {fclose(fp); ClearConfig(); return false;}
		//	}
		//	else
		//	{//舍弃超出区域可容最多雷达数的信息
		//		char tempPath[PATH_LEN];
		//		if(!ReadOneItem(tempPath, sizeof(char), PATH_LEN, fp)) {fclose(fp); ClearConfig(); return false;}
		//	}
		//}
		//是否删除实时模式下处理过的基数据
		g_iDeleteOldBaseData = 0;
		//if(!ReadOneItem(&g_iDeleteOldBaseData, sizeof(g_iDeleteOldBaseData), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//单站格点数据处理选项
		g_iOptionsGridData = 1;
		//if(!ReadOneItem(&g_iOptionsGridData, sizeof(g_iOptionsGridData), 1, fp)) {fclose(fp); ClearConfig(); return false;}	
	    //新增的产品生成选择参数
		g_AddDerivedProdSelect.GenQPEPRod = 0;
		g_AddDerivedProdSelect.GenCtProd = 0;
		g_AddDerivedProdSelect.Spare2 = 0;
		g_AddDerivedProdSelect.Spare3 = 0;
		g_AddDerivedProdSelect.Spare4 = 0;
		g_AddDerivedProdSelect.Spare5 = 0;
		g_AddDerivedProdSelect.Spare6 = 0;
		g_AddDerivedProdSelect.Spare7 = 0;
		g_AddDerivedProdSelect.Spare8 = 0;
		g_AddDerivedProdSelect.Spare9 = 0;
		g_AddDerivedProdSelect.Spare10 = 0;
		g_AddDerivedProdSelect.Spare11 = 0;
		g_AddDerivedProdSelect.Spare12 = 0;
		g_AddDerivedProdSelect.Spare13 = 1;
		g_AddDerivedProdSelect.Spare14 = 0;
		//if(!ReadOneItem(&g_AddDerivedProdSelect, sizeof(g_AddDerivedProdSelect), 1, fp)) {
		//	fclose(fp); 
		//	ClearConfig(); 
		//	return false;
		//}
		//新增日志显示参数
		g_LogShowParam.bShowLog = true;
		g_LogShowParam.iMaxStatusItem = 30;
		g_LogShowParam.iMaxErrorItem = 5;
		//if(!ReadOneItem(&g_LogShowParam, sizeof(g_LogShowParam), 1, fp)) {
		//	fclose(fp); 
		//	ClearConfig(); 
		//	return false;
		//}
		
		//fclose(fp);
		g_iSitesInZone = min(g_iSitesInZone,TOTAL_SITES);

	}
	else
	{//写操作

		//Open the file,return FALSE if fiales to open it.
		string strNameBack="";//[PATH_LEN];
		strNameBack.assign(appPath);
		strNameBack += std::experimental::filesystem::path::preferred_separator;
		strNameBack.append(SETTINGFILE_BCK);

		TCHAR wszSrcFile[PATH_LEN]=_TEXT("");
		TCHAR wsBbackFile[PATH_LEN] = _TEXT("");
		CharToTchar(strFullName.c_str(), wszSrcFile);
		CharToTchar(strNameBack.c_str(), wsBbackFile);
		//backup the configure file
		CopyFile(wszSrcFile,wsBbackFile, FALSE);

		//Open the file,return FALSE if fiales to open it.
		err=fopen_s(&fp,strFullName.c_str(),"wb");
		if(err!=0)
			return FALSE;

		//区域名
		if(!WriteOneItem(g_strZoneName, sizeof(g_strZoneName), 1, fp)) {fclose(fp); return false;}
		//雷达站数
		if(!WriteOneItem(&g_iSitesInZone, sizeof(g_iSitesInZone), 1, fp)) {fclose(fp); return false;}
		for(i=0; i<g_iSitesInZone; i++)
		{
			//每个雷达站参数
			if(!WriteOneItem(&g_cRadarSiteInfo[i], sizeof(g_cRadarSiteInfo[i]), 1, fp)) {fclose(fp); return false;}
			//每个雷达站基数据位置
			if(!WriteOneItem(g_strRadarDataPath[i], sizeof(char), PATH_LEN, fp)) {fclose(fp); return false;}
		}
		//数据路径
		if(!WriteOneItem(&g_DataDir, sizeof(g_DataDir), 1, fp)) {fclose(fp); return false;}
		//分辨率
		if(!WriteOneItem(&g_HorizRes, sizeof(g_HorizRes), 1, fp)) {fclose(fp); return false;}
		//是否用户自定义高度
		if(!WriteOneItem(&g_bUserDefineLevels, sizeof(g_bUserDefineLevels), 1, fp)) {fclose(fp); return false;}
		//高度层数
		if(!WriteOneItem(&g_iNumHeight, sizeof(g_iNumHeight), 1, fp)) {fclose(fp); return false;}
		//高度
		for(j=0; j<g_iNumHeight; j++)
		{
			if(!WriteOneItem(&g_iHeights[j], sizeof(g_iHeights[j]), 1, fp)) {fclose(fp); return false;}
		}
		//间隔时间
		if(!WriteOneItem(&g_iIntervalTime, sizeof(g_iIntervalTime), 1, fp)) {fclose(fp); return false;}
		//匹配时间
		if(!WriteOneItem(&g_iMatchingTime, sizeof(g_iMatchingTime), 1, fp)) {fclose(fp); return false;}
		//网络延迟时间
		if(!WriteOneItem(&g_iDelayTime, sizeof(g_iDelayTime), 1, fp)) {fclose(fp); return false;}
		
		//是否进行基数据质量控制
		if(!WriteOneItem(&g_iQualityControl, sizeof(g_iQualityControl), 1, fp)) {fclose(fp); return false;}
		//是否保存单站笛卡儿坐标数据
		if(!WriteOneItem(&g_iSaveSingleCAPPI, sizeof(g_iSaveSingleCAPPI), 1, fp)) {fclose(fp); return false;}
		//产品生成选择参数
		if(!WriteOneItem(&g_DerivedProdSelect, sizeof(g_DerivedProdSelect), 1, fp)) {fclose(fp); return false;}
		//是否保存NETCDF格式的产品
		if(!WriteOneItem(&g_iSaveNetCDFData, sizeof(g_iSaveNetCDFData), 1, fp)) {fclose(fp); return false;}
		//产品保存天数
		if(!WriteOneItem(&g_iProductRemainDays, sizeof(g_iProductRemainDays), 1, fp)) {fclose(fp); return false;}
		//是否保存质量控制的基数据
		if(!WriteOneItem(&g_iSaveQcData, sizeof(g_iSaveQcData), 1, fp)) {fclose(fp); return false;}
		//质量控制基数据保存位置
		for(i=0; i<g_iSitesInZone; i++)
			if(!WriteOneItem(g_strQcDataPath[i], sizeof(char), PATH_LEN, fp)) {fclose(fp); return false;}
		//是否删除实时模式下处理过的基数据
		if(!WriteOneItem(&g_iDeleteOldBaseData, sizeof(g_iDeleteOldBaseData), 1, fp)) {fclose(fp); return false;}//m_bDeleteOldBaseData
		//格点数据类型标记
		if(!WriteOneItem(&g_iOptionsGridData, sizeof(g_iOptionsGridData), 1, fp)) {fclose(fp); return false;}
	   //新增的产品生成选择参数
		if(!WriteOneItem(&g_AddDerivedProdSelect, sizeof(g_AddDerivedProdSelect), 1, fp)) {fclose(fp); return false;}
		//新增日志显示参数
		if(!WriteOneItem(&g_LogShowParam, sizeof(g_LogShowParam), 1, fp)) {fclose(fp); ClearConfig(); return false;}

		fclose(fp);
		return TRUE;

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//拷贝原始配置文件到临时文件夹下，为格点径向速度数据处理模块提供参数
		std::string strConfig2Copy(g_DataDir.strTemDataDir);
		strConfig2Copy += std::experimental::filesystem::path::preferred_separator;
		strConfig2Copy += SGL_MSCV_FOLDER;
		strConfig2Copy += std::experimental::filesystem::path::preferred_separator;
		strConfig2Copy += SETTINGFILE;

		//backup the configure file
		//CharToTchar(strFullName.c_str(), wszSrcFile);
		CharToTchar(strConfig2Copy.c_str(), wsBbackFile);		
		CopyFile(wszSrcFile, wsBbackFile, FALSE);
	}
	return (true);
}

//速度格点化的参数
BOOL ConfigureFileOfVelGridIO(BOOL bRead)
{
	FILE *fp=0;
	errno_t err = 0;
	char strFullName[PATH_LEN] = " ";
	char strFullNameOld[PATH_LEN] = " ";
	uint32_t  j;

	/////////////////////////////////////////////////////////////
	//Name of the site information file
	strcpy_s(strFullName,SETTINGVELGRIDFILE);	
	
	if(bRead)
	{//读操作
		//Open the file,return FALSE if fiales to open it.
		err =fopen_s(&fp,strFullName,"rb");
		if(err!=0)
			return FALSE;

		if(!ReadOneItem(&g_PolarToGrdParam, sizeof(g_PolarToGrdParam), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//是否用户自定义高度
		if(!ReadOneItem(&g_bUserDefineVelLevels, sizeof(g_bUserDefineVelLevels), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//高度层数
		if(!ReadOneItem(&g_iNumVelHeight, sizeof(g_iNumVelHeight), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		//高度
		for(j=0; j<g_iNumVelHeight; j++)
		{//
			if(!ReadOneItem(&g_iVelHeights[j], sizeof(g_iVelHeights[j]), 1, fp)) {fclose(fp); ClearConfig(); return false;}
		}
		fclose(fp);
	}
	else
	{//写操作
		//Open the file,return FALSE if fiales to open it.
		err=fopen_s(&fp,strFullName,"wb");
		if(err!=0)
			return FALSE;

				//分辨率
		if(!WriteOneItem(&g_PolarToGrdParam, sizeof(g_PolarToGrdParam), 1, fp)) {fclose(fp); return false;}
		//是否用户自定义高度
		if(!WriteOneItem(&g_bUserDefineVelLevels, sizeof(g_bUserDefineVelLevels), 1, fp)) {fclose(fp); return false;}
		//高度层数
		if(!WriteOneItem(&g_iNumVelHeight, sizeof(g_iNumVelHeight), 1, fp)) {fclose(fp); return false;}
		//高度
		for(j=0; j<g_iNumVelHeight; j++)
		{
			if(!WriteOneItem(&g_iVelHeights[j], sizeof(g_iVelHeights[j]), 1, fp)) {fclose(fp); return false;}
		}
		fclose(fp);
	}

	return (true);
}
/////////////////////////////////////////////////////////////////////////////
// int32_t WriteOneItem()
//
//Function: write data into a file
//
//Params:   FILE* fp     --  stream of the file to write data in
//          void *Data   --  Pointer to data to be written
//          size_t size  --  Item size in bytes.
//          size_t count --  Maximum number of items to be written.
//
// Description: 
//	- ...
//	- ...
//
// Input/Output data elements: None.
//
// Author:  Whyan
// Date:    2007.06.29
//
BOOL WriteOneItem(void *Data, size_t size, size_t count, FILE *fp)
{
	size_t iostat;
	iostat = fwrite(Data, size, count, fp);

	if(iostat<count)
	{
		if(feof(fp)!=0) 
		{
			ReportLog_Error("Reading data pass the end of the file!");
			fclose(fp);
		}
		else if(ferror(fp)!=0)
		{
			ReportLog_Error("Read data error!");
			fclose(fp);
		}
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// int32_t ReadOneItem()
//
//Function: read data from a file
//
//Params:   FILE* fp     --  stream of the file to read data from.
//          void *Data   --  Storage location for data fread from the file.
//          size_t size  --  Item size in bytes.
//          size_t count --  Maximum number of items to be written.
//
// Description: 
//	- ...
//	- ...
//
// Input/Output data elements: None.
//
// Author:  Whyan
// Date:    2007.06.29
//
BOOL ReadOneItem(void *Data, size_t size, size_t count, FILE *fp)
{
	size_t iostat;	
	iostat = fread(Data, size, count, fp);

	if(iostat<count)
	{
		if(feof(fp)!=0) 
		{
			ReportLog_Error("Reading data pass the end of the file!");
			fclose(fp);
		}
		else if(ferror(fp)!=0)
		{
			ReportLog_Error("Read data error!");
			fclose(fp);
		}
		return false;
	}
	return true;
}

void ClearConfig()
{

	char strFullName[PATH_LEN] = " ";
	char strFullNameOld[PATH_LEN] = " ";	

	/////////////////////////////////////////////////////////////

	//Name of the site information file
	strcpy_s(strFullName,SETTINGFILE);
	strcpy_s(strFullNameOld,SETTINGFILEOLD);

	//清除配置文件
	remove(strFullName);
	remove(strFullNameOld);
}


//检查是否所有设置的路径都有效或存在,不存在的自动创建
void CheckDirectory()
{	
	char strPath[PATH_LEN]=" ", strBackPath[PATH_LEN] = " ";
	
	//产品路径
	CreateDir(g_DataDir.strProdDataDir);

	//Log 路径
	TCHAR wszChar[PATH_LEN] = _TEXT("");
	CharToTchar(g_strAppTmpFolder, wszChar);
	if (GetCurrentDirectory(PATH_LEN, wszChar) != 0) //g_strAppTmpFolder)!=0)
	{
		//Create folder for log files
		sprintf_s(strPath, "%s%s%s", g_strAppTmpFolder, std::to_string(std::experimental::filesystem::path::preferred_separator).c_str(), LOGS_FOLDER);
		CreateDir(strPath);
	}

	//Temp数据上级目录
	CreateDir(g_DataDir.strTemDataDir);
	
	//创建临时数据子目录
	if(g_DataDir.strTemDataDir[strlen(g_DataDir.strTemDataDir)-1]=='\\' || g_DataDir.strTemDataDir[strlen(g_DataDir.strTemDataDir) - 1] == '/')
		sprintf_s(strBackPath, "%s%s", g_DataDir.strTemDataDir, TEMP_FOLDER);
	else
		sprintf_s(strBackPath, "%s%s%s", g_DataDir.strTemDataDir, std::to_string(std::experimental::filesystem::path::preferred_separator).c_str(),
			TEMP_FOLDER);
	CreateDir(strBackPath);

	//创建临时保存单站统一格式基数据的目录
	sprintf_s(strPath, "%s%s%s", strBackPath, std::to_string(std::experimental::filesystem::path::preferred_separator).c_str(), UNIFORM_POLAR_FOLDER);
	DeleteDirectory(strPath,0);
	CreateDir(strPath);

	//创建临时保存单站笛卡儿坐标反射率因子数据的目录
	sprintf_s(strPath, "%s%s%s", strBackPath, 
		std::to_string(std::experimental::filesystem::path::preferred_separator).c_str(), SGL_MSC_FOLDER);
	if (g_iSaveSingleCAPPI == 0) DeleteDirectory(strPath, 0); //删除旧的单站数据
	CreateDir(strPath);
	
	//创建临时保存单站笛卡儿坐标径向速度数据的目录
	if(g_iOptionsGridData==GRIDDATA_OPTION_VEL || g_iOptionsGridData==GRIDDATA_OPTION_ALL)//(g_iGridVelocity==1)
	{
		sprintf_s(strPath, "%s%s%s", strBackPath, std::to_string(std::experimental::filesystem::path::preferred_separator).c_str(), SGL_MSCV_FOLDER);
		DeleteDirectory(strPath,0); //删除旧的单站数据
		CreateDir(strPath);
	}

	//清理临时数据空间
	char szTempDataFolder[MAX_PATH] = "";
	sprintf_s(szTempDataFolder, "%s%s", g_DataDir.strTemDataDir, TEMP_FOLDER);
	ClearDirectory(szTempDataFolder);
}

BOOL ClearDirectory(char chrDirName[])
{
	TCHAR wszChar[PATH_LEN] = _TEXT("");
	CharToTchar(chrDirName, wszChar);
    CString strDirName=_T("");
	strDirName.Format(_T("%s"), wszChar);

    BOOL result;
    HANDLE Handle;
    WIN32_FIND_DATA fData;
    CString strTemp;
    DWORD errorcode;

	CString strFilter = _T("");
	strFilter.Format(_T("%s%s*.*"), wszChar, _T("\\"));
    Handle = ::FindFirstFile(strFilter, &fData);

    if (Handle == INVALID_HANDLE_VALUE)
        return FALSE;

    do {
        errorcode = GetLastError();

        if ( fData.cFileName[0] == '.' )
            continue;

        if (fData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) 
		{
			continue;
            if (strDirName[strDirName.GetLength()-1] != '\\')
			{
				 char chA[MAX_PATH]="";
                CString strA = strDirName+'\\'+ fData.cFileName;

				TcharToChar(strA.GetBuffer(), chA);
                //strcpy_s(chA, strA.GetLength() + 1, (char const*)(strA.GetBuffer()));
				strA.ReleaseBuffer();
                ClearDirectory(chA);
            }
            else{
				char chB[MAX_PATH]="";
                CString strB = strDirName + fData.cFileName;
				TcharToChar(strB.GetBuffer(), chB);
               // strcpy_s(chB, strB.GetLength() + 1, (char const*)strB.GetBuffer());
				strB.ReleaseBuffer();
                ClearDirectory(chB);
            }
			CString tmpStr = _T("");
			tmpStr.Format(_T("%s%s"), _T("\\"),fData.cFileName);
			strTemp = strDirName + tmpStr;// "\\" + fData.cFileName;

            SetFileAttributes(strTemp, ~FILE_ATTRIBUTE_READONLY);
            if (!RemoveDirectory(strTemp))
                result = FALSE;
            else
                result = TRUE;
        }
        else
        {
			CString tmpStr = _T("");
			tmpStr.Format(_T("%s%s"), _T("\\"), fData.cFileName);
            strTemp = strDirName + tmpStr;// "\\" + fData.cFileName;
            BOOL bl = SetFileAttributes(strTemp, ~FILE_ATTRIBUTE_READONLY);

            if (!DeleteFile(strTemp))
                result = FALSE;
            else
                result = TRUE;
        }

    }while(FindNextFile(Handle,&fData));

    errorcode = GetLastError();

    if (errorcode == ERROR_NO_MORE_FILES)//空目录
    {
        ::RemoveDirectory(strDirName);
        result = TRUE;
    }

    if (Handle)
        FindClose(Handle);

    return result;
}
//#pragma pack(pop)