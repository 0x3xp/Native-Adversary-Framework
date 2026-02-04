# Cryptography & Payload Obfuscation

### ❓ Why this folder?
In offensive security, static signatures are the enemy. This folder contains the tools and logic required to encrypt strings, shellcode, and configuration data to bypass signature-based detection (AV/EDR).

### 📦 What's included?
* **XOR_Obfuscator.c**: A C-based utility to encrypt payloads using symmetric XOR logic.
* **Decryption Stubs**: Lightweight functions designed to be embedded into loaders to decrypt payloads in memory just before execution.
* **Entropy Management**: Code focused on breaking common patterns in binaries that scanners look for.
