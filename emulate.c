/*	Written by: Jonathan Ruggiero
 *  Date: 8/9/18
 *
 */
 
 #include "chip8.h"

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
	//free(reg);
	return 0;
}

WINDOW * startWin(){
	WINDOW * window;
	if (!initscr()) {
		window = NULL;
		return window;
	}	
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
	
	window = newwin(winH, winW, winX0, winY0);
	wborder(window,0,0,0,0,ACS_ULCORNER,
				ACS_URCORNER,
				ACS_LLCORNER,
				ACS_LRCORNER);
	wtimeout(window,0);
	keypad(window, TRUE);
	return window;
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
			break;
		default:
			retVal = 199;
	
	}
	return retVal;
}


int emulate(uint8_t * lrom, int fsize){
	uint8_t mem[RAMSIZE] = {0},
			key[16] = {0},
			display[WINDOW_W/2][WINDOW_H] = {0},
			nib1,
			nib2,
			nib3,
			nib4,
			byte,
			r,
			pixel,
			press;
	
	int ands[8] = { 128, 64, 32, 16, 8, 4, 2, 1 },
		i,
		x_coord,
		y_coord,
		tc,
		spArrLen = SPRITE_ARR_LEN;

	uint16_t instr,
		 displayB=0x0f00,
		 displayT=0x0fff,
		 stackB=0x0ea0,
		 stackT=0x0eff,
		 add,
		 sub,
		 stack[16];

	struct timespec * tp = malloc(sizeof(struct timespec*));
	
	struct chip8_registers *reg= malloc(sizeof(struct chip8_registers*));
	//uint8_t emptyReg[16] = {0};
	//memcpy(reg->V, emptyReg, sizeof(emptyReg));
	//*reg = {0}
	
	int test1 = sizeof(reg);
	int test2 = sizeof(*reg);
	
	memset(reg, 0, sizeof(*reg));
	
	
	
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
	if (!win) {
		return 2;	
	}

	reg->PC = 0x200;
	reg->SP = stackB;

	void * test = memcpy(&mem[reg->PC], lrom, fsize * sizeof(uint8_t));
	if(test == NULL) return 4;

	// NOTE: Load sprites
	for (i=0; i<SPRITE_ARR_LEN; i++){
		mem[i] = sprite[i];
	}
	
	time_t startSec;
	long startNan;
	clock_gettime(CLOCK_REALTIME, tp);
	startSec = tp->tv_sec;
	startNan = tp->tv_nsec;
	
	printf("STARTING PROGRAM:\n");
	while(reg->PC < RAMSIZE && reg->PC > -1 && (reg->PC <= (0x200 + fsize))){

		instr = (mem[reg->PC]) << 8 | mem[reg->PC + 1];
		
		// NOTE: Bytes 1-4 retrieved and stored for easier printing and comparison	
		nib1= instr >> 12;
		nib2= (instr & 0x0f00) >> 8;
		nib3= (instr & 0x00f0) >> 4;
		nib4= instr & 0x000f;
		byte= instr & 0x00ff;
		
		int new_key = wgetch(win);
		if(new_key != ERR){
		
			press = getKeyPress(new_key);
			switch(press){
				case 99:
					return 0;
				case 199:
					break; 		
				default:
					key[press] = 1;
			}
			press = 199;
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
				} /*else {
					reg->PC = instr & 0x0fff;
					reg->PC-=2; 			//  because at the end it inc by 2 everytime, so i need to balance it.
				}*/
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
			case 0x7: {
				reg->V[nib2] += byte;
				break;
			}
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
						return 3;
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
			case 0xd: {

				// First write into memory
				// FOUND ONLINE
				
				// draws a sprite to the screen
				// uses coordinates stored in VX and VY, with height given by N
				// 
				// Converting to signed
				//int8_t signx = reg->V[nib2];
				//int8_t signy = reg->V[nib3];
				x_coord = reg->V[nib2];
				y_coord = reg->V[nib3];

				// because the sprite is represented by hexadecimal numbers
				// bitwise operators are necessary to obtain each pixel

				// set carry flag to 0
				reg->V[0xf] = 0;
				// drawing loop
				for (int i = 0; i < nib4; i++) {
					//y_coord = (y_coord + i) % WINDOW_H;
					for (int j = 0; j < 8; j++) {
						// allows sprite to wrap around screen
						// set carry flag to 1 if a sprite changes from set to unset
						if ((display[(x_coord + j) % (WINDOW_W/2)][(y_coord + i) % WINDOW_H] == 1) &&
							(((mem[reg->I + i] & ands[j]) >> (8 - j - 1)) == 1)) {
							reg->V[0xf] = 1;
						}

						// bitwise operations decode each bit of sprite and XOR with the current pixel on screen
						display[(x_coord + j) % (WINDOW_W/2)][(y_coord + i) % WINDOW_H] ^= ((mem[reg->I + i] & ands[j]) >> (8 - j - 1));
					}
					x_coord = reg->V[nib2];
					y_coord = reg->V[nib3];
				}
				
				if(wmove(win,0,0) == ERR) return 5;
				for(int y = 0; y < WINDOW_H; y++){
					for(int x=0; x < WINDOW_W/2; x++){
						if(display[x][y]){
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
			}
			case 0xe:	
				
				if (byte == 0x9e) {
					if(key[reg->V[nib2]]) {		
						reg->PC+=2;
						key[reg->V[nib2]] = 0;
					}		
				}	
				else if (byte == 0xa1){
					if(!key[reg->V[nib2]]) 	{
						reg->PC+=2;	
					}else{
						key[reg->V[nib2]] = 0;
						
					}		
				}else{
					return 3;
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
						return 3;
				}
				break;
			default:
				return 3;
		}
		

		reg->PC+=2; 											// NOTE: Each instruction is 2 bytes
		
		clock_gettime(CLOCK_REALTIME, tp);
		if (((tp->tv_sec - startSec) + (tp->tv_nsec - startNan)/1E9) >= CLOCK_MATH()) {
        		if(reg->DT > 0)	reg->DT--;
			if(reg->ST > 0) {
				reg->ST--;
				wmove(win, 0, 0);
				wprintw(win, "BEEP");
			}
			clock_gettime(CLOCK_REALTIME, tp);
    		startSec = tp->tv_sec;
			startNan = tp->tv_nsec;
		}

		usleep(2500);
		
			
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
