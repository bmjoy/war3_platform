#ifndef WARIIIUDPPACKET_H__
#define WARIIIUDPPACKET_H__

#include <windows.h>
using namespace std;

enum UDP_PACKET_OPERATION_TYPE
{
	UDP_PACKET_INITIALIZE = 0x1,		//������Ϸ��ʼ����ʱ���͵�
	UDP_PACKET_QUERY_HOST = 0x2F,		//��ѯ������Ϸ����
	UDP_PACKET_ECHO_HOST,				//Ӧ�������ͻ���
	UDP_PACKET_CREATE_HOST,				//�����˴�����ͼʱ
	UDP_PACKET_PEOPLECHANGE,			//��������ҷ������
	UDP_PACKET_CANCELHOST,				//������ȡ����ͼ����
};

//UDP����ͷ��
typedef struct _WAR3_UDP_HEAD_PACKET
{
	BYTE SignTrue;				//��־��
	BYTE Operation;				//������
	WORD PacketLen;				//�������
}WAR3_UDP_HEAD_PACKET;

//UDP��������

//	������0x2F
//  ����:�㲥���ؾ�������ѯ�ɼ������Ϸ����
typedef struct _WAR3_UDP_QUERY_PACKET
{
	WAR3_UDP_HEAD_PACKET	Head;		//ͷ������
	DWORD					GameType;	//��Ϸ����
	DWORD					GameVersion;//��Ϸ�汾
	DWORD					GameID;		//��ϷID<�㲥ʱ����>
}WAR3_UDP_QUERY_PACKET;

//	������0x31
//	����:�������˽����÷���󣬻Ὣ�����ݽ��й㲥����֪�����ͻ��������Ѿ�������
typedef struct _WAR3_UDP_CREATEHOST_PACKET
{
	WAR3_UDP_HEAD_PACKET	Head;			//ͷ
	DWORD					GameType;		//��Ϸ����
	DWORD					GameVersion;	//��Ϸ�汾����ͬ�汾���Ӵ˷��
	DWORD					GameID;			//��ϷID����ʼΪ1�����������Ϸ�Ĵ�����������
}WAR3_UDP_CREATEHOST_PACKET;

//	������0x32
//	����:�������˽����÷������Ϸ�ڵ����������������㲥�˷��
typedef struct _WAR3_UDP_PEOPLECHANGE_PACKET
{
	WAR3_UDP_HEAD_PACKET	Head;			//ͷ
	DWORD					GameID;			//��ϷID����ʼΪ1�����������Ϸ�Ĵ�����������
	DWORD					PeopleNumber;	//��ǰ�������
	DWORD					PeopleTotal;	//����ɼ�����������
}WAR3_UDP_PEOPLECHANGE_PACKET;

//	������0x33
//	����:��������ȡ�������õķ���󣬻�㲥�����ݰ�
typedef struct _WAR3_UDP_CANCELHOST_PACKET
{
	WAR3_UDP_HEAD_PACKET	Head;			//ͷ
	DWORD					GameID;			//��ϷID����ʼΪ1�����������Ϸ�Ĵ�����������
}WAR3_UDP_CANCELHOST_PACKET;

#endif