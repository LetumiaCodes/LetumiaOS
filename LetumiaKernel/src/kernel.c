//Headers + Custom headers
#include <stdint.h>
#include <stdbool.h>
#include "letio.h"

//VGA
#define vgaWidth 80
#define vgaHeight 25
#define VGA_BUFFER ((volatile uint16_t*)0xB8000)

//Keyboard
#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64

#define INPUT_BUF_SIZE 128

bool process_1 = true; //Shutdown!!!
bool shift_pressed = false;
bool bf_input_mode = false;

char input[INPUT_BUF_SIZE];
int input_pos = 0;

extern void outb(uint16_t port, uint8_t val);
extern uint8_t inb(uint16_t port);

char scancode_to_ascii(uint8_t scancode); 

char get_key_blocking() {
    while (1) {
        uint8_t status = inb(0x64);
        if (status & 1) {
            uint8_t scancode = inb(0x60);

            if (scancode == 0x2A || scancode == 0x36) {  
                shift_pressed = true;
                continue;
            } else if (scancode == 0xAA || scancode == 0xB6) {  
                shift_pressed = false;
                continue;
            }

            if (!(scancode & 0x80)) {  
                return scancode_to_ascii(scancode);
            }
        }
    }
}

//Multiboot
__attribute__((section(".multiboot"), used))
volatile const uint32_t multiboot_header[] = {
    0x1BADB002,
    0x00000000,
    (uint32_t)-(0x1BADB002 + 0x00000000)
};


void scroll() {
    for (int row = 1; row < vgaHeight; row++) {
        for (int col = 0; col < vgaWidth; col++) {
            VGA_BUFFER[(row - 1) * vgaWidth + col] = VGA_BUFFER[row * vgaWidth + col];
        }
    }

    for (int col = 0; col < vgaWidth; col++) {
        VGA_BUFFER[(vgaHeight - 1) * vgaWidth + col] = ' ' | (0x07 << 8);
    }
}

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

char scancode_to_ascii(uint8_t scancode) {
    static const char map[128] = {
        0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
        'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
        'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' ', 
    };

    static const char shift_map[128] = {
        0,  27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
        '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
        'A','S','D','F','G','H','J','K','L',':','"','~', 0,'|',
        'Z','X','C','V','B','N','M','<','>','?', 0,'*', 0,' ',
    };
    
    if (scancode < 128) {
        return shift_pressed ? shift_map[scancode] : map[scancode];
    }
    return 0;
}


int letstr(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}


void run_brainfuck(const char* code, int* y) {
    unsigned char tape[3000] = {0};
    unsigned char* ptr = tape;
    const char* pc = code;

    while (*pc) {
        switch (*pc) {
            case '>': ptr++; break;
            case '<': ptr--; break;
            case '+': ++(*ptr); break;
            case '-': --(*ptr); break;
            case '.':
                {
                    char s[2] = {*ptr, '\0'};
                    if (*y >= vgaHeight) {
                        scroll();
                        *y = vgaHeight - 1;
                    }
                    println(s, 0, *y);
                    (*y)++;
                }
                break;
                case ',':
                {
                  cursorUpdate(0, *y);
                  
                  char c = get_key_blocking();
                  *ptr = c;
                }
                break;  
                case '[':
                if (!*ptr) {
                    int loop = 1;
                    while (loop > 0) {
                        pc++;
                        if (*pc == '[') loop++;
                        else if (*pc == ']') loop--;
                    }
                }
                break;
            case ']':
                if (*ptr) {
                    int loop = 1;
                    while (loop > 0) {
                        pc--;
                        if (*pc == '[') loop--;
                        else if (*pc == ']') loop++;
                    }
                }
                break;
        }
        pc++;
    }
}

//Main function... DUHHHHHHH
void kernel_main() {
  int x = 12;
  int y = 6;
  
  const char *bootMessage = "Welcome to LetumiaKernel";
  const char *welcomeMessage = "Check github for more updates";
  const char *topBorder = "==============================================";
  const char *warning = "| [WARNING] LetumiaKernel is in developement |";
  const char *bottomBorder = "=============================================="; 
  const char *commandParse = "$letumia/# ";
  
  clear();
  enable_cursor(0, 15);
  println(bootMessage, 0, 0);
  println(welcomeMessage, 0, 1);
  println(topBorder, 0, 2);
  println(warning, 0, 3);
  println(bottomBorder, 0, 4);
  println(commandParse, 0, y);
  cursorUpdate(x, y);

  
while (process_1) {
  
    char c = get_key_blocking();

    if (c == '\b') {
        if (input_pos > 0) {
            input_pos--;
            x--;
            println(" ", x, y);    
            cursorUpdate(x, y);
        }
    } else if (c == '\n') {
        input[input_pos] = '\0';  

        if (letstr(input, "help")) {
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("end <process> | Kills process", 0, y);
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("str <process> | Start process", 0, y);
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("crash -cpu    | Stress test CPU", 0, y);
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("devs          | The team that made this project possible", 0, y);
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("bf            | Brainfuck interpreter", 0, y);      
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("opri          | Kernel info", 0, y);
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
        } else if (letstr(input, "devs")) {
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("Team: Letumia (Developer) & madhav (Contributer)", 0, y);
        } else if (letstr(input, "end process1")) {
          process_1 = false;
        } else if (letstr(input, "crash -cpu")) {
          const char *crash = "crashTest";
          
          while (1) {
            y++;
            if (y >= vgaHeight) {
              scroll();
              y = vgaHeight - 1;
            }
            println(crash, 0, y);
          }
        } else if (letstr(input, "opri")) {
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("About: ", 0, y);
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("LetumiaKernel version: v0.15", 0, y);
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          println("Last updated: September 17th 2025", 0, y);
        } else if (letstr(input, "clear")) {
          clear();
          input_pos = 0;
        }  else if (!bf_input_mode && letstr(input, "bf")) {
          bf_input_mode = true;
          input_pos = 0;
          clear();
          x = 5;
          y = 0;
          println(">>> ", 0, y);
          cursorUpdate(x, y);

        } else if (bf_input_mode) {
          input[input_pos] = '\0';
          bf_input_mode = false;

          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          
          println("output:", 0, y++);
          run_brainfuck(input, &y);

          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          
          println(commandParse, 0, y);
          x = 12;
          cursorUpdate(x, y);
          input_pos = 0;

          continue;          
        } else {
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          x = 12;
          println("Illegal command", 0, y);
        }
        if (!bf_input_mode) {
          y++;
          if (y >= vgaHeight) {
            scroll();
            y = vgaHeight - 1;
          }
          x = 12;
          println(commandParse, 0, y);
          cursorUpdate(x, y);
          input_pos = 0;
        }       
    } else if (c && input_pos < INPUT_BUF_SIZE - 1) {
        input[input_pos] = c;
        input_pos++;

        char s[2] = {c, '\0'};
        println(s, x, y);
        x++;
        cursorUpdate(x, y);

        if (x >= vgaWidth) {   
            x = bf_input_mode ? 5 : 12;
            y++;
            if (y >= vgaHeight) {
              scroll();
              y = vgaHeight - 1;
            }
            cursorUpdate(x, y);
        }
      }
    }
}

