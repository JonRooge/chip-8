#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ncurses.h>


#define RAMSIZE 4096
#define WINDOW_W 128
#define WINDOW_H 32 
#define ESC 27
#define SPRITE_ARR_LEN 96
#define CLOCK_HZ 60
#define MOD 1		// This is here to make the hz more accurate (unused)
#define CLOCK_MATH(int) ((1.0/CLOCK_HZ) * MOD)


int cleanup();

int emulate(uint8_t * lrom, int fsize, int wrap);

int decompile(uint8_t * lrom, int fsize);
