#ifndef __UniformFormatDef_H
#define __UniformFormatDef_H

#include "CommVar.h"
//体扫的PPI数据结构
const long nazim=512;
const long ngate=1000;
struct PPISTRUCT
{
     char	radar_name[SITE_NAME_LEN];
	 char   data_type[SITE_TYPE_LEN];
	 char   data_name[DATA_TYPE_LEN];
     short int	vcpnum;
     short int 	year;
     short int	month;
     short int	day;
     short int	hour;
     short int	minute;
     short int	second;
     long	    radlat;		//*1000
     long	    radlon;		//*1000
     short int	radhgt;		// meters
     short int	elev_angle;	//*10
     short int	fstgatdis;	//*10
     short int	nyq_vel;	//*10
     short int	num_beam;
     short int	num_gate;
     short int	gateWidth;//[nazim];  //m
     short int	elev[nazim];	//*10
     short int	azim[nazim];	//*10
     short int	field[nazim][ngate];  //*10
};//PPISTRUCT;
const size_t SIZE_PPI=sizeof(PPISTRUCT);

#endif