#pragma once
#include <list>
#include "GlobalInformation.h"
using namespace std;

class CMyOverlapped
{
public:
	CMyOverlapped(void);
	~CMyOverlapped(void);
public:
	list<MYOVERINFO *> m_lstUsingOverInfo;
	list<MYOVERINFO *> m_lstUsedOverInfo;
	CRITICAL_SECTION m_cs;
public:
	//����һ��MYOVERINFO ����ӵ�������
	MYOVERINFO * NewOverInfoAndAddList();
	//��������ɾ��ָ����OVERINFO
	void DelOverInfoFromList(MYOVERINFO * pDel);
	//ɾ�������е�ȫ��
	void DeleteAllInList();
public:
	list<MYOVERGAMEINFO *> m_lstUsingGameOverInfo;
	list<MYOVERGAMEINFO *> m_lstUsedGameOverInfo;
	CRITICAL_SECTION m_Gamecs;
public:
	//����һ��MYOVERINFO ����ӵ�������
	MYOVERGAMEINFO * NewOverGameInfoAndAddList();
	//��������ɾ��ָ����OVERINFO
	void DelOverGameInfoFromList(MYOVERGAMEINFO * pDel);
};

