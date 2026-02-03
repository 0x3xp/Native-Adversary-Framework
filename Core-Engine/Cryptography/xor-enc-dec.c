#include <stdio.h>
#include <string.h>

int main() {
    
    char text[256] = "";
    char key = 'K';
    char EncryptedOutPut[256] = "";

    printf("Enter the text: ");
    fgets(text, sizeof(text), stdin);
    text[strlen(text) - 1 ] = '\0';

    int _buff_len_ = strlen(text); 

    for(int i = 0; i < _buff_len_; i++){
        EncryptedOutPut[i] = text[i] ^ key; 
        printf("0x%02x", EncryptedOutPut[i]);
        if(i < strlen(text) - 1) printf(", ");
    }

    printf("\n");

    for(int i = 0; i < _buff_len_; i++){
        char DecryptedPutPut = EncryptedOutPut[i] ^ key;
        printf("%c", DecryptedPutPut); 
    }

    return 0;
}
