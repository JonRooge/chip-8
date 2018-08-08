# chip-8
Chip 8 disassembler and emulator

## compile

`gcc -o chip8 chip8.c -lcurses -lsdl`

Requires ncurses, and SDL libraries (Still developing good solution, currently, SDL is not needed) for display and keyboard input respectively.
Everything is in one file due to the way I think through things. I may separate out functionality into multiple c and h files after I hit a certain point.

## run
`./chip8.exe <path_to_rom>`

### functions in main menu
* 0 - Shows raw rom data
  * Basic unformatted dump of hex instructions
* 1 - Decompiles loaded rom
* 2 - Run program (ESC to quit safely)
  * ESCing doesn't work if the program is waiting for input. WIP
* 3 - Quit menu
