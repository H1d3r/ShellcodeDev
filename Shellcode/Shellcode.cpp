#include<windows.h>
#include<utils.hpp>
#include<ShellcodeHelper.hpp>
#include<stdio.h>
#define EXPORT extern "C" __declspec(dllexport)

// ����������
#pragma comment(linker,"/entry:shellcode")

void shellcode() {
	_DWORD dwKernel32 = GetKernel32Addr();
	_DWORD** functions = GetWindowsAPIs();
	volatile char szshellcode[] = { 's', 'h', 'e', 'l', 'l', 'c', 'o', 'd', 'e', '\0' };
	pMessageBoxA(0, (char*)szshellcode, (char*)szshellcode, 0);
}

// ��shellcode�Ժ�������ʽ������������Ϊshell�Ĵ���Σ�������ȡ
#pragma code_seg("shell")
EXPORT void function() {
	// shellcode����
	// ...
	return;
}