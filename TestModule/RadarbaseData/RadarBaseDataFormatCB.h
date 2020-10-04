#ifndef FORMAT_CB_H
#define FORMAT_CB_H
#pragma once
#pragma pack(1)

#include <vector>
//#include <list>

#define BEAM_WIDTH_SA 0.95  //������ȣ��ȣ�
//#define MAX_RADS 400        //ÿ����� RADIAL��
#define SURV_BINS 800
#define DOPL_BINS 1600
#define MAX_MOMENTS 3  //Ҫ�ظ���
// PPI status
#define stat_vol_beg 3
#define stat_vol_end 4
#define stat_elv_beg 0
#define stat_elv_end 2
#define get_angle(code) (code * 180. / 32768)  //�Ƕȴ��뵽��ʵ�Ƕȣ��ȣ���ת��
//
// tagBaseData 98D�״���Ϣ�ṹ��Ŀ��ṹ��
const  uint16_t RADIAL_LEN_98D_CB= 4000;

struct stdRadialHeaderCB
{
	int16_t temp1[7];                         //����
	uint16_t RadarStatus;                     //1 - ��ʾΪ�״�����
	int16_t temp2[6];                         //����
	uint32_t mSeconds;                        //���������ռ�ʱ��
	uint16_t JulianDate;                      //��1970/1/1�������
	uint16_t URange;                          //��ģ������
	uint16_t Az;                              //��λ�Ƕ�
	uint16_t RadialNumber;                    //�����������
	uint16_t RadialStatus;                    //��������״̬
	uint16_t El;                              //����
	uint16_t ElNumber;                        //��ɨ�ڵ����Ǳ��
	uint16_t RangeToFirstGateOfReflectivity;  //��һ�����������ݱ�ʾ��ʵ�ʾ���
	uint16_t RangeToFirstGateOfDoppler;	 //��һ�����������ݱ�ʾ��ʵ�ʾ���
	uint16_t GateSizeOfReflectivity;     //���������ݵľ���ⳤ
	uint16_t GateSizeOfDoppler;          //���������ݵľ���ⳤ
	uint16_t GatesNumberOfReflectivity;  //���������ݵľ������
	uint16_t GatesNumberOfDoppler;       //���������ݵľ������
	int16_t  temp3[3];                   //����
	uint16_t PtrOfReflectivity;          //����������ָ��
	uint16_t PtrOfVelocity;              //�ٶ�����ָ��
	uint16_t PtrOfWidth;                 //�׿�����ָ��
	int16_t  ResolutionOfVelocity;       //�������ٶȷֱ���
	uint16_t VcpNumber;                  //��ɨ��
	int16_t  temp4[4];                   //����
	uint16_t PtrOfArcReflectivity;       //����������ָ��
	uint16_t PtrOfArcVelocity;           //�ٶ�����ָ��
	uint16_t PtrOfArcWidth;              //�׿�����ָ��
	uint16_t Nyquist;                    //��ģ���ٶ�
	int16_t temp46;                      //����
	int16_t temp47;                      //����
	int16_t temp48;                      //����
	int16_t CircleTotal;                 //������
	uint8_t temp5[30];                   //����
};
//const uint16_t SIZE_98D_RADIAL_HEADER = sizeof(stdRadialHeaderCB);

struct stdRadialDataCB:public stdRadialHeaderCB
{	
	uint8_t data[RADIAL_LEN_98D_CB];   // 129��588 ��460�ֽڷ���������
								    // 129��1508 ��1380�ֽ��ٶ�����
									// 129��2428 ��2300�ֽ��׿�����
	int16_t temp6[2]; //spare
	stdRadialDataCB();
};
const uint16_t SIZE_MET_RADIAL_CB = sizeof(stdRadialDataCB);

#endif