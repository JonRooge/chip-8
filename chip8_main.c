
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
	    fsize = 0,
	    err = 0;
	char line[3];
	
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
	while(ch != '0'){
		printf("Main menu:\n 0 ... Quit\n 1 ... Print out raw rom data\n 2 ... Decompile rom\n 3 ... Run program W/O vertical wrapping (Quit with ESC)\n 4 ... Run program with vertical wrapping (Quit with ESC)\n>>>");
		fflush(stdin);
		if(fgets(line, 3, stdin) == NULL){
			printf("Error reading input.");
			continue;
		}
		if(!strchr(line, '\n')) while (fgetc(stdin) != '\n'); //discards until newline. Credit-> https://stackoverflow.com/questions/30388101/how-to-remove-extra-characters-input-from-fgets-in-c
		ch = line[0];
		switch(ch){
			case '1': 
				loadFile(argc, argv, 1, &fsize);
				break;
			case '2':
				decompile(lrom, fsize);
				break;
			case '3':
				err = emulate(lrom, fsize, 0);
				cleanup();
				if(err == 1){
					printf("Emulator error.\n");
				}else if (err == 2){
					printf("Screen initialization failed. Quitting program. Try increasing the size of the terminal window.\n");
					return 1;
				}else if (err == 3){
					printf("Unknown instruction. ROM may be corrupted.\n");
				}else{
					printf("Exited with code %d. Code 0 is normal.\n", err);
				}
				fflush(stdin);
				break;
			case '4':
				err = emulate(lrom, fsize, 1);
				cleanup();
				if(err == 1){
					printf("Emulator error.\n");
				}else if (err == 2){
					printf("Screen initialization failed. Quitting program. Try increasing the size of the terminal window.\n");
					return 1;
				}else if (err == 3){
					printf("Unknown instruction. ROM may be corrupted.\n");
				}else{
					printf("Exited with code %d. Code 0 is normal.\n", err);
				}
				fflush(stdin);
				break;
			case '0':
				break;
			default:
				printf("\nUnknown option...\n\n");
		}
	}

	return 0;

}
