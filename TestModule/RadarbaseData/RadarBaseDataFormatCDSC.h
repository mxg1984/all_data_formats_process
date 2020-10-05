
#ifndef FORMAT_CD_SC_H
#define FORMAT_CD_SC_H

#include "RadarBaseDataMomentTypeDef.h"
#include "RadarBaseDataFormatTypeDef.h"
#pragma once
//#pragma pack(1)

#include <string> 
#include <vector>
#include <map>
using namespace std;

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

#define RECORD_LEN			998
#define SINGLE_RECORD_LEN	992
#define RVP7_RECORD_LEN		4000
#define RVP7_GATE_WIDTH		250

struct stdRawBin
{
	uint8_t m_dbz;
	uint8_t m_vel;
	uint8_t m_undbz;
	uint8_t m_sw;
};

struct stdRVP7RadialData
{
	uint16_t startaz;
	uint16_t startel;
	uint16_t endaz;
	uint16_t endel;
	stdRawBin  RawData[RECORD_LEN];
};

struct stdSingleIntensity
{
	uint16_t startaz;
	uint16_t startel;
	uint16_t endaz;
	uint16_t endel;
	uint8_t dbz[SINGLE_RECORD_LEN];
};

#define SC_RADAR_DATA_HEADER_LEN	1024   //�����ļ��ļ�ͷ����
struct stdSCCutParam
{
	uint8_t  ambiguousp; 	//������ģ��״̬
							//	 0 = ����ģ��״̬
							//	 1 = �����ģ��
							//	 2 = ˫T��ģ��
							//	 3 = ��ʽ��ģ��
							//	 4 = ˫T + �����ģ��
							//	 5 = ��ʽ + �����ģ��
							//	 6 = ˫PPI��ģ��
							//	 9 = ������ʽ
	uint16_t	 Arotate;	//	��������ת��,������λ:0.01��/��
	uint16_t	 Prf1;		//	����ĵ�һ�������ظ�Ƶ��,������λ: 1/10 Hz
	uint16_t	 Prf2;		//	����ĵڶ��������ظ�Ƶ��,������λ: 1/10 Hz
	uint16_t	 spulseW;	//	�����������,������λ:	΢��
	uint16_t	 MaxV;		//	��������ɲ��ٶ�,������λ:	����/��
	uint16_t	 MaxL;		//	��������ɲ���룬��10��Ϊ������λ	
	uint16_t	 binWidth;	//	�������ݵĿⳤ���Է���Ϊ������λ	
	uint16_t	 binnumber;	//	����ɨ����ˮƽ����ĵ���
	uint16_t	 recordnumber;// ����ɨ���ߴ�ֱ����ĵ���
	int16_t		 Swangles;//	��������ǣ�������λ	��1/100��
};

struct stdSCRadarSite  //170 Bytes
{
	char   country[30];//			���������ı���ʽ����
	char   province[20];//			ʡ�����ı���ʽ����
	char   station[40];//			վ�����ı���ʽ����
	char   stationnumber[10];//		��վ�ţ��ı���ʽ����
	char   radartype[20];//			�״��ͺţ��ı���ʽ����
	char   longitude[16];//			�������ھ��ȣ��ı���ʽ����.��д��ʽ����E 115��32��12��
	char   latitude[16];//			��������γ�ȣ��ı���ʽ����.��д��ʽ����N 35��30��15��
	int32_t  longitudevalue;//	    �������ھ��ȵ���ֵ���Ժ���Ϊ������λ��ʮ���ƣ�
							//		������E��Ϊ����������W��Ϊ��	 	
	int32_t  lantitudevalue;//		��������γ�ȵ���ֵ���Ժ���Ϊ������λ��ʮ���ƣ�
							//		��γ��N��Ϊ������γ��S��Ϊ��
	int32_t  height;	//			���ߵĺ��θ߶��Ժ���Ϊ������λ
	int16_t   Maxangle;	//			��վ���ܵ����赲��������ǣ�����Ϊ������λ��
	int16_t   Opangle;	//			��վ����ѹ۲����ǣ�����ز�ǿ��<10dbz������Ϊ											������λ��
	int16_t   MangFreq;	//			�ſع�Ƶ�ʣ�ͨ����Ƶ�ʿɼ����״ﲨ����//Whyan
};

