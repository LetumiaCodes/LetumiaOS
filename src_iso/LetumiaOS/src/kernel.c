#include <stdint.h>
#include "letio.h"

#define vgaWidth 80
#define vgaHeight 25
#define VGA_BUFFER ((volatile uint16_t*)0xB8000)

//Multiboot
__attribute__((section(".multiboot"), used))
volatile const uint32_t multiboot_header[] = {
    0x1BADB002,
    0x00000000,
    (uint32_t)-(0x1BADB002 + 0x00000000)
};

//Main function... DUHHHHHHH
void kernel_main() {
  const char *bootMessage = "Welcome to Letumia OS V_0.05";
  const char *welcomeMessage = "Check github for more updates";
  const char *topBorder = "==========================================";
  const char *warning = "| [WARNING] LetumiaOS is in developement |";
  const char *bottomBorder = "=========================================="; 
  const char *commandParse = "$ -> ";
  
  clear();
  println(bootMessage, 0, 0);
  println(welcomeMessage, 0, 1);
  println(topBorder, 0, 2);
  println(warning, 0, 3);
  println(bottomBorder, 0, 4);
  println(commandParse, 0, 6);
  
  while(1) {}
}
