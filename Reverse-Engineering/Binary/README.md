# Compiled Binaries & Research Samples

### ❓ Why this folder?
A key part of Security Research is the ability to analyze compiled code without access to the source. This folder serves as a "CrackMe" lab, containing various binaries compiled from C, C++, and x64 Assembly designed to test reverse engineering skills and debugging techniques.

### 📦 What's included?
* **C-Based CrackMes**: Small programs focused on logic bypasses, string obfuscation, and basic authentication checks.
* **C++ Object Binaries**: Samples designed to study Class structures, Virtual Function Tables (vTables), and Name Mangling in compiled code.
* **ASM Native Stubs**: Extremely small, high-performance binaries written in x64 Assembly to study raw instruction flow and system calls (syscalls).
* **Protected Samples**: Binaries implemented with basic Anti-Debugging (e.g., `IsDebuggerPresent`) and Anti-VM techniques to practice detection bypasses.

### ⚠️ Usage Note
These binaries are intended for use in a **Sandbox Environment** using debuggers like **x64dbg**, **WinDbg**, or disassemblers like **Ghidra** and **IDA Pro**. They are harmless but simulate the behavior of protected software.
