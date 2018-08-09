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
#define CLOCK_MATH(int) ((1.0/CLOCK_HZ) * 1000)

struct chip8_registers
{
	uint8_t V[16];

	uint16_t I;
	
	uint8_t DT,
		ST;

	uint16_t PC,
		 SP;
};


int cleanup(){
	endwin();
	//free();
	return 0;
}

void delay(int number_of_milli)
{
    // Converting time into milli_seconds
    int milli_seconds = number_of_milli;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds)
        ;
}

uint8_t * loadFile(int argc, char ** argv, int sw){
	int bytep = 0,
	    fsize = 0;
	uint8_t * membuffer;
	
	FILE * fp = fopen(argv[1], "r");

	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	membuffer = (uint8_t*) malloc(sizeof(uint8_t) * (fsize));

	fseek(fp, 0, SEEK_SET);
	if (fread(membuffer, 1, fsize, fp) < fsize) {
		printf("ERROR reading file");
	}
	
	fclose(fp);
	
	if(argc == 1 || fsize <= 1){
		printf("Failed to load file.");
	}
	printf("ROM LOADED\n");
	if(sw){
		while(bytep != fsize){
			printf("%x ", membuffer[bytep]);
			bytep++;
		}
	}
	printf("\n");
	return membuffer;
}

int decompile(uint8_t * lrom){
	int size, 
	    ip=0, 
	    startsWithZ=0;
	uint16_t instr = 0x0,
		 firstN = 0x0,
		 instrCnt = 0x0;
	
	uint8_t	 nib1,
		 nib2,
		 nib3,
		 nib4;
       	size = malloc_usable_size(lrom);
	while(ip < size){
		instr = lrom[ip] << 8 | lrom[ip+1];
	
		// Bytes 1-4 retrieved and stored for easier printing and comparison	
		nib1= instr >> 12,
		nib2= (instr & 0x0f00) >> 8,
		nib3= (instr & 0x00f0) >> 4,
		nib4= instr & 0x000f;

		printf("%#5x  %x%x %x%x    ", instrCnt++, nib1, nib2, nib3, nib4);
		
		switch(nib1){
			// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM for instructions
			case 0x0:
				if 	(nib3 == 0xe && nib4 == 0xe) 	printf("RET");
				else if (nib3 == 0xe) 			printf("CLS");
				else 					printf("SYS %x", instr);
				break;
			case 0x1:
				printf("JP %x", instr & 0x0fff);
				break;
			case 0x2:
				printf("CALL %x", instr & 0x0fff);
				break;
			case 0x3:
				printf("SE V%x, %x", nib2, instr & 0x00ff);
				break;
			case 0x4:
				printf("SNE V%x, %x", nib2, instr & 0x00ff);
				break;
			case 0x5:
				printf("SE V%x, V%x", nib2, nib3);
				break;
			case 0x6:
				printf("LD V%x, %x", nib2, instr & 0x00ff);
				break;
			case 0x7:
				printf("SE V%x, %x", nib2, instr & 0x00ff);
				break;
			case 0x8:
				switch(nib4){
					case 0x0:
						printf("LD V%x, V%x", nib2, nib3);
						break;
					case 0x1:
						printf("OR V%x, V%x", nib2, nib3);
						break;
					case 0x2:
						printf("AND V%x, V%x", nib2, nib3);
						break;
					case 0x3:
						printf("XOR V%x, V%x", nib2, nib3);
						break;
					case 0x4:
						printf("ADD V%x, V%x", nib2, nib3);
						break;
					case 0x5:
						printf("SUB V%x, V%x", nib2, nib3);
						break;
					case 0x6:
						printf("SHR V%x {, V%x}", nib2, nib3); 
						break;
					case 0x7:
						printf("SUBN V%x, V%x", nib2, nib3);
						break;
					case 0xe:
						printf("SHL V%x {, V%x}", nib2, nib3); 
						break;
					default:
						printf("ERROR reading 4th nibble for 0x8 code");
				}
				break;
			case 0x9:
				printf("SNE V%x, V%x", nib2, nib3);
				break;
			case 0xa:
				printf("LD I, %x", instr & 0x0fff);
				break;
			case 0xb:
				printf("JP V0, %x", instr & 0x0fff);
				break;
			case 0xc:
				printf("RND V%x, V%x", nib2, instr & 0x00ff);
				break;
			case 0xd:
				printf("DRW V%x, V%x, %x", nib2, nib3, nib4);
				break;
			case 0xe:
				if 	((nib3 << 4 | nib4) == 0x9e) 	printf("SKP V%x", nib2);
				else if ((nib3 << 4 | nib4) == 0xa1)	printf("SKNP V%x", nib2);
				else					printf("ERROR reading 4th nibble of 0xe OP");
				break;
			case 0xf:
				switch(instr & 0x00ff){
					case 0x07:
						printf("LD V%x, DT", nib2);
						break;
					case 0x0a:
						printf("LD V%x, K", nib2);
						break;
					case 0x15:
						printf("LD DT, V%x", nib2);
						break;
					case 0x18:
						printf("LD ST, V%x", nib2);
						break;
					case 0x1e:
						printf("ADD I, V%x", nib2);
						break;
					case 0x29:
						printf("LD F, V%x", nib2);
						break;
					case 0x33:
						printf("LD B, V%x", nib2);
						break;
					case 0x55:
						printf("LD [I], V%x", nib2);
						break;
					case 0x65:
						printf("LD V%x, [I]", nib2);
						break;
					default:
						printf("ERROR reading 2nd byte of 0xf OP");
				}
				break;
			default:
			      printf("ERROR reading instruction.");
		}
		printf("\n");
		//printf("OP->%x \n", instr);
		ip+=2; // Each instruction is 2 bytes
	}
	return 0;
}


