#ifndef FORMAT_98D_H
#define FORMAT_98D_H
#pragma once
#pragma pack(1)

#include <vector>
//#include <list>

#define BEAM_WIDTH 0.95		//波束宽度（度
#define MAX_RADS 400        //每层最多 RADIAL数
#define MAX_MOMENTS 3		//要素个数
//S band
#define SURV_BINS_S 460
#define DOPL_BINS_S 920
const  uint16_t RADIAL_LEN_S = 2300;
//C band
#define SURV_BINS_C 800
#define DOPL_BINS_C 1600
const  uint16_t RADIAL_LEN_C= 4000;

// PPI status
#define stat_vol_beg 3
#define stat_vol_end 4
#define stat_elv_beg 0
#define stat_elv_end 2

#define get_angle(code) (code * 180. / 32768)  //角度代码到真实角度（度）的转换
//
// tagBaseData 98D雷达信息结构（目标结构）
struct stdRadialHeader98D
{
	int16_t temp1[7];                         //保留
	uint16_t RadarStatus;                     //1 - 表示为雷达数据
	int16_t temp2[6];                         //保留
	uint32_t mSeconds;                        //径向数据收集时间
	uint16_t JulianDate;                      //从1970/1/1起的日期
	uint16_t URange;                          //不模糊距离
	uint16_t Az;                              //方位角度
	uint16_t RadialNumber;                    //径向数据序号
	uint16_t RadialStatus;                    //径向数据状态
	uint16_t El;                              //仰角
	uint16_t ElNumber;                        //体扫内的仰角编号
	int16_t RangeToFirstGateOfReflectivity;  //第一个反射率数据表示的实际距离
	int16_t RangeToFirstGateOfDoppler;	 //第一个多普勒数据表示的实际距离
	uint16_t GateSizeOfReflectivity;     //反射率数据的距离库长
	uint16_t GateSizeOfDoppler;          //多普勒数据的距离库长
	uint16_t GatesNumberOfReflectivity;  //反射率数据的距离库数
	uint16_t GatesNumberOfDoppler;       //多普勒数据的距离库数
	int16_t  temp3[3];                   //保留
	uint16_t PtrOfReflectivity;          //反射率数据指针
	uint16_t PtrOfVelocity;              //速度数据指针
	uint16_t PtrOfWidth;                 //谱宽数据指针
	int16_t  ResolutionOfVelocity;       //多普勒速度分辨率
	uint16_t VcpNumber;                  //体扫号
	int16_t  temp4[4];                   //保留
	uint16_t PtrOfArcReflectivity;       //反射率数据指针
	uint16_t PtrOfArcVelocity;           //速度数据指针
	uint16_t PtrOfArcWidth;              //谱宽数据指针
	uint16_t Nyquist;                    //不模糊速度
	int16_t temp46;                      //保留
	int16_t temp47;                      //保留
	int16_t temp48;                      //保留
	int16_t CircleTotal;                 //仰角数
	uint8_t temp5[30];                   //保留
};
const uint16_t SIZE_RADIAL_HEADER_98D = sizeof(stdRadialHeader98D);
//S band
struct stdRadial98DS:public stdRadialHeader98D
{	
	uint8_t data[RADIAL_LEN_S];   // 129－588 共460字节反射率数据
								  // 129－1508 共1380字节速度数据
								  // 129－2428 共2300字节谱宽数据
	int16_t temp6[2]; //spare
	stdRadial98DS();
};
const uint16_t SIZE_RADIAL_98DS = sizeof(stdRadial98DS);

//C band
struct stdRadial98DC :public stdRadialHeader98D
{
	uint8_t data[RADIAL_LEN_C];   // 129－588 共460字节反射率数据
								  // 129－1508 共1380字节速度数据
								  // 129－2428 共2300字节谱宽数据
	int16_t temp6[2]; //spare
	stdRadial98DC();
};
const uint16_t SIZE_RADIAL_98DC = sizeof(stdRadial98DC);

#endif