#ifndef GLOBALINFORMATION_H__
#define GLOBALINFORMATION_H__

#include <WinSock2.h>
#include "WarIIIUDPPacket.h"

//�ͻ��˶˿�
#define CLIENT_PORT 4200
//�������˿�
#define SERVER_PORT 3200
//��������Ϸ
#define SERVER_GAME_PORT 3201
//Ĭ�ϻ�������С
#define DEAFAULT_BUFFER_SIZE 1024
//��½�û�������󳤶�
#define MAX_USER_NAME 20
//��Ϸ��ͼ MAX SIZE
#define GAME_UDP_MAP_BUFFER	300
//��ѯ����С
#define GAME_UDP_QUERY_SIZE	16
//��������IP����
#define MAX_VIRTUAL_IP_NUM 200
//���IP�ַ�������
#define MAX_IP_STR_LENGTH 20
//��󷿼���
#define MAX_CREATE_ROOM_NUM 200
//�����������
#define MAX_CONNECT_NUM 20

enum CONNECTSTATE{CC_CLIENT,CC_HOST};
enum ROOMNUMISUSED{RM_NOUSING,RM_USED};
enum VIRTUALIPISUSED{VI_NOUSING,VI_UESD};
enum NETTYPE{NT_RECVFROM,NT_SENDTO,NT_ACCEPT,NT_RECV,NT_SEND};
enum PERSON_STATES{PS_INITIAL,PS_SELECTROOM,PS_GAMING};

typedef struct _TEMP_STORE_GAME_PACKET
{
	DWORD dwNumOfPacket;
	BYTE szbuf[DEAFAULT_BUFFER_SIZE];
}TEMP_STORE_GAME_PACKET;

typedef struct _ASSOCIATEIPADDR
{
	in_addr RealIpAddr;
	in_addr VirtualIpAddr;
}ASSOCIATEIPADDR;

typedef struct _MYOVERINFO
{
	WSAOVERLAPPED wv;
	WSABUF wb;
	NETTYPE type;
	CHAR szbuf[DEAFAULT_BUFFER_SIZE];
	sockaddr_in sockaddrfrom;
	INT sockaddrfromlen;
}MYOVERINFO;

//game��
typedef struct _MYOVERGAMEINFO
{
	WSAOVERLAPPED wv;
	WSABUF wb;
	NETTYPE type;
	DWORD dwRecv;
	SOCKET sock;
	CHAR szbuf[DEAFAULT_BUFFER_SIZE];
}MYOVERGAMEINFO;

//��Ϸ�汾 ����
enum GAME_VERSION_TYPE
{
	GAME_VERSION_UNKNOWN,
	GAME_VERSION_120E,
	GAME_VERSION_121,
	GAME_VERSION_122,
	GAME_VERSION_123,
	GAME_VERSION_124B,
	GAME_VERSION_124E,
	GAME_VERSION_125B,
	GAME_VERSION_126,
};

//������socket
typedef struct _ASSOCIATE_SOCKET
{
	SOCKET sock1;
	SOCKET sock2;
	in_addr RealAddr;
	BOOL bIsGaming;
}ASSOCIATE_SOCKET;

typedef struct _PERSON_INFORMATION
{
	DWORD dwGameVersion;                                  //��Ϸ�汾<Э����ȡ>
	CHAR szName[MAX_USER_NAME];
	GAME_VERSION_TYPE FileVersion;                        //��Ϸ�汾��
	ASSOCIATEIPADDR *lst_client[MAX_CONNECT_NUM];         //�����Ŀͻ�������
	DWORD RoomId;                                         //������
	INT ClientIndex;                                      //�ͻ����±�
	INT VirIpIndex;                                       //����ip�±�
	in_addr VirtualIPAddress;                             //��������IP��ַ
	in_addr RealIPAddress;                                //������ʵIP��ַ
	PERSON_STATES PersonStates;                           //״̬
	BYTE QueryPacket[GAME_UDP_QUERY_SIZE];                //��ѯ��
	BYTE MapPacket[GAME_UDP_MAP_BUFFER];                  //��ͼ��
	DWORD dwMapLen;				                          //��ͼ���ĳ���
}PERSON_INFO;

