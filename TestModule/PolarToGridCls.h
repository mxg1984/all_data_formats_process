// PolarToGridCls.h: interface for the CPolarToGridCls class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLARTOGRIDCLS_H__52B0FE66_BEFC_4DC7_8FF2_ABF501957150__INCLUDED_)
#define AFX_POLARTOGRIDCLS_H__52B0FE66_BEFC_4DC7_8FF2_ABF501957150__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./RadarbaseData/RadarBaseDataMomentTypeDef.h"
#include "./RadarbaseData/RadarBaseDataFormatTypeDef.h"
#include "./RadarbaseData/RadarBaseDataCommonPPI.h"
#include "./RadarbaseData/RadarBaseDataInputDef.h"

float const PAI		=	float(3.14159265);    
float const PI0		=	float(3.14159265/180.);
float const earth_R	=	float(6371.004);			//地球半径 (km)
float const earth_Rm=	float(4.0*earth_R/3.0);		//等效地球半径 (km)
//*
class CPolarToGridCls  
{
public:
	CPolarToGridCls();
	CPolarToGridCls(int32_t sitecode, int32_t numcuts, LPBYTE pPolarData);
	CPolarToGridCls(int32_t sitecode,char* pUniformBaseDataFile);
	virtual ~CPolarToGridCls();
	
	virtual	int32_t Run(){return -1;};

	//virtual void VolumnScanTo3DGrid(struct PPISTRUCT* strct_Data, int16_t nCuts, uint8_t***  &outData,
	//				  int16_t x_Num, float x_Dlt, int16_t y_Num, float y_Dlt, int16_t z_Num, uint16_t* Height, 
	//				  float radLon, float radLat, uint16_t radHgt){};
	virtual void LoadParameters();// {};
	virtual bool SaveGridData(){return false;};//(char *prodType, char *prodDir);
	void ObsvDateTime();
	void ObsvDateTime(stdUniformCutData firstCut);
	bool InitGridData();	
	const char* GetProdFileName();
	void LoadUniformPolarData(char* pUniformBaseDataFile);
	void LoadUniformPolarData(char* pUniformBaseDataFile, stdUniformScanData &ScanData);
	void ClearTempDatas(char *fileName);
	void ClearTempDatas(TCHAR *fileName);
	void ReleaseTempDatBuffer();

	float DecodeUniformPolarValue(int16_t src_value, int16_t factor, int16_t offset);

protected:	
	int32_t m_runNum;
	int16_t m_numCuts;
	int16_t m_numGates;
	int16_t m_numRadials;
	int32_t m_siteCode;
	struct PPISTRUCT* m_pPPIs;	
	stdUniformScanData m_commScanData;
	stdUniformScanData *m_pCommScanData;

	//parameters	
	float m_paramBeamWidth;
	struct RADARSITEINFO m_ParamSiteInfo;
	double m_paramGridLenX;
	double m_paramGridLenY;
	int32_t m_paramNumX;
	int32_t m_paramNumY;
	int32_t m_paramNumZ;
	uint16_t* m_pHeights; //m	
	std::string m_paramProdDir="";//结果路径

	std::string m_gridFileName = ""; //结果文件
	uint8_t ***m_GridData;
	int16_t m_year;
	int16_t m_month;
	int16_t m_day;
	int16_t m_hour;
	int16_t m_minute;
};
//*/
/////////////////////////////////////////////////
class CPolarToGridOfRefCls : public CPolarToGridCls
{
public:
	CPolarToGridOfRefCls();
	CPolarToGridOfRefCls(int32_t sitecode, int32_t numcuts, LPBYTE pPolarData):CPolarToGridCls(sitecode,numcuts,pPolarData){};
	CPolarToGridOfRefCls(int32_t sitecode,char* pUniformBaseDataFile):CPolarToGridCls(sitecode,pUniformBaseDataFile){};
	~CPolarToGridOfRefCls();

	//virtual	int32_t Run();
	virtual	int32_t Run1();
	virtual void LoadParameters();// {};
	///virtual void VolumnScanTo3DGrid(struct PPISTRUCT* strct_Data, int16_t nCuts, uint8_t***  &outData,
	//				  int16_t x_Num, double x_Dlt, int16_t y_Num, double y_Dlt, int16_t z_Num, uint16_t* Height,
	//	double radLon, double radLat, uint16_t radHgt);
	virtual void VolumnScanTo3DGrid(stdUniformScanData &vsData, int16_t nCuts, uint8_t***  &outData,
		int16_t x_Num, double x_Dlt, int16_t y_Num, double y_Dlt, int16_t z_Num, uint16_t* Height,
		double radLon, double radLat, uint16_t radHgt);
	virtual bool SaveGridData();
};

/////////////////////////////////////////////////
class CPolarToGridOfDopplerCls: public CPolarToGridCls
{
public:
	CPolarToGridOfDopplerCls();
	CPolarToGridOfDopplerCls(int32_t sitecode, int32_t numcuts, LPBYTE pPolarData):CPolarToGridCls(sitecode,numcuts,pPolarData){};
	CPolarToGridOfDopplerCls(int32_t sitecode,char* pUniformBaseDataFile):CPolarToGridCls(sitecode,pUniformBaseDataFile){};
	~CPolarToGridOfDopplerCls();

	//virtual	int32_t Run();
	virtual	int32_t Run1();// (stdCommScan vsScan);
	
	virtual void LoadParameters();// {};
	//virtual void VolumnScanTo3DGrid(struct PPISTRUCT* strct_Data, int16_t nCuts, uint8_t***  &outData,
	//				  int16_t x_Num, double x_Dlt, int16_t y_Num, double y_Dlt, int16_t z_Num, uint16_t* Height,
	//	double radLon, double radLat, uint16_t radHgt);
	virtual void VolumnScanTo3DGrid(stdUniformScanData &vsData, int16_t nCuts, uint8_t***  &outData,
		int16_t x_Num, double x_Dlt, int16_t y_Num, double y_Dlt, int16_t z_Num, uint16_t* Height,
		double radLon, double radLat, uint16_t radHgt);
	virtual bool SaveGridData();
};

#endif // !defined(AFX_POLARTOGRIDCLS_H__52B0FE66_BEFC_4DC7_8FF2_ABF501957150__INCLUDED_)
