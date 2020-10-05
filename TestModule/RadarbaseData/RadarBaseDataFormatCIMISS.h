#pragma once
#pragma pack(1)

#include <vector>
#include <list>
using namespace std;
//////////////////////////////////////////////////////////////////////////////////
//
int const GDT_BASEDATA = 1;
int const GDT_PRODUCT = 2;
int const GDT_TASK_CONFIG = 3;
int const GDT_CS_ECHO = 4;
int const GDT_TASKSCHD_CONFIG = 5;
int const GDT_MAGIC = 0x4D545352;//MTSR
int const MAJOR_VER = 1;
int const MINOR_VER = 1;

#define MAX_TILTS_NUM 14
#define RADAR_NOECHO -1280
#define RADAR_FOLDED -1281
#define RADAR_NODATA -32768

#define NOPRF 0
#define PRF_1 322
#define PRF_2 446
#define PRF_3 644
#define PRF_4 857
#define PRF_5 1014
#define PRF_6 1095
#define PRF_7 1181
#define PRF_8 1282

// ��¼1 -- �����������붨��
// BIT(LSB)		MOMENT			REMARKS
// 1            dBT				�˲�ǰ�����ʣ�Total reflectivity)
// 2	        dBZ				�˲������� (reflectivity)
// 3			V				�����ٶ�
// 4			W				�׿�
// 5			SQI				�ź�����ָ��
// 6			CPA				�Ӳ���λһ���� ��Clutter Phase Alignment)
// 7			ZDR				��ַ�����
// 8			LDR				��ƫ���
// 9			CC				Э���ϵ��(Cross Correlation Coeffcient)
//10			PHIDP			�������
//11			KDP				���������
//12			CP				�Ӳ�������(Clutter Probability)
//13			Reserved		���ݱ�־,����
//14			HCL				˫ƫ����̬���ࣨHydro Classification)
//15			CF				�Ӳ���־(Clutter Flag)
//16	        SNRH			ˮƽͨ�������
//17-31			Reserved		���ݱ�־,���� 
//32			Zc				����������
//33			Vc				���������ٶ�
//34			Wc				�������׿�
//35			ZDRc			�������ַ�����
// ��¼1


// ��¼2 -- �˲��������붨��
// BIT(LSB)		Filter Method			REMARKS
// 0            ���Ź���				����������ĸ��Ź����㷨
// 1            ��������				���������㷨
// 2	        һά�����ʵ��Ӳ�����	�Է���������ʹ��һά���Ӳ�����
// 3			һά�����յ��Ӳ�����	�Զ���������ʹ��һά���Ӳ����� 
// 4			��ά�����ʵ��Ӳ�����	�Է���������ʹ�ö�ά(3*3��λ�;���)���Ӳ�����
// 5			��ά�����յ��Ӳ�����	�Զ���������ʹ�ö�ά(3*3��λ�;���)���Ӳ����� 
// 6-31			����
// ��¼2

// ��¼3 -- �ʿ��������붨��
// BIT(LSB)		Threshold				REMARKS
// 0            SQI						�ź�����ָ��
// 1            SIG						�����ź�ǿ��
// 2	        CSR						�����Ӳ��������źű���
// 3			LOG						�����
// 4			CPA						�����Ӳ���λ�ȶ�ָ��
// 5			PMI						���������ź�ָ��
// 6 			DPLOG					ƫ���������
// 7-31			����
// ��¼3

//�����岿�ֹ۲�Ҫ��ֵ˵��
// ��������ͷ����Ϊ����������α���ľ�������
// �������ݿ���:  ��������ͷ�еĲ�������Length/���ֽڳ���BinLength
// ������������:  1�ֽڻ�2�ֽڳ��ȵ��޷�������
// ������ʽ:      ����洢
// ���뷽ʽ:      ����ֵ>=5��ʵ��ֵ = (����ֵ-Offset)/Scale
//				  ����ֵ==4������
//				  ����ֵ==3��δ֪����
//				  ����ֵ==2��δɨ�裬�����������
//				  ����ֵ==1�������۵�
//				  ����ֵ==0���ź�С������,��Ϊ�޻ز�//  
//the end


