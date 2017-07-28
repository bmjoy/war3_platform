
// testclientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "testclient.h"
#include "testclientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtestclientDlg �Ի���



CtestclientDlg::CtestclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtestclientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_GameProcess = NULL;
}

void CtestclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CtestclientDlg::OnBnClickedButton1)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CtestclientDlg ��Ϣ�������

BOOL CtestclientDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//�����ʼ��
	this->m_udp.InitialUDP();
	//ģ���½
	this->m_udp.SendToUserLoginInfo("hjs","123456");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CtestclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtestclientDlg::OnPaint()
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
HCURSOR CtestclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CtestclientDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��������
	CHAR szRunParamete[16] = {0};
	//���ص�dll·��
	CHAR szDllPath[MAX_PATH] = {0};
	//������Ϸ·��
	CHAR szLocalGamePath[MAX_PATH] = {0};
	BOOL bIsSuccess = FALSE;
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	DWORD dwServiceIp = 0;
	ZeroMemory(&sInfo,sizeof(sInfo));
	ZeroMemory(&pInfo,sizeof(pInfo));

	sInfo.cb=sizeof(sInfo);

	//��ȡע���dll·��
	strcpy_s(szDllPath,MAX_PATH,"E:\\flym\\war3\\testclient\\Debug\\HookNetwork.dll");
	//��ȡ��Ϸ·��
	strcpy_s(szLocalGamePath,MAX_PATH,"F:\\ħ��3-��������\\war3.exe");

	strcpy_s(szRunParamete,16,"tango -window");

	//������Ϸ���̣������ڹ���״̬
	if(CreateProcess(szLocalGamePath,szRunParamete,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&sInfo,&pInfo))
	{
		//Զ���߳�ע��ģ��
		if(RemoteThreadInjectModu(szDllPath,pInfo.hProcess))
		{
			TRACE("Initialize Game Success!\r\n");
			bIsSuccess = TRUE;
			//������̶�����
			m_GameProcess = pInfo.hProcess;
			//�ָ���Ϸ���߳�����
			ResumeThread(pInfo.hThread);
			//�ر����̵߳��߳̾��
			CloseHandle(pInfo.hThread);
		}
		else
		{
			TerminateProcess(pInfo.hProcess,0);
			CloseHandle(pInfo.hThread);
			CloseHandle(pInfo.hProcess);
		}
	}
	else
	{
		TRACE("Create Game Process Failed!\r\n");
	}
}

BOOL CtestclientDlg::RemoteThreadInjectModu(TCHAR * szInjectDllPath,HANDLE hProcess)
{
	ASSERT(hProcess != INVALID_HANDLE_VALUE);
	LPVOID		ParameterAddress = NULL;
	DWORD		dwWirite = 0;
	LPVOID		lpLibraryAddress = NULL;
	HANDLE		hRemote = INVALID_HANDLE_VALUE;
	BOOL		bIsSuccess = FALSE;

	//���Ͻṹ���쳣����������Դ����
	__try
	{
		//���������ڴ�
		ParameterAddress = VirtualAllocEx(hProcess,NULL,MAX_PATH+1,MEM_COMMIT,PAGE_READWRITE);
		if (!ParameterAddress)
		{
			__leave;
		}
		
		//д��ע���dll·��
		if(!WriteProcessMemory(hProcess,ParameterAddress,szInjectDllPath,MAX_PATH+1,&dwWirite))
		{
			__leave;
		}
		
		//���Һ�����ַ
		lpLibraryAddress = GetProcAddress(GetModuleHandle("Kernel32.dll"),"LoadLibraryA");
		if (!lpLibraryAddress)
		{
			__leave;
		}
		
		//����Զ���߳�ע��
		hRemote = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)lpLibraryAddress,ParameterAddress,0,NULL);
		if(!hRemote)
		{
			__leave;
		}
		//�ȴ�Զ���̷߳���
		WaitForSingleObject(hRemote,INFINITE);
		bIsSuccess = TRUE;
	}
	__finally
	{
		//�ر�Զ���߳̾��
		if(hRemote)
		{
			CloseHandle(hRemote);	
		}
		//�ͷ��ڴ�
		VirtualFreeEx(hProcess,ParameterAddress,0,MEM_RELEASE);
	}
	return bIsSuccess;
}

void CtestclientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	this->m_udp.SendToUserQuitInfo();
	Sleep(100);
	this->m_udp.UnInitial();
}

void CtestclientDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnClose();
}
