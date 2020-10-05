
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

//tagVPPISCANPARAMETER���ǲ�Ľṹ
struct stdCC1CutParam
{
	uint16_t usMaxV;			//���ɲ��ٶ�(����/��)
	uint16_t usMaxL;			//���ɲ����(10��)
	uint16_t usBindWidth;		//�ⳤ(��)
	uint16_t usBinNumber;		//ÿ�������
	uint16_t usRecordNumber;	//��Ȧ������
	uint16_t usArotate;		    //��Ȧת��(0.01��/��)
	uint16_t usPrf1;			//��Ȧ��һ���ظ�Ƶ��(0.1Hz)��Ӧ����Ƶ��˫��Ƶ�ĸ���
	uint16_t usPrf2;			//��Ȧ�ڶ����ظ�Ƶ��(0.1Hz)��Ӧ˫��Ƶ�ĵ���
	uint16_t usSpulseW;		    //��Ȧ����(΢��)
	int16_t	 usAngle;			//����(0.01��)
	uint8_t cSweepStatus;		//1=��Ҫ��	2=��Ҫ��(����Ƶ)	3=��Ҫ��(˫��Ƶ)
	uint8_t cAmbiguousp;		//0=�������ģ��	1=�����ģ��
};

struct stdCC1RadarSite
{
	char cFileType[16];						//3830���ݱ�ʶ(CINRADC)
											//973�����ʶ��"973"��
	char cCountry[30];						//������
	char cProvince[20];						//ʡ��
	char cStation[40];						//վ��	
	char cStationNumber[10];				//��վ��
	char cRadarType[20];					//�״��ͺ�
	char cLongitude[16];					//�������ھ���
	char cLatitude[16];						//��������γ��
	int32_t lLongitudeValue;				//���徭�� ����
	int32_t lLatitudeValue;					//����γ�� ����
	int32_t lHeight;							//���ߺ��θ߶� (mm)
	int16_t sMaxAngle;						//�����赲�������
	int16_t sOptAngle;						//��ѹ۲�����
};

struct stdCC1ObsvInfo
{
	uint8_t ucSYear1;					//�۲⿪ʼʱ�����ǧ��λ(19-20)
	uint8_t ucSYear2;					//�۲⿪ʼʱ�����ʮ��λ(00-99)
	uint8_t ucSMonth;					//�۲⿪ʼʱ�����(1-12)
	uint8_t ucSDay;					//�۲⿪ʼʱ�����(1-31)
	uint8_t ucSHour;					//�۲⿪ʼʱ���ʱ(0-23)
	uint8_t ucSMinute;				//�۲⿪ʼʱ��ķ�(0-59)
	uint8_t ucSSecond;				//�۲⿪ʼʱ�����(0-59)
	uint8_t ucTimeFrom;				//ʱ����Դ 0-�����ʱ��(1����δ��ʱ)		
									//		   1-�����ʱ��(1�����Ѷ�ʱ)
									//		   2-GPS
									//		   3-����
	uint8_t ucEYear1;					//�۲����ʱ�����ǧ��λ(19-20)
	uint8_t ucEYear2;					//�۲����ʱ�����ʮ��λ(00-99)
	uint8_t ucEMonth;					//�۲����ʱ�����(1-12)
	uint8_t ucEDay;					//�۲����ʱ�����(1-31)
	uint8_t ucEHour;					//�۲����ʱ���ʱ(0-23)
	uint8_t ucEMinute;				//�۲����ʱ��ķ�(0-59)
	uint8_t ucESecond;				//�۲����ʱ�����(0-59)
	uint8_t ucScanMode;				//ɨ�跽ʽ  1-RHI
									//		   10-PPI��ZPPI 
									//		   1XX=VPPI(XXΪɨ��Ȧ��)
	uint32_t ulSmilliSecond;			//��΢��Ϊ��λ��ʾ�����С��λ
	uint16_t usRHIA;					//RHI���ڵķ�λ��(0.01��Ϊ��λ)
										// PPI��VPPIʱΪFFFF
	int16_t sRHIL;							//RHI���ڵ��������(0.01��Ϊ��λ) 
											//PPI��VPPIʱΪFFFF
	int16_t sRHIH;							//RHI���ڵ��������(0.01��Ϊ��λ) 
											//PPI��VPPIʱΪFFFF
	uint16_t usEchoType;				//�ز�����  0x405a-Z  0x406a-V  0x407a-W  
										//		   0x408a-ZVW��Ҫ��
	uint16_t usProdCode;				//��������  0x8001-PPI����  0x8002-RHI����  
										//   0x8003-VPPI����  0x8004-��ǿ��PPI����    
										//	0x8005-CAPPI����
	uint8_t ucCalibration;			//��У״̬  0-��  1-�Զ�  2-1�������˹�
									//		    3-1�����˹�
	uint8_t remain1[3];				//������
	stdCC1CutParam cutParam[30];		// remain2[660]������,��VPPISCANPARAMETER����
									//�ýṹ��˵������
};

