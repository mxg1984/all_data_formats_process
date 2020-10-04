#ifndef DATA_FORMAT_DEF_H
#define DATA_FORMAT_DEF_H

#pragma once

//两类版本格式文件名区分
const char FNAME_RD_CIMISS[] = "_CAP_FMT.";
const char FNAME_RD_CINRAD[] = "_CAP.";

//数据格式标记KEY
const char FM_RD_CIMISS[8] = "CIMISS";//CIMISS 新版雷达格式-2019
const char FM_RD_SA[4] = "SA";		//Metstar SA 雷达格式
const char FM_RD_SB[4] = "SB";		//Metstar SB 雷达格式
const char FM_RD_CA[4] = "CA";		//Metstar CA 雷达格式
const char FM_RD_CB[4] = "CB";		//Metstar CB 雷达格式
const char FM_RD_SC[4] = "SC";		//SC雷达原始格式
const char FM_RD_CD[4] = "CD";		//CD雷达原始格式
const char FM_RD_CC[4] = "CC";		//CC雷达原始格式
const char FM_RD_CC2[4] = "CC2";	//CC雷达2.0版格式
const char FM_RD_SC2[6] = "SC2";	//SC雷达转存为Metstar格式--大部分雷达
const char FM_RD_CD2[6] = "CD2";	//CD雷达转存为Metstar格式--大部分雷达
const char FM_RD_88D[4] = "88D";	//上海青浦88D雷达

#endif