#include "stdafx.h"

#include<experimental/filesystem>

#include "UserTask.h"
#include <cstdlib>
#include <thread>

#include "CommFunctions.h"
#include "GlobalParams.h"
#include "BaseDaraQCCls.h"
#include "PolarToGridCls.h"
#include "GridMosaicCls.h"

bool StartTask()
{
	//pthread_t _pthread;
	auto _thread = std::thread(actionTask);
//	if (pthread_create(&_pthread, NULL, actionTask, NULL) != 0)
		//return false;//cerr << "线程创建失败,线程号 = " << count <<endl;
	_thread.detach();
	return true;
}

void actionTask()
{
	/*
	stdRealTimeRunControl playbackCtrl;
	playbackCtrl.Reset();
	playbackCtrl.bRun = false;
	playbackCtrl.bExit = false;
	SendMessageToPlayBack(&playbackCtrl);
	*/
	std::string  strSrcFile[10] = { 
		"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702114800_O_DOR_SA_CAP.bin.bz2",
"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702115400_O_DOR_SA_CAP.bin.bz2",
"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702120000_O_DOR_SA_CAP.bin.bz2",
"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702120600_O_DOR_SA_CAP.bin.bz2",
"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702121200_O_DOR_SA_CAP.bin.bz2",
"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702121800_O_DOR_SA_CAP.bin.bz2",
"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702122400_O_DOR_SA_CAP.bin.bz2",
"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702123000_O_DOR_SA_CAP.bin.bz2",
"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702123600_O_DOR_SA_CAP.bin.bz2",
"C:/Users/mengxiangang/Desktop/z9010/Z9010/Z_RADR_I_Z9010_20200702124200_O_DOR_SA_CAP.bin.bz2"	};

	int maxcount = 10;
	int  count = 0;
	int i = 0;
	do
	{
		Process(strSrcFile[i]);		
		i++;
		if (i > 9)
			i = 0;

		count++;

		Sleep(50);
	} while (count <maxcount);
	::MessageBox(nullptr, L"PrOcEsS FiNiShEd", L"ReSuLt", IDOK);
}

void Process(std::string srcFile)
{
	int32_t scode = 10;
	
	//read data---------------------------------------------
	//string  srcFile = "D:/data/20200706-huabei/Z9010/Z_RADR_I_Z9010_20200706000000_O_DOR_SA_CAP.bin.bz2";
	//读原始观测数据 转存为临时统一格式的体扫数据
	char strMsg[LEN_MSG] = "";
	std::string m_strUniformRefFileName = "";
	std::string m_strUniformVelFileName = "";

	if (std::experimental::filesystem::exists(srcFile))
	{
		CBaseDataIOandQCCls RadarData(scode, (char*)(srcFile.c_str()));
		if (RadarData.Run() > 0)
		{
			m_strUniformRefFileName.assign(RadarData.GetRefFileName());
			m_strUniformVelFileName.assign(RadarData.GetVelFileName());
		}
		//delete pRadarData;
		//pRadarData = 0;
	}
	//Sleep(50);

	//polar to grid------------------------------------		
	std::string m_strRefGridFileName = "";
	std::string m_strVelGridFileName = "";
	if (g_iOptionsGridData == GRIDDATA_OPTION_ALL || g_iOptionsGridData == GRIDDATA_OPTION_REF)
	{
		if (std::experimental::filesystem::exists(m_strUniformRefFileName))
		{
			CPolarToGridOfRefCls polarToGrid(scode, (char*)(m_strUniformRefFileName.c_str()));
			polarToGrid.Run1();
			m_strRefGridFileName.assign(polarToGrid.GetProdFileName());
			//	delete polarToGrid;
			//	polarToGrid = 0x0;
			//}
		}
	}
	if (g_iOptionsGridData == GRIDDATA_OPTION_ALL || g_iOptionsGridData == GRIDDATA_OPTION_VEL)
	{
		if (FilePathExists(m_strUniformVelFileName.c_str()))
		{
			CPolarToGridOfDopplerCls polarToGrid(scode, (char*)(m_strUniformVelFileName.c_str()));
			//if (polarToGrid != 0x0)
			{
				polarToGrid.Run1();
				m_strVelGridFileName.assign(polarToGrid.GetProdFileName());
				//delete polarToGrid;
				//polarToGrid = 0x0;
			}
		}
	}

	//Mosaic ---------------//开始组网 !!!!!-------------------------------
	stdDateTime dateTime;	
	int16_t inputFiles = 1;
	std::string m_strMosaicRefFile = "";
	std::vector<std::string> m_strGridRefFileList;
	m_strGridRefFileList.push_back(m_strRefGridFileName);
	dateTime.year = 2020;
	dateTime.mon = 7;//0706000000
	dateTime.day = 6;
	dateTime.day = 6;
	dateTime.hor = 0;
	dateTime.min = 0;
	dateTime.sec = 0;
	if (FilePathExists(m_strRefGridFileName.c_str()))
	{
		CGridMosaicCls radarsMosaic(inputFiles, m_strGridRefFileList, &dateTime);
		radarsMosaic.Run();
		m_strMosaicRefFile.assign(radarsMosaic.GetProdFileName());
	}
}