; Project  : Indirect Syscall - No API Dependencies Edition
; File     : ntdll.asm / syscalls-masm.asm
; Author   : Piyusha Akash (0x3xp)
; Website  : https://0x3xp.github.io
; Blog     : https://0x3xp.github.io/blog
; Academy  : https://0x3xp.github.io/academy
; GitHub   : https://github.com/0x3xp
; Series   : AV/EDR Evasion Lessons
; License  : Educational use only. Attribution required for derivative works. Unauthorized redistribution or commercial use is prohibited.

OPTION CASEMAP:NONE

.CODE

GetNtdllBase PROC PUBLIC
    xor     rax, rax
    mov     rax, GS:[60h]           ; PEB
    mov     rax, [rax + 18h]        ; PEB->Ldr
    mov     rax, [rax + 20h]        ; First Flink (InMemoryOrderModuleList)
    mov     rax, [rax]              ; Next Flink
    mov     rax, [rax + 20h]        ; --> ntdll.dll DllBase
    ret
GetNtdllBase ENDP

END