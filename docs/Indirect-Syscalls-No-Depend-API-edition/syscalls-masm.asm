; Project  : Indirect Syscall - No API Dependencies Edition
; File     : ntdll.asm / syscalls-masm.asm
; Author   : Piyusha Akash (0x3xp)
; Website  : https://0x3xp.github.io
; Blog     : https://0x3xp.github.io/blog
; Academy  : https://0x3xp.github.io/academy
; GitHub   : https://github.com/0x3xp
; Series   : AV/EDR Evasion Lessons
; License  : Educational use only. Attribution required for derivative works. Unauthorized redistribution or commercial use is prohibited.


EXTERN wNtAllocateVirtualMemory:DWORD  ; Extern keyword indicates that the symbol is defined in another module. Here it's the syscall number for NtAllocateVirtualMemory.
EXTERN sysNtAllocateVirtualMemory:QWORD ; The actual address of the NtAllocateVirtualMemory syscall in ntdll.dll.

.CODE

NtAllocateVirtualMemory PROC
	mov r10, rcx
	mov eax, wNtAllocateVirtualMemory
	jmp QWORD PTR [sysNtAllocateVirtualMemory]
NtAllocateVirtualMemory ENDP

END