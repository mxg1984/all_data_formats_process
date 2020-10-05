#pragma once

#include <string>

#include "RadarbaseData/RadarBaseDataInputCls.h"

class CBaseDataIOandQCCls  
{
public:
	CBaseDataIOandQCCls(int32_t siteid, const std::string& srcfile);
	virtual ~CBaseDataIOandQCCls();
	
	int32_t Run();
	const char* GetRefFileName();
	const char* GetVelFileName();
protected:
	int32_t m_siteIndex = 0;
	std::string m_srcBaseDataFileName;
	struct RADARSITEINFO m_siteInfo;

	bool m_paramSaveRef;
	bool m_paramSaveVel;
	std::string m_strParamProdDir="";
	std::string m_strRefDataName = "";
	std::string m_strVelDataName = "";
public:
	int32_t m_siteCode; 
	int32_t m_maxNumCuts = 11;
	int32_t m_numCuts = 0;	
	int32_t m_numRadials;
	int32_t m_numGates;	
private:
	bool PrepareObsvDataFileForReading(const std::string& szSrcFileName,
		std::string* const szDstFileName);
	void LoadParameter();
	bool FileIsZipped(const std::string& strSrcName, std::string *const strDstName);
};