#pragma once

//WAR3�˿�
#define GAME_PORT 6112
//�ͻ��˶˿ں�
#define CLIENT_PORT 4200
//�ͻ�������Ϸͨ�Ŷ˿�
#define CLIENT_TCP_GAME 4201
//��ͼ������С
#define MAX_MAP_SIZE 300
//event
#define MY_QUERY_EVENT_NAME "MyQueryEventName"
//continue event
#define MY_CONTINUE_ENENT_NAME "MyContinueEventName"
//map
#define MY_MAPFILE_NAME "MyMapFileName"

//Э��
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
	TRANSMIT_PROTOCOL_GAMEINFO       //tcp��Ϸ��
};

//HOOK��Ϣ
typedef struct _HOOKADDRINFO
{
	DWORD OldFuncAddr;
	DWORD NewFuncAddr;
	BOOL  bIsHook;
}HOOKADDRINFO;

typedef struct _ASSOCIATE_VIRTUAL_IP_ADDR
{
	in_addr ClientVirtualIp;
}ASSOCIATE_VIRTUAL_IP_ADDR;

//���������
typedef struct _TRANSMIT_PROTOCOL_METHOD
{
	TRANSMIT_PROTOCOL_TYPE Protocol;
}TRANSMIT_PROTOCOL_METHOD;

//��ȡָ�������������ͼ���ķ���ṹ
typedef struct _TRANSMIT_GAMEHOUSE_MAPINFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIp;
	ULONG uMapLen;												//��ͼ len
	ULONG uHouseId;												//����ID
	BYTE lpszMapInfo;										    //ʵ�ʵĵ�ͼ��data	
}TRANSMIT_GAMEHOUSE_MAPINFO;

//��֪�ͻ�����ϷҪ���ӵĵ�ַ
typedef struct _TRANSMIT_CONNECT_INFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIpAddr;
}TRANSMIT_CONNECT_INFO;

//���������������ip���ؿͻ���
typedef struct _TRANSMIT_VIRTUAL_IP
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr VirtualIpAddr;
}TRANSMIT_VIRTUAL_IP;
