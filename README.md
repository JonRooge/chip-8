# chip-8
Chip 8 disassembler and emulator

## compile
`gcc chip8.c`

Everything is in one file due to the way I think through things. I may separate out functionality into multiple c and h files after I hit a certain point.

## run
`./a.out roms/romname.ch8`

Running will start by spitting out the ROM raw, followed by a disassembly output, followed again by trying to run it. I use this format for debugging. Planning to change it to accept a choice if you want to run either the disassembler or the program.