struct stdCC1PerformanceParam
{
	int32_t lAntennaG;							//��������(0.001dB)
	int32_t lPower;							//��ֵ����(��)
	int32_t lWavelength;						//����(΢��)
	uint16_t usBeamH;					//��ֱ�������(��)
	uint16_t usBeamL;					//ˮƽ�������(��)
	uint16_t usPolarization;			//����״̬ 0-ˮƽ 1-��ֱ 2-˫ƫ��
										// 		  3-Բƫ�� 4-����
	uint16_t usLogA;					//������̬��Χ(0.01dB)
	uint16_t usLineA;					//���Զ�̬��Χ(0.01dB)
	uint16_t usAGCP;					//AGC�ӳ���(΢��)
	uint16_t usFreqMode;				//Ƶ�ʷ�ʽ	1-���ظ�Ƶ��  2-˫�ظ�Ƶ��3:2  
										//			3-˫�ظ�Ƶ��4:3
	uint16_t usFreqRepeat;			//�ظ�Ƶ��
	uint16_t usPPPPulse;				//PPP������
	uint16_t usFFTPoint;				//FFT�������
	uint16_t usProcessType;			//�źŴ���ʽ	1-PPP	2-ȫ��FFT
									//				3-����FFT
	uint8_t ucClutterT;				//�Ӳ�������ֵ(��STC)
	int8_t cSidelobe;				//��һ�԰�(dB)
	uint8_t ucVelocityT;			//�ٶ�����
	uint8_t ucFilderP;				//����������ʽ	0-��		1-IIR�˲���1
									//			2-IIR�˲���2	3-IIR�˲���3
									//			4-IIR�˲���4
	uint8_t ucNoiseT;				//����������ֵ(��ǿ������)
	uint8_t ucSQIT;					//SQI����
	uint8_t ucIntensityC;				//DVIPǿ��ֵ������õ�ͨ��
										//	 1-����ͨ�� 2-����ͨ��
	uint8_t ucIntensityR;				//ǿ��ֵ�����Ƿ���붩��
										// 0-��(dB) 1-�Ѷ���(dBZ)
	uint8_t ucCalNoise;				//����ϵ���궨ֵ
	uint8_t ucCalPower;				//���书�ʱ궨ֵ
	uint8_t ucCalPulseWidth;			//�����ȱ궨ֵ
	uint8_t ucCalWorkFreq;			//����Ƶ�ʱ궨ֵ
	uint8_t ucCalLog;					//����б�ʱ궨ֵ	
};

struct stdCC1RadarDataHeader
{
	stdCC1RadarSite siteInfo;
	stdCC1ObsvInfo obsvInfo;
	stdCC1PerformanceParam permformInfo;
	int8_t remain3[92];						//������
	uint32_t liDataOffset;			//����ƫ�Ƶ�ַ
	int8_t remain4[3];//������
};
const uint32_t SIZE_CC1_DATA_HEADER = sizeof(stdCC1RadarDataHeader);
////////////////////////////////////////////////////////////////////////////////
//tagVPPISCANPARAMETER���ǲ�Ľṹ��Դ�ṹ���ӽṹ��



#endif