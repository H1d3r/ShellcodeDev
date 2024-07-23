#pragma once

// constexpr���ڱ���ʱ���㳣��

constexpr DWORD Hash(const char* functionName) {
	DWORD hash = 0;
	while (*functionName) {
		hash = (hash * 138) + *functionName;
		functionName++;
	}
	return hash;
}

// ����Hash
constexpr auto szLoadLibraryA = Hash("LoadLibraryA");
constexpr auto szMessageBoxA = Hash("MessageBoxA");