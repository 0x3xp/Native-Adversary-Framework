# Core Engine: The Framework Backbone

The `Core-Engine` contains the foundational logic shared across all modules. It is designed with a **Zero-CRT** philosophy, ensuring that the framework remains lightweight and stealthy.

## 🛠 Key Areas
- **Internal Structures:** Custom C-struct definitions for the `PEB`, `TEB`, and `LDR_DATA_TABLE_ENTRY` to allow seamless interaction between C and ASM.
- **API Hashing Logic:** Implementation of algorithms (e.g., DJB2, ROR13) to resolve system functions without leaving string artifacts in the binary.
- **Native Definitions:** Undocumented NTAPI function signatures and type definitions.

## 📁 Content
- `Internals.h`: Core structure definitions.
- `Hashing.c`: Logic for runtime API resolution.
