#ifndef FORMAT_CDSC_H
#define FORMAT_CDSC_H

#pragma once
#pragma pack(1)

#include <string> 
#include <vector>
#include <map>
using namespace std;

#define  cMaxMoment_SC  3 //要素总数量//
const uint32_t MOMENT_TYPE_ID_CDSC[cMaxMoment_SC]{ M_Z_ID, M_V_ID, M_W_ID };

//SC CD
// Base Data File Structure:
// Total File = Header(1024 Bytes) + Observed Base Data (Elevations*360*4*sizeof(unsigned char) Bytes)
// Where, the header includes: 
//   (1) RADARSITE
//	 (2) RADARPERFORMANCEPARAM
//   (3) RADAROBSERVATIONPARAM

#define PMODE_PPP     (0)       /* Pulse Pair Doppler */
#define PMODE_FFT     (1)       /* FFT Doppler */
#define PMODE_RPHASE  (2)       /* Random Phase Doppler */
#define PMODE_KNMI    (3)       /* KNMI custom LOG clutter filter */
#define PMODE_DPT1    (4)	/* Dual-PRT Type-1 (for low duty cycle) */
#define PMODE_DPT2    (5)	/* Dual-PRT Type-2 (for velocity unfolding) */

#define RAWRECORDLENGTH			998
#define SINGLERAWRECORDLENGTH	992
#define RVP7RECORDLENGTH		4000

struct stdRawBin
{
	uint8_t m_dbz, m_vel, m_undbz, m_sw;
};

struct stdRVP7Data
{
	uint16_t startaz, startel, endaz, endel;
	RawBin  RawData[RAWRECORDLENGTH];
};

struct stdSingleIntensity
{
	uint16_t startaz, startel, endaz, endel;
	uint8_t dbz[SINGLERAWRECORDLENGTH];
};

#define NEWRADARHEADERLENGTH	1024   //数据文件文件头长度
struct stdLAYERPARAM
{
	uint8_t  ambiguousp; 	//本层退模糊状态
								//	 0 = 无退模糊状态
								//	 1 = 软件退模糊
								//	 2 = 双T退模糊
								//	 3 = 批式退模糊
								//	 4 = 双T + 软件退模糊
								//	 5 = 批式 + 软件退模糊
								//	 6 = 双PPI退模糊
								//	 9 = 其他方式
	uint16_t	 Arotate;	//	本层天线转速,计数单位:0.01度/秒
	uint16_t	 Prf1;		//	本层的第一种脉冲重复频率,计数单位: 1/10 Hz
	uint16_t	 Prf2;		//	本层的第二种脉冲重复频率,计数单位: 1/10 Hz
	uint16_t	 spulseW;	//	本层的脉冲宽度,计数单位:	微秒
	uint16_t	 MaxV;		//	本层的最大可测速度,计数单位:	厘米/秒
	uint16_t	 MaxL;		//	本层的最大可测距离，以10米为计数单位	
	uint16_t	 binWidth;	//	 本层数据的库长，以分米为计数单位	
	uint16_t	 binnumber;	//	本层扫描线水平方向的点数
	uint16_t	 recordnumber;//	 本层扫描线垂直方向的点数
	int16_t		Swangles;//		本层的仰角，计数单位	：1/100度
};

struct stdRADARSITE  //170 Bytes
{
	char   country[30];//			国家名，文本格式输入
	char   province[20];//			省名，文本格式输入
	char   station[40];//			站名，文本格式输入
	char   stationnumber[10];//	区站号，文本格式输入
	char   radartype[20];//		雷达型号，文本格式输入
	char   longitude[16];//		天线所在经度，文本格式输入.书写格式例：E 115°32′12″
	char   latitude[16];//		天线所在纬度，文本格式输入.书写格式例：N 35°30′15″
	int32_t  longitudevalue;//	    天线所在经度的数值，以毫秒为计数单位（十进制）
							//	东经（E）为正，西经（W）为负	 	
	int32_t  lantitudevalue;//		天线所在纬度的数值，以毫秒为计数单位（十进制）
							//	北纬（N）为正，南纬（S）为负
	int32_t  height;	//			天线的海拔高度以毫米为计数单位
	int16_t   Maxangle;	//			测站四周地物阻挡的最大仰角（以秒为计数单位）
	int16_t   Opangle;	//			测站的最佳观测仰角（地物回波强度<10dbz，以秒为											计数单位）
	int16_t   MangFreq;	//			磁控管频率（通过此频率可计算雷达波长）//Whyan
};

