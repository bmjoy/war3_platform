#include "InlineHook.h"
#include <iostream>
using namespace std;

#define NAKED __declspec(naked)
//������ַ�ṹ
HOOKADDRINFO ConnectAddr;
HOOKADDRINFO SendAddr;
HOOKADDRINFO SendtoAddr;
HOOKADDRINFO WSARecvFromAddr;
HOOKADDRINFO AcceptExAddr;
HOOKADDRINFO GetPeernameAddr;
HOOKADDRINFO GetsocknameAddr;
HOOKADDRINFO WSARecvAddr;
HOOKADDRINFO GetAcceptExSockaddrsAddr;
//�����socket
SOCKET udpsocket;
//�ں˶���
HANDLE hQueryEvent;
HANDLE hContinueEvent;
HANDLE hMap;
ASSOCIATE_VIRTUAL_IP_ADDR * pavia;
//����������ַ
in_addr LocalVirtualIpAddr;
//���������ַ
in_addr RemoteVirtualIpAddr;

//test
extern FILE *fp;

CInlineHook::CInlineHook()
{
	hQueryEvent = NULL;
	hContinueEvent = NULL;
	hMap = NULL;
	pavia = NULL;
	udpsocket = NULL;
	ZeroMemory(&ConnectAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&SendtoAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&AcceptExAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&SendAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&WSARecvFromAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&GetPeernameAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&GetsocknameAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&WSARecvAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&GetAcceptExSockaddrsAddr,sizeof(HOOKADDRINFO));
	//�����ں˶���
	LoadEventAndMapFile();
	//��ʼ��HOOK��
	InitializeHookPoint();
	//��ʼHOOK ����API
	StartHook();

	fprintf(fp,"Initial Inline Hook Successful\n");
}

CInlineHook::~CInlineHook()
{
	//ȡ��HOOK
	StopHook();
}

