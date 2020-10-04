#ifndef FORMAT_CDSC2_H
#define FORMAT_CDSC2_H

#include "RadarBaseDataFormat98D.h"
#pragma once
#pragma pack(1)
//SC CD
#define SURV_BINS_SC2 1000
#define DOPL_BINS_SC2 1000
const  uint16_t RADIAL_LEN_SC2 = 3000;

//SC CD radar data saved as 98D format
struct stdRadialSC2:public stdRadialHeader98D
{	
	uint8_t data[RADIAL_LEN_SC2];   // 129－... 共460字节反射率数据
	int16_t temp6[2]; //spare
	stdRadialSC2();
};
const uint16_t SIZE_RADIAL_SC2 = sizeof(stdRadialSC2);

#endif