# chip-8
Chip 8 disassembler and emulator

## compile

`gcc -o chip8 chip8_main.c decompile.c emulate.c -lcurses`

Requires ncurses for display and keyboard input.

## run
`./chip8.exe <path_to_rom>`

### functions in main menu
* 0 - Shows raw rom data
  * Basic unformatted dump of hex instructions
  * Mostly used for debugging in the beginning, but I left it here because I thought it was interesting.
* 1 - Decompiles loaded rom
  * Spits out basic interpretation all at once
* 2 - Run program
  * ESC exits safely, restores cmd line, and returns you back to the menu
  * Keys are mapped to their corresponding key 1->1, 2->2 ... F->F
* 3 - Quit menu