//----------------------�����γ���ջ֡����------------------------------------------------
NAKED int WINAPI ConnectMiddle(SOCKET s, const struct sockaddr *name, int namelen)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[ConnectAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED int WINAPI SendtoMiddle(SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[SendtoAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED SOCKET WINAPI AcceptExMiddle(SOCKET sListenSocket,SOCKET sAcceptSocket,PVOID lpOutputBuffer,DWORD dwReceiveDataLength,DWORD dwLocalAddressLength,DWORD dwRemoteAddressLength,LPDWORD lpdwBytesReceived,    LPOVERLAPPED lpOverlapped  )
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[AcceptExAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED int WINAPI sendMiddle(SOCKET s,const char FAR *buf,int len,int flags)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[SendAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}	
}

NAKED int WINAPI WSARecvFromMiddle(SOCKET s,LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,struct sockaddr FAR *lpFrom,LPINT lpFromlen,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[WSARecvFromAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}		
}

NAKED int WINAPI getpeernameMiddle(SOCKET s,struct sockaddr FAR *name,int FAR *namelen)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[GetPeernameAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED int WINAPI getsocknameMiddle(SOCKET s,struct sockaddr FAR *name,int FAR *namelen)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[GetsocknameAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED int WINAPI WSARecvMiddle(SOCKET s, LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[WSARecvAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED VOID WINAPI GetAcceptExSockaddrsMiddle(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength, struct sockaddr **LocalSockaddr, LPINT LocalSockaddrLength, struct sockaddr **RemoteSockaddr, LPINT RemoteSockaddrLength)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[GetAcceptExSockaddrsAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

//--------------------------------------------------------------------------����ȫ������

BOOL CInlineHook::LoadEventAndMapFile()
{
	hQueryEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,MY_QUERY_EVENT_NAME);
	hContinueEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,MY_CONTINUE_ENENT_NAME);
	hMap = OpenFileMapping(FILE_MAP_READ,FALSE,MY_MAPFILE_NAME);
	pavia = (ASSOCIATE_VIRTUAL_IP_ADDR *)MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
	return TRUE;
}

//��ʼ��HOOK��
BOOL CInlineHook::InitializeHookPoint()
{
	ConnectAddr.OldFuncAddr = (DWORD)connect;
	ConnectAddr.NewFuncAddr = (DWORD)HookConnect;
	
	SendtoAddr.OldFuncAddr = (DWORD)sendto;
	SendtoAddr.NewFuncAddr = (DWORD)Hooksendto;

	AcceptExAddr.OldFuncAddr = (DWORD)*(PULONG) *(ULONG**)((ULONG)AcceptEx + 2);
	AcceptExAddr.NewFuncAddr = (DWORD)HookAcceptEx;

	SendAddr.OldFuncAddr = (DWORD)send;
	SendAddr.NewFuncAddr = (DWORD)Hooksend;

	WSARecvFromAddr.OldFuncAddr = (DWORD)WSARecvFrom;
	WSARecvFromAddr.NewFuncAddr = (DWORD)HookWSARecvFrom;

	//GetPeernameAddr
	GetPeernameAddr.OldFuncAddr = (DWORD)getpeername;
	GetPeernameAddr.NewFuncAddr = (DWORD)Hookgetpeername;

	GetsocknameAddr.OldFuncAddr = (DWORD)getsockname;
	GetsocknameAddr.NewFuncAddr = (DWORD)Hookgetsockname;

	WSARecvAddr.OldFuncAddr = (DWORD)WSARecv;
	WSARecvAddr.NewFuncAddr = (DWORD)HookWSARecv;

	GetAcceptExSockaddrsAddr.OldFuncAddr = (DWORD)*(PULONG) *(ULONG**)((ULONG)GetAcceptExSockaddrs + 2);
	GetAcceptExSockaddrsAddr.NewFuncAddr = (DWORD)HookGetAcceptExSockaddrs;
	return TRUE;
}

//Inline HOOK
//����1:HOOK�����ĵ�ַ��Ϣ ����2:�Ƿ���HOOK ����UNHOOK
VOID CInlineHook::InlineHook(HOOKADDRINFO * lpHookInfo,BOOL bIsHook)
{
	//ԭʼ�ڴ�ҳ����
	DWORD	OrigProtect = 0;

	//HOOKָ�� �����jmp ��ת
	UCHAR   uszHookCode[5] = {0xE9};

	//ԭʼָ��<ͨ�������������е�HOOK����ͷ����������������ֱ��ͷ��HOOK>
	UCHAR uszOrigCode[5] = {0x8B,0xFF,0x55,0x8B,0xEC};
	//���ȱ���Ҫ��������ֵ������Ч��(��Ч����ᱼ������)
	if (lpHookInfo->OldFuncAddr && lpHookInfo->NewFuncAddr)
	{
		//�޸��ڴ�ҳ����
		VirtualProtect((LPVOID)lpHookInfo->OldFuncAddr,5,PAGE_READWRITE,&OrigProtect);
		if (bIsHook)
		{
			//�����ǿ�ʼHOOK

			//����HOOK��  ��ʽ:��ת��Ŀ���ַ - HOOK ������ԭʼ��ַ - 5 == ԭʼ������ַ����һ��ָ��ĵ�ַ
			(*(ULONG*)&uszHookCode[1]) = lpHookInfo->NewFuncAddr - lpHookInfo->OldFuncAddr - 5;
			
			CopyMemory((LPVOID)lpHookInfo->OldFuncAddr,uszHookCode,5);
		}
		else
		{
			//�����ǻָ�HOOK
			//�ָ�һ��ԭ���ľͿ�����<������ܻ᲻�ȶ�����Ϊ�ָ���ʱ��һ�����ڳ������е�ʱ��>
			CopyMemory((LPVOID)lpHookInfo->OldFuncAddr,uszOrigCode,5);
		}
		lpHookInfo->bIsHook = bIsHook;
		//�ָ��ڴ�ҳ����
		VirtualProtect((LPVOID)lpHookInfo->OldFuncAddr,5,OrigProtect,&OrigProtect);

	}
}

//��ʼHOOK
BOOL CInlineHook::StartHook()
{
	InlineHook(&ConnectAddr);
	InlineHook(&SendtoAddr);
	InlineHook(&AcceptExAddr);
	InlineHook(&SendAddr);
	InlineHook(&WSARecvFromAddr);
	InlineHook(&GetPeernameAddr);
	InlineHook(&GetsocknameAddr);
	InlineHook(&WSARecvAddr);
	InlineHook(&GetAcceptExSockaddrsAddr);

	return TRUE;
}

//ֹͣHOOK
VOID CInlineHook::StopHook()
{
	InlineHook(&ConnectAddr,FALSE);
	InlineHook(&SendtoAddr,FALSE);
	InlineHook(&AcceptExAddr,FALSE);
	InlineHook(&SendAddr,FALSE);
	InlineHook(&WSARecvFromAddr,FALSE);
	InlineHook(&GetPeernameAddr,FALSE);
	InlineHook(&GetsocknameAddr,FALSE);
	InlineHook(&WSARecvAddr,FALSE);
	InlineHook(&GetAcceptExSockaddrsAddr,FALSE);
}

//�����Լ��Ĺ��˺���
int  WINAPI CInlineHook::Hooksendto(SOCKET s,const char FAR *buf,int len,int flags,const struct sockaddr FAR *to,int tolen)
{
	//������game udp socket
	udpsocket = s;
	//�޸�Ϊ��սƽ̨
	sockaddr_in localaddr = {0};
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	localaddr.sin_port = htons(CLIENT_PORT);
	fprintf(fp,"sendto local client\n");

	return SendtoMiddle(s,buf,len,flags,(PSOCKADDR)&localaddr,tolen);
}

BOOL WINAPI CInlineHook::HookAcceptEx(SOCKET sListenSocket,SOCKET sAcceptSocket,PVOID lpOutputBuffer,DWORD dwReceiveDataLength,DWORD dwLocalAddressLength,    DWORD dwRemoteAddressLength,LPDWORD lpdwBytesReceived,LPOVERLAPPED lpOverlapped  )
{
	fprintf(fp,"HookAcceptEx success\n");
	return AcceptExMiddle(sListenSocket,sAcceptSocket,lpOutputBuffer,dwReceiveDataLength,dwLocalAddressLength,dwRemoteAddressLength,lpdwBytesReceived,lpOverlapped);
}

int  WINAPI CInlineHook::HookConnect(SOCKET s, const struct sockaddr *name, int namelen)
{
	//֪ͨ�ͻ���Ҫ�����ĸ�IP����ͼ��
	SendConnectHousePacket(udpsocket,((PSOCKADDR_IN)name)->sin_addr);
	//�������������ַ
	RemoteVirtualIpAddr.S_un.S_addr = ((PSOCKADDR_IN)name)->sin_addr.S_un.S_addr;
	Sleep(100);
	//�޸�Ҫconnect��IP��ַ
	sockaddr_in VirtualIp;
	VirtualIp.sin_family = AF_INET;
	VirtualIp.sin_port = htons(CLIENT_TCP_GAME);
	VirtualIp.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	fprintf(fp,"HookConnect success\n");
	return ConnectMiddle(s,(PSOCKADDR)&VirtualIp,namelen);
}

int WINAPI CInlineHook::Hooksend(SOCKET s,const char FAR *buf,int len,int flags)
{
	fprintf(fp,"Hooksend success\n");
	return sendMiddle(s,buf,len,flags);
}

int WINAPI CInlineHook::HookWSARecvFrom(SOCKET s,LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,struct sockaddr FAR *lpFrom,LPINT lpFromlen,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine  )
{
	int res;
	//����ԭʼ�Ļ�����
	WSABUF wb;
	wb = *lpBuffers;
	//���Ļ�����Ϊ�Զ����
	UCHAR tempszbuf[MAX_MAP_SIZE] = {0};
	lpBuffers->buf = (PCHAR)tempszbuf;
	//����
	res = WSARecvFromMiddle(s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpFrom,lpFromlen,lpOverlapped,lpCompletionRoutine);
	if(res == 0 && WSAGetLastError() == 0)
	{
		BYTE *SignTrue = (BYTE *)tempszbuf;
		if(*SignTrue == 0xF7)
		{
			CopyMemory(wb.buf,tempszbuf,*lpNumberOfBytesRecvd);
			fprintf(fp,"query packet recv success\n");
		}
		else
		{
			TRANSMIT_PROTOCOL_METHOD *tpm = (TRANSMIT_PROTOCOL_METHOD *)tempszbuf;
			if(tpm->Protocol == TRANSMIT_PROTOCOL_HOUSEMAPINFO)
			{
				TRANSMIT_GAMEHOUSE_MAPINFO *tgm = (TRANSMIT_GAMEHOUSE_MAPINFO *)tempszbuf;
				//��ͼ���ݴ浽��������
				CopyMemory(wb.buf,&tgm->lpszMapInfo,tgm->uMapLen);
				//���������ֽڵĴ�С
				*lpNumberOfBytesRecvd = tgm->uMapLen;
				//α�췢��Դ��ַ
				((PSOCKADDR_IN)lpFrom)->sin_addr.s_addr = tgm->HostVirtualIp.S_un.S_addr;
				fprintf(fp,"map info recv success\n");
			}
			else
			{
				//����ip�����ø���Ϸ ֱ��return
				TRANSMIT_VIRTUAL_IP *tvi = (TRANSMIT_VIRTUAL_IP *)tempszbuf;
				LocalVirtualIpAddr.S_un.S_addr = tvi->VirtualIpAddr.S_un.S_addr;
				fprintf(fp,"virtual ip address load success\n");
			}
		}
	}
	//�ѻ���������ȥ
	lpBuffers->buf = wb.buf;

	return res;
}

int  WINAPI CInlineHook::Hookgetpeername(SOCKET s,struct sockaddr FAR *name,int FAR *namelen)
{
	int res;
	//����ԭʼ������ȡֵ
	res = getpeernameMiddle(s,name,namelen);
	//�޸�
	((PSOCKADDR_IN)name)->sin_addr.S_un.S_addr = RemoteVirtualIpAddr.S_un.S_addr;
	((PSOCKADDR_IN)name)->sin_port = htons(GAME_PORT);

	fprintf(fp,"Hookgetpeername Remote Ip:%s  Port:%d\n",inet_ntoa(((PSOCKADDR_IN)name)->sin_addr),ntohs(((PSOCKADDR_IN)name)->sin_port));
	return res;
}

int  WINAPI CInlineHook::Hookgetsockname(SOCKET s,struct sockaddr FAR *name,int FAR *namelen)
{
	int res;
	//����ԭʼ������ȡֵ
	res = getsocknameMiddle(s,name,namelen);
	//�޸�
	((PSOCKADDR_IN)name)->sin_addr.S_un.S_addr = LocalVirtualIpAddr.S_un.S_addr;

	fprintf(fp,"Hookgetsockname Local Ip:%s  Port:%d\n",inet_ntoa(((PSOCKADDR_IN)name)->sin_addr),ntohs(((PSOCKADDR_IN)name)->sin_port));
	return res;
}

int CInlineHook::HookWSARecv(SOCKET s, LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	fprintf(fp,"HookWSARecv success\n");
	return WSARecvMiddle(s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpOverlapped,lpCompletionRoutine);
}

VOID WINAPI CInlineHook::HookGetAcceptExSockaddrs(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength, sockaddr **LocalSockaddr, LPINT LocalSockaddrLength, sockaddr **RemoteSockaddr, LPINT RemoteSockaddrLength)
{
	//����ԭʼ������ȡֵ
	GetAcceptExSockaddrsMiddle(lpOutputBuffer,dwReceiveDataLength,dwLocalAddressLength,dwRemoteAddressLength,LocalSockaddr,LocalSockaddrLength,RemoteSockaddr,RemoteSockaddrLength);
	//�޸�
	SetEvent(hQueryEvent);
	if(WAIT_OBJECT_0 == WaitForSingleObject(hContinueEvent,INFINITE))
	{
		//�ȵ�ͬ����Ϣ
		(*(sockaddr_in **)LocalSockaddr)->sin_addr.S_un.S_addr = LocalVirtualIpAddr.S_un.S_addr;
		(*(sockaddr_in **)RemoteSockaddr)->sin_addr.S_un.S_addr = pavia->ClientVirtualIp.S_un.S_addr;

		fprintf(fp,"HookGetAcceptExSockAddrs, local ip:%s port:%d\n",inet_ntoa((*(sockaddr_in **)LocalSockaddr)->sin_addr),ntohs((*(sockaddr_in **)LocalSockaddr)->sin_port));
		fprintf(fp,"HookGetAcceptExSockAddrs, remote ip:%s port:%d\n",inet_ntoa((*(sockaddr_in **)RemoteSockaddr)->sin_addr),ntohs((*(sockaddr_in **)RemoteSockaddr)->sin_port));
	}
}

BOOL WINAPI CInlineHook::SendConnectHousePacket(SOCKET ConSock,in_addr VirtualAddr)
{
	TRANSMIT_CONNECT_INFO tci;
	tci.Type = TRANSMIT_PROTOCOL_CONNECTHOUSE;
	tci.HostVirtualIpAddr.S_un.S_addr = VirtualAddr.S_un.S_addr;
	Hooksendto(ConSock,(PCHAR)&tci,sizeof(TRANSMIT_CONNECT_INFO),0,NULL,sizeof(sockaddr_in));
	return TRUE;
}

