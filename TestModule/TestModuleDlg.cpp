
// TestModuleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestModule.h"
#include "TestModuleDlg.h"
#include "afxdialogex.h"
#include "UserTask.h"

//
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTestModuleDlg �Ի���



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


// CTestModuleDlg ��Ϣ�������

BOOL CTestModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestModuleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTestModuleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestModuleDlg::OnBnClickedButtonReaddata()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	StartTask();

	/*int32_t scode = 10;
	int  i = 0;
	do
	{
		string  szSrcFile = "D:/data/20200706-huabei/Z9010/Z_RADR_I_Z9010_20200706000000_O_DOR_SA_CAP.bin.bz2";

		//��ԭʼ�۲����� ת��Ϊ��ʱͳһ��ʽ����ɨ����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	return;
	/*int32_t scode = 10;
	string m_strRefGridFileName = "";
	string m_strVelGridFileName = "";
	string szUniformFile = "D:\\data\\20200706-huabei\\prod\\TempData\\UNIFORMBD\\����\\202007060005.Z.bin";

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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

		
}