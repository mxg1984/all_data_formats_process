#pragma once
 
#include <vector>
#include <string>

#include "CommVar.h"
#include "stdDateTime.h"
#include "BaseDataFormatDef.h"
#include "MosaicDataFormatDef.h"

struct stdPosInSiteGrid
{
	uint16_t sIdx;   //site Index;
	uint16_t xPos;	 //点在本单站网格中的x坐标
	uint16_t yPos;	 //点在本单站网格中的y坐标
	float    sRng;	 //点到雷达的距离
	uint16_t sHt;	 //点距雷达的高度
};
typedef std::vector<stdPosInSiteGrid> vct_sitelyPos;
typedef std::vector<vct_sitelyPos> vct_mosAlongXPos;
typedef std::vector<vct_mosAlongXPos> vct_mosAlongXYPos;

class CGridMosaicCls  
{
public:
	CGridMosaicCls(int16_t numFile, std::vector<std::string> &dataFileName,  stdDateTime *pMosaicDateTime);
	virtual ~CGridMosaicCls();

	int32_t Run();
	const char* GetProdFileName();

private:
	int16_t m_numValidSite;
	std::vector<MOSAICDATAHEADER> m_siteGridHeader;
	std::vector<RADARSITEINFO> m_siteInfo;
	std::vector<BaseDataInfo> m_siteGirdFiles;
	std::vector<FILE*> m_pFiles;
	std::vector<SITEPOSINMOSAIC> m_siteGridPosInMosaic;
	struct stdDateTime m_mosaicDateTime;
	vct_mosAlongXYPos m_mosGridPosInSite;  //组网的每个格点在单站中的相对位置参数	

	float m_paramAlgThrRng; //=100km
	int32_t m_paramNumX;
	int32_t m_paramNumY;
	double m_paramMinX;
	double m_paramMinY;
	double m_paramMaxX;
	double m_paramMaxY;	
	double m_paramGridLenX;
	double m_paramGridLenY;
	int32_t m_paramNumZ;
	uint16_t* m_pHeights; //m	
	std::string m_paramProdDir="";//结果路径

	std::string m_mosGridFileName="";  //结果文件
	uint8_t ***m_mosGridData;

	int16_t LoadSiteGridFileAndHeader(int16_t numFile, std::vector<std::string>  &pFilesNameList);
	void InitGridPosInSites();
	bool LoadParameters();
	void InitMosGrid();
	int DoMultiRadarsMosaic();
	bool SaveGridData();
	void ClearTempDatas();
	void ClearGridPosInSites();
	void PrepareWindRetrival();
};
