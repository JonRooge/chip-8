#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ncurses.h>
//#include <SDL/SDL.h>

#define RAMSIZE 4096
#define ESC 27

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

WINDOW * startWin(){
	initscr();	
	noecho();
	cbreak();
	//curs_set(FALSE);
	// 64x32 window size
	//	printf("%d, %d, %d, %d\n", LINES, COLS, winX0, winY0);
	//keypad(stdscr, TRUE);
	
	int winW = 64,
	    winH = 32,
	    winX0 = (LINES/2)-(winH/2),
	    winY0 = (COLS/2)-(winW/2);
	
	//WINDOW * box = newwin(winH+2, winW+2, winX0-2, winY0-2);
	WINDOW * window = newwin(winH, winW, winX0, winY0);
	wborder(window,0,0,0,0,ACS_ULCORNER,
				ACS_URCORNER,
				ACS_LLCORNER,
				ACS_LRCORNER);
	wtimeout(window,0);
	//nodelay(TRUE)
	return window;
}

int emulate(uint8_t * lrom){
	uint8_t mem[RAMSIZE];
	uint16_t stack[16];
	
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
		 pixel;

	int i;
	int8_t key;

	struct chip8_registers *reg= malloc(sizeof(struct chip8_registers*));
	
	int spArrLen = 96;
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

	// END OF DECL
	// ------------------------------------------------
	

	WINDOW * win = startWin();	


	reg->PC = 0x200;
	reg->SP = stackB;

       	size = malloc_usable_size(lrom);
	memcpy(&mem[reg->PC], lrom, size * sizeof(uint8_t));

	// NOTE: Load sprites
	for (i=0; i<80; i++){
		mem[i] = sprite[i];
	}
	
	printf("STARTING PROGRAM:\n");
	while(reg->PC < RAMSIZE && reg->PC > -1 && (reg->PC <= (0x200 + size)) && wgetch(win) != ESC){
		instr = (mem[reg->PC]) << 8 | mem[reg->PC + 1];
		
		/* DEBUG */
		
		//printf("Instruction: %x\nPC: %x\nSP: %x\n", instr, reg->PC, reg->SP);
		//getchar();
		
		/* END DEBUG */

		// NOTE: Bytes 1-4 retrieved and stored for easier printing and comparison	
		nib1= instr >> 12;
		nib2= (instr & 0x0f00) >> 8;
		nib3= (instr & 0x00f0) >> 4;
		nib4= instr & 0x000f;
		byte= instr & 0x00ff;
		
		switch(nib1){

			// NOTE: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM for instructions
			
			case 0x0:
				if 	(byte == 0x00ee) {
					reg->PC	= mem[reg->SP];
					reg->SP--;
					reg->PC	= reg->PC | (mem[reg->SP] << 8);
					reg->SP--;
				}
				else if (byte == 0x00e0)	wclear(win);
				else {
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

				// Found online
				// First write into memory
				reg->V[0xf] = 0;
				for (int y=0; y<nib4; y++){
					pixel = mem[reg->I + y];
					for (int x=0; x<8; x++){
						if((pixel & (0x80 >> x)) != 0){
							if(mem[displayB + (reg->V[nib2] + x) + (reg->V[nib3] + y)] == 1){
								reg->V[0xf] = 1;
							}
							mem[displayB + (reg->V[nib2] + x) + (reg->V[nib3] + y)] ^= 1;
						}
					}
				}
				//Draw memory to screen
				
				wmove(win,0,0);
				for(int loc = displayB; loc <= displayT; loc++){
					for(int bit=0; bit < 8; bit++){
						if(mem[loc] & (0x80 >> bit) == 1)
							waddch(win, ACS_BLOCK);
						else
							waddch(win, ' ');
						wrefresh(win);
						delay(30);
					}
				}
				
				
				/*for (int a=displayB; a<displayT; a+=64){
					for (int b=0; b<64; b++){
						if(mem[a + b] == 1)
							mvwaddch(win, (a-displayB)/64, b, ACS_BLOCK);
						else
							mvwaddch(win, (a-displayB)/64, b, ' ');
					}
				}*/
				wrefresh(win);
				break;
			case 0xe:	
				if (byte == 0x9e) {
					key = wgetch(win);
					if(key != ERR && key == reg->V[nib2]) 	reg->PC+=2;		
				}	
				else if (byte == 0xa1){
					key = wgetch(win);
					if(key != ERR && key != reg->V[nib2]) 	reg->PC+=2;			
				}
				break;
			case 0xf:
				switch(byte){
					case 0x07:
						reg->V[nib2] = reg->DT;
						break;
					case 0x0a:
						wtimeout(win, -1);	
						reg->V[nib2] = (uint8_t) wgetch(win);
						wtimeout(win, 0);
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
						mem[reg->I+2] = (((uint8_t)reg->V[nib2] & 100) % 10);
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
		

		//getchar();
		reg->PC+=2; 											// NOTE: Each instruction is 2 bytes
		delay(50);
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
