#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define RAMSIZE 4096

struct chip8_registers
{
	uint8_t V[16];

	uint16_t I;
	
	uint8_t DT,
		ST;

	uint16_t PC;
	uint8_t  SP;
};



uint8_t * loadFile(int argc, char ** argv){
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
	printf("ROM BYTE CODE:\n");
	while(bytep != fsize){
		printf("%x ", membuffer[bytep]);
		bytep++;
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



int emulate(uint8_t * lrom){
	uint8_t mem[RAMSIZE];
	uint16_t stack[16];
	
	uint16_t instr,
		 displayB=0xf00,
		 displayT=0xfff,
		 stackB=0xea0,
		 stackT=0xeff,
		 add,
		 sub;
	
	uint8_t	 size, 
		 nib1,
		 nib2,
		 nib3,
		 nib4,
		 byte,
		 r;

	int i;

	struct chip8_registers *reg;
	
	srand(time(NULL));

	// END OF DECL
	// ------------------------------------------------
	
	reg->PC = 0x200;
	reg->SP = stackB;

       	size = malloc_usable_size(lrom);
	memcpy(&mem[reg->PC], lrom, size * sizeof(uint8_t));

	/*
	if(loadIntoMemory(lrom, &mem)) {
		printf("Could not load into memory\n");
		return 1;	
	}*/

	printf("STARTING PROGRAM:\n");
	while(reg->PC < RAMSIZE && reg->PC > -1 && reg->PC <= 0x200 + size){
		instr = (mem[reg->PC]) << 8 | mem[reg->PC + 1];
	
		// Bytes 1-4 retrieved and stored for easier printing and comparison	
		nib1= instr >> 12;
		nib2= (instr & 0x0f00) >> 8;
		nib3= (instr & 0x00f0) >> 4;
		nib4= instr & 0x000f;
		byte= instr & 0x00ff;
		
		switch(nib1){
			// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM for instructions
			case 0x0:
				if 	(byte == 0x00ee) {
					reg->PC	= reg->SP;
					reg->SP--;
				}
				else if (byte == 0x00e0)	printf("CLS");
				else 				reg->I = instr & 0x0fff;
				break;
			case 0x1:
				reg->PC = instr & 0x0fff;
				break;
/*FIX*/			case 0x2:
				reg->SP++;
				mem[reg->SP] = (reg->PC & 0xff00) >> 8;
				reg->SP++;
				mem[reg->SP] = reg->PC & 0x00ff;


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
						reg->V[0xf] &= 0x1;
						reg->V[nib2] >>= 1;
						break;
					case 0x7:
						sub = reg->V[nib3] - reg->V[nib2];
						reg->V[nib2] = (uint8_t) sub;
						reg->V[0xf] = (sub > 0); 
						break;
					case 0xe:
						reg->V[0xf] = (reg->V[0xf] >> 7 & 0x1);
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
				break;
			case 0xc:
				r = rand() % 0xff;
				reg->V[nib2] = r & byte;
				break;
			case 0xd:
// INCOMPLETE
				
				break;
			case 0xe:
// INCOMPLETE			
				if (byte == 0x9e) {
					if(reg->V[nib2] == ) reg->PC+=2;		
				}	
				else if (byte == 0xa1){
					reg->PC+=2;			
				}
				break;
			case 0xf:
				switch(byte){
					case 0x07:
						reg->V[nib2] = reg->DT;
						break;
					case 0x0a:
						reg->V[nib2] = (uint8_t) getchar();
						break;
					case 0x15:
						reg->DT = reg->V[nib2];
						break;
					case 0x18:
						reg->ST = reg->V[nib2];
						break;
					case 0x1e:
						if(reg->I + reg->V[nib2] > 0xfff) return 1; // I is not allowed above 0x0fff so we error out of program.
						reg->I += reg->V[nib2];
						break;
					case 0x29:
// INCOMPLETE						reg->I = *reg->V[nib2]; //this is a guess, I need to return to this
						break;
					case 0x33:
// INCOMPLETE						
						break;
					case 0x55:
						for(i=0; i<reg->V[nib2]; i++){ // Is this OP inclusive?
							mem[reg->I + i] = reg->V[i];
						}
						break;
					case 0x65:
						for(i=0; i<reg->V[nib2]; i++){ 
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
		reg->PC+=2; // Each instruction is 2 bytes
	}
	return 0;
}

int main(int argc, char ** argv){
	uint8_t * lrom; //loaded rom

	lrom = loadFile(argc, argv);
	decompile(lrom);
	emulate(lrom);

	return 0;

}
