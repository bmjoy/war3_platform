#include <iostream>
#include "UDPNet.h"
using namespace std;

void Run()
{
	int endnum = 0;
	while(1)
	{
		cin>>endnum;
		if(endnum == -1)
		{
			break;
		}
		cout<<"����������Ч������-1�رո÷�����"<<endl;
	}
}

int main()
{
	CUDPNet udpnet;
	udpnet.InitialUDP();

	Run();

	udpnet.UnInitial();

	return 0;
}