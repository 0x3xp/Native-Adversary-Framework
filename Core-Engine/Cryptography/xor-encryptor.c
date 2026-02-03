#include <stdio.h>
#include <string.h>

int main() {
    char url[] = "C:\\Users\\Public\\SKULL.exe"; //Replace any string with this
    char key = 'E';
    int len = strlen(url);

    printf("Encrypted URL: { ");
    for (int i = 0; i < len; i++) {
        printf("0x%02x", (unsigned char)(url[i] ^ key));
        if (i < len - 1) printf(", ");
    }
    printf(" };\n");
    printf("Length: %d\n", len);

    return 0;
}
