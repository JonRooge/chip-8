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
		 instrCnt = 0x0;
       	size = malloc_usable_size(lrom);
	while(ip < size){
		instr = lrom[ip] << 8 | lrom[ip+1];
		
		
		printf("%x  %x %x    ", instrCnt++, instr >> 2, instr & 0x00ff);
	
		// Bytes 1-4 retrieved and stored for easier printing and comparison	
		uint8_t nib1= instr >> 12,
			nib2= (instr & 0x0f00) >> 8,
			nib3= (instr & 0x00f0) >> 4,
			nib4= instr & 0x000f;
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
						printf("SHR V%x {, V%x}", nib2, nib3); // <<<<?
						break;
					case 0x7:
						printf("SUBN V%x, V%x", nib2, nib3);
						break;
					case 0xe:
						printf("SHL V%x {, V%x}", nib2, nib3); // <<<<?
						break;
					default:
						printf("ERROR reading 4th nibble for 8th code");
				}
				break;
			case 0x9:
				break;
			case 0xa:
				break;
			case 0xb:
				break;
			case 0xc:
				break;
			case 0xd:
				break;
			case 0xe:
				break;
			case 0xf:
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
