#include <stdint.h>

extern "C" {
__attribute__((section(".multiboot"), used))
volatile const uint32_t multiboot_header[] = {
    0x1BADB002,                    
    0x00000000,                     
    (uint32_t)-(0x1BADB002 + 0x0)   
};
}

extern "C" void print(const char* str) {
    volatile char* VGA = (volatile char*)0xB8000;
    int i = 0;

    while (str[i] != '\n') {
        VGA[i * 2] = str[i];
        VGA[i * 2 + 1] = 0x07;
    }
}

extern "C" void kernel_main() {
    print("Welcome to Cunix");

    while(1) {}
}
