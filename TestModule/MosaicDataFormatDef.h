#ifndef __MosaicDataFormateDef_H
#define __MosaicDataFormateDef_H


//
#include "CommVar.h"
#pragma pack(1)

//��ά�״�ƴͼ����ͷ
struct MOSAICDATAHEADER			// 112 Bytes (108�ֽ� ?)
{
	uint16_t iVersion;	//��ʽ�汾�� 1��2��- 2�ֽ�
	uint16_t iTemp[7];	//���� -14�ֽ�
	char           strZoneName[SITE_NAME_LEN];//�������� - 20�ֽ�
	char           strDataType[DATA_TYPE_LEN];//������  -4 Bytes
	uint16_t iNumRadars;	//�������״��� - 2�ֽ�
	uint16_t iStYear;		//�۲⿪ʼʱ��-�� 2�ֽ�
	uint16_t iStMonth;	//�۲⿪ʼʱ��-�� 2�ֽ�
	uint16_t iStDay;		//�۲⿪ʼʱ��-�� 2�ֽ�
	uint16_t iStHour;		//�۲⿪ʼʱ��-ʱ 2�ֽ�
	uint16_t iStMinute;	//�۲⿪ʼʱ��-�� 2�ֽ�
	int32_t		   iFstLon;		//����ʼ���� (��*1000) - 4�ֽ�
	int32_t		   iFstLat;		//����ʼγ�� (��*1000) - 4�ֽ�
	int32_t	       iLstLon;		//����������� (��*1000) - 4�ֽ�
	int32_t           iLstLat;		//�������γ�� (��*1000) - 4�ֽ�
	uint16_t iFstHgt;		//����ʼ�߶� (��) - 2�ֽ�
	uint16_t iLstHgt;		//��������߶� (��) - 2�ֽ�
	uint16_t iLonGrids;	//�������� - 2�ֽ�
	uint16_t iLatGrids;	//γ������ - 2�ֽ�
	uint16_t iLevels;		//�߶Ȳ���   - 2�ֽ�
	short		   temp1;		//��iLevels�������4�ֽ���������β  !!!!!!
	uint32_t   iLonRes;		//������ֱ��� (��*10000) - 4�ֽ�
	uint32_t   iLatRes;		//γ����ֱ��� (��*10000) - 4�ֽ�
	short		   iHgtRes;		//�߶ȷֱ���(��; -1��ʾ�߶ȼ�಻����) - 2�ֽ�
	int16_t          temp2;		//��iHgtRes�������4�ֽ���������β!!!!!!!
	//???/���λ����д��ʱ���ƺ������4���ֽ� (FFFF0000)
	uint32_t   dwRadInfoOffset;	//�״�վ��Ϣ���ݿ�ʼλ��(�����ͷĩβ)  - 4�ֽ�
	uint32_t   dwLevHgtOffset;  //����߶����ݿ�ʼλ��(�����ͷĩβ)    - 4�ֽ�
	uint32_t   dwObvDataOffset;	//����(������\�ٶȵ�)��ʼλ��(�����ͷĩβ) - 4�ֽ�
	int16_t          temp[4];		//���� -8�ֽ�
};
//const int32_t SIZE_MOSAIC_HEADER= sizeof(MOSAICDATAHEADER);
//SIZE_RADAR_INFO

#endif