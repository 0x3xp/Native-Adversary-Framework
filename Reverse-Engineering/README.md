# 03. Reverse Engineering & Binary Analysis

This section documents my process of dissecting binaries to understand opaque logic and bypass software protections.

## 🛠 Key Areas
- **Static Analysis:** Manually parsing the **Portable Executable (PE)** format to identify suspicious sections, imports, and entry points.
- **Dynamic Analysis:** Using debuggers (x64dbg) and disassemblers (Ghidra/IDA) to trace instruction flow and patch binary logic.
- **Anti-RE Research:** Understanding how malware detects analysis environments to build better detection bypasses.

## 📁 Content
- `/Custom-Parsers`: C implementations of PE/COFF header dumpers.
- `/Writeups`: Analysis reports of Crackmes and malware samples.
