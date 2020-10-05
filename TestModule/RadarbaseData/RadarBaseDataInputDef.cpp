#include "RadarBaseDataInputDef.h"

#include "../CommVar.h"
#include "RadarBaseDataMomentTypeDef.h"
#include "RadarBaseDataFormatTypeDef.h"
#include "RadarBaseDataFormatSA.h"

#include "RadarBaseDataWsr98D.h"
//#include "RadarBaseDataCIMISSCls.h"
#include "RadarBaseDataInputCls.h"


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
	std::string strType = "";
	strType.assign(szType);
	if (strType.find(M_Z) != std::string::npos)
		return M_Z_ID;
	else if (strType.find(M_V) != std::string::npos)
		return M_V_ID;
	else if (strType.find(M_W) != std::string::npos)
		return M_W_ID;
	else if (strType.find(M_ZDR) != std::string::npos)
		return M_ZDR_ID;
	else if (strType.find(M_PDP) != std::string::npos)
		return M_PDP_ID;
	else if (strType.find(M_KDP) != std::string::npos)
		return M_KDP_ID;
	else if (strType.find(M_LDR) != std::string::npos)
		return M_LDR_ID;
	else if (strType.find(M_RHV) != std::string::npos)
		return M_RHV_ID;
	else if (strType.find(M_Zc) != std::string::npos)
		return M_Zc_ID;
	else if (strType.find(M_Vc) != std::string::npos)
		return M_Vc_ID;
	else if (strType.find(M_Wc) != std::string::npos)
		return M_Wc_ID;
	else if (strType.find(M_ZDRc) != std::string::npos)
		return M_ZDRc_ID;

	return 0;
}

char* CommonMomentIdToType(uint16_t id, char *szType)
{
	switch (id)
	{
	case M_Z_ID:
		strcpy(szType, M_Z);
		break;
	case M_V_ID:
		strcpy(szType, M_V);
		break;
	case M_W_ID:
		strcpy(szType, M_W);
		break;
	case M_ZDR_ID:
		strcpy(szType, M_ZDR);
		break;
	case M_PDP_ID:
		strcpy(szType, M_PDP);
		break;
	case M_KDP_ID:
		strcpy(szType, M_KDP);
		break;
	case M_LDR_ID:
		strcpy(szType, M_LDR);
		break;
	case M_RHV_ID:
		strcpy(szType, M_RHV);
		break;
	case M_Zc_ID:
		strcpy(szType, M_Zc);
		break;
	case M_Vc_ID:
		strcpy(szType, M_Vc);
		break;
	case M_Wc_ID:
		strcpy(szType, M_Wc);
		break;
	case M_ZDRc_ID:
		strcpy(szType, M_ZDRc);
		break;
	default:
		strcpy(szType, "");
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