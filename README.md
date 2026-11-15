# CHIP-8 Emulator
This is a basic CHIP-8 emulator written in C++. Built using this guide (https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)

It implements all basic functionality of the CHIP-8 spec, but does not implement any SUPER-CHIP functionality as of right now.

## Running the Emulator
Compile the emulator using the makefile provided. Make sure you have SDL3 installed to display the programs. The compiler will produce a `chip8` file, and you can then run programs using the following command:
``` sh
./chip8 "PATH_TO_FILE"
```

## Examples
### IBM Logo
Here is the **IBM logo program** running on this emulator, showing basic register functionality for `00E0` (clear), `1NNN` (jump), `6XNN` (set VX), `7XNN` (add VX), `ANNN` (set index), `DXYN` (draw):

<img width="1006" height="550" alt="image" src="https://github.com/user-attachments/assets/38d708c1-213a-4bfc-ae83-776b0b6b7bb7" />

### Full Test Suite
Here is the **basic test program** to show all registers work correctly:

<img width="1006" height="550" alt="image" src="https://github.com/user-attachments/assets/32ed3120-994d-426c-a6bb-69a44e5c9fc4" />

### Credit
*These roms were obtained from https://github.com/Timendus/chip8-test-suite*
