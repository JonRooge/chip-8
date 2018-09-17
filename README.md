# chip-8
Chip 8 disassembler and emulator

## compile

`gcc -o chip8 chip8_main.c decompile.c emulate.c -lcurses`

Requires ncurses for display and keyboard input.

## run
`./chip8.exe <path_to_rom>`

### functions in main menu
* 1 - Shows raw rom data
  * Basic unformatted dump of hex instructions
  * Mostly used for debugging in the beginning, but I left it here because I thought it was interesting.
* 2 - Decompiles loaded rom
  * Spits out basic interpretation all at once
* 3 or 4 - Run program with or without vertical wrapping enabled
  * ESC exits safely after short pause, restores cmd line, and returns you back to the menu
  * Keys are mapped to their corresponding key 1->1, 2->2 ... F->F
  * Terminal window should be larger than 128x32 (each pixel is two characters wide '[]')
* 0 - Quit menu
  * Exits with code 0 if everthing was ok.
