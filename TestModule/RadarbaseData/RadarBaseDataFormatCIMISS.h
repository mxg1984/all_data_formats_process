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

// 附录1 -- 数据类型掩码定义
// BIT(LSB)		MOMENT			REMARKS
// 1            dBT				滤波前反射率（Total reflectivity)
// 2	        dBZ				滤波后反射率 (reflectivity)
// 3			V				径向速度
// 4			W				谱宽
// 5			SQI				信号质量指数
// 6			CPA				杂波相位一致性 （Clutter Phase Alignment)
// 7			ZDR				差分反射率
// 8			LDR				退偏振比
// 9			CC				协相关系数(Cross Correlation Coeffcient)
//10			PHIDP			差分相移
//11			KDP				差分相移率
//12			CP				杂波可能性(Clutter Probability)
//13			Reserved		数据标志,保留
//14			HCL				双偏振相态分类（Hydro Classification)
//15			CF				杂波标志(Clutter Flag)
//16	        SNRH			水平通道信噪比
//17-31			Reserved		数据标志,保留 
//32			Zc				订正后反射率
//33			Vc				订正后径向速度
//34			Wc				订正后谱宽
//35			ZDRc			订正后差分反射率
// 附录1


// 附录2 -- 滤波设置掩码定义
// BIT(LSB)		Filter Method			REMARKS
// 0            干扰过滤				基于脉冲检查的干扰过滤算法
// 1            奇异点过滤				奇异点过滤算法
// 2	        一维反射率点杂波过滤	对反射率数据使用一维点杂波过滤
// 3			一维多普勒点杂波过滤	对多普勒数据使用一维点杂波过滤 
// 4			二维反射率点杂波过滤	对反射率数据使用二维(3*3方位和距离)点杂波过滤
// 5			二维多普勒点杂波过滤	对多普勒数据使用二维(3*3方位和距离)点杂波过滤 
// 6-31			保留
// 附录2

// 附录3 -- 质控门限掩码定义
// BIT(LSB)		Threshold				REMARKS
// 0            SQI						信号质量指数
// 1            SIG						天气信号强度
// 2	        CSR						地物杂波与天气信号比率
// 3			LOG						信噪比
// 4			CPA						地物杂波相位稳定指数
// 5			PMI						极化天气信号指数
// 6 			DPLOG					偏振量信噪比
// 7-31			保留
// 附录3

//数据体部分观测要素值说明
// 径向数据头后面为按距离库依次保存的径向数据
// 径向数据库数:  径向数据头中的参数长度Length/库字节长度BinLength
// 径向数据类型:  1字节或2字节长度的无符号整形
// 保存形式:      编码存储
// 解码方式:      编码值>=5，实际值 = (编码值-Offset)/Scale
//				  编码值==4，保留
//				  编码值==3，未知数据
//				  编码值==2，未扫描，如电子消掩区
//				  编码值==1，距离折叠
//				  编码值==0，信号小于门限,认为无回波//  
//the end


//要素掩码位定义 64 bits
// 0000 0000 0000 0000 
// 0000 0000 0000 0000 
// 0000 0000 0000 0000
// 0000 0000 0000 0000                    //bit位
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

//BIT位INDEX
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

//径向状态
#define EL_FIRST 0
#define EL_LAST 2
#define VS_FIRST 3
#define VS_LAST 4
#define RHI_FIRST 5
#define RHI_LAST 6

