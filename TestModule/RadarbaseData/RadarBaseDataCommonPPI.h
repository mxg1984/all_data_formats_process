#pragma once

#include <vector>
#include <string>

#include "../CommVar.h"

/////////////////////////////////////////////////////////////////////////
//
#define DATA_TYP_REF 1   //反射率
#define DATA_TYP_VEL 2   //速度
#define DATA_TYP_SPW 3   //谱宽
#define DATA_TYP_ZDR 4   //反射率差值
#define DATA_TYP_PHDP 5  //播相移差值
#define DATA_TYP_KDP 6   //比差分相移值
#define DATA_TYP_LDR 7   //退极化比值
#define DATA_TYP_ROHV 8  //相关系数值

//
const int16_t  VALUE_INVALID = -9900; //实际值中的特殊标记, 表示无有效观测数
const int16_t  VALUE_RANFOLD = -9901; //实际值中的特殊标记, 表示有距离模糊
const int16_t  VALUE_CLUTTER = -9902;
const int16_t  VALUE_CLEARAIR = -9903;
const int16_t  VALUE_SEACLUTTER = -9904;



//------------------------------------------------------------------
//雷达类型(不是指型号,而是指观测模式,多普勒or双线偏振)
const char ObsvType[2][10 ]=//[]=//[SITE_TYPE_LEN] =
{
	"Doppler",    //多普勒雷达
	"DPolar"  //双线偏振雷达
};
const int16_t id_obsv_dop = 0;  //指向ObsvType[0]
const int16_t id_obsv_dpl = 1;  //指向ObsvType[1]

//-------------------------------------------------------------------
#define req_gen_elv_dif 0.8
#define deg_to_rad 0.01745329  // Degree to radian
//#define earth_R 6371           //地球半径KM
//#define earth_Rm 8484.7        //等效地球半径(标准大气)
							   // Scan mode
#define vcp_code_ppi 41        // VCP code for PPI scan
#define vcp_code_rhi 61   // VCP code for RHI scan
#define vcp_code_vs11 11  // VCP code for volume scan mode of 11
#define vcp_code_vs21 21  // VCP code for volume scan mode of 21
#define vcp_code_vs31 31  // VCP code for volume scan mode of 31

// PPI status
#define FIRST_ELV 3    // PPI status 3: First elevation
#define LAST_ELV 4     // PPI status 4: Last elevatio
#define MIDDLE_ELV 1   // PPI status 1: Middle elevatio
#define SINGLE_ELV 10  // scan with single cut( rhi, ppi)

#define c_Factor10 10.        // Factor = 10.
#define c_Factor100 100.      // Factor = 100.
#define c_Factor1000 1000.    // Factor = 1000.
#define c_Factor10000 10000.  // Factor = 10000.
#define c_Point5 0.5
#define c_Rat_flt_int 1.
//*
#define msec_in_sec 1000  //mseconds in a second
#define hor_in_day 24  // hours in a day
#define min_in_hor 60  // minutes in an hour
#define sec_in_min 60  // seconds in a minute
#define sec_in_day 86400
#define sec_in_hor 3600
//*/
//-----------------------------------------------------------------------------
//体扫的PPI数据结构
typedef std::vector<int16_t> vct_byte2;
typedef vct_byte2 vct_uniformRadial;
typedef std::vector<vct_uniformRadial> vct_uniformSector;  //扇区--即多个radials, 包括PPI、RHI、扇扫

//体扫的PPI数据结构
struct stdUniformCutHeader
{
	char	radar_name[SITE_NAME_LEN];
	char    radar_code[SITE_CODE_LEN];
	char    radar_type[SITE_TYPE_LEN];
	char    data_name[DATA_TYPE_LEN];
	char	obsv_type[SCAN_TYPE_LEN];
	uint16_t 	year;
	uint16_t	month;
	uint16_t	day;
	uint16_t	hour;
	uint16_t	minute;
	uint16_t	second;
	uint32_t	radlat;			//*1000
	uint32_t	radlon;			//*1000
	uint16_t	radhgt;			//*meters
	int16_t		elev_angle;		//*10
	uint16_t	fstgatdis;		//*10
	uint16_t	nyq_vel;		//*10
	uint16_t	num_beam;
	uint16_t	num_gate;
	uint16_t	gateWidth;		//[nazim];  //m
	stdUniformCutHeader();
	void ScanStartDataAndTime(uint16_t& date, uint32_t& time);
};
const size_t SIZE_COM_CUT_HEADER = sizeof(stdUniformCutHeader);

struct stdUniformCutData {// 
	stdUniformCutHeader header;
	vct_byte2   elev;			//*10
	vct_byte2   azim;			//*10
	vct_uniformSector  field;	//*10  or 100
	stdUniformCutData();
    void ReleaseBuf();
};
typedef std::vector<stdUniformCutData> vct_uniformCuts;  //a Cut

//-----------------------------------------------------------
//Scan (one cut or more)
struct stdUniformScanHeader
{
	char	radar_name[SITE_NAME_LEN];
	char    radar_code[SITE_CODE_LEN];
	char    radar_type[SITE_TYPE_LEN];
	char    data_name[DATA_TYPE_LEN];
	char	obsv_type[SCAN_TYPE_LEN];
	uint32_t	radlat;			//*1000
	uint32_t	radlon;			//*1000
	uint16_t	radhgt;			//*meters
	int16_t     factor;	
	int16_t     offset;			//	
	int16_t     ndata;          //-9900.
								//数据体部分每个采样点的存储的编码与实际值的关系: 
								//编码=实际值*factor+offset
	stdUniformScanHeader();
};
const size_t SIZE_COM_SCAN_HEADER = sizeof(stdUniformScanHeader);

struct stdUniformScanData //一个要素的扫描数据
{
	stdUniformScanHeader header;  //扫描数据头	
	vct_uniformCuts CutsData;         //扫描数据提
	stdUniformScanData();
	void ReleaseBuf();
};