enum TRANSMIT_PROTOCOL_TYPE
{
	TRANSMIT_PROTOCOL_USERLOGIN,	  //����һ���û���½�İ�
	TRANSMIT_PROTOCOL_GAMEUDP,		  //����һ����ϷUDP���ݰ�
	TRANSMIT_PROTOCOL_FILEVERSION,	  //������Ϸ�ļ��İ汾��
	TRANSMIT_PROTOCOL_USERQUIT,	      //����һ���û��˳��İ�
	TRANSMIT_PROTOCOL_HOUSECANCEL,	  //ָ������Ϸ�����Ѿ����ر�
	TRANSMIT_PROTOCOL_HOUSEMAPINFO,	  //ָ������Ϸ������ͼ����Ϣ
	TRANSMIT_PROTOCOL_CONNECTHOUSE,   //�ͻ�����������ָ������Ϸ����
	TRANSMIT_PROTOCOL_VIRTUALIPADDR,  //��������IP��ַ
	TRANSMIT_PROTOCOL_PEOPLECHANGE,   //�����������
	TRANSMIT_PROTOCOL_CONNECTINFO,    //֪ͨ������������Ϣ
	TRANSMIT_PROTOCOL_CLIENTINFO,     //�����������Ŀͻ�����Ϣ
	TRANSMIT_PROTOCOL_GAMEINFO        //tcp��Ϸ��
};

//���������
typedef struct _TRANSMIT_PROTOCOL_METHOD
{
	TRANSMIT_PROTOCOL_TYPE Protocol;
}TRANSMIT_PROTOCOL_METHOD;

//��Ϸ��UDP���ݰ�
typedef struct _TRANSMIT_GAME_UDP_PACKET
{
	TRANSMIT_PROTOCOL_TYPE Type;		
	WAR3_UDP_HEAD_PACKET  vBody;	//����
}TRANSMIT_GAME_UDP_PACKET;

//�û���¼��սƽ̨����˵ķ���ṹ
typedef struct _TRANSMIT_USER_LOGIN
{
	TRANSMIT_PROTOCOL_TYPE Type;
	CHAR	szUserName[MAX_USER_NAME];	//�û���
	CHAR    szUserPassword[MAX_USER_NAME]; //�û�����
}TRANSMIT_USER_LOGIN;

//�û��˳���սƽ̨����˵ķ���ṹ
typedef struct _TRANSMIT_USER_QUIT
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr VirtualIpAddr;
}TRANSMIT_USER_QUIT;

//���������������ip���ؿͻ���
typedef struct _TRANSMIT_VIRTUAL_IP
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr VirtualIpAddr;
}TRANSMIT_VIRTUAL_IP;

//��ȡָ������ĵ�������ͼ���ķ���ṹ
typedef struct _TRANSMIT_GAMEHOUSE_MAPINFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIp;                                      //��������IP��ַ
	ULONG uMapLen;												//��ͼ len
	ULONG uHouseId;											    //����ID
	BYTE lpszMapInfo;										    //ʵ�ʵĵ�ͼ��data
}TRANSMIT_GAMEHOUSE_MAPINFO;

//��Ϸ������ұ����
typedef struct _TRANSMIT_GAMEHOUSE_PEOPLECHANGE
{
	TRANSMIT_PROTOCOL_TYPE Type;
	WAR3_UDP_PEOPLECHANGE_PACKET War3ChangePeople;
}TRANSMIT_GAMEHOUSE_PEOPLECHANGE;

//��Ϸ����ȡ��
typedef struct _TRANSMIT_GAMEHOUSE_CANCLE
{
	TRANSMIT_PROTOCOL_TYPE Type;
	WAR3_UDP_CANCELHOST_PACKET War3CancleHouse;
}TRANSMIT_GAMEHOUSE_CANCLE;

//��֪�ͻ�����ϷҪ���ӵĵ�ַ
typedef struct _TRANSMIT_CLIENT_INFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr ClientVirtualIp;
}TRANSMIT_CLIENT_INFO;

//��֪������˭Ҫ����˭
typedef struct _TRANSMIT_CLIENT_CONNECT_HOST
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIpAddr;
	in_addr ClientVirtualIpAddr;
}TRANSMIT_CLIENT_CONNECT_HOST;

//��Ϸ����
typedef struct _TRANSMIT_GAME_INFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	CONNECTSTATE ConType;
	in_addr ClientAddr;
	in_addr ServerAddr;
}TRANSMIT_GAME_INFO;
#endif