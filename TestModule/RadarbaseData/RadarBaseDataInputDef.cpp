#include "stdafx.h"
#include "../CommVar.h"
#include "RadarBaseDataMomentTypeDef.h"
#include "RadarBaseDataFormatTypeDef.h"
#include "RadarBaseDataCommonPPI.h"
#include "RadarBaseDataFormatSA.h"
#include "RadarBaseDataInputDef.h"
#include "RadarBaseDataWsr98D.h"
//#include "RadarBaseDataCIMISSCls.h"
#include "RadarBaseDataInputCls.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

stdGeneralInputScanData::stdGeneralInputScanData()
{
	Needed = false;
	Available = false;
}
stdMomentFlag::stdMomentFlag()
{
	bZ=false;
	bZc = false;
	bV = false;
	bVc = false;
	bW = false;
	bWc = false;
	bZDR = false;
	bZDRc = false;
	bPDP = false;
	bKDP = false;
	bRHV = false;
	bLDR = false;
}

void stdMomentFlag::Reset()
{
	stdMomentFlag();
}

uint16_t CommonMomentTypeToID(char *szType)
{
	string strType = "";
	strType.assign(szType);
	if (strType.find(M_Z) != string::npos)
		return M_Z_ID;
	else if (strType.find(M_V) != string::npos)
		return M_V_ID;
	else if (strType.find(M_W) != string::npos)
		return M_W_ID;
	else if (strType.find(M_ZDR) != string::npos)
		return M_ZDR_ID;
	else if (strType.find(M_PDP) != string::npos)
		return M_PDP_ID;
	else if (strType.find(M_KDP) != string::npos)
		return M_KDP_ID;
	else if (strType.find(M_LDR) != string::npos)
		return M_LDR_ID;
	else if (strType.find(M_RHV) != string::npos)
		return M_RHV_ID;
	else if (strType.find(M_Zc) != string::npos)
		return M_Zc_ID;
	else if (strType.find(M_Vc) != string::npos)
		return M_Vc_ID;
	else if (strType.find(M_Wc) != string::npos)
		return M_Wc_ID;
	else if (strType.find(M_ZDRc) != string::npos)
		return M_ZDRc_ID;

	return 0;
}

char* CommonMomentIdToType(uint16_t id, char *szType)
{
	switch (id)
	{
	case M_Z_ID:
		strcpy_s(szType, strlen(M_Z) + 1, M_Z);
		break;
	case M_V_ID:
		strcpy_s(szType, strlen(M_V) + 1, M_V);
		break;
	case M_W_ID:
		strcpy_s(szType, strlen(M_W) + 1, M_W);
		break;
	case M_ZDR_ID:
		strcpy_s(szType, strlen(M_ZDR) + 1, M_ZDR);
		break;
	case M_PDP_ID:
		strcpy_s(szType, strlen(M_PDP) + 1, M_PDP);
		break;
	case M_KDP_ID:
		strcpy_s(szType, strlen(M_KDP) + 1, M_KDP);
		break;
	case M_LDR_ID:
		strcpy_s(szType, strlen(M_LDR) + 1, M_LDR);
		break;
	case M_RHV_ID:
		strcpy_s(szType, strlen(M_RHV) + 1, M_RHV);
		break;
	case M_Zc_ID:
		strcpy_s(szType, strlen(M_Zc) + 1, M_Zc);
		break;
	case M_Vc_ID:
		strcpy_s(szType, strlen(M_Vc) + 1, M_Vc);
		break;
	case M_Wc_ID:
		strcpy_s(szType, strlen(M_Wc) + 1, M_Wc);
		break;
	case M_ZDRc_ID:
		strcpy_s(szType, strlen(M_ZDRc) + 1, M_ZDRc);
		break;
	default:
		strcpy_s(szType, 2, "");
		break;
	}

	return szType;
}

stdBaseDataRadarInfo::stdBaseDataRadarInfo()
{
	strRadarName = "";
	strRadarType = "";
	strRadarCode = "";
	strScanType = "";
	fRadarLon = 0.;
	fRadarLat = 0.;
	fRadarHt = 0.;
}

void stdBaseDataRadarInfo::Reset()
{
	stdBaseDataRadarInfo();
}