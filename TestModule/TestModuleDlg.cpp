
// TestModuleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestModule.h"
#include "TestModuleDlg.h"
#include "afxdialogex.h"
#include "UserTask.h"

//
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestModuleDlg 对话框



CTestModuleDlg::CTestModuleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTMODULE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestModuleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_READDATA, &CTestModuleDlg::OnBnClickedButtonReaddata)
	ON_NOTIFY(BCN_DROPDOWN, IDC_BUTTON_READDATA, &CTestModuleDlg::OnDropdownButtonReaddata)
	ON_BN_CLICKED(IDC_BUTTON_POLARTOGRID, &CTestModuleDlg::OnClickedButtonPolartogrid)
END_MESSAGE_MAP()


// CTestModuleDlg 消息处理程序

BOOL CTestModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestModuleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestModuleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestModuleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestModuleDlg::OnBnClickedButtonReaddata()
{
	// TODO: 在此添加控件通知处理程序代码
	StartTask();

	/*int32_t scode = 10;
	int  i = 0;
	do
	{
		string  szSrcFile = "D:/data/20200706-huabei/Z9010/Z_RADR_I_Z9010_20200706000000_O_DOR_SA_CAP.bin.bz2";

		//读原始观测数据 转存为临时统一格式的体扫数据
		char strMsg[LEN_MSG] = "";

		string m_strUniformRefFileName = "";
		string m_strUniformVelFileName = "";

		CBaseDataIOandQCCls* pRadarData = new CBaseDataIOandQCCls(0, scode, (char*)(szSrcFile.c_str()));
		if (pRadarData->Run() > 0)
		{
			m_strUniformRefFileName.assign(pRadarData->GetRefFileName());
			m_strUniformVelFileName.assign(pRadarData->GetVelFileName());

			OnClickedButtonPolartogrid();
		}
		delete pRadarData;
		pRadarData = 0;

		Sleep(50);

		i++;
	} while (i < 50);*/

	bool bFlag = true;
}

//#include "PolarToGridCls.h"
void CTestModuleDlg::OnClickedButtonPolartogrid()
{
	// TODO: 在此添加控件通知处理程序代码
	return;
	/*int32_t scode = 10;
	string m_strRefGridFileName = "";
	string m_strVelGridFileName = "";
	string szUniformFile = "D:\\data\\20200706-huabei\\prod\\TempData\\UNIFORMBD\\北京\\202007060005.Z.bin";

	if (!FilePathExists(szUniformFile.c_str()))
		return;

	int i = 0;
	do
	{
		if (g_iOptionsGridData == GRIDDATA_OPTION_ALL || g_iOptionsGridData == GRIDDATA_OPTION_REF)
		{
			CPolarToGridOfRefCls *polarToGrid = new CPolarToGridOfRefCls(scode, (char*)(szUniformFile.c_str()));
			if (polarToGrid != 0x0)
			{
				polarToGrid->Run1();
				//strcpy_s(szGridFile,polarToGrid->GetProdFileName());
				m_strRefGridFileName.assign(polarToGrid->GetProdFileName());

				delete polarToGrid;
				polarToGrid = 0x0;
			}
		}
		if (g_iOptionsGridData == GRIDDATA_OPTION_ALL || g_iOptionsGridData == GRIDDATA_OPTION_VEL)
		{
			CPolarToGridOfDopplerCls *polarToGrid = new CPolarToGridOfDopplerCls(scode, (char*)(szUniformFile.c_str()));
			if (polarToGrid != 0x0)
			{
				polarToGrid->Run1();
				//strcpy_s(szGridFile,polarToGrid->GetProdFileName());
				m_strVelGridFileName.assign(polarToGrid->GetProdFileName());

				delete polarToGrid;
				polarToGrid = 0x0;
			}
		}
		i++;
	}while (i < 1);*/

	bool bFlag = true;
}


void CTestModuleDlg::OnDropdownButtonReaddata(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMBCDROPDOWN pDropDown = reinterpret_cast<LPNMBCDROPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

		
}