#include<windows.h>
#include<shlobj.h>
#include<api.h>
#include<hash.h>
#if defined(_WIN64)
	#define _PEB_Offset 0x60
	#define _Ldr_Offset 0x18
	#define _IOM_Offset 0x10
	typedef DWORD64 _DWORD;
	typedef PDWORD64 _PDWORD;
	typedef PIMAGE_NT_HEADERS64 _PIMAGE_NT_HEADERS;
#else
	#define _PEB_Offset 0x30
	#define _Ldr_Offset 0x0C
	#define _IOM_Offset 0x0C
	typedef DWORD _DWORD;
	typedef PDWORD _PDWORD;
	typedef PIMAGE_NT_HEADERS _PIMAGE_NT_HEADERS;
#endif

__forceinline DWORD GetFuncHash(char* functionName) {
	DWORD hash = 0;
	while (*functionName) {
		hash = (hash * 138) + *functionName;
		functionName++;
	}

	return hash;
}

__forceinline _PDWORD GetInLoadOrderModuleList() {
	_DWORD dwKernel32 = 0;
	_TEB* pTeb = NtCurrentTeb();
	_PDWORD pPeb = (_PDWORD) * (_PDWORD)((_DWORD)pTeb + _PEB_Offset);
	_PDWORD pLdr = (_PDWORD) * (_PDWORD)((_DWORD)pPeb + _Ldr_Offset);
	_PDWORD InLoadOrderModuleList = (_PDWORD)((_DWORD)pLdr + _IOM_Offset);
	return InLoadOrderModuleList;
}

__forceinline _DWORD GetKernel32Addr() {
	_DWORD dwKernel32 = 0;
	_PDWORD InLoadOrderModuleList = GetInLoadOrderModuleList();
	_PDWORD pModuleExe = (_PDWORD)*InLoadOrderModuleList;
	_PDWORD pModuleNtdll = (_PDWORD)*pModuleExe;
	_PDWORD pModuleKernel32 = (_PDWORD)*pModuleNtdll;
	dwKernel32 = pModuleKernel32[6];
	return dwKernel32;
}

__forceinline _DWORD GetExeBaseAddr() {
	_DWORD dwExe = 0;
	_PDWORD InLoadOrderModuleList = GetInLoadOrderModuleList();
	_PDWORD pModuleExe = (_PDWORD)*InLoadOrderModuleList;
	dwExe = pModuleExe[6];
	return dwExe;
}

__forceinline _DWORD GetNtdllAddr() {
	_DWORD dwNtll = 0;
	_PDWORD InLoadOrderModuleList = GetInLoadOrderModuleList();
	_PDWORD pModuleExe = (_PDWORD)*InLoadOrderModuleList;
	_PDWORD pModuleNtdll = (_PDWORD)*pModuleExe;
	dwNtll = pModuleNtdll[6];
	return dwNtll;
}


__forceinline _DWORD GetFuncAddrByHash(_DWORD dwBase, _DWORD hash) {
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)dwBase;
	_PIMAGE_NT_HEADERS pNt = (_PIMAGE_NT_HEADERS)(dwBase + pDos->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(dwBase + pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	PDWORD pEAT = (PDWORD)(dwBase + pExport->AddressOfFunctions);
	PDWORD pENT = (PDWORD)(dwBase + pExport->AddressOfNames);
	PWORD pEIT = (PWORD)(dwBase + pExport->AddressOfNameOrdinals);
	for (DWORD i = 0; i < pExport->NumberOfNames; i++) {
		char* szFuncName = (char*)(dwBase + pENT[i]);
		if (GetFuncHash(szFuncName) == hash) {
			return dwBase + pEAT[pEIT[i]];
		}
	}
	return 0;
}

__forceinline void XOR(unsigned char* data, long dataLen, const unsigned char* key, long keyLen) {
	for (long i = 0; i < dataLen; ++i) {
		data[i] ^= key[i % keyLen];
	}
}

__forceinline void RC4(unsigned char* data, long dataLen, const unsigned char* key, long keyLen) {
	unsigned char S[256];
	unsigned char T[256];
	for (int i = 0; i < 256; ++i) {
		S[i] = i;
		T[i] = key[i % keyLen];
	}
	int j = 0;
	for (int i = 0; i < 256; ++i) {
		j = (j + S[i] + T[i]) % 256;
		unsigned char temp = S[i];
		S[i] = S[j];
		S[j] = temp;
	}
	int i = 0;
	j = 0;
	for (long k = 0; k < dataLen; ++k) {
		i = (i + 1) % 256;
		j = (j + S[i]) % 256;
		unsigned char temp = S[i];
		S[i] = S[j];
		S[j] = temp;
		unsigned char K = S[(S[i] + S[j]) % 256];
		data[k] ^= K;
	}
}
