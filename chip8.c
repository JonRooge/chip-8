#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define RAMSIZE 4096

uint8_t * loadFile(int argc, char ** argv){
	int bytep = 0,
	    fsize = 0;
	uint8_t * membuffer;
	FILE * fp = fopen(argv[1], "r");

	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	membuffer = (uint8_t*) malloc(sizeof(uint8_t) * (fsize+1));

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
	return 0;
}

int main(int argc, char ** argv){
	uint8_t * lrom; //loaded rom

	lrom = loadFile(argc, argv);
	decompile(lrom);

	return 0;

}
