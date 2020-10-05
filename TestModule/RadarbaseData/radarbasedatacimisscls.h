#pragma once
//
#include "stdio.h"
#include <stdint.h>
#include <string> 
#include <vector>
#include <list>
#include <map>
#include "RadarBaseDataFormatCIMISS.h"
#include "julian_date.h"
#include "RadarBaseDataCommonPPI.h"

using namespace std;
//-------------------------------------------------------//
//
struct stdCimissMomentRadial
{
	stdCimissGenRadialHeader genHeader;
	stdCimissOneMomentRadial Radial;
};
typedef list<stdCimissMomentRadial> vct_momentCut;	//��Ҫ��һ��CUT������radials
typedef list<vct_momentCut> vct_momentCuts;
struct stdCimissMomentCuts
{
	bool Needed;	//�������
	bool Available; //�ɻ��
	vct_momentCuts CutsData;//�������ǵ�����
	stdCimissMomentCuts();
};
//typedef map<uint16_t, stdCimissMomentCuts> map_Cimiss_MomentsCuts;

#define  cMaxMoment  19			//Ҫ��������//
typedef enum CIMISS_MOMENT_ID
{	
	CIMISS_M_ID_Z=0,
	CIMISS_M_ID_Zc,
	CIMISS_M_ID_V,
	CIMISS_M_ID_Vc,
	CIMISS_M_ID_W,
	CIMISS_M_ID_Wc,
	CIMISS_M_ID_ZDR,
	CIMISS_M_ID_ZDRc,
	CIMISS_M_ID_LDR,
	CIMISS_M_ID_CC,
	CIMISS_M_ID_PDP,
	CIMISS_M_ID_KDP,
	CIMISS_M_ID_HCL,
	CIMISS_M_ID_DBT,
	CIMISS_M_ID_CP,
	CIMISS_M_ID_CF,
	CIMISS_M_ID_SQI,
	CIMISS_M_ID_CPA,
	CIMISS_M_ID_SNR
}MomentID;//
const int64_t CIMISS_M_BIT_ID[cMaxMoment]//��MOMENT_ID˳���Ӧ��
{
	BIT_DBZ,BIT_Zc,BIT_V,BIT_Vc,BIT_W,BIT_Wc,BIT_ZDR,BIT_ZDRc,
	BIT_LDR,BIT_CC,BIT_PDP,BIT_KDP,BIT_HCL,
	BIT_DBT,BIT_CP,BIT_CF,BIT_SQI,BIT_CPA,BIT_SNR
};//
//data type index
const uint32_t CIMISS_M_TYPE_ID[cMaxMoment]
{	
	BIT_Index_DBZ,
	BIT_Index_Zc,
	BIT_Index_V,
	BIT_Index_Vc,
	BIT_Index_W,
	BIT_Index_Wc,
	BIT_Index_ZDR,
	BIT_Index_ZDRc,
	BIT_Index_LDR,
	BIT_Index_CC,
	BIT_Index_PDP,
	BIT_Index_KDP,
	BIT_Index_HCL,
	BIT_Index_DBT,	
	BIT_Index_CP,	
	BIT_Index_CF,
	BIT_Index_SQI,
	BIT_Index_CPA,
	BIT_Index_SNR
};
//
////////////////////////////////////////////////////////////////////////////////////
//
class CCIMISSRadarDataCls
{
public:
	CCIMISSRadarDataCls();
	CCIMISSRadarDataCls(string strInFileName);
	~CCIMISSRadarDataCls();
	
	void GetObsvData(const bool &bZ=true, const bool &bV= true, const bool &bW = true, const bool &bZDR = false,
		const bool &bZc = false, const bool &bVc= false, const bool &bWc= false, const bool &bZDRc= false,
		const bool &bLDR=false, const bool &bCC=false, const bool &bPHIDP=false, const bool &bKDP=false, const bool &bHCL=false, const bool &bDBT=false,
		const bool &bSQI=false, const bool &bCPA=false, const bool &bCP=false, const bool &bCF=false, const bool &bSNRH=false);//��ȡһ�����ǵ�PPI/SPPI ���ݣ���һ����λRHI����
	
	bool IsPPI(int16_t &bnCuts);	//�Ƿ�Ϊ��/���RHIɨ��
	bool IsSPPI(int16_t &nCuts);	//�Ƿ�Ϊ��/�����ɨ
	bool IsRHI(int16_t &nAzs);	//�Ƿ�Ϊ��/�෽λRHIɨ��

	uint16_t GetMomentTypeId(const char* szMomentName); //����Ҫ����������������Ӧ��Ҫ��ID

	float GetBinValue(uint8_t code, int32_t offset, int32_t scale, float ndata); //Decode 
	float GetBinValue(uint16_t code, int32_t offset, int32_t scale, float nata);
	uint16_t GetLengthOfBins(uint16_t momid, uint16_t cutid);
	uint16_t GetNumberOfBins(uint16_t momid, uint16_t cutid);
	uint16_t GetNumberOfRadials(uint16_t momid, uint16_t cutid);
	uint16_t GetBinWidth(uint16_t momid, uint16_t cutid);
	void GetBeamWidth(float &hbw, float &vbw);
	stdCimissCutConfig* GetCutConfig(uint16_t cutid);
	stdCimissScanTaskConfig* GetScanConfig();
	void GetRadarSite(string &sname, string &scode, string &stype, float &lon, float &lat, float &ht);
	void GetRadarSite(char* sname, char* scode, char* stype, float &lon, float &lat, float &ht);
	uint16_t GetCutNum(uint16_t momid);
	vct_momentCut* GetMomentCut(uint16_t momid, uint16_t cutidx);
	void GetCutStartDateTime(uint16_t cutid, uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second);
	void GetScanStartDateTime(uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second);
	
private:
	void GetCommonBlock();
	void ClearMomentsCuts();
	int ConfirmMomentsToGet(uint16_t  cutIndex);
	void ConfirmMomentsObserved(uint16_t  cutIndex);
	void ConfirmMomentsNeeded(bool bZ, bool bZc, bool bV, bool bVc, bool bW, bool bWc, bool bZDR, bool bZDRc,
		bool bLDR, bool bCC, bool bPHIDP, bool bKDP, bool bHCL, bool bDBT,
		bool bSQI, bool bCPA, bool bCP, bool bCF, bool bSNRH);

	void TransDateTime(uint32_t JulSeconds, uint16_t &year, uint16_t &day, uint16_t &month, uint16_t &hour, uint16_t &minute, uint16_t &second);
	void TransDate(uint16_t JulDate, uint16_t &year, uint16_t &day, uint16_t &month);
	void TransTime(uint32_t Seconds, uint16_t &hour, uint16_t &minute, uint16_t &second);

	bool IsCutStart(uint16_t RadialStat);
	bool IsCutEnd(uint16_t RadialStat);
	int16_t MomentBitIndex(short m);

	string			m_strDataFileName;
	FILE*			m_pDataFile;

	int32_t  m_MomentCutId[cMaxMoment];		//��ͬҪ�ص�������� 1��ʼ

	int32_t  m_CurrentCutId;//��ǰ������� 1��ʼ---����Ҫ��
	map<uint16_t, stdCimissMomentCuts>	m_MomentsData; // ��Ҫ�ض������
	stdCimissCommonBlock	m_CommonBlock; // �������ݿ�
};