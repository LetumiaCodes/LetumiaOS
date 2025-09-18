#include <stdint.h>
#include "letio.h"

#define vgaWidth 80
#define vgaHeight 25

//clear function
void clear() {
  volatile unsigned short* VGA = (unsigned short*)0xB8000;
  for (int i = 0; i < vgaWidth * vgaHeight; ++i) {
     VGA[i] = 0x0720;
   }
 }

//print function
void println(const char* str, int x, int y) {  
  volatile char* VGA = (volatile char*)0xB8000;
   for (int i = 0; str[i] != '\0'; ++i) {     
     int offset = (y * vgaWidth + x + i) * 2;

         if (x + i >= vgaWidth || y >= vgaHeight) {
             break;
         }
                
     VGA[offset] = str[i];
     VGA[offset + 1] = 0x07;
   }
 }
