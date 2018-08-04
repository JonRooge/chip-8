# chip-8
Chip 8 disassembler and emulator

## compile

`gcc -o chip8 chip8.c -lcurses`

Requires ncurses library installed.
Everything is in one file due to the way I think through things. I may separate out functionality into multiple c and h files after I hit a certain point.

## run
`./chip8.exe <path_to_rom>`

### functions
* 0 - Shows raw rom data
* 1 - Decompiles loaded rom
* 2 - Run program (ESC to quit safely)
  * `ESC` exits the program safely.
* 3 - Quit menu
