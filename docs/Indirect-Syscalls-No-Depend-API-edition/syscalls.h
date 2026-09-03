// Project  : Indirect Syscall - No API Dependencies Edition
// File     : syscalls.c / syscalls.h
// Author   : Piyusha Akash (0x3xp)
// Website  : https://0x3xp.github.io
// Blog     : https://0x3xp.github.io/blog
// Academy  : https://0x3xp.github.io/academy
// GitHub   : https://github.com/0x3xp
// Series   : AV/EDR Evasion Lessons
// License  : Educational use only. Attribution required for derivative works. Unauthorized redistribution or commercial use is prohibited.

#define _SYSCALLS_H

#include <windows.h>

typedef long NTSTATUS; //NTSTATUS as long data type
typedef NTSTATUS* PNTSTATUS; //Pointern to NTSTATUS

extern NTSTATUS NtAllocateVirtualMemory(
	HANDLE ProcessHandle,
	PVOID* BaseAddress,
	ULONG_PTR ZeroBits,
	PSIZE_T RegionSize,
	ULONG AllocationType,
	ULONG Protect
);

#endif