struct stdRADARPERFORMANCEPARAM  //31 bytes
{
	int32_t   AntennaG;//		天线增益，以0.001db为计数单位
	uint16_t  BeamH;//		垂直波束宽度，以微秒为计数单位
	uint16_t  BeamL;// 		水平波束宽度，以微秒为计数单位
	uint8_t   polarizations;/*	极化状况
								 0 = 水平
								 1 = 垂直
								 2 = 双偏振
								 3 = 圆偏振
								 4 = 其他*/
	char	  sidelobe;//			第一旁瓣计数单位：db（注意：输入符号）
	uint32_t  Power;//			雷达脉冲峰值功率，以瓦为计数单位
	uint32_t  wavelength;//		波长，以微米为计数单位
	uint16_t  logA;//		对数接收机动态范围,以0.01db为计数单位
	uint16_t  LineA;//		线性接收机动态范围,以0.01为计数单位
	uint16_t  AGCP;//		AGC延迟量，以微秒为计数单位
	uint8_t   clutterT;//		杂波消除阀值，计数单位0.01db
	uint8_t	  VelocityP;/*	速度处理方式
							   0 = 无速度处理
							   1 = PPP
							   2 = FFT
							   3 = 随机编码*/
	uint8_t	 filderP;/*		地物消除方式
							 0 = 无地物消除
							 1 = 地物杂波图扣除法
							 2 = 地物杂波图 + 滤波器处理
							 3 = 滤波器处理
							 4 = 谱分析处理*/
	uint8_t	 noiseT;//		噪声消除阀值	（0-255）
	uint8_t	 SQIT;//		SQI阀值，以0.01为计数单位
	uint8_t	 intensityC;/*	rvp强度值估算采用的通道
								1 = 对数通道
								2 = 线性通道*/
	uint8_t	intensityR;/*	强度估算是否进行了距离订正
							   0 = 无
							   1 = 已进行了距离订正*/
};


struct stdRADAROBSERVATIONPARAM
{
	uint8_t 	 stype;	/*		扫描方式
						1 = RHI
						10 = PPI
						1XX = Vol 	XX为扫描圈数*/
	//unsigned char 	 syear1;//		观测记录开始时间的年的千位百位（19-20）
	uint16_t 	 syear;//		观测记录开始时间的年的十位个位（01-99）
	uint8_t 	 smonth;//		观测记录开始时间的月（1-12）
	uint8_t 	 sday;//		观测记录开始时间的日（1-31）
	uint8_t 	 shour;//		观测记录开始时间的时（00-23）
	uint8_t 	 sminute;//		观测记录开始时间的分（00-59）
	uint8_t 	 ssecond;//		观测记录开始时间的秒（00-59）
	uint8_t 	 Timep;/*		时间来源
						   0 = 计算机时钟，但一天内未进行对时
						   1 = 计算机时钟，但一天内已进行对时
						   2 = GPS
						   3 = 其他*/
	uint32_t	 smillisecond;// 		秒的小数位（计数单位微秒）
	uint8_t 	 calibration;/*	标校状态
								 0 = 无标校
								 1 = 自动标校
								 2 = 1星期内人工标校
								 3 = 1月内人工标校
								 其他码不用*/
	uint8_t 	 intensityI;//		强度积分次数（32-128）
	uint8_t		 VelocityP;//	速度处理样本数（31-255）(样本数-1）
	struct stdLAYERPARAM LayerParam[30]; //各圈扫描状态设置
									  //注：当扫描方式为RHI或PPI时，只在第一个元素中填写，其他元素为0。
	uint16_t	 RHIA;//		作RHI时的所在方位角，计数单位为1/100度
					  //			作PPI和立体扫描时不用
	int16_t		 RHIL;//				作RHI时的最低仰角，计数单位为1/100度
				  //					作其他扫描时不用
	int16_t		 RHIH;//				作RHI时的最高仰角，计数单位为1/100度
				  //				作其他扫描时不用
				  //unsigned char 	 Eyear1;//		观测结束时间的年的千位百位（19-20）
	uint16_t 	 Eyear;//		观测结束时间的年的十位个位（01-99）
	int16_t  	 Emonth;//		观测结束时间的月（1-12）
	int16_t		 Eday;//		观测结束时间的日（1-31）
	int16_t		 Ehour;//		观测结束时间的时（00-23）
	int16_t		 Eminute;//		观测结束时间的分（00-59）
	int16_t		 Esecond;//		观测结束时间的秒（00-59）
	int16_t	 	 Etenth;//		观测结束时间的1/100秒（00-59）
};
//基数据文件头格式，长度为1024个字节
struct stdNewRadarHeader
{
	struct stdRADARSITE  SiteInfo;						//170 bytes
	struct stdRADARPERFORMANCEPARAM  PerformanceInfo;	//31  bytes
	struct stdRADAROBSERVATIONPARAM  ObservationInfo;	//718 bytes  ?
	char Reserved[163];								//163 bytes  ?
};


#endif