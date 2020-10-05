#include "RadarBaseDataCommonPPI.h"

#include <cmath>

#include "../CommVar.h"

//*
///-------------------------------------------------------------------
//����Ϊһ��cutɨ�赥Ҫ�ص�������(�� PPI\RHI��SECTOR)
stdUniformCutHeader::stdUniformCutHeader(){
	strcpy(radar_name, "");
	strcpy(radar_type, "");
	strcpy(data_name, "");
	strcpy(radar_code, "");
	strcpy(obsv_type, "");
	year=0;
	month=0;
	day=0;
	hour=0;
	minute=0;
	second=0;
	radlat=0;		//*1000
	radlon=0;		//*1000
	radhgt=0;		// meters
	elev_angle=0;	//*10
	fstgatdis=0;	//*10
	nyq_vel=0;		//*10
	num_beam=0;
	num_gate=0;
	gateWidth=0;//[nazim];  //m
}


// date - ��1970��1��1�����������UTC��
// time - ��00:00:00�������
#include <time.h>
void stdUniformCutHeader::ScanStartDataAndTime(uint16_t& date, uint32_t& time) {
	time_t t;
	struct tm tm1;
	// Scan start time //
	tm1.tm_year = year - 1900;
	tm1.tm_mon = month - 1;
	tm1.tm_mday = day;
	tm1.tm_hour = hour;
	tm1.tm_min = minute;
	tm1.tm_sec = second;
	t = mktime(&tm1);
	// Store the volume scan date
	date = uint16_t(t / sec_in_day);
	// Store the volume scan time (s)
	time = uint32_t(t - date * sec_in_day);
	// Scan start time //
	tm1.tm_year = year - 1900;
	tm1.tm_mon = month - 1;
	tm1.tm_mday = day;
	tm1.tm_hour = hour;
	tm1.tm_min = minute;
	tm1.tm_sec = second;
	t = mktime(&tm1);
	// Store the volume scan date
	date = uint16_t(t / sec_in_day);
	// Store the volume scan time (ms)
	time = uint32_t(t - date * sec_in_day);
	// s to ms//(unsigned long)(hour*min_in_hor+minute)*sec_in_min*c_Factor1000;
}

stdUniformCutData::stdUniformCutData()
{	
	elev.resize(0);
	azim.resize(0);
	field.resize(0);
}

void stdUniformCutData::ReleaseBuf()
{
	elev.clear();
	vct_byte2().swap(elev);
	azim.clear();
	vct_byte2().swap(azim);

	vct_uniformSector:: iterator itr;
	for (itr = field.begin(); itr != field.end(); itr++)
	{
		itr->clear();
		vct_uniformRadial().swap(*itr);
	}
	field.clear();
	vct_uniformSector().swap(field);
}

//����Ϊһ��ɨ�赥Ҫ��������
stdUniformScanHeader::stdUniformScanHeader()
{
	strcpy_s(radar_name, 2, "");// [SITE_NAME_LEN];
	strcpy_s(radar_type, 2, "");//[SITE_TYPE_LEN];
	strcpy_s(data_name, 2, "");//[DATA_TYPE_LEN];
	strcpy_s(radar_code,2, "");
	strcpy_s(obsv_type, 2,"");
	radlat = 0;		//*1000
	radlon = 0;		//*1000
	radhgt = 0;		//*meters	
	ndata = 0;       //-9900.
	factor = 0;		// �����岿��ÿ��������Ĵ洢�ı�����ʵ��ֵ�Ĺ�ϵ: ����=ʵ��ֵ*factor+offset
	offset = 0;		//
}

stdUniformScanData::stdUniformScanData()
{
	//Needed = false;
	//Available = false;
	CutsData.resize(0);
}

void stdUniformScanData:: ReleaseBuf()
{
	vct_uniformCuts::iterator itc;
	for (itc = CutsData.begin(); itc != CutsData.end(); itc++)
		itc->ReleaseBuf();
	
	CutsData.clear();
	vct_uniformCuts().swap(CutsData);
}