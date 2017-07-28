#ifndef GLOBALINFORMATION_H__
#define GLOBALINFORMATION_H__

#include "WarIIIUDPPacket.h"

//WAR3�˿�
#define GAME_PORT 6112
//�ͻ��˶˿�
#define CLIENT_PORT 4200
//�ͻ�������Ϸͨ�Ŷ˿�
#define CLIENT_TCP_GAME 4201
//�������˿�
#define SERVER_PORT 3200
//�������Ϸ
#define SERVER_GAME_PORT 3201
//Ĭ�ϻ�������С
#define DEAFAULT_BUFFER_SIZE 1024
//��½�û�������󳤶�
#define MAX_USER_NAME 20
//���IP�ַ�������
#define MAX_IP_STR_LENGTH 20
//event
#define MY_QUERY_EVENT_NAME "MyQueryEventName"
//continue event
#define MY_CONTINUE_ENENT_NAME "MyContinueEventName"
//map
#define MY_MAPFILE_NAME "MyMapFileName"
//һҳ��С
#define ONE_PAGE_SZIE 8

enum CONNECTSTATE{CC_CLIENT,CC_HOST};
enum NETTYPE{NT_RECVFROM,NT_SENDTO,NT_ACCEPT,NT_RECV,NT_SEND};

typedef struct _ASSOCIATE_SOCK
{
	SOCKET sock1;
	SOCKET sock2;
}ASSOCIATE_SOCK;

typedef struct _VIRTUALCLIENT
{
	SOCKET sock;
	ASSOCIATE_SOCK Assosocket;
}VIRTUALCLIENT;

typedef struct _ASSOCIATE_VIRTUAL_IP_ADDR
{
	in_addr ClientVirtualIp;
}ASSOCIATE_VIRTUAL_IP_ADDR;

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
	in_addr VirtualIPAddr;
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
	in_addr HostVirtualIp;
	ULONG uMapLen;												//��ͼ len
	ULONG uHouseId;												//����ID
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
typedef struct _TRANSMIT_CONNECT_INFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIpAddr;
}TRANSMIT_CONNECT_INFO;

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