typedef struct __tagGenericHeader
{//32 bytes
	uint32_t MagicNumber;		//4 bytes 规定标记=0x4D545352，指示雷达数据文件
	uint16_t MajorVersion;		//2 bytes 主版本号
	uint16_t MinorVersion;		//2 bytes 次版本号 
	uint32_t GenericType;		//4 bytes 文件类型,1-2    1:基数据; 2:气象产品
	uint32_t ProdutType;		//4 bytes 产品类型,1-100  GenericType为1时此参数有效
	uint32_t  Reserved[4];		//16 bytes 保留字段
}stdCimissGenericHeader;
//
typedef struct __tagSiteConfig
{//128 bytes
	char     SiteCode[8];		//8 bytes  站号标记，如：Z9010
	char     SiteName[32];		//32 bytes 站名，如: Beijing
	float    Latitude;			//4 bytes  站址纬度(degree),-90~90
	float    Longitude;			//4 bytes  站址经度(degree),-180~180
	uint32_t RadarHeight;		//4 bytes  天线高度(MASL) 0-9000
	uint32_t GroundHeight;		//4 bytes  雷达站地面高度(masl) 0-9000
	float    Frequency;			//4 bytes  工作频率(MHz) 1.0~999000.0
	float    BeamWidthH;		//4 bytes  水平波束宽度(degree) 0.1-2.0
	float    BeamWidthV;		//4 bytes  垂直波束宽度(degree) 0.1-2.0
	uint32_t VersionRDA;		//4 bytes  RDA软件的版本号
	uint16_t RadarType;			//2 bytes  1-SA,2-SB,3-SC, 33-CA,34-CB,35-CC,36-CCJ,37-CD, 4-SAD,65-XA...
	int16_t  Reserved[27];		//54 bytes 保留字段
}stdCimissSiteConfig;
//
typedef struct __tagScanTaskConfig
{//256 bytes  
	char     Name[32];			//32 bytes 任务名 如:VCP21
	char     Description[128];	//128 bytes 任务描述
	uint32_t PolarizationType;	//4  bytes 极化方式，1~4, 1-水平极化,2-垂直极化,3-水平/垂直同时,4-水平/垂直交替
	uint32_t ScanType;			//4  bytes 扫描方式，0~6, 0-体扫,1-单层PPI,2-单层RHI,3-单层扇扫,4-扇体扫,5-多层RHI,6-手工扫描
	uint32_t PulsWidth;			//4  bytes 脉冲宽度(ns), 0-10000, 
	uint32_t ScanStartTime;		//4  bytes 扫描开始时间(s),0-..., UTC 儒略日时间
	uint32_t CutNumber;			//4  bytes 扫描层数 1~256
	float	 NoiseH;			//4  bytes 水平通道噪声电平(dBm) -100.0~-.0,
	float    NoiseV;			//4  bytes 垂直通道噪声电平(dBm) -100.0~-.0
	float	 CalibrationH;		//4  bytes 水平通道反射率标定常数(dB) 0.0~200.0
	float	 CalibrationV;		//4  bytes 垂直通道反射率标定常数(dB) 0.0~200.0 
	float    NoiseTemperatureH;	//4  bytes 水平通道噪声温度(K) 0.0~800.0
	float	 NoiseTemperatureV;	//4  bytes 垂直通道噪声温度(K) 0.0~800.00
	float	 CalibrationZDR;	//4  bytes ZDR标定偏差(dB) -10.00~10.00
	float	 CalibrationPDP;	//4  bytes 差分相移标定偏差(Degree) -180.0~180.00
	float	 CalibrationLDR;	//4  bytes 系统LDR标定偏差(dB) -60~0
	int32_t	 Reserved[10];		//40 bytes 保留字段
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
	uint32_t ProcessMode;		//4  bytes 回波信号处理模式  1-PPP，2-FFT
	uint32_t WaveForm;			//4  bytes 波形  0-CS连续监测,1-CD连续Doppler,2-CDX多普勒扩展,3-Rx Test,4-BATCH批模式 5-Dual PRF双PRF模式,6-参差PRT
	float	 PRF1;				//4  bytes 脉冲重复频率1(Hz) 1~3000。 对于BATCH、双PRF、参差PRT模式，表示高PRF值；对于其它单PRF模式，表示唯一的PRF值
	float    PRF2;				//4  bytes 脉冲重复频率2(Hz) 1~3000。 对于BATCH、双PRF、参差PRT模式，表示低PRF值；对于其它单PRF模式，无效
	uint32_t DealiasingMode;	//4  bytes 退速度模糊方案 1~4.  1-单PRF,2- 3:2双PRF模式;3- 4:3双PRF模式;4- 5:4双PRF模式
	float	 Azimuth;			//4  bytes RHI模式的方位角(degree) 0~360.00
	float	 Elevation;			//4  bytes PPI模式的俯仰角(degree) -2.0~90.00
	float	 StartAngle;		//4  bytes PPI扇扫起始方位/RHI模式高限仰角(degree) -10.00~360.00
	float	 EndAngle;			//4  bytes PPI扇扫结束方位/RHI模式低限仰角(degree) -10.00~360.00
	float	 AnguleResolution;	//4  bytes PPI模式径向数据的角度分辨率 (degree) 0.00~2.00
	float	 ScanSpeed;			//4  bytes PPI扫描方位转速/RHI扫描俯仰转速(degree) 0.00~36.00
	uint32_t LogResolution;		//4  bytes 强度数据（如反射率）的距离分辨率(m) 1~5000
	uint32_t DopplerResolution;	//4  bytes 多普勒数据（如速度、谱宽）的距离分辨率(m) 1~5000
	uint32_t MaxRange1;			//4  bytes 对应脉冲重复频率1的最大可探测距离(m 1~500,000)
	uint32_t MinRange2;			//4  bytes 对应脉冲重复频率2的最大可探测距离(m 1~500,000)
	uint32_t StartRange;		//4  bytes 起始探测距离(m) 1~500,00
	uint32_t Sample1;			//4  bytes 对应于脉冲重复频率1的采样个数 2~512
	uint32_t Sample2;			//4  bytes 对应于脉冲重复频率2的采样个数 2~512
	uint32_t PhaseMode;			//4  bytes 相位编码模式 1-固定模式；2-随机相位；3-SZ编码
	float	 AtmosphericLoss;	//4  bytes 双程大气衰减(dB/km), 0.000000~10.000000 精度为小数点后6位
	float	 NyquistSpeed;		//4  bytes 理论最大不模糊速度(m/s) 0~100 
	int64_t  MomentMask;		//8  bytes 数据类型掩码，0~0xFFFFFFFFFFFFFFFF 0-不允许获取数据;1-允许获取数据; 具体掩码定义见附录1
	int64_t  MomentSizeMask;	//8  bytes 数据类型掩码，0~0xFFFFFFFFFFFFFFFF  以掩码形式表示的数据类型字节数，0-1字节;1-2字节;对应数据类型顺序同附录1
	//uint32_t MiscFilterMask;	//4	 bytes 滤波设置掩码  0 - 未应用;1-应用 0~0xFFFFFFFF 具体掩码见附录2
	stdCimissGeneFilterMask MiscFilterMask;
	stdCimissGeneQcThresh QcThreshhod;
	stdCimissGeneQcMask QcMash;
	uint32_t ScanSync;			//4  bytes 保留字段，用于标记多部雷达同步观测
	uint32_t ScanDirection;		//4  bytes PPI模式下天线转动方向 1-顺时针;2-逆时针
	uint16_t ClutterClassifierType;	//2 bytes 地物杂波区分类型  1-所有数据都不滤波  2-全滤波  3-使用实时动态滤波图 4-使用静态滤波图
	uint16_t ClutterFilterType;		//2  bytes 地物杂波抑制类型 0-不滤波；1-频域自适应滤波;2-固定带宽频域滤波器;3-可变带宽频域滤波器;4-可变最小方差频域滤波器;5-IIR时域滤波
	uint16_t ClutterFilterNotchWidth;//2 bytes 地物滤波宽度(0.1m/s)  0.1~10.0
	uint16_t ClutterFilterWindow;	//2 bytes  滤波算法FFT窗口类型   0-矩形窗;1-汉明窗;2-Blackman窗; 3-自适应窗; 4-无
	char twins;
	//ground clutter filter related options
	unsigned char gcfMinWidth;
	unsigned char gcfEdgePoints;
	unsigned char gcfSlopePoints; 
	uint32_t Reserved[17];			//72 bytes  保留字段	 
}stdCimissCutConfig;

