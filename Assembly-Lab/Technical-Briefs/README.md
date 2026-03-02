# Technical Briefs

This directory contains the theoretical summaries and research notes for the Assembly Lab. Each brief focuses on a specific architectural concept or a Windows-specific low-level requirement.

## 📋 Research Modules

* **Architecture Fundamentals:** Documenting the transition from x86 to x64, including register expansion and new instruction sets.
* **The Win64 ABI:** Comprehensive notes on the Microsoft x64 calling convention, detailing how parameters are passed via RCX, RDX, R8, and R9.
* **Stack & Shadow Space:** Analysis of the 32-byte shadow space requirement and 16-byte stack alignment necessary for stable API execution.
* **Control Flow Analysis:** Breakdown of how the CPU handles branching, jumps, and procedure calls at the opcode level.

## 🎯 Objective

The goal of these briefs is to provide a "Theory First" approach. By documenting the mechanics here, the logic in the **Native-Adversary-Framework** becomes easier to debug and more resilient against detection.