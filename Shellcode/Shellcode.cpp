#include<windows.h>
#include<utils.h>
#include<stdio.h>
#define EXPORT extern "C" __declspec(dllexport)

// ����������
#pragma comment(linker,"/entry:shellcode")

void shellcode() {
	_DWORD dwKernel32 = GetKernel32Addr();
	// volatile���ڽ�ֹ�������Ż���������ֹ�������������ݶ�
	volatile char szUser32dll[] = { 'U', 's', 'e', 'r', '3', '2', '.', 'd', 'l', 'l', '\0' };
	volatile char msg[] = { 's', 'h', 'e', 'l', 'l', 'c', 'o', 'd', 'e', '\0' };
	LoadLibraryAFunc pLoadLibraryA = (LoadLibraryAFunc)GetFuncAddrByHash(dwKernel32, szLoadLibraryA);
	_DWORD hUser32 = (_DWORD)pLoadLibraryA((char*)szUser32dll);
	MessageBoxAFunc pMessageBoxA = (MessageBoxAFunc)GetFuncAddrByHash((_DWORD)hUser32, szMessageBoxA);
	pMessageBoxA(0, (char*)msg, (char*)msg, 0);
}

// ��shellcode�Ժ�������ʽ������������Ϊshell�Ĵ���Σ�������ȡ
#pragma code_seg("shell")
EXPORT void function() {
	// shellcode����
	// ...
	return;
}