//Ҫ������λ���� 64 bits
// 0000 0000 0000 0000 
// 0000 0000 0000 0000 
// 0000 0000 0000 0000
// 0000 0000 0000 0000                    //bitλ
#define BIT_DBT		0x0000000000000002 //1
#define BIT_DBZ		0x0000000000000004 //2
#define BIT_V		0x0000000000000008 //3
#define BIT_W		0x0000000000000010 //4
#define BIT_SQI		0x0000000000000020 //5
#define BIT_CPA		0x0000000000000040 //6
#define BIT_ZDR		0x0000000000000080 //7
#define BIT_LDR		0x0000000000000100 //8
#define BIT_CC		0x0000000000000200 //9
#define BIT_PDP		0x0000000000000400 //10
#define BIT_KDP		0x0000000000000800 //11
#define BIT_CP		0x0000000000001000 //12
#define BIT_HCL		0x0000000000004000 //14
#define BIT_CF		0x0000000000008000 //15
#define BIT_SNR		0x0000000000010000 //16
#define BIT_Zc		0x0000000100000000 //32
#define BIT_Vc		0x0000000200000000 //33
#define BIT_Wc		0x0000000400000000 //34
#define BIT_ZDRc	0x0000000800000000 //35

//BITλINDEX
#define BIT_Index_DBT	1 //1
#define BIT_Index_DBZ	2 //2
#define BIT_Index_V		3 //3
#define BIT_Index_W		4 //4
#define BIT_Index_SQI	5 //5
#define BIT_Index_CPA	6 //6
#define BIT_Index_ZDR	7 //7
#define BIT_Index_LDR	8 //8
#define BIT_Index_CC	9 //9
#define BIT_Index_PDP	10 //10
#define BIT_Index_KDP	11 //11
#define BIT_Index_CP	12 //12
#define BIT_Index_HCL	14 //14
#define BIT_Index_CF	15 //15
#define BIT_Index_SNR	16 //16
#define BIT_Index_Zc	32 //32
#define BIT_Index_Vc	33 //33
#define BIT_Index_Wc	34 //34
#define BIT_Index_ZDRc	35 //35

//����״̬
#define EL_FIRST 0
#define EL_LAST 2
#define VS_FIRST 3
#define VS_LAST 4
#define RHI_FIRST 5
#define RHI_LAST 6

typedef struct __tagGenericHeader
{//32 bytes
	uint32_t MagicNumber;		//4 bytes �涨���=0x4D545352��ָʾ�״������ļ�
	uint16_t MajorVersion;		//2 bytes ���汾��
	uint16_t MinorVersion;		//2 bytes �ΰ汾�� 
	uint32_t GenericType;		//4 bytes �ļ�����,1-2    1:������; 2:�����Ʒ
	uint32_t ProdutType;		//4 bytes ��Ʒ����,1-100  GenericTypeΪ1ʱ�˲�����Ч
	uint32_t  Reserved[4];		//16 bytes �����ֶ�
}stdCimissGenericHeader;
//
typedef struct __tagSiteConfig
{//128 bytes
	char     SiteCode[8];		//8 bytes  վ�ű�ǣ��磺Z9010
	char     SiteName[32];		//32 bytes վ������: Beijing
	float    Latitude;			//4 bytes  վַγ��(degree),-90~90
	float    Longitude;			//4 bytes  վַ����(degree),-180~180
	uint32_t RadarHeight;		//4 bytes  ���߸߶�(MASL) 0-9000
	uint32_t GroundHeight;		//4 bytes  �״�վ����߶�(masl) 0-9000
	float    Frequency;			//4 bytes  ����Ƶ��(MHz) 1.0~999000.0
	float    BeamWidthH;		//4 bytes  ˮƽ�������(degree) 0.1-2.0
	float    BeamWidthV;		//4 bytes  ��ֱ�������(degree) 0.1-2.0
	uint32_t VersionRDA;		//4 bytes  RDA����İ汾��
	uint16_t RadarType;			//2 bytes  1-SA,2-SB,3-SC, 33-CA,34-CB,35-CC,36-CCJ,37-CD, 4-SAD,65-XA...
	int16_t  Reserved[27];		//54 bytes �����ֶ�
}stdCimissSiteConfig;
//
typedef struct __tagScanTaskConfig
{//256 bytes  
	char     Name[32];			//32 bytes ������ ��:VCP21
	char     Description[128];	//128 bytes ��������
	uint32_t PolarizationType;	//4  bytes ������ʽ��1~4, 1-ˮƽ����,2-��ֱ����,3-ˮƽ/��ֱͬʱ,4-ˮƽ/��ֱ����
	uint32_t ScanType;			//4  bytes ɨ�跽ʽ��0~6, 0-��ɨ,1-����PPI,2-����RHI,3-������ɨ,4-����ɨ,5-���RHI,6-�ֹ�ɨ��
	uint32_t PulsWidth;			//4  bytes ������(ns), 0-10000, 
	uint32_t ScanStartTime;		//4  bytes ɨ�迪ʼʱ��(s),0-..., UTC ������ʱ��
	uint32_t CutNumber;			//4  bytes ɨ����� 1~256
	float	 NoiseH;			//4  bytes ˮƽͨ��������ƽ(dBm) -100.0~-.0,
	float    NoiseV;			//4  bytes ��ֱͨ��������ƽ(dBm) -100.0~-.0
	float	 CalibrationH;		//4  bytes ˮƽͨ�������ʱ궨����(dB) 0.0~200.0
	float	 CalibrationV;		//4  bytes ��ֱͨ�������ʱ궨����(dB) 0.0~200.0 
	float    NoiseTemperatureH;	//4  bytes ˮƽͨ�������¶�(K) 0.0~800.0
	float	 NoiseTemperatureV;	//4  bytes ��ֱͨ�������¶�(K) 0.0~800.00
	float	 CalibrationZDR;	//4  bytes ZDR�궨ƫ��(dB) -10.00~10.00
	float	 CalibrationPDP;	//4  bytes ������Ʊ궨ƫ��(Degree) -180.0~180.00
	float	 CalibrationLDR;	//4  bytes ϵͳLDR�궨ƫ��(dB) -60~0
	int32_t	 Reserved[10];		//40 bytes �����ֶ�
}stdCimissScanTaskConfig;

