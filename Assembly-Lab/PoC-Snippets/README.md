# PoC Snippets

This directory contains isolated code samples and Proof-of-Concept (PoC) scripts written in pure Assembly (NASM/MASM). These snippets are used to validate the theories documented in the **Technical-Briefs**.

## 💻 Laboratory Snippets

* **Register-Logic:** Small routines demonstrating bitwise operations (XOR, AND, OR) and data movement between GPRs.
* **Stack-Frames:** Examples of manual function prologues and epilogues without compiler assistance.
* **API-Invocations:** Minimalist code to invoke Windows APIs (e.g., MessageBoxA, ExitProcess) using the correct calling convention.
* **Logic-Gates:** Re-implementing high-level `if/else` and `loop` structures using JMP and CMP instructions.

## 🛠 Usage

To compile these snippets, ensure you have the appropriate assembler installed:
* **NASM:** `nasm -f win64 snippet.asm -o snippet.obj`
* **Linker:** `ld snippet.obj -o snippet.exe` (or use Microsoft `link.exe`)

---

> **Warning:** These snippets are for educational testing only. They are not designed for standalone deployment.