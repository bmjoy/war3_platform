// testclientDlg.h : ͷ�ļ�
//
#pragma once

#include "UDPNet.h"
#include "GlobalInformation.h"

// CtestclientDlg �Ի���
class CtestclientDlg : public CDialogEx
{
// ����
public:
	CtestclientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDestroy();
//Զ��ע��
public:
	BOOL RemoteThreadInjectModu(TCHAR * szInjectDllPath,HANDLE hProcess);
	HANDLE m_GameProcess;
//����
public:
	CUDPNet m_udp;
	
	afx_msg void OnClose();
};
