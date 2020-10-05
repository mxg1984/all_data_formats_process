#ifndef DATA_INPUT_H
#define DATA_INPUT_H

#pragma once

#include <map>

using namespace std;

//以下供读出数据用
struct stdGeneralInputScanData : public stdUniformScanData
{
	bool Needed;			//需提出来
	bool Available;			//可获得	
	stdGeneralInputScanData();
};
typedef map<uint16_t, stdGeneralInputScanData> map_GeneralScanDatas; //
//以上用于存储读出的观测数据

//标记要素 需要或者可获取
struct stdMomentFlag
{
	bool bZ;
	bool bZc;
	bool bV;
	bool bVc;
	bool bW;
	bool bWc;
	bool bZDR;
	bool bZDRc;
	bool bPDP;
	bool bKDP;
	bool bRHV;
	bool bLDR;
	stdMomentFlag();
	void Reset();
};

enum UniformMomentId
{
	M_Z_ID = 1,
	M_V_ID,   //3
	M_W_ID,    //4
	M_ZDR_ID,
	M_Zc_ID,
	M_Vc_ID,
	M_Wc_ID,
	M_ZDRc_ID,
	M_PDP_ID,
	M_KDP_ID,
	M_RHV_ID,
	M_LDR_ID,
};

uint16_t CommonMomentTypeToID(char *szType); //类型名对应的类型Index
char* CommonMomentIdToType(uint16_t id, char *szType);//类型Index对应的类型名

struct stdBaseDataRadarInfo
{
	string strRadarName;
	string strRadarCode;
	string strRadarType;
	string strScanType;
	float fRadarLon;
	float fRadarLat;
	float fRadarHt;
	stdBaseDataRadarInfo();
	void Reset();
};

#endif