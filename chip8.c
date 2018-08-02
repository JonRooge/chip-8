#include <stdint.h>
#include <stdio.h>


int loadFile(char * arg, char * membuffer[]){
	FILE * fp = fopen(arg, "r");
	
	if (fscanf(fp, "%x", membuffer)){
		return 1;
	}
	return 0;
}


int main(int argc, char ** argv){
	int result = 0;
	char * membuffer[38];

	if(argc == 1 || loadFile(argv[1], membuffer)){
		printf("Failed to load file.");
		return 1;
	}

	int fend = 38;
	int pointer = 0;

	while(pointer != fend){
		printf("byte: %x\n", membuffer[pointer]);
		pointer++;
	}
	//result = decompile(f);
	
	return 0;
}
