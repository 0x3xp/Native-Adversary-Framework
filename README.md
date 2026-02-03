# Native-Adversary-Framework
### 🛠️ Advanced Research Lab: Windows Internals, Exploit Dev & Malware Engineering

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language: C](https://img.shields.io/badge/Language-C-blue.svg)](https://en.cppreference.com/w/c)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-purple.svg)](https://isocpp.org/)
[![Language: x64_ASM](https://img.shields.io/badge/Language-x64__ASM-red.svg)](https://www.nasm.us/)

**Native-Adversary-Framework** is a professional-grade research repository documenting my work in offensive security and low-level systems programming. This project showcases the synergy between **C**, **C++**, and **x64 Assembly** to subvert modern Windows security mitigations by interacting directly with the OS's internal structures.

---

## 🔬 Core Technical Pillars

### 1. Offensive Engineering (C/C++)
Implementation of sophisticated payload delivery and execution vectors using the Win32 and Native API (NTAPI).
* **Process Injection:** Advanced techniques including **Process Hollowing**, **Thread Context Hijacking**, and **NTAPI Section Mapping**.
* **System Reconnaissance:** Native implementations of host discovery tools using APIs like `NetUserEnum`, `GetAdaptersInfo`, and `EnumServicesStatusEx`.
* **Credential Harvesting:** Research into data recovery using `CryptUnprotectData` and LSA session enumeration.

### 2. Stealth & Evasion (x64 ASM & Internals)
Operating beneath the visibility of EDR/AV by manipulating hardware-level structures.
* **PEB Walking:** Custom x64 ASM logic to traverse the **Process Environment Block** (`gs:[0x60]`) for module resolution, bypassing the need for `GetProcAddress`.
* **API Hashing:** Reducing static signatures by implementing custom hashing algorithms to resolve functions at runtime.
* **Anti-Analysis:** Detecting debuggers and virtualized environments through `NtQueryInformationProcess` and timing-based side-channel checks.



### 3. Exploit Development & Reverse Engineering
Deep analysis of binary vulnerabilities and the Portable Executable (PE) format.
* **Custom Shellcoding:** Writing 100% position-independent, null-free x64 assembly code.
* **Binary Analysis:** Custom C-based parsers for the **PE File Format**, including IAT/EAT reconstruction and section header analysis.
* **Logic Subversion:** Practical examples of binary patching and bypassing software validation through instruction-level modification.



---

## 🏗️ Repository Architecture

```text
📂 Native-Adversary-Framework
├── 📂 01-Exploit-Dev          # x64 Shellcode, ROP Chains, and Memory Corruption
├── 📂 02-Malware-Dev          # Injection, Persistence, and Evasion Modules
├── 📂 03-Reverse-Engineering  # Custom PE Parsers, Binary Analysis, and RE Writeups
└── 📂 Core-Engine             # Pure C/C++ Headers, Internal Structs (PEB/TEB), and Hash Logic
