import sys

# ANSI Color Codes for Terminal
GREEN = "\033[1;32m"
YELLOW = "\033[1;33m"
CYAN = "\033[1;36m"
RED = "\033[1;31m"
RESET = "\033[0m"

def hydra_style_cracker():
    print(f"{CYAN}=== ADVANCED ASSEMBLY HEX CRACKER (HYDRA-MODE) ==={RESET}")
    print("Enter hex chunks (e.g. 7B175614497B5D49). Type 'done' to crack.\n")

    hex_chunks = []
    while True:
        try:
            user_input = input(f"{YELLOW}[+]{RESET} Enter Chunk {len(hex_chunks) + 1}: ").strip().lower()
            if user_input == 'done': break
            
            # Clean '0x', 'h', and spaces
            clean = "".join(c for c in user_input if c in "0123456789abcdef")
            if len(clean) % 2 != 0:
                print(f"{RED}[!] Error: Hex length must be even (full bytes).{RESET}")
                continue
            hex_chunks.append(clean)
        except KeyboardInterrupt:
            print("\nExiting..."); return

    # 1. Process Little-Endian (Flip each chunk)
    full_bytes = bytearray()
    for chunk in hex_chunks:
        b = bytearray.fromhex(chunk)
        b.reverse() # The Little-Endian Flip
        full_bytes.extend(b)

    print(f"\n{CYAN}[*] Attempting Brute-Force on {len(full_bytes)} bytes...{RESET}\n")

    # 2. Brute Force XOR
    results_found = 0
    for key in range(256):
        decoded_chars = []
        printable_count = 0
        
        for b in full_bytes:
            char_code = b ^ key
            # Printable ASCII range: 32 (space) to 126 (~)
            if 32 <= char_code <= 126:
                decoded_chars.append(chr(char_code))
                printable_count += 1
            else:
                decoded_chars.append(".") # Mask non-printable
        
        decoded_str = "".join(decoded_chars)
        score = (printable_count / len(full_bytes)) * 100

        # 3. Highlighting Logic (Like Hydra)
        if score > 80:  # If 80% or more is readable text
            results_found += 1
            print(f"{GREEN}[SUCCESS] Key: 0x{key:02X} | Score: {score:3.0f}% | Password: {decoded_str}{RESET}")
        elif score > 40: # Potential lead
            print(f"{YELLOW}[CHECK]   Key: 0x{key:02X} | Score: {score:3.0f}% | Fragment: {decoded_str}{RESET}")

    if results_found == 0:
        print(f"{RED}[!] No valid passwords found. Check your Endianness or Hex input.{RESET}")
    else:
        print(f"\n{CYAN}[*] Crack complete. {results_found} potential password(s) found.{RESET}")

if __name__ == "__main__":
    hydra_style_cracker()
