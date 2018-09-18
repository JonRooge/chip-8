// old test code for reg-

	//uint8_t emptyReg[16] = {0};
	//memcpy(reg->V, emptyReg, sizeof(emptyReg));
	//*reg = {0}
	
	//int test1 = sizeof(reg);
	//int test2 = sizeof(*reg);
	
	// Instead of calloc (which initializes the alloc to 0), I do this.
	
	
	
	

//old draw code
			/*
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
						if (wrap){
							vert = (y_coord + i) % (WINDOW_H);
						} else {
							vert = (y_coord + i);
							if (vert >= WINDOW_H){
								break;
							}
						}
						//
						if ((display[(x_coord + j) % (WINDOW_W/2)][vert] == 1) &&
							(((mem[reg->I + i] & ands[j]) >> (8 - j - 1)) == 1)) {
							reg->V[0xf] = 1;
						}

						// bitwise operations decode each bit of sprite and XOR with the current pixel on screen
						display[(x_coord + j) % (WINDOW_W/2)][vert] ^= ((mem[reg->I + i] & ands[j]) >> (8 - j - 1));
					}
					//x_coord = reg->V[nib2];
					//y_coord = reg->V[nib3];
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
				
				
				wrefresh(win);*/
				
				
				
/*------------------*/
// bad child code for timers

			
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