//
typedef struct __tagRadialHeader
{//64 bytes 
	uint32_t RadialState;		//4  bytes 径向数据状态 0~6: 0-仰角开始,1-中间径向,2-仰角结束,3-体扫开始,4-体扫结束,5-RHI开始,6-RHI结束
	uint32_t SpotBlank;			//4  bytes 消隐标志 表示跳过该径向跳过，不扫描。 0-正常，1-消隐藏
	uint32_t SequenceNumber;	//4  bytes 从1开始计数的本体扫内径向数,1~65536
	uint32_t RadialNumber;		//4  bytes 从1开始计数的本PPI或RHI内径向数  1~1000
	uint32_t ElevationNumber;	//4  bytes 从1开始计数的本体扫仰角编号,1~50
	float	 Azimuth;			//4  bytes 方位角(degree) 0.00~360.00
	float	 Elevation;			//4  bytes 俯仰角(degree) -2.00~90.00
	uint32_t Seconds;			//4  bytes 径向数据采集时间(s) 儒略日时间 UTC
	uint32_t Microseconds;		//4  bytes 径向数据采集时间去掉秒数后余下的微秒数(ms)
	uint32_t RadialLength;		//4  bytes 径向数据块的长度(bytes) 0~100000
	uint32_t MomentNumber;		//4  bytes 观测要素的数量,如Z、V、W各位一种要素 0~64
	int16_t	 Reserved[10];		//20 bytes 保留字段
}stdCimissGenRadialHeader;
const uint16_t SIZE_GEN_RADIAL_HEADER = sizeof(stdCimissGenRadialHeader);

