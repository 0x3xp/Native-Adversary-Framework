# C2 Socket Framework: Cross-Platform Communication (C)

### Author: Piyusha Akash (0x3xp)
**Portfolio:** [0x3xp.github.io](https://0x3xp.github.io) | **GitHub:** [github.com/0x3xp](https://github.com/0x3xp)

---
```md
Author: Piyusha Akash (0x3xp) | https://0x3xp.github.io
GitHub: https://github.com/0x3xp
Project: Basic C2 Socket Framework (Research & Education)
Disclaimer: This code is for educational purposes and authorized security testing only.
```
---

## 📌 Project Overview
This project demonstrates the fundamental building blocks of a **Command & Control (C2) architecture** using low-level C socket programming. It features a bidirectional communication link between a **Linux-based Server** (Handler) and a **Windows-based Client** (Implant).

By bypassing high-level abstractions, this framework provides direct control over memory, buffers, and network protocols, which is essential for developing custom offensive security tools.

## 🛠️ Technical Implementation
* **Server (Linux/Debian):** Utilizes POSIX sockets (`sys/socket.h`) to bind to a local IP and listen for incoming connections.
* **Client (Windows):** Implements the Windows Socket API (**Winsock2**) to establish a remote connection.
* **Protocol:** TCP (SOCK_STREAM) for reliable, ordered data delivery.
* **Cross-Platform Handling:** Manages the architectural differences between Linux and Windows networking stacks (e.g., `WSAStartup` vs. standard socket initialization).

---

## 🚀 Getting Started

### 1. Server Setup (Linux)
Compile the server using `gcc`:
```bash
gcc server.c -o server
./server