uint8_t getKeyPress(int press){
	uint8_t retVal;
	switch(press){
		case 48:
			retVal = 0;
			break;
		case 49:
			retVal = 1;
			break;
		case 50:
			retVal = 2;
			break;
		case 51:
			retVal = 3;
			break;
		case 52:
			retVal = 4;
			break;
		case 53:
			retVal = 5;
			break;
		case 54:
			retVal = 6;
			break;
		case 55:
			retVal = 7;
			break;
		case 56:
			retVal = 8;
			break;
		case 57:
			retVal = 9;
			break;
		case 97:
			retVal = 0xa;
			break;
		case 98:
			retVal = 0xb;
			break;
		case 99:
			retVal = 0xc;
			break;
		case 100:
			retVal = 0xd;
			break;
		case 101:
			retVal = 0xe;
			break;
		case 102:
			retVal = 0xf;
			break;
		case 27:
			retVal = 99;
		default:
			retVal = 199;
	
	}
	return retVal;
}


WINDOW * startWin(){
	initscr();	
	noecho();
	raw();
	cbreak();
	curs_set(FALSE);
	// 64x32 window size
	keypad(stdscr, TRUE);
	
	int winW = WINDOW_W, // the screen is squished when using ascii so i doubled the width
	    winH = WINDOW_H,
	    winX0 = (LINES/2)-(winH/2),
	    winY0 = (COLS/2)-(winW/2);
	
	WINDOW * window = newwin(winH, winW, winX0, winY0);
	wborder(window,0,0,0,0,ACS_ULCORNER,
				ACS_URCORNER,
				ACS_LLCORNER,
				ACS_LRCORNER);
	wtimeout(window,0);
	keypad(window, TRUE);
	return window;
}

