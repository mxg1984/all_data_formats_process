// GridMosaicCls.h: interface for the CGridMosaicCls class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#if !defined(AFX_GRIDMOSAICCLS_H__0157FF0B_400A_451C_8219_F6FA8AA484AC__INCLUDED_)
#define AFX_GRIDMOSAICCLS_H__0157FF0B_400A_451C_8219_F6FA8AA484AC__INCLUDED_
 
#include "CommVar.h"
#include "stdDateTime.h"
#include "BaseDataFormatDef.h"
#include "MosaicDataFormatDef.h"
#include <vector>
//#include <string>

using namespace std;


struct stdPosInSiteGrid
{
	uint16_t sIdx;   //site Index;
	uint16_t xPos;	 //点在本单站网格中的x坐标
	uint16_t yPos;	 //点在本单站网格中的y坐标
	float    sRng;	 //点到雷达的距离
	uint16_t sHt;	 //点距雷达的高度
};
typedef vector<stdPosInSiteGrid> vct_sitelyPos;
typedef vector<vct_sitelyPos> vct_mosAlongXPos;
typedef vector<vct_mosAlongXPos> vct_mosAlongXYPos;

class CGridMosaicCls  
{
public:
	CGridMosaicCls(int16_t numFile, vector<string> &dataFileName,  stdDateTime *pMosaicDateTime);
	virtual ~CGridMosaicCls();

	int32_t Run();
	const char* GetProdFileName();

private:
	int16_t m_numValidSite;
	vector<MOSAICDATAHEADER> m_siteGridHeader;
	vector<RADARSITEINFO> m_siteInfo;
	vector<BaseDataInfo> m_siteGirdFiles;
	vector<FILE*> m_pFiles;
	vector<SITEPOSINMOSAIC> m_siteGridPosInMosaic;
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

	int16_t LoadSiteGridFileAndHeader(int16_t numFile, vector<string>  &pFilesNameList);
	void InitGridPosInSites();
	bool LoadParameters();
	void InitMosGrid();
	int DoMultiRadarsMosaic();
	bool SaveGridData();
	void ClearTempDatas();
	void ClearGridPosInSites();
	void PrepareWindRetrival();
};


#endif // !defined(AFX_GRIDMOSAICCLS_H__0157FF0B_400A_451C_8219_F6FA8AA484AC__INCLUDED_)