typedef struct __tagGeneFilterMask {
	uint32_t interFilter : 1;
	uint32_t censorFilter : 1;
	uint32_t spekFilter1DLog : 1;
	uint32_t spekFilter1DDop : 1;
	uint32_t spekFilter2DLog : 1;
	uint32_t spekFilter2DDop : 1;
	uint32_t nebor : 1;//1 for enable noise estimation
	uint32_t gccal : 1;//1 for enable ground clutter calibration 
	uint32_t spared : 24;
}stdCimissGeneFilterMask;
typedef struct __tagGeneQcThresh {
	float sqi;//signal quality index
	float sig;//weather signal thresh
	float csr;//cluter signal ratio
	float log;//singal noise ratio
	float cpa;//const phase aligment
	float pmi;//polarimetric meteo index
	float dplog;
	float spare[1];
}stdCimissGeneQcThresh;
typedef struct __tagGeneQcMask {
	int32_t dbt;
	int32_t dbz;
	int32_t vel;
	int32_t wid;
	int32_t dpvar;//thresh apply to dual pol variable
	int32_t spare[3];
}stdCimissGeneQcMask;

typedef struct __tagCutConfig
{//256 bytes
	uint32_t ProcessMode;		//4  bytes �ز��źŴ���ģʽ  1-PPP��2-FFT
	uint32_t WaveForm;			//4  bytes ����  0-CS�������,1-CD����Doppler,2-CDX��������չ,3-Rx Test,4-BATCH��ģʽ 5-Dual PRF˫PRFģʽ,6-�β�PRT
	float	 PRF1;				//4  bytes �����ظ�Ƶ��1(Hz) 1~3000�� ����BATCH��˫PRF���β�PRTģʽ����ʾ��PRFֵ������������PRFģʽ����ʾΨһ��PRFֵ
	float    PRF2;				//4  bytes �����ظ�Ƶ��2(Hz) 1~3000�� ����BATCH��˫PRF���β�PRTģʽ����ʾ��PRFֵ������������PRFģʽ����Ч
	uint32_t DealiasingMode;	//4  bytes ���ٶ�ģ������ 1~4.  1-��PRF,2- 3:2˫PRFģʽ;3- 4:3˫PRFģʽ;4- 5:4˫PRFģʽ
	float	 Azimuth;			//4  bytes RHIģʽ�ķ�λ��(degree) 0~360.00
	float	 Elevation;			//4  bytes PPIģʽ�ĸ�����(degree) -2.0~90.00
	float	 StartAngle;		//4  bytes PPI��ɨ��ʼ��λ/RHIģʽ��������(degree) -10.00~360.00
	float	 EndAngle;			//4  bytes PPI��ɨ������λ/RHIģʽ��������(degree) -10.00~360.00
	float	 AnguleResolution;	//4  bytes PPIģʽ�������ݵĽǶȷֱ��� (degree) 0.00~2.00
	float	 ScanSpeed;			//4  bytes PPIɨ�跽λת��/RHIɨ�踩��ת��(degree) 0.00~36.00
	uint32_t LogResolution;		//4  bytes ǿ�����ݣ��練���ʣ��ľ���ֱ���(m) 1~5000
	uint32_t DopplerResolution;	//4  bytes ���������ݣ����ٶȡ��׿��ľ���ֱ���(m) 1~5000
	uint32_t MaxRange1;			//4  bytes ��Ӧ�����ظ�Ƶ��1������̽�����(m 1~500,000)
	uint32_t MinRange2;			//4  bytes ��Ӧ�����ظ�Ƶ��2������̽�����(m 1~500,000)
	uint32_t StartRange;		//4  bytes ��ʼ̽�����(m) 1~500,00
	uint32_t Sample1;			//4  bytes ��Ӧ�������ظ�Ƶ��1�Ĳ������� 2~512
	uint32_t Sample2;			//4  bytes ��Ӧ�������ظ�Ƶ��2�Ĳ������� 2~512
	uint32_t PhaseMode;			//4  bytes ��λ����ģʽ 1-�̶�ģʽ��2-�����λ��3-SZ����
	float	 AtmosphericLoss;	//4  bytes ˫�̴���˥��(dB/km), 0.000000~10.000000 ����ΪС�����6λ
	float	 NyquistSpeed;		//4  bytes �������ģ���ٶ�(m/s) 0~100 
	int64_t  MomentMask;		//8  bytes �����������룬0~0xFFFFFFFFFFFFFFFF 0-�������ȡ����;1-�����ȡ����; �������붨�����¼1
	int64_t  MomentSizeMask;	//8  bytes �����������룬0~0xFFFFFFFFFFFFFFFF  ��������ʽ��ʾ�����������ֽ�����0-1�ֽ�;1-2�ֽ�;��Ӧ��������˳��ͬ��¼1
	//uint32_t MiscFilterMask;	//4	 bytes �˲���������  0 - δӦ��;1-Ӧ�� 0~0xFFFFFFFF �����������¼2
	stdCimissGeneFilterMask MiscFilterMask;
	stdCimissGeneQcThresh QcThreshhod;
	stdCimissGeneQcMask QcMash;
	uint32_t ScanSync;			//4  bytes �����ֶΣ����ڱ�Ƕಿ�״�ͬ���۲�
	uint32_t ScanDirection;		//4  bytes PPIģʽ������ת������ 1-˳ʱ��;2-��ʱ��
	uint16_t ClutterClassifierType;	//2 bytes �����Ӳ���������  1-�������ݶ����˲�  2-ȫ�˲�  3-ʹ��ʵʱ��̬�˲�ͼ 4-ʹ�þ�̬�˲�ͼ
	uint16_t ClutterFilterType;		//2  bytes �����Ӳ��������� 0-���˲���1-Ƶ������Ӧ�˲�;2-�̶�����Ƶ���˲���;3-�ɱ����Ƶ���˲���;4-�ɱ���С����Ƶ���˲���;5-IIRʱ���˲�
	uint16_t ClutterFilterNotchWidth;//2 bytes �����˲����(0.1m/s)  0.1~10.0
	uint16_t ClutterFilterWindow;	//2 bytes  �˲��㷨FFT��������   0-���δ�;1-������;2-Blackman��; 3-����Ӧ��; 4-��
	char twins;
	//ground clutter filter related options
	unsigned char gcfMinWidth;
	unsigned char gcfEdgePoints;
	unsigned char gcfSlopePoints; 
	uint32_t Reserved[17];			//72 bytes  �����ֶ�	 
}stdCimissCutConfig;