int emulate(uint8_t * lrom){
	uint8_t mem[RAMSIZE] = {0};
	uint16_t stack[16];
	int ands[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
	uint8_t display[WINDOW_W/2][WINDOW_H] = {0};
	uint8_t key[16] = {0};

	uint16_t instr,
		 displayB=0x0f00,
		 displayT=0x0fff,
		 stackB=0x0ea0,
		 stackT=0x0eff,
		 add,
		 sub,
		 size;
	
	uint8_t	 nib1,
		 nib2,
		 nib3,
		 nib4,
		 byte,
		 r,
		 pixel,
		 press;

	int i, x_coord, y_coord;
	
	clock_t startTime;

	struct chip8_registers *reg= malloc(sizeof(struct chip8_registers*));
	
	int spArrLen = SPRITE_ARR_LEN;
	uint8_t sprite[] = {
		0x0, 0xF0,0x90,0x90,0x90,0xF0,  //0
		0x1, 0x20,0x60,0x20,0x20,0x70,  //1
		0x2, 0xF0,0x10,0xF0,0x80,0xF0,  //2
		0x3, 0xF0,0x10,0xF0,0x10,0xF0,  //3
		0x4, 0x90,0x90,0xF0,0x10,0x10,  //4
		0x5, 0xF0,0x80,0xF0,0x10,0xF0,  //5
		0x6, 0xF0,0x80,0xF0,0x90,0xF0,  //6
		0x7, 0xF0,0x10,0x20,0x40,0x40,  //7
		0x8, 0xF0,0x90,0xF0,0x90,0xF0,  //8
		0x9, 0xF0,0x90,0xF0,0x10,0xF0,  //9
		0xa, 0xF0,0x90,0xF0,0x90,0x90,  //a
		0xb, 0xE0,0x90,0xE0,0x90,0xE0,  //b
		0xc, 0xF0,0x80,0x80,0x80,0xF0,  //c
		0xd, 0xE0,0x90,0x90,0x90,0xE0,  //d
		0xe, 0xF0,0x80,0xF0,0x80,0xF0,  //e
		0xf, 0xF0,0x80,0xF0,0x80,0x80   //f
	};


	srand(time(NULL));

	WINDOW * win = startWin();	


	reg->PC = 0x200;
	reg->SP = stackB;

       	size = malloc_usable_size(lrom);
	memcpy(&mem[reg->PC], lrom, size * sizeof(uint8_t));

	// NOTE: Load sprites
	for (i=0; i<SPRITE_ARR_LEN; i++){
		mem[i] = sprite[i];
	}
	
	startTime = clock();
	printf("STARTING PROGRAM:\n");
	while(reg->PC < RAMSIZE && reg->PC > -1 && (reg->PC <= (0x200 + size)) && wgetch(win) != ESC){
		
		instr = (mem[reg->PC]) << 8 | mem[reg->PC + 1];
		
		// NOTE: Bytes 1-4 retrieved and stored for easier printing and comparison	
		nib1= instr >> 12;
		nib2= (instr & 0x0f00) >> 8;
		nib3= (instr & 0x00f0) >> 4;
		nib4= instr & 0x000f;
		byte= instr & 0x00ff;
		
		press = getKeyPress(wgetch(win));
		switch(press){
			case 99:
				return 0;
			case 199:
				break;
			default:
				for (int i = 0; i < 16; i++) key[i] = 0;
				key[press] = 1;
		}


		switch(nib1){

			// NOTE: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM for instructions
			
			case 0x0:
				if 	(byte == 0x00ee) {
					reg->PC	= mem[reg->SP];
					reg->SP--;
					reg->PC	= reg->PC | (mem[reg->SP] << 8);
					reg->SP--;
				}else if (byte == 0x00e0){
					for (int y = 0; y < WINDOW_H; y++){
						for (int x = 0; x < WINDOW_W/2; x++ ){
							display[x][y] = 0;
						}
					}
				}else {
					reg->PC = instr & 0x0fff;
					reg->PC-=2; 			//  because at the end it inc by 2 everytime, so i need to balance it.
				}
				break;
			case 0x1:
				reg->PC = instr & 0x0fff;
				reg->PC-=2; 				//  because at the end it inc by 2 everytime, so i need to balance it.
				break;
			case 0x2:
				reg->SP++;
				mem[reg->SP] = (reg->PC & 0xff00) >> 8;
				reg->SP++;
				mem[reg->SP] = reg->PC & 0x00ff;
				reg->PC = instr & 0x0fff; 
				reg->PC-=2; 				// because at the end it inc by 2 everytime, so i need to balance it.
				break;
			case 0x3:
				if(reg->V[nib2] == byte)		reg->PC+=2;
				break;
			case 0x4:
				if(reg->V[nib2] != byte)		reg->PC+=2;
				break;
			case 0x5:
				if(reg->V[nib2] == reg->V[nib3])	reg->PC+=2;
				break;
			case 0x6:
				reg->V[nib2] = byte;
				break;
			case 0x7:
				reg->V[nib2] += byte;
				break;
			case 0x8:
				switch(nib4){
					case 0x0:
						reg->V[nib2] = reg->V[nib3];
						break;
					case 0x1:
						reg->V[nib2] |= reg->V[nib3];
						break;
					case 0x2:
						reg->V[nib2] &= reg->V[nib3];
						break;
					case 0x3:
						reg->V[nib2] ^= reg->V[nib3];
						break;
					case 0x4:
						add = reg->V[nib2] + reg->V[nib3];
						reg->V[nib2] = (uint8_t) add;
						reg->V[0xf] = (add > 0xff); 
						break;
					case 0x5:
						sub = reg->V[nib2] - reg->V[nib3];
						reg->V[nib2] = (uint8_t) sub;
						reg->V[0xf] = (sub > 0); 
						break;
					case 0x6:
						reg->V[0xf] = reg->V[nib2] & 0x1;
						reg->V[nib2] >>= 1;
						break;
					case 0x7:
						sub = reg->V[nib3] - reg->V[nib2];
						reg->V[nib2] = (uint8_t) sub;
						reg->V[0xf] = (sub > 0); 
						break;
					case 0xe:
						reg->V[0xf] = ((reg->V[nib2] >> 7) & 0x1);
						reg->V[nib2] <<= 1;
						break;
					default:
						printf("UNKNOWN CMD: %x\n", instr);
						break;
				}
				break;
			case 0x9:
				if(reg->V[nib2] != reg->V[nib3])	reg->PC+=2;
				break;
			case 0xa:
				reg->I = instr & 0x0fff;
				break;
			case 0xb:
				reg->PC = reg->V[0] + (instr & 0x0fff);
				reg->PC-=2; 			//  because at the end it inc by 2 everytime, so i need to balance it.
				break;
			case 0xc:
				r = rand() % 0xff;
				reg->V[nib2] = r & byte;
				break;
			case 0xd:

				// First write into memory
				// FOUND ONLINE
				
				// draws a sprite to the screen
				// uses coordinates stored in VX and VY, with height given by N
				x_coord = reg->V[nib2];
				y_coord = reg->V[nib3];
				
				// because the sprite is represented by hexadecimal numbers
				// bitwise operators are necessary to obtain each pixel

				// set carry flag to 0
				reg->V[0xf] = 0;
				// drawing loop
				for (int i = 0; i < nib4; i++) {
					for (int j = 0; j < 8; j++) {
						// allows sprite to wrap around screen
						if (x_coord + j == WINDOW_W/2) {
							x_coord = -j;
						}
						if (y_coord + i == WINDOW_H) {
							y_coord = -i;
						}

						// set carry flag to 1 if a sprite changes from set to unset
						if ((display[x_coord + j][y_coord + i] == 1) &&
							(((mem[reg->I + i] & ands[j]) >> (8 - j - 1)) == 1)) {
							reg->V[0xf] = 1;
						}

						// bitwise operations decode each bit of sprite and XOR with the current pixel on screen
						display[x_coord + j][y_coord + i] ^= ((mem[reg->I + i] & ands[j]) >> (8 - j - 1));
					}
					x_coord = reg->V[nib2];
					y_coord = reg->V[nib3];
				}
				
				wmove(win,0,0);
				for(int x = 0; x < WINDOW_H; x++){
					for(int y=0; y < WINDOW_W/2; y++){
						if(display[y][x]){
							waddch(win, '[');
							waddch(win, ']');
						}else{
							waddch(win, ' ');
							waddch(win, ' ');
						}
					}
				}
				
				
				wrefresh(win);
				break;
			case 0xe:	
				if (byte == 0x9e) {
					if(key[reg->V[nib2]]) 		
						reg->PC+=2;		
				}	
				else if (byte == 0xa1){
					if(!key[reg->V[nib2]]) 	
						reg->PC+=2;			
				}
				break;
			case 0xf:
				switch(byte){
					case 0x07:
						reg->V[nib2] = reg->DT;
						break;
					case 0x0a:
						;
						uint8_t keyPress = 0; 
						while (keyPress == 0){
							press = wgetch(win);
							keyPress = getKeyPress(press);
							if (keyPress == 99){
								return 0;
							} else if (keyPress != 199){
								reg->V[nib2] = keyPress;
							} else {
								keyPress = 0;
							}
						}
						break;
					case 0x15:
						reg->DT = reg->V[nib2];
						break;
					case 0x18:
						reg->ST = reg->V[nib2];
						break;
					case 0x1e:
						if((reg->I + reg->V[nib2]) > 0xfff) {
							cleanup();
							return 1;
						}								// NOTE: I is not allowed above 0x0fff so we error out of program.
						reg->I += reg->V[nib2];
						break;
					case 0x29:
						
						// NOTE: In the sprite section of memory, before every 5 byte sprite (0-F), is the byte that the next 5 elements represent.
						
						for(i=0; i<spArrLen; i+=6)
							if(reg->V[nib2] == mem[i]) {
								reg->I = (uint16_t) i+1; 			// + SpriteOffset; NOTE: UNUSED because I put the sprite array at mem location 0.
							}
						break;
					case 0x33:	
						mem[reg->I] = (uint8_t)reg->V[nib2] / 100;
						mem[reg->I+1] = ((uint8_t)reg->V[nib2] % 100) / 10;
						mem[reg->I+2] = (((uint8_t)reg->V[nib2] % 100) % 10);
						break;
					case 0x55:
						for(i=0; i<=nib2; i++){ 					// NOTE: OP is inclusive
							mem[reg->I + i] = reg->V[i];
						}
						break;
					case 0x65:
						for(i=0; i<=nib2; i++){ 
							reg->V[i] = mem[reg->I + i]; 
						}
						break;
					default:
						printf("UNKNOWN CMD: %x\n", instr);
						break;
				}
				break;
			default:
				printf("UNKNOWN CMD: %x\n", instr);
				break;
		}
		

		reg->PC+=2; 											// NOTE: Each instruction is 2 bytes
		
		if ((clock() - startTime) >= CLOCK_MATH()) {
        		if(reg->DT > 0)	reg->DT--;
			if(reg->ST > 0) {
				reg->ST--;
				wmove(win, 0, -5);
				wprintw(win, "BEEP");
			}
			startTime = clock();
    		}
		
		delay(1);	
		
		
		
		
		
		
		
		
		
		// Tried to solve timers with children. Became complicated.
		//
		/*
		int time = reg->DT;
		
		int status;
		if(time > 0){
			int pid;
			if (!currently_forking) pid = fork();
			else pid = 1;
			if(pid == 0){
				// Child's only purpose is to manage the timers
				
				while(time != 0) {
					delay(1000/60);
					time -= 1;
				}
				exit(EXIT_SUCCESS);
			}else if (pid > 0){
				currently_forking = 1;
				(void)waitpid(-1, &status, WNOHANG);
				if (WIFEXITED(status)){
					currently_forking = 0;
					reg->DT = 0;
				}

				// Cannot handle case when a chip8 programmer might want to look at the current value of DT to check if it is at a certian point.
			}else{
				
			}

		}*/
	}
	return 0;
}

int main(int argc, char ** argv){
	uint8_t * lrom; 											//NOTE: stands for 'loaded rom'
	int ch;

	lrom = loadFile(argc, argv, 0);

	printf("Welcome to the chip8 emulator.\n");
	while(ch != 3){
		printf("Main menu:\n 0 ... Print out raw rom data\n 1 ... Decompile rom\n 2 ... Run program (Quit with ESC)\n 3 ... Quit\n>>>");
		scanf(" %d", &ch);
		switch(ch){
			case 0:
				loadFile(argc, argv, 1);
				break;
			case 1:
				decompile(lrom);
				break;
			case 2:
				if(emulate(lrom)){
					printf("\nEmulator error...\n");
				}
				cleanup();
				break;
			case 3:
				break;
			default:
				printf("\nUnknown option...\n\n");
		}
	}

	return 0;

}
