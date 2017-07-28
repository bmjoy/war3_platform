#include "UDPNet.h"

CUDPNet::CUDPNet(void)
{
	this->m_udpsocket = NULL;
	this->m_tcpsocket = NULL;
	this->m_comioport = NULL;
	ZeroMemory(this->m_VirtualIP,MAX_VIRTUAL_IP_NUM);
	ZeroMemory(this->m_RoomNum,MAX_CREATE_ROOM_NUM);
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_connnectlock);
}

CUDPNet::~CUDPNet(void)
{
	DeleteAllPersonInfo();
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_connnectlock);
}

BOOL CUDPNet::InitialUDP()
{
	//1.���ؿ�
	WORD wVersionRequested;
    WSADATA wsaData;

	wVersionRequested = MAKEWORD(2,2);
	WSAStartup(wVersionRequested,&wsaData);

	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		return false;
	}

	//����socket
	m_udpsocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(m_udpsocket == INVALID_SOCKET)
	{
		UnInitial();
		return FALSE;
	}

	//��bind
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERVER_PORT);

	if(SOCKET_ERROR == bind(m_udpsocket,(const sockaddr *)&addr,sizeof(sockaddr_in)))
	{
		UnInitial();
		return false;
	}

	//����socket����
	bool optval = true;
	setsockopt(m_udpsocket,SOL_SOCKET,SO_BROADCAST,(const char *)&optval,sizeof(bool));

	//���ϵͳ�ĺ���
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	//������ɶ˿�
	this->m_comioport = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,0);
	if(m_comioport == NULL)
	{
		UnInitial();
		return FALSE;
	}

	//��socket������ɶ˿ڹ���
	CreateIoCompletionPort((HANDLE)m_udpsocket,m_comioport,m_udpsocket,0);

	//��ʼ��MYSOCKETEX
	PostRecvfrom(m_udpsocket);

	//���������߳�
	for(DWORD i=0;i<si.dwNumberOfProcessors * 2;i++)
	{
		HANDLE hthread = (HANDLE)_beginthreadex(NULL,0,&ThreadComIOPortWork,this,0,NULL);
		if(hthread)
		{
			this->m_lstthreadhandle.push_back(hthread);
		}
	}

	CreateTCPInitial();

	cout<<"��������ʼ���ɹ�"<<endl;

	return TRUE;
}

BOOL CUDPNet::CreateTCPInitial()
{
	//����socket
	this->m_tcpsocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(this->m_tcpsocket == INVALID_SOCKET)
	{
		return FALSE;
	}
	//bind
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_GAME_PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if(SOCKET_ERROR == bind(m_tcpsocket,(PSOCKADDR)&addr,sizeof(sockaddr_in)))
	{
		closesocket(m_tcpsocket);
		return FALSE;
	}
	//listen
	if(SOCKET_ERROR == listen(m_tcpsocket,SOMAXCONN))
	{
		closesocket(m_tcpsocket);
		return FALSE;
	}
	//������ɶ˿ڹ���
	CreateIoCompletionPort((HANDLE)m_tcpsocket,m_comioport,m_tcpsocket,0);
	//����
	for(int i=0;i<4;i++)
	{
		PostAccept();
	}

	return TRUE;
}

BOOL CUDPNet::PostRecvfrom(SOCKET sock)
{
	MYOVERINFO *pmyoverinfo = this->m_myoverlapped.NewOverInfoAndAddList();
	ZeroMemory(pmyoverinfo,sizeof(MYOVERINFO));
	pmyoverinfo->type = NT_RECVFROM;
	pmyoverinfo->wv.hEvent = WSACreateEvent();
	pmyoverinfo->wb.buf = pmyoverinfo->szbuf;
	pmyoverinfo->wb.len = sizeof(pmyoverinfo->szbuf);
	pmyoverinfo->sockaddrfromlen = sizeof(sockaddr_in)+16;

	DWORD dwNumberOfBytesRecvd;
	DWORD dwFlags = 0;

	if(WSARecvFrom(sock,&pmyoverinfo->wb,1,&dwNumberOfBytesRecvd,&dwFlags,(sockaddr *)&pmyoverinfo->sockaddrfrom,&pmyoverinfo->sockaddrfromlen,&pmyoverinfo->wv,NULL))
	{
		int error = WSAGetLastError();
		if(error != WSA_IO_PENDING)
		{
			return FALSE;
		}
	}

	return TRUE;
}

