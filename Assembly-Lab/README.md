# 00. Assembly Lab

This directory serves as the technical foundation for the Native-Adversary-Framework. It documents the transition from high-level logic to raw machine instructions, focusing on the x64 architecture and Windows internals.

## 🛠 Key Areas

* **ISA Foundations:** Deep dive into x86-64 General Purpose Registers (GPRs), instruction pointers, and the Scale-Index-Base (SIB) byte.
* **Win64 Calling Convention:** Mastering the Microsoft x64 ABI, including shadow space allocation, volatile/non-volatile register management, and 16-byte stack alignment.
* **Low-Level Logic:** Implementing high-level constructs (Loops, Conditionals, Logic Gates) using pure Assembly to minimize compiler-generated signatures.
* **Memory Management:** Understanding stack frames (Prologue/Epilogue), heap interactions, and direct memory addressing.

## 📁 Content

* **01-Technical-Briefs:** Module summaries and theoretical research into CPU architecture and Windows-specific calling standards.
* **02-PoC-Snippets:** Isolated Assembly source files (.asm/.nasm) demonstrating specific instructions and logic flows.

---

> **Note:** This lab is strictly for educational documentation and fundamental research. Weaponized implementations of these concepts are located in the `Exploit-Dev` and `Malware-Dev` directories.