
#ifndef FORMAT_CC1_H
#define FORMAT_CC1_H

#include "RadarBaseDataMomentTypeDef.h"
#include "RadarBaseDataFormatTypeDef.h"
#pragma once
#pragma pack(1)

#include <string> 
#include <vector>
#include <map>
using namespace std;

#define MAXAZIMUTH 1000
#define MAXBINNUMBER 1000
#define MAXELEVNUMBER 15

#define INVALID_VALUE_CC1 -32768

//tagVPPISCANPARAMETER仰角层的结构
struct stdCC1CutParam
{
	uint16_t usMaxV;			//最大可测速度(厘米/秒)
	uint16_t usMaxL;			//最大可测距离(10米)
	uint16_t usBindWidth;		//库长(米)
	uint16_t usBinNumber;		//每径向库数
	uint16_t usRecordNumber;	//本圈径向数
	uint16_t usArotate;		    //本圈转速(0.01度/秒)
	uint16_t usPrf1;			//本圈第一次重复频率(0.1Hz)对应单重频或双重频的高者
	uint16_t usPrf2;			//本圈第二次重复频率(0.1Hz)对应双重频的低者
	uint16_t usSpulseW;		    //本圈脉宽(微秒)
	int16_t	 usAngle;			//仰角(0.01度)
	uint8_t cSweepStatus;		//1=单要素	2=三要素(单重频)	3=三要素(双重频)
	uint8_t cAmbiguousp;		//0=无软件退模糊	1=软件退模糊
};

struct stdCC1RadarSite
{
	char cFileType[16];						//3830数据标识(CINRADC)
											//973试验标识（"973"）
	char cCountry[30];						//国家名
	char cProvince[20];						//省名
	char cStation[40];						//站名	
	char cStationNumber[10];				//区站号
	char cRadarType[20];					//雷达型号
	char cLongitude[16];					//天线所在经度
	char cLatitude[16];						//天线所在纬度
	int32_t lLongitudeValue;				//具体经度 毫秒
	int32_t lLatitudeValue;					//具体纬度 毫秒
	int32_t lHeight;							//天线海拔高度 (mm)
	int16_t sMaxAngle;						//地物阻挡最大仰角
	int16_t sOptAngle;						//最佳观测仰角
};

struct stdCC1ObsvInfo
{
	uint8_t ucSYear1;					//观测开始时间的年千百位(19-20)
	uint8_t ucSYear2;					//观测开始时间的年十个位(00-99)
	uint8_t ucSMonth;					//观测开始时间的月(1-12)
	uint8_t ucSDay;					//观测开始时间的日(1-31)
	uint8_t ucSHour;					//观测开始时间的时(0-23)
	uint8_t ucSMinute;				//观测开始时间的分(0-59)
	uint8_t ucSSecond;				//观测开始时间的秒(0-59)
	uint8_t ucTimeFrom;				//时间来源 0-计算机时钟(1天内未对时)		
									//		   1-计算机时钟(1天内已对时)
									//		   2-GPS
									//		   3-其它
	uint8_t ucEYear1;					//观测结束时间的年千百位(19-20)
	uint8_t ucEYear2;					//观测结束时间的年十个位(00-99)
	uint8_t ucEMonth;					//观测结束时间的月(1-12)
	uint8_t ucEDay;					//观测结束时间的日(1-31)
	uint8_t ucEHour;					//观测结束时间的时(0-23)
	uint8_t ucEMinute;				//观测结束时间的分(0-59)
	uint8_t ucESecond;				//观测结束时间的秒(0-59)
	uint8_t ucScanMode;				//扫描方式  1-RHI
									//		   10-PPI和ZPPI 
									//		   1XX=VPPI(XX为扫描圈数)
	uint32_t ulSmilliSecond;			//以微秒为单位表示的秒的小数位
	uint16_t usRHIA;					//RHI所在的方位角(0.01度为单位)
										// PPI和VPPI时为FFFF
	int16_t sRHIL;							//RHI所在的最低仰角(0.01度为单位) 
											//PPI和VPPI时为FFFF
	int16_t sRHIH;							//RHI所在的最高仰角(0.01度为单位) 
											//PPI和VPPI时为FFFF
	uint16_t usEchoType;				//回波类型  0x405a-Z  0x406a-V  0x407a-W  
										//		   0x408a-ZVW三要素
	uint16_t usProdCode;				//数据类型  0x8001-PPI数据  0x8002-RHI数据  
										//   0x8003-VPPI数据  0x8004-单强度PPI数据    
										//	0x8005-CAPPI数据
	uint8_t ucCalibration;			//标校状态  0-无  1-自动  2-1星期内人工
									//		    3-1月内人工
	uint8_t remain1[3];				//保留字
	stdCC1CutParam cutParam[30];		// remain2[660]保留字,放VPPISCANPARAMETER数据
									//该结构的说明见后
};

struct stdCC1PerformanceParam
{
	int32_t lAntennaG;							//天线增益(0.001dB)
	int32_t lPower;							//峰值功率(瓦)
	int32_t lWavelength;						//波长(微米)
	uint16_t usBeamH;					//垂直波束宽度(秒)
	uint16_t usBeamL;					//水平波束宽度(秒)
	uint16_t usPolarization;			//极化状态 0-水平 1-垂直 2-双偏振
										// 		  3-圆偏振 4-其它
	uint16_t usLogA;					//对数动态范围(0.01dB)
	uint16_t usLineA;					//线性动态范围(0.01dB)
	uint16_t usAGCP;					//AGC延迟量(微秒)
	uint16_t usFreqMode;				//频率方式	1-单重复频率  2-双重复频率3:2  
										//			3-双重复频率4:3
	uint16_t usFreqRepeat;			//重复频率
	uint16_t usPPPPulse;				//PPP脉冲数
	uint16_t usFFTPoint;				//FFT间隔点数
	uint16_t usProcessType;			//信号处理方式	1-PPP	2-全程FFT
									//				3-单库FFT
	uint8_t ucClutterT;				//杂波消除阀值(即STC)
	int8_t cSidelobe;				//第一旁瓣(dB)
	uint8_t ucVelocityT;			//速度门限
	uint8_t ucFilderP;				//地物消除方式	0-无		1-IIR滤波器1
									//			2-IIR滤波器2	3-IIR滤波器3
									//			4-IIR滤波器4
	uint8_t ucNoiseT;				//噪声消除阀值(即强度门限)
	uint8_t ucSQIT;					//SQI门限
	uint8_t ucIntensityC;				//DVIP强度值估算采用的通道
										//	 1-对数通道 2-线性通道
	uint8_t ucIntensityR;				//强度值估算是否距离订正
										// 0-无(dB) 1-已订正(dBZ)
	uint8_t ucCalNoise;				//噪声系数标定值
	uint8_t ucCalPower;				//发射功率标定值
	uint8_t ucCalPulseWidth;			//脉冲宽度标定值
	uint8_t ucCalWorkFreq;			//工作频率标定值
	uint8_t ucCalLog;					//对数斜率标定值	
};

struct stdCC1RadarDataHeader
{
	stdCC1RadarSite siteInfo;
	stdCC1ObsvInfo obsvInfo;
	stdCC1PerformanceParam permformInfo;
	int8_t remain3[92];						//保留字
	uint32_t liDataOffset;			//数据偏移地址
	int8_t remain4[3];//保留字
};
const uint32_t SIZE_CC1_DATA_HEADER = sizeof(stdCC1RadarDataHeader);
////////////////////////////////////////////////////////////////////////////////
//tagVPPISCANPARAMETER仰角层的结构（源结构的子结构）



#endif