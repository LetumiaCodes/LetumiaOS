#include <stdint.h>
#include "letio.h"

#define vgaWidth 80
#define vgaHeight 25
#define VGA_BUFFER ((volatile uint16_t*)0xB8000)

extern void outb(uint16_t port, uint8_t val);
extern uint8_t inb(uint16_t port);

//Multiboot
__attribute__((section(".multiboot"), used))
volatile const uint32_t multiboot_header[] = {
    0x1BADB002,
    0x00000000,
    (uint32_t)-(0x1BADB002 + 0x00000000)
};

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
  outb(0x3D4, 0x0A);
  outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

  outb(0x3D4, 0x0B);
  outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void cursorUpdate(int x, int y) {
  uint16_t pos = y * vgaWidth + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

//Main function... DUHHHHHHH
void kernel_main() {
  int x = 0;
  int y = 7;

  int num = 25;
  
  const char *bootMessage = "Welcome to Letumia OS V_0.05";
  const char *welcomeMessage = "Check github for more updates";
  const char *topBorder = "==========================================";
  const char *warning = "| [WARNING] LetumiaOS is in developement |";
  const char *bottomBorder = "=========================================="; 
  const char *commandParse = "$ -> ";
  
  clear();
  enable_cursor(0, 15);
  println(bootMessage, 0, 0);
  println(welcomeMessage, 0, 1);
  println(topBorder, 0, 2);
  println(warning, 0, 3);
  println(bottomBorder, 0, 4);
  println(commandParse, 0, 6);
  cursorUpdate(6, 6);
  
  while(1) {}
}
