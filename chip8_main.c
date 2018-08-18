
#include "chip8.h"

uint8_t * loadFile(int argc, char ** argv, int sw, int * fsize){
	int bytep = 0;
	uint8_t * membuffer;
	uint8_t error = 1;
	
	FILE * fp = fopen(argv[1], "r");
	
	if (!fp) { 
		membuffer = &error;
		return membuffer;
	}
	fseek(fp, 0, SEEK_END);
	*fsize = ftell(fp);
	membuffer = (uint8_t*) malloc(sizeof(uint8_t) * (*fsize));

	fseek(fp, 0, SEEK_SET);
	if (fread(membuffer, 1, *fsize, fp) < *fsize) {
		printf("ERROR reading file");
	}
	
	fclose(fp);
	
	if(argc == 1 || *fsize <= 1){
		printf("Failed to load file.");
	}
	printf("ROM LOADED\n");
	if(sw){
		while(bytep != *fsize){
			printf("%x ", membuffer[bytep]);
			bytep++;
		}
	}
	printf("\n");
	return membuffer;
}

int main(int argc, char ** argv){
	uint8_t * lrom; 											//NOTE: stands for 'loaded rom'
	int ch, 
	    fsize = 0;
	
	if (argc < 2) {
		printf("Must include 1 argument that is the ROM location.\n");
		return 1;
	}
	lrom = loadFile(argc, argv, 0, &fsize);

	if(*lrom == 1){
		printf("Something went wrong trying to open '%s'.\n", argv[1]);
		return 1;
	}

	printf("Welcome to the chip8 emulator.\n");
	while(ch != 3){
		printf("Main menu:\n 0 ... Print out raw rom data\n 1 ... Decompile rom\n 2 ... Run program (Quit with ESC)\n 3 ... Quit\n>>>");
		scanf(" %d", &ch);
		switch(ch){
			case 0:
				loadFile(argc, argv, 1, &fsize);
				break;
			case 1:
				decompile(lrom, fsize);
				break;
			case 2:
				if(emulate(lrom, fsize)){
					printf("\nEmulator error...\n");
				}
				cleanup();
				fflush(stdin);
				break;
			case 3:
				break;
			default:
				printf("\nUnknown option...\n\n");
		}
	}

	return 0;

}