unsigned __stdcall CUDPNet::ThreadComIOPortWork( void * lpvoid)
{
	CUDPNet * pthis = (CUDPNet *)lpvoid;
	BOOL result;
	DWORD dwNumberOfBytes;
	ULONG_PTR CompletionKey;
	MYOVERINFO * pmyoverinfo;

	while(TRUE)
	{
		result = GetQueuedCompletionStatus(pthis->m_comioport,&dwNumberOfBytes,(PULONG_PTR)&CompletionKey,(LPOVERLAPPED*)&pmyoverinfo,INFINITE);
		if(result == TRUE)
		{
			if(pmyoverinfo && CompletionKey)
			{
				switch(pmyoverinfo->type)
				{
				case NT_RECVFROM:
					{
						pthis->PostRecvfrom(pthis->m_udpsocket);
						pthis->DealUDPData(pmyoverinfo);
						pthis->m_myoverlapped.DelOverInfoFromList(pmyoverinfo);
					}
					break;
				case NT_SENDTO:
					{
						pthis->m_myoverlapped.DelOverInfoFromList(pmyoverinfo);
					}
					break;
				case NT_ACCEPT:
					{
						pthis->PostAccept();
						pthis->DealGameTcpConnect((MYOVERGAMEINFO *)pmyoverinfo);
						pthis->PostRecv(((MYOVERGAMEINFO *)pmyoverinfo)->sock);
						pthis->m_myoverlapped.DelOverGameInfoFromList((MYOVERGAMEINFO *)pmyoverinfo);
					}
					break;
				case NT_RECV:
					{
						pthis->PostRecv(((MYOVERGAMEINFO *)pmyoverinfo)->sock);
						pthis->DealGameTcpRecv((ASSOCIATE_SOCKET *)CompletionKey,(MYOVERGAMEINFO *)pmyoverinfo,dwNumberOfBytes);
						pthis->m_myoverlapped.DelOverGameInfoFromList((MYOVERGAMEINFO *)pmyoverinfo);
					}
					break;
				case NT_SEND:
					{
						pthis->m_myoverlapped.DelOverGameInfoFromList((MYOVERGAMEINFO *)pmyoverinfo);
					}
					break;
				default:
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	return 0;
}

void CUDPNet::DealUDPData(MYOVERINFO *pmyoverinfo)
{
	TRANSMIT_PROTOCOL_METHOD * method = (TRANSMIT_PROTOCOL_METHOD *)pmyoverinfo->szbuf;

	switch(method->Protocol)
	{
	case TRANSMIT_PROTOCOL_GAMEUDP:
		{
			TRANSMIT_GAME_UDP_PACKET *tgup = (TRANSMIT_GAME_UDP_PACKET *)pmyoverinfo->szbuf;
			AnalysisGameUdpData(pmyoverinfo,(WAR3_UDP_HEAD_PACKET *)&tgup->vBody);
		}
		break;
	case TRANSMIT_PROTOCOL_USERLOGIN:
		{
			AnalysisUserLoginData(pmyoverinfo);
		}
		break;
	case TRANSMIT_PROTOCOL_USERQUIT:
		{
			AnalysisUserQuitData(pmyoverinfo);
		}
		break;
	case TRANSMIT_PROTOCOL_CONNECTINFO:
		{
			AnalysisConnectData(pmyoverinfo);
		}
		break;
	default:
		break;
	}
}

BOOL CUDPNet::PostSendto(SOCKET sock,char *szbuf,int buflen,sockaddr_in *sendto)
{
	MYOVERINFO *pmyoverinfo = this->m_myoverlapped.NewOverInfoAndAddList();
	ZeroMemory(pmyoverinfo,sizeof(MYOVERINFO));
	DWORD dwNumberOfBytesSent;
	DWORD dwFlags = 0;
	
	pmyoverinfo->type = NT_SENDTO;
	pmyoverinfo->wv.hEvent = WSACreateEvent();
	pmyoverinfo->wb.buf = szbuf;
	pmyoverinfo->wb.len = buflen;

	WSASendTo(sock,&pmyoverinfo->wb,1,&dwNumberOfBytesSent,dwFlags,(sockaddr *)sendto,sizeof(sockaddr_in),(LPWSAOVERLAPPED)&pmyoverinfo->wv,NULL);

	return TRUE;
}

void CUDPNet::UnInitial()
{
	//������ɶ˿��˳���Ϣ
	int num = this->m_lstthreadhandle.size();
	while(num)
	{
		PostQueuedCompletionStatus(this->m_comioport,NULL,NULL,NULL);
		num--;
	}

	//�ر��߳�
	list<HANDLE>::iterator ite = this->m_lstthreadhandle.begin();
	while(ite != this->m_lstthreadhandle.end())
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(*ite,20))
		{
			TerminateThread(*ite,-1);
		}
		CloseHandle(*ite);
		*ite = NULL;
		++ite;
	}

	if(m_udpsocket)
	{
		closesocket(m_udpsocket);
		m_udpsocket = NULL;
	}

	WSACleanup();
}

void CUDPNet::DeleteAllPersonInfo()
{
	list<PERSON_INFO *>::iterator itePerson = this->m_lstPersonInfo.begin();
	while(itePerson != this->m_lstPersonInfo.end())
	{
		delete *itePerson;
		*itePerson = NULL;
		++itePerson;
	}
}

void CUDPNet::AnalysisGameUdpData(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf)
{
	int type = szbuf->Operation;

	switch (type)
	{
	case UDP_PACKET_QUERY_HOST:
		{
			AnalysisGameQueryData(pmyoverinfo,szbuf);
		}
		break;
	case UDP_PACKET_ECHO_HOST:
		{
			AnalysisGameMapInfo(pmyoverinfo,szbuf);
		}
		break;
	case UDP_PACKET_CREATE_HOST:
		{
			AnalysisGameCreateRoom(pmyoverinfo,szbuf);
		}
		break;
	case UDP_PACKET_PEOPLECHANGE:
		{
			AnalysisGamePeopleChange(pmyoverinfo,szbuf);
		}
		break;
	case UDP_PACKET_CANCELHOST:
		{
			AnalysisGameCancleHost(pmyoverinfo,szbuf);
		}
		break;
	default:
		break;
	}
}

void CUDPNet::AnalysisUserLoginData(MYOVERINFO *pmyoverinfo)
{
	TRANSMIT_USER_LOGIN *tul = (TRANSMIT_USER_LOGIN *)pmyoverinfo->szbuf;
	PERSON_INFO *pi = new PERSON_INFO;
	ZeroMemory(pi,sizeof(PERSON_INFO));
	CopyMemory(pi->szName,tul->szUserName,MAX_USER_NAME);
	pi->RealIPAddress.S_un.S_addr = pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr;
	pi->PersonStates = PS_INITIAL;

	cout<<"�û�"<<tul->szUserName<<"��½"<<endl;

	EnterCriticalSection(&m_cs);
	DistributeVirtualIPAddress(pi);
	LeaveCriticalSection(&m_cs);

	SendToClientVirtualIP(pi);

	EnterCriticalSection(&m_cs);
	this->m_lstPersonInfo.push_back(pi);
	LeaveCriticalSection(&m_cs);
}

void CUDPNet::DistributeVirtualIPAddress(PERSON_INFO *pi)
{
	CHAR szVirtualIPAddr[MAX_IP_STR_LENGTH] = {0};
	for(int i=0;i<MAX_VIRTUAL_IP_NUM;i++)
	{
		if(this->m_VirtualIP[i] == VI_NOUSING)
		{
			sprintf_s(szVirtualIPAddr,MAX_IP_STR_LENGTH,"100.100.10.%d",10+i);
			pi->VirtualIPAddress.S_un.S_addr = inet_addr(szVirtualIPAddr);
			pi->VirIpIndex = i;
			cout<<"��"<<pi->szName<<"���������IPΪ"<<szVirtualIPAddr<<endl;
			this->m_VirtualIP[i] = VI_UESD;
			break;
		}
	}
}

BOOL CUDPNet::SendToClientVirtualIP(PERSON_INFO *pi)
{
	BOOL bIsSend = FALSE;
	TRANSMIT_VIRTUAL_IP tvi;
	tvi.Type = TRANSMIT_PROTOCOL_VIRTUALIPADDR;
	tvi.VirtualIpAddr.S_un.S_addr = pi->VirtualIPAddress.S_un.S_addr;
	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons(CLIENT_PORT);
	to.sin_addr.S_un.S_addr = pi->RealIPAddress.S_un.S_addr;
	bIsSend = PostSendto(this->m_udpsocket,(PCHAR)&tvi,sizeof(TRANSMIT_VIRTUAL_IP),&to);
	cout<<"��"<<pi->szName<<"��������IP��Ϣ"<<endl;
	return bIsSend;
}

void CUDPNet::AnalysisUserQuitData(MYOVERINFO *pmyoverinfo)
{
	TRANSMIT_USER_QUIT *tuq = (TRANSMIT_USER_QUIT *)pmyoverinfo->szbuf;

	//�Ƿ����������������
	list<PERSON_INFO *>::iterator iteHost = this->m_lstHostRoom.begin();
	while(iteHost != this->m_lstHostRoom.end())
	{
		if((*iteHost)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
		{
			EnterCriticalSection(&m_cs);
			this->m_lstHostRoom.erase(iteHost);
			LeaveCriticalSection(&m_cs);
			break;
		}
		++iteHost;
	}

	//�����������������
	list<PERSON_INFO *>::iterator itePerson = this->m_lstPersonInfo.begin();
	while(itePerson != this->m_lstPersonInfo.end())
	{
		if((*itePerson)->VirtualIPAddress.S_un.S_addr == tuq->VirtualIpAddr.S_un.S_addr)
		{
			if((*itePerson)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
			{
				//��������IP
				this->m_VirtualIP[(*itePerson)->VirIpIndex] = VI_NOUSING;
				delete *itePerson;
				*itePerson = NULL;
				EnterCriticalSection(&m_cs);
				this->m_lstPersonInfo.erase(itePerson);
				LeaveCriticalSection(&m_cs);
				break;
			}
		}
		++itePerson;
	}
	cout<<inet_ntoa(tuq->VirtualIpAddr)<<"�˳���"<<endl;
}

void CUDPNet::AnalysisGameQueryData(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf)
{
	WAR3_UDP_QUERY_PACKET *wuqp = (WAR3_UDP_QUERY_PACKET *)szbuf;
	//����ǰ����������ɾ������
	list<PERSON_INFO *>::iterator iteHostBefore = this->m_lstHostRoom.begin();
	while(iteHostBefore != this->m_lstHostRoom.end())
	{
		if((*iteHostBefore)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
		{
			EnterCriticalSection(&m_cs);
			this->m_lstHostRoom.erase(iteHostBefore);
			LeaveCriticalSection(&m_cs);
			break;
		}		
		++iteHostBefore;
	}
	//�������з������Ϣ
	list<PERSON_INFO *>::iterator iteHostRoom = this->m_lstHostRoom.begin();
	while(iteHostRoom != this->m_lstHostRoom.end())
	{
		if((*iteHostRoom)->dwGameVersion == wuqp->GameVersion)
		{
			SendToClientAllRoomInfo(*iteHostRoom,pmyoverinfo);
			cout<<"����"<<(*iteHostRoom)->RoomId<<"�ŷ����ͼ��"<<inet_ntoa(pmyoverinfo->sockaddrfrom.sin_addr)<<endl;
		}
		++iteHostRoom;
	}
	//�����ѯ��
	list<PERSON_INFO *>::iterator itePerson = this->m_lstPersonInfo.begin();
	while(itePerson != this->m_lstPersonInfo.end())
	{
		if((*itePerson)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
		{
			(*itePerson)->dwGameVersion = wuqp->GameVersion;
			CopyMemory((*itePerson)->QueryPacket,wuqp,sizeof(GAME_UDP_QUERY_SIZE));
			(*itePerson)->PersonStates = PS_SELECTROOM;
			break;
		}
		++itePerson;
	}

	cout<<(*itePerson)->szName<<"������ѯ���з�����Ϣ"<<endl;
}

BOOL CUDPNet::SendToClientAllRoomInfo(PERSON_INFO *pi,MYOVERINFO *pmyoverinfo)
{
	BOOL bIsSend = FALSE;
	TRANSMIT_GAMEHOUSE_MAPINFO *tgm = (TRANSMIT_GAMEHOUSE_MAPINFO *)new BYTE[sizeof(TRANSMIT_GAMEHOUSE_MAPINFO) - 4 + pi->dwMapLen];
	tgm->Type = TRANSMIT_PROTOCOL_HOUSEMAPINFO;
	tgm->HostVirtualIp.S_un.S_addr = pi->VirtualIPAddress.S_un.S_addr;
	tgm->uHouseId = pi->RoomId;
	tgm->uMapLen = pi->dwMapLen;
	CopyMemory(&tgm->lpszMapInfo,pi->MapPacket,pi->dwMapLen);
	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons(CLIENT_PORT);
	to.sin_addr.S_un.S_addr = pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr;
	bIsSend = PostSendto(m_udpsocket,(PCHAR)tgm,sizeof(TRANSMIT_GAMEHOUSE_MAPINFO) - 4 + pi->dwMapLen,&to);
	Sleep(10);
	delete[] tgm;
	return bIsSend;
}

void CUDPNet::AnalysisGameCreateRoom(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf)
{
	WAR3_UDP_CREATEHOST_PACKET *wucp = (WAR3_UDP_CREATEHOST_PACKET *)szbuf;
	//��������
	list<PERSON_INFO *>::iterator itePerson = this->m_lstPersonInfo.begin();
	while(itePerson != this->m_lstPersonInfo.end())
	{
		if((*itePerson)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
		{
			(*itePerson)->dwGameVersion = wucp->GameVersion;
			//���䷿���
			DistributeGameRoomNum(*itePerson);
			(*itePerson)->PersonStates = PS_GAMING;
			PERSON_INFO * pi = *itePerson;

			EnterCriticalSection(&m_cs);
			this->m_lstHostRoom.push_back(pi);
			LeaveCriticalSection(&m_cs);

			cout<<(*itePerson)->szName<<"������"<<pi->RoomId<<"�ŷ���"<<endl;
			break;
		}
		++itePerson;
	}
}

void CUDPNet::DistributeGameRoomNum(PERSON_INFO *pi)
{
	for(int i=0;i<MAX_CREATE_ROOM_NUM;i++)
	{
		if(this->m_RoomNum[i] == RM_NOUSING)
		{
			pi->RoomId = i+1;
			cout<<"��"<<pi->szName<<"����ķ����Ϊ"<<pi->RoomId<<endl;
			this->m_RoomNum[i] = VI_UESD;
			break;
		}
	}
}

void CUDPNet::AnalysisGameMapInfo(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf)
{
	PERSON_INFO *pi = NULL;

	//��������������
	list<PERSON_INFO *>::iterator iteHostRoom = this->m_lstHostRoom.begin();
	while(iteHostRoom != this->m_lstHostRoom.end())
	{
		if((*iteHostRoom)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
		{
			pi = (*iteHostRoom);
			break;
		}
		++iteHostRoom;
	}

	//û�ҵ� ��������������
	if(pi == NULL)
	{
		list<PERSON_INFO *>::iterator itePerson = this->m_lstPersonInfo.begin();
		while(itePerson != this->m_lstPersonInfo.end())
		{
			if((*itePerson)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
			{
				pi = *itePerson;
				break;
			}
			++itePerson;
		}
	}

	//�޸ĵ�ͼ���ķ����Ϊ�Զ��巿���
	WAR3_UDP_ECHO_PACKET *wuep = (WAR3_UDP_ECHO_PACKET *)szbuf;
	wuep->GameID = pi->RoomId;
	pi->dwMapLen = szbuf->PacketLen;
	CopyMemory(pi->MapPacket,szbuf,szbuf->PacketLen);
	cout<<"����"<<pi->szName<<"�����ĵ�ͼ��Ϣ"<<endl;
}

void CUDPNet::AnalysisGamePeopleChange(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf)
{
	list<PERSON_INFO *>::iterator iteHost = this->m_lstHostRoom.begin();
	while(iteHost != this->m_lstHostRoom.end())
	{
		//�ҵ����ĸ�����
		if((*iteHost)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
		{
			//�޸İ��еķ����
			WAR3_UDP_PEOPLECHANGE_PACKET *wup = (WAR3_UDP_PEOPLECHANGE_PACKET *)szbuf;
			wup->GameID = (*iteHost)->RoomId;

			//���͸��ͻ���
			list<PERSON_INFO *>::iterator itePerson = this->m_lstPersonInfo.begin();
			while(itePerson != this->m_lstPersonInfo.end())
			{
				if((*itePerson)->PersonStates == PS_SELECTROOM)
				{
					SendToClientPeopleChange(*itePerson,szbuf);
					cout<<"���͸�"<<(*itePerson)->szName<<"һ�����������޸���Ϣ"<<endl;
				}
				++itePerson;
			}

			//�˳�
			break;
		}
		++iteHost;
	}
}

BOOL CUDPNet::SendToClientPeopleChange(PERSON_INFO *pi,WAR3_UDP_HEAD_PACKET *szbuf)
{
	BOOL bIsSend = FALSE;
	TRANSMIT_GAMEHOUSE_PEOPLECHANGE tgp;
	tgp.Type = TRANSMIT_PROTOCOL_PEOPLECHANGE;
	CopyMemory(&tgp.War3ChangePeople,(LPVOID)szbuf,szbuf->PacketLen);
	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons(CLIENT_PORT);
	to.sin_addr.S_un.S_addr = pi->RealIPAddress.S_un.S_addr;
	bIsSend = PostSendto(m_udpsocket,(PCHAR)&tgp,sizeof(TRANSMIT_GAMEHOUSE_PEOPLECHANGE),&to);
	Sleep(10);
	return bIsSend;
}

void CUDPNet::AnalysisGameCancleHost(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf)
{
	list<PERSON_INFO *>::iterator iteHost = this->m_lstHostRoom.begin();
	while(iteHost != this->m_lstHostRoom.end())
	{
		if((*iteHost)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
		{
			//�޸ķ����
			WAR3_UDP_CANCELHOST_PACKET *wucp = (WAR3_UDP_CANCELHOST_PACKET *)szbuf;
			wucp->GameID = (*iteHost)->RoomId;

			//����ҷ�����Ϸȡ����
			list<PERSON_INFO *>::iterator itePerson = this->m_lstPersonInfo.begin();
			while(itePerson != this->m_lstPersonInfo.end())
			{
				if((*itePerson)->PersonStates == PS_SELECTROOM)
				{
					SendToClientCancleHost(*itePerson,szbuf);
					cout<<"���͸�"<<(*itePerson)->szName<<"һ��ȡ��������Ϣ"<<endl;
				}
				++itePerson;
			}

			//���շ����
			this->m_RoomNum[(*iteHost)->RoomId - 1] = RM_NOUSING;
			cout<<"ȡ��"<<(*iteHost)->RoomId<<"�ŷ���"<<endl;

			//ɾ�������пͻ��˵Ľڵ�
			for(int i=0;i<MAX_CONNECT_NUM;i++)
			{
				if((*iteHost)->lst_client[i] != NULL)
				{
					delete (*iteHost)->lst_client[i];
					(*iteHost)->lst_client[i] = NULL;
				}
			}
			(*iteHost)->ClientIndex = 0;

			//�������б������
			EnterCriticalSection(&m_cs);
			this->m_lstHostRoom.erase(iteHost);
			LeaveCriticalSection(&m_cs);
			break;
		}
		++iteHost;
	}
}

BOOL CUDPNet::SendToClientCancleHost(PERSON_INFO *pi,WAR3_UDP_HEAD_PACKET *szbuf)
{
	BOOL bIsSend = FALSE;
	TRANSMIT_GAMEHOUSE_CANCLE tgc;
	tgc.Type = TRANSMIT_PROTOCOL_HOUSECANCEL;
	CopyMemory(&tgc.War3CancleHouse,(LPVOID)szbuf,szbuf->PacketLen);
	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_port = htons(CLIENT_PORT);
	to.sin_addr.S_un.S_addr = pi->RealIPAddress.S_un.S_addr;
	bIsSend = PostSendto(m_udpsocket,(PCHAR)&tgc,sizeof(TRANSMIT_GAMEHOUSE_CANCLE),&to);
	Sleep(10);
	return bIsSend;
}

void CUDPNet::AnalysisConnectData(MYOVERINFO *pmyoverinfo)
{
	TRANSMIT_CLIENT_CONNECT_HOST *tcch = (TRANSMIT_CLIENT_CONNECT_HOST *)pmyoverinfo->szbuf;

	//��ѯ��������
	PERSON_INFO * host = NULL;
	list<PERSON_INFO *>::iterator ite = this->m_lstHostRoom.begin();
	while(ite != this->m_lstHostRoom.end())
	{
		//�ҵ���Ҫ���ӵ�����
		if((*ite)->VirtualIPAddress.S_un.S_addr == tcch->HostVirtualIpAddr.S_un.S_addr)
		{
			//�����Ӱ��ȷ���������
			SendToClientHostConnectInfo(*ite,tcch->ClientVirtualIpAddr);

			//��¼������Ϣ
			host = *ite;
			break;
		}
		++ite;
	}

	//�������ͻ������������
	ASSOCIATEIPADDR * aia = new ASSOCIATEIPADDR;
	aia->RealIpAddr.S_un.S_addr = pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr;
	aia->VirtualIpAddr.S_un.S_addr = tcch->ClientVirtualIpAddr.S_un.S_addr;
	host->lst_client[host->ClientIndex] = aia;
	host->ClientIndex++;

	//�޸Ŀͻ�����״̬
	list<PERSON_INFO *>::iterator itePerson = this->m_lstPersonInfo.begin();
	while(itePerson != this->m_lstPersonInfo.end())
	{
		if((*itePerson)->RealIPAddress.S_un.S_addr == pmyoverinfo->sockaddrfrom.sin_addr.S_un.S_addr)
		{
			(*itePerson)->PersonStates = PS_GAMING;
			break;
		}
		++itePerson;
	}
}

BOOL CUDPNet::SendToClientHostConnectInfo(PERSON_INFO *pi,in_addr ClientVirtualIp)
{
	BOOL bIsSend;
	TRANSMIT_CLIENT_INFO tci;
	tci.Type = TRANSMIT_PROTOCOL_CLIENTINFO;
	tci.ClientVirtualIp.S_un.S_addr = ClientVirtualIp.S_un.S_addr;
	sockaddr_in host;
	host.sin_family = AF_INET;
	host.sin_port = htons(CLIENT_PORT);
	host.sin_addr.S_un.S_addr = pi->RealIPAddress.S_un.S_addr;
	bIsSend = PostSendto(this->m_udpsocket,(PCHAR)&tci,sizeof(TRANSMIT_CLIENT_INFO),&host);
	return bIsSend;
}

BOOL CUDPNet::PostAccept()
{
	DWORD dwBytesReceived;
	SOCKET sock = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
	MYOVERGAMEINFO * mygameinfo = this->m_myoverlapped.NewOverGameInfoAndAddList();
	mygameinfo->type = NT_ACCEPT;
	mygameinfo->wv.hEvent = WSACreateEvent();
	mygameinfo->sock = sock;

	if(!AcceptEx(this->m_tcpsocket,sock,mygameinfo->szbuf,0,sizeof(sockaddr_in)+16,sizeof(sockaddr_in)+16,&dwBytesReceived,&mygameinfo->wv))
	{
		if(WSAGetLastError() != ERROR_IO_PENDING)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CUDPNet::PostRecv(SOCKET sock)
{
	DWORD dwFlags = false;
	MYOVERGAMEINFO *pmygameinfo = this->m_myoverlapped.NewOverGameInfoAndAddList();
	ZeroMemory(pmygameinfo,sizeof(MYOVERGAMEINFO));
	pmygameinfo->type = NT_RECV;
	pmygameinfo->wv.hEvent = WSACreateEvent();
	pmygameinfo->wb.buf = pmygameinfo->szbuf;
	pmygameinfo->wb.len = sizeof(pmygameinfo->szbuf);
	pmygameinfo->sock = sock;

	if(WSARecv(sock,&pmygameinfo->wb,1,&pmygameinfo->dwRecv,&dwFlags,&pmygameinfo->wv,NULL))
	{
		if(WSAGetLastError() != ERROR_IO_PENDING)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CUDPNet::PostSend(SOCKET sock,char *szbuf,int buflen)
{
	DWORD dwNumberOfBytesSend;
	MYOVERGAMEINFO * pmygameinfo = this->m_myoverlapped.NewOverGameInfoAndAddList();
	pmygameinfo->type = NT_SEND;
	pmygameinfo->wb.buf = szbuf;
	pmygameinfo->wb.len = buflen;
	pmygameinfo->wv.hEvent = WSACreateEvent();

	if(SOCKET_ERROR == WSASend(sock,&pmygameinfo->wb,1,&dwNumberOfBytesSend,0,&pmygameinfo->wv,NULL))
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CUDPNet::DealGameTcpConnect(MYOVERGAMEINFO *pmyoverinfo)
{
	//������ӵĵ�ַ
	sockaddr_in * LocalAddr = NULL;
	sockaddr_in * RemoteAddr = NULL;
	int Addrsize1,Addrsize2;
	GetAcceptExSockaddrs(pmyoverinfo->szbuf,0,sizeof(sockaddr_in)+16,sizeof(sockaddr_in)+16,(sockaddr **)&LocalAddr,&Addrsize1,(sockaddr **)&RemoteAddr,&Addrsize2);

	//��ӵ�������
	ASSOCIATE_SOCKET * as = new ASSOCIATE_SOCKET;
	ZeroMemory(as,sizeof(ASSOCIATE_SOCKET));
	as->sock1 = pmyoverinfo->sock;
	as->RealAddr.S_un.S_addr = RemoteAddr->sin_addr.S_un.S_addr;
	this->m_GameSock.push_back(as);

	//������ɶ˿ڹ���
	CreateIoCompletionPort((HANDLE)as->sock1,this->m_comioport,(ULONG_PTR)as,0);
}

void CUDPNet::DealGameTcpRecv(ASSOCIATE_SOCKET * assosock,MYOVERGAMEINFO *pmyoverinfo,DWORD dwNumberOfRecv)
{
	if(assosock->bIsGaming == FALSE)
	{
		//�ҵ��ͻ���
		TRANSMIT_GAME_INFO *tgi = (TRANSMIT_GAME_INFO *)pmyoverinfo->szbuf;
		if(tgi->Type != TRANSMIT_PROTOCOL_GAMEINFO)
		{
			TEMP_STORE_GAME_PACKET * tsgp = new TEMP_STORE_GAME_PACKET;
			tsgp->dwNumOfPacket = dwNumberOfRecv;
			CopyMemory(tsgp->szbuf,pmyoverinfo->szbuf,dwNumberOfRecv);
			return ;
		}
		PERSON_INFO * person = NULL;
		list<PERSON_INFO *>::iterator itePerson = this->m_lstPersonInfo.begin();
		while(itePerson != this->m_lstPersonInfo.end())
		{
			if((*itePerson)->VirtualIPAddress.S_un.S_addr == tgi->ClientAddr.S_un.S_addr)
			{
				person = *itePerson;
				break;
			}
			++itePerson;
		}

		//�ҵ���Ӧ��asso
		ASSOCIATE_SOCKET * client = NULL;
		ASSOCIATE_SOCKET * host = NULL;
		list<ASSOCIATE_SOCKET *>::iterator iteSock = this->m_GameSock.begin();
		while(iteSock != this->m_GameSock.end())
		{
			if((*iteSock)->RealAddr.S_un.S_addr == person->RealIPAddress.S_un.S_addr)
			{
				client = *iteSock;
				break;
			}
			++iteSock;
		}

		//����
		client->sock2 = assosock->sock1;
		assosock->sock2 = client->sock1;
		client->bIsGaming = TRUE;
		assosock->bIsGaming = TRUE;
		
		if(this->m_TempGamePack.empty() == false)
		{
			list<TEMP_STORE_GAME_PACKET *>::iterator ite = this->m_TempGamePack.begin();
			while(ite != this->m_TempGamePack.end())
			{
				PostSend(assosock->sock1,(PCHAR)(*ite)->szbuf,(*ite)->dwNumOfPacket);
				delete *ite;
				*ite = NULL;
				++ite;
			}
			this->m_TempGamePack.clear();
		}
	}
	else
	{
		PostSend(assosock->sock2,pmyoverinfo->szbuf,dwNumberOfRecv);
	}

}