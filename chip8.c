#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define RAMSIZE 4096



int main(int argc, char ** argv){
	int result = 0,
	    bytep = 0,
	    fsize = 0;
	uint8_t * membuffer;
	FILE * fp = fopen(argv[1], "r");

	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	membuffer = (uint8_t*) malloc(sizeof(uint8_t) * (fsize+1));

	fseek(fp, 0, SEEK_SET);
	if (fread(membuffer, 1, fsize, fp) < fsize) {
		return 1;
	}
	
	fclose(fp);
	
	if(argc == 1 || fsize <= 1){
		printf("Failed to load file.");
		return 1;
	}

	while(bytep != fsize){
		printf("%x ", membuffer[bytep]);
		bytep++;
	}
	//result = decompile(f);
	
	return 0;

}