//
typedef struct __tagRadialHeader
{//64 bytes 
	uint32_t RadialState;		//4  bytes ��������״̬ 0~6: 0-���ǿ�ʼ,1-�м侶��,2-���ǽ���,3-��ɨ��ʼ,4-��ɨ����,5-RHI��ʼ,6-RHI����
	uint32_t SpotBlank;			//4  bytes ������־ ��ʾ�����þ�����������ɨ�衣 0-������1-������
	uint32_t SequenceNumber;	//4  bytes ��1��ʼ�����ı���ɨ�ھ�����,1~65536
	uint32_t RadialNumber;		//4  bytes ��1��ʼ�����ı�PPI��RHI�ھ�����  1~1000
	uint32_t ElevationNumber;	//4  bytes ��1��ʼ�����ı���ɨ���Ǳ��,1~50
	float	 Azimuth;			//4  bytes ��λ��(degree) 0.00~360.00
	float	 Elevation;			//4  bytes ������(degree) -2.00~90.00
	uint32_t Seconds;			//4  bytes �������ݲɼ�ʱ��(s) ������ʱ�� UTC
	uint32_t Microseconds;		//4  bytes �������ݲɼ�ʱ��ȥ�����������µ�΢����(ms)
	uint32_t RadialLength;		//4  bytes �������ݿ�ĳ���(bytes) 0~100000
	uint32_t MomentNumber;		//4  bytes �۲�Ҫ�ص�����,��Z��V��W��λһ��Ҫ�� 0~64
	int16_t	 Reserved[10];		//20 bytes �����ֶ�
}stdCimissGenRadialHeader;
const uint16_t SIZE_GEN_RADIAL_HEADER = sizeof(stdCimissGenRadialHeader);