typedef struct __tagMomentRadialHeader
{// bytes 
	uint32_t	DataType;		//4 bytes 数据类型(附录1) 1~64
	int32_t		Scale;			//4 bytes 数据编码比例    0~32768
	int32_t		Offset;			//4 bytes 数据编码偏移量  0~32768
	uint16_t    OneBinLength;	//2 bytes 保存一个距离库的值所用的长度 （byte）1~2
	uint16_t    Flags;			//2 bytes 数据标志位,暂时保留待用
	uint32_t    AllBinLength;	//4 bytes 保存所有距离库数据所有的长度(bytes),不包含要素径向头(__tagMomentRadialHeader)自身
	int32_t     Reserved[3];	//12bytes 保留字段
}stdCimissMomentRadialHeader;
const uint16_t SIZE_MOM_RADIAL_HEADER = sizeof(stdCimissMomentRadialHeader);

typedef vector<uint8_t> vcRadialData;
typedef struct __tagOneMomentRadial	//包含单个要素的单个径向数据
{// bytes
	stdCimissMomentRadialHeader header;	//要素头
	vcRadialData Data;		//要素数据 总字节数stdMomentRadialHeader.AllBinLength，单个库长度位stdMomentRadialHeader.OneBinLength
}stdCimissOneMomentRadial;
/*
typedef struct __tagOneRadial		//包含所有要素的单个径向数据，以及径向头
{
	stdRadialHeader RadialHeader;
	stdMomentRadial* pRadialData;   //pRadialData=new stdMomentRadial[RadialHeader.MomentNumber];
}stdOneRadial;
*/

//---- 观测数据文件由2大块组成: 1)公共数据块 + 2)径向数据块
typedef vector<stdCimissCutConfig> cvCimissCutConfig;
typedef struct __tagCommoBolck  
{//1）公共数据块
	stdCimissGenericHeader	GenricHeader;
	stdCimissSiteConfig		SiteConfig;
	stdCimissScanTaskConfig   ScanTaskConfig;
	cvCimissCutConfig			CutConfig; //pCutConfig[nCuts]
}stdCimissCommonBlock;

//typedef struct __tagRadialBolck	//包含一次扫描所有要素的所有径向数据
//{//2）径向数据块
	//stdOneRadial** pRadials;		//Cimiss = new *stdOneRadial[stdScanTaskConfig.CutNumber]; 
									//pRadials[iCut]= new stdOneRadial[iRadial]; //iRadial根据stdRadialHeader.RadialState计算出来
//}stdRadialBlock;
