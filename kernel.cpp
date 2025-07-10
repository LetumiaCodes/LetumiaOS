#include <stdint.h>

extern "C" {
__attribute__((section(".multiboot"), used))
volatile const uint32_t multiboot_header[] = {
    0x1BADB002,                    
    0x00000000,                     
    (uint32_t)-(0x1BADB002 + 0x0)   
};
}

extern "C" void ClearScreen() {
    volatile unsigned short* VGA = (unsigned short*)0xB8000;
    for (int i = 0; i < 80 * 25; ++i) {
        VGA[i] = 0x0720;
    }
}

extern "C" void print(const char* str) {
    volatile char* VGA = (volatile char*)0xB8000;
    for (int i = 0; str[i] != '\0'; ++i) {
        VGA[i * 2] = str[i];
        VGA[i * 2 + 1] = 0x07;
    }
}

extern "C" void kernel_main() {
    ClearScreen();
    print("Welcome to NovaKernel v0.01");

    while(1) {}
}
