#define _DLL_EXPORT
#include "InlineHook.h"
#include "HookNetwork.h"
#include <WINDOWS.H>
#include <iostream>
using namespace std;

//����API HOOK ��
CInlineHook* g_InlineHook = NULL;

FILE *fp;

//dll���
BOOL WINAPI   DllMain(HINSTANCE  hModule,DWORD  dwReason,LPVOID  lpvReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		//���̸���		
		fopen_s(&fp,"..\\..\\test.txt", "w+");
		fprintf(fp,"Load Dll Successful\n");
		//fwrite("111", 3, 1, fp);
		g_InlineHook = new CInlineHook();		
		
		break;
	case DLL_PROCESS_DETACH:
		//����ж��
		fclose(fp);
		g_InlineHook?delete g_InlineHook:NULL;
		break;
	}
	
	return TRUE;
}

int WINAPI Add(int a,int b)
{
	return a+b;
}