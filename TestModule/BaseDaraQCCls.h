// BaseDaraQCCls.h: interface for the CBaseDataIOandQCCls class.
//
/********************************************************************************
*                                                                               *
*  Function:  Do quality controlling of base data, and send base processed data *
*             to the next task to do further work.                              *
*                                                                               *
*  Parameter: pointer to the pameter passed to thie task(File name)             *
*                                                                               *
*  Output:    DWORD                                                             *
*                                                                               *
********************************************************************************/
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEDARAQCCLS_H__C43F69B3_5891_46AE_B48A_EBAFA1FD73B6__INCLUDED_)
#define AFX_BASEDARAQCCLS_H__C43F69B3_5891_46AE_B48A_EBAFA1FD73B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RadarbaseData/RadarBaseDataInputCls.h"
#include <string>
using namespace std;

class CBaseDataIOandQCCls  
{
public:
	CBaseDataIOandQCCls(int32_t runid, int32_t siteid, char* srcfile);
	virtual ~CBaseDataIOandQCCls();
	
	int32_t Run();
	const char* GetRefFileName();
	const char* GetVelFileName();
protected:
	int32_t m_runNum;
	int32_t m_siteIndex;
	char m_srcBaseDataFileName[PATH_LEN];
	struct RADARSITEINFO m_siteInfo;

	bool m_paramSaveRef;
	bool m_paramSaveVel;
	string m_strParamProdDir;
	string m_strRefDataName;
	string m_strVelDataName;
public:
	int32_t m_siteCode; 
	int32_t m_maxNumCuts;
	int32_t m_numCuts;	
	int32_t m_numRadials;
	int32_t m_numGates;	
private:
	int32_t findStr(char *str, char *substr);
	bool PrepareObsvDataFileForReading(char* szSrcFileName, char* szDstFileName);
	void LoadParameter();
	bool FileIsZipped(const char *srcFileName, char *dstFileName);
};

#endif // !defined(AFX_BASEDARAQCCLS_H__C43F69B3_5891_46AE_B48A_EBAFA1FD73B6__INCLUDED_)