typedef struct __tagMomentRadialHeader
{// bytes 
	uint32_t	DataType;		//4 bytes ��������(��¼1) 1~64
	int32_t		Scale;			//4 bytes ���ݱ������    0~32768
	int32_t		Offset;			//4 bytes ���ݱ���ƫ����  0~32768
	uint16_t    OneBinLength;	//2 bytes ����һ��������ֵ���õĳ��� ��byte��1~2
	uint16_t    Flags;			//2 bytes ���ݱ�־λ,��ʱ��������
	uint32_t    AllBinLength;	//4 bytes �������о�����������еĳ���(bytes),������Ҫ�ؾ���ͷ(__tagMomentRadialHeader)����
	int32_t     Reserved[3];	//12bytes �����ֶ�
}stdCimissMomentRadialHeader;
const uint16_t SIZE_MOM_RADIAL_HEADER = sizeof(stdCimissMomentRadialHeader);

typedef vector<uint8_t> vcRadialData;
typedef struct __tagOneMomentRadial	//��������Ҫ�صĵ�����������
{// bytes
	stdCimissMomentRadialHeader header;	//Ҫ��ͷ
	vcRadialData Data;		//Ҫ������ ���ֽ���stdMomentRadialHeader.AllBinLength�������ⳤ��λstdMomentRadialHeader.OneBinLength
}stdCimissOneMomentRadial;
/*
typedef struct __tagOneRadial		//��������Ҫ�صĵ����������ݣ��Լ�����ͷ
{
	stdRadialHeader RadialHeader;
	stdMomentRadial* pRadialData;   //pRadialData=new stdMomentRadial[RadialHeader.MomentNumber];
}stdOneRadial;
*/

//---- �۲������ļ���2������: 1)�������ݿ� + 2)�������ݿ�
typedef vector<stdCimissCutConfig> cvCimissCutConfig;
typedef struct __tagCommoBolck  
{//1���������ݿ�
	stdCimissGenericHeader	GenricHeader;
	stdCimissSiteConfig		SiteConfig;
	stdCimissScanTaskConfig   ScanTaskConfig;
	cvCimissCutConfig			CutConfig; //pCutConfig[nCuts]
}stdCimissCommonBlock;

//typedef struct __tagRadialBolck	//����һ��ɨ������Ҫ�ص����о�������
//{//2���������ݿ�
	//stdOneRadial** pRadials;		//Cimiss = new *stdOneRadial[stdScanTaskConfig.CutNumber]; 
									//pRadials[iCut]= new stdOneRadial[iRadial]; //iRadial����stdRadialHeader.RadialState�������
//}stdRadialBlock;
