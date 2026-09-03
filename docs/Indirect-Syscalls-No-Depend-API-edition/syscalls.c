// Project  : Indirect Syscall - No API Dependencies Edition
// File     : syscalls.c / syscalls.h
// Author   : Piyusha Akash (0x3xp)
// Website  : https://0x3xp.github.io
// Blog     : https://0x3xp.github.io/blog
// Academy  : https://0x3xp.github.io/academy
// GitHub   : https://github.com/0x3xp
// Series   : AV/EDR Evasion Lessons
// License  : Educational use only. Attribution required for derivative works. Unauthorized redistribution or commercial use is prohibited.

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include "syscalls.h"

DWORD wNtAllocateVirtualMemory;
UINT_PTR sysNtAllocateVirtualMemory;

extern PVOID GetNtdllBase();

UINT_PTR ManualGetProcAddress(PVOID moduleBase, const char* funcName) {
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)moduleBase;
    PIMAGE_NT_HEADERS nt  = (PIMAGE_NT_HEADERS)((BYTE*)moduleBase + dos->e_lfanew);

    PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)(
        (BYTE*)moduleBase +
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
    );

    DWORD* names    = (DWORD*)((BYTE*)moduleBase + exports->AddressOfNames);
    WORD*  ordinals = (WORD*) ((BYTE*)moduleBase + exports->AddressOfNameOrdinals);
    DWORD* funcs    = (DWORD*)((BYTE*)moduleBase + exports->AddressOfFunctions);

    for (DWORD i = 0; i < exports->NumberOfNames; i++) {
        char* name = (char*)((BYTE*)moduleBase + names[i]);

        // Simple strcmp — or replace with your own to drop CRT dependency
        if (strcmp(name, funcName) == 0) {
            return (UINT_PTR)moduleBase + funcs[ordinals[i]];
        }
    }
    return 0;
}

int main() {

	printf("Indirect syscall NtAllocateVirtualMemory!\n");
	
	PVOID buffer = NULL;
	SIZE_T size = 0x1000;
	unsigned char shellcode[] = "\x90\x90\x90\x90";

	//HANDLE hntdll = GetModuleHandleA("ntdll.dll"); //handle to ntdll.dll
	PVOID ntdllBase = GetNtdllBase(); // We avoid GetModuleHandleA like Usermode APIs to become more stealth. We walk PEB and find the ntdll base address.

	UINT_PTR pNtAllocateVirtualMemory = ManualGetProcAddress(ntdllBase, "NtAllocateVirtualMemory"); //get the address of NtAllocateVirtualMemory from ntdll.dll
	wNtAllocateVirtualMemory = ((unsigned char*)(pNtAllocateVirtualMemory + 4))[0]; //get the SSN of NtAllocateVirtualMemory
	sysNtAllocateVirtualMemory = pNtAllocateVirtualMemory + 0x12;

	NTSTATUS status = NtAllocateVirtualMemory((HANDLE)-1, &buffer, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if(status == 0) {
		printf("Allocated %zu bytes at %p\n", size, buffer);
	} else {
		printf("NtAllocateVirtualMemory failed: 0x%08X\n", (unsigned int)status);
	}

	printf("\nPress enter to exit...");
	getchar();


	return 0;
}