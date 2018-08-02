#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define RAMSIZE 4096

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
		 instrCnt = 0x0,
		 nib1,
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

int main(int argc, char ** argv){
	uint8_t * lrom; //loaded rom

	lrom = loadFile(argc, argv);
	decompile(lrom);

	return 0;

}