struct stdSCRadarPerformanceParam  //31 bytes
{
	int32_t   AntennaG;//		�������棬��0.001dbΪ������λ
	uint16_t  BeamH;//			��ֱ������ȣ���΢��Ϊ������λ
	uint16_t  BeamL;// 			ˮƽ������ȣ���΢��Ϊ������λ
	uint8_t   polarizations;/*	����״��
								0 = ˮƽ
								1 = ��ֱ
								2 = ˫ƫ��
								3 = Բƫ��
								4 = ����*/
	char	  sidelobe;//		��һ�԰������λ��db��ע�⣺������ţ�
	uint32_t  Power;//			�״������ֵ���ʣ�����Ϊ������λ
	uint32_t  wavelength;//		��������΢��Ϊ������λ
	uint16_t  logA;//			�������ջ���̬��Χ,��0.01dbΪ������λ
	uint16_t  LineA;//			���Խ��ջ���̬��Χ,��0.01Ϊ������λ
	uint16_t  AGCP;//			AGC�ӳ�������΢��Ϊ������λ
	uint8_t   clutterT;//		�Ӳ�������ֵ��������λ0.01db
	uint8_t	  VelocityP;	/*	�ٶȴ���ʽ
						0 = ���ٶȴ���
						1 = PPP
						2 = FFT
						3 = �������*/
	uint8_t	 filderP;/*		����������ʽ
					 0 = �޵�������
					 1 = �����Ӳ�ͼ�۳���
					 2 = �����Ӳ�ͼ + �˲�������
					 3 = �˲�������
					 4 = �׷�������*/
	uint8_t	 noiseT;//		����������ֵ	��0-255��
	uint8_t	 SQIT;//		SQI��ֵ����0.01Ϊ������λ
	uint8_t	 intensityC;/*	rvpǿ��ֵ������õ�ͨ��
						1 = ����ͨ��
						2 = ����ͨ��*/
	uint8_t	intensityR;/*	ǿ�ȹ����Ƿ�����˾��붩��
					   0 = ��
					   1 = �ѽ����˾��붩��*/
};


struct stdSCRadarObservationParam
{
	uint8_t 	 stype;	/*		ɨ�跽ʽ
						1 = RHI
						10 = PPI
						1XX = Vol 	XXΪɨ��Ȧ��*/
						//unsigned char 	 syear1;//		�۲��¼��ʼʱ������ǧλ��λ��19-20��
	uint16_t 	 syear;//		�۲��¼��ʼʱ������ʮλ��λ��01-99��
	uint8_t 	 smonth;//		�۲��¼��ʼʱ����£�1-12��
	uint8_t 	 sday;//		�۲��¼��ʼʱ����գ�1-31��
	uint8_t 	 shour;//		�۲��¼��ʼʱ���ʱ��00-23��
	uint8_t 	 sminute;//		�۲��¼��ʼʱ��ķ֣�00-59��
	uint8_t 	 ssecond;//		�۲��¼��ʼʱ����루00-59��
	uint8_t 	 Timep;/*		ʱ����Դ
					   0 = �����ʱ�ӣ���һ����δ���ж�ʱ
					   1 = �����ʱ�ӣ���һ�����ѽ��ж�ʱ
					   2 = GPS
					   3 = ����*/
	uint32_t	 smillisecond;// 		���С��λ��������λ΢�룩
	uint8_t 	 calibration;/*	��У״̬
							 0 = �ޱ�У
							 1 = �Զ���У
							 2 = 1�������˹���У
							 3 = 1�����˹���У
							 �����벻��*/
	uint8_t 	 intensityI;//		ǿ�Ȼ��ִ�����32-128��
	uint8_t		 VelocityP;//	�ٶȴ�����������31-255��(������-1��
	struct stdSCCutParam LayerParam[30]; //��Ȧɨ��״̬����
										 //ע����ɨ�跽ʽΪRHI��PPIʱ��ֻ�ڵ�һ��Ԫ������д������Ԫ��Ϊ0��
	uint16_t	 RHIA;//		��RHIʱ�����ڷ�λ�ǣ�������λΪ1/100��
					  //		��PPI������ɨ��ʱ����
	int16_t		 RHIL;//		��RHIʱ��������ǣ�������λΪ1/100��
					  //		������ɨ��ʱ����
	int16_t		 RHIH;//		��RHIʱ��������ǣ�������λΪ1/100��
					  //		������ɨ��ʱ����
	uint16_t 	 eyear;//		�۲����ʱ������ʮλ��λ��01-99��
	uint8_t  	 emonth;//		�۲����ʱ����£�1-12��
	uint8_t		 eday;//		�۲����ʱ����գ�1-31��
	uint8_t		 ehour;//		�۲����ʱ���ʱ��00-23��
	uint8_t		 eminute;//		�۲����ʱ��ķ֣�00-59��
	uint8_t		 esecond;//		�۲����ʱ����루00-59��
	uint8_t	 	 etenth;//		�۲����ʱ���1/100�루00-59��
};
//�������ļ�ͷ��ʽ������Ϊ1024���ֽ�
struct stdSCDataHeader
{
	struct stdSCRadarSite  SiteInfo;						//170 bytes
	struct stdSCRadarPerformanceParam  PerformanceInfo;	//31  bytes
	struct stdSCRadarObservationParam  ObservationInfo;	//718 bytes  ?
	uint8_t Reserved[163];								//163 bytes  ?

};
const uint32_t SIZE_SC_DATA_HEADER = sizeof(stdSCDataHeader);


#endif