#include "chip8.h"
#include <fstream>
#include <iostream>

constexpr std::array<uint8_t, FONTSET_SIZE> FONTSET = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
{
    pc = START_ADDRESS; //initialise PC

    std::copy(FONTSET.begin(), FONTSET.end(), memory.begin() + FONTSET_START_ADDRESS); //copy fontset into memory

}

bool Chip8::load_rom(const std::string &filepath)
{
    //open rom in binary mode, position cursor at the end to measure size immediately
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if(!file.is_open()) {
        std::cerr << "Failed to open ROM from path: " << filepath << "\n";
        return false;
    }

    std::streamsize size = file.tellg();
    std::size_t max_size = MEMORY_SIZE - START_ADDRESS;

    if(size <= 0 || static_cast<std::size_t>(size) > max_size) {
        std::cerr << "Invalid file size: " << size << "bytes\n";
        return false;
    }

    //seek cursor to start of the file
    file.seekg(0, std::ios::beg);
    
    //read bytes into memory
    //reinterpret_cast bridges uint8_t* expected by memory to the char* expected by ifstream::read
    if(!file.read(reinterpret_cast<char*>(&memory[START_ADDRESS]), size)) {
        std::cerr << "Failed to read ROM contents into memory\n";
        return false;
    }

    return true;
}

void Chip8::cycle()
{
    // --- FETCH ---

    /*
    Memory is read 8-bits at a time, but instructions are 16-bits
    This combines two readings by reading the upper half, then  bitwise shifting it 8 bits up and ORing it with the lower half
    */
    uint16_t opcode = (memory[pc] << 8 | memory[pc + 1]);

    pc += 2; //increment PC to next instruction

    // --- DECODE --- 
    const uint8_t first = (opcode & 0xF000) >> 12; //defines instruction type
    const uint8_t X = (opcode & 0x0F00) >> 8; //used to look up one of the 16 registers (VX)
    const uint8_t Y = (opcode & 0x00F0) >> 4; //used to look up one of the 16 registers (VY)
    const uint8_t N = (opcode & 0x000F); //4-bit number
    const uint8_t NN = (opcode & 0x00FF); //8-bit immediate number
    const uint16_t NNN = (opcode & 0x0FFF); //12-bit immediate memory address

    // --- EXECUTE ---
    switch(first) {
        case 0x0:
            switch(opcode) {
                case 0x00E0: //clear screen instruction
                    draw_flag = true;
                    display.fill(0);
                    break;
                case 0x00EE: //subroutines (return)
                    pop_stack(pc);
                    break;
            }
            break;
        case 0x1: //jump instructions
            pc = NNN; //jump to memory location in NNN
            break;
        case 0x2: //0x2NNN call subroutine
            push_stack(pc);
            pc = NNN;
            break;
        case 0x3: //3XNN skip if equal (VX == NN)
            if(V[X] == NN) {
                pc += 2;
            }
            break;
        case 0x4: //4XNN skip if not equal (VX != NN)
            if(V[X] != NN) {
                pc += 2;
            }
            break;
        case 0x5: //5XY0 skip if equal (VX == VY)
            if(V[X] == V[Y]) {
                pc += 2;
            }
            break;
        case 0x6: //0x6XNN set
            V[X] = NN;
            break;
        case 0x7:
            V[X] += NN;
            break;
        case 0x8:
            switch(opcode & 0x000F) { //last nibble determines instruction type
                case 0x0: //set
                    V[X] = V[Y];
                    break;
                case 0x1: //binary OR
                    V[X] = V[X] | V[Y];
                    break;
                case 0x2: //binary AND
                    V[X] = V[X] & V[Y];
                    break;
                case 0x3: //logical XOR
                    V[X] = V[X] ^ V[Y];
                    break;
                case 0x4: //add
                    V[X] = V[X] + V[Y];
                    
                    //set carry flag if overflow
                    if (V[X] > 255) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    break;
                case 0x5: { //subtract Y - X
                    uint8_t flag = (V[X] >= V[Y]) ? 1 : 0;
                    V[X] = V[X] - V[Y];
                    V[0xF] = flag;
                    break;
                }
                case 0x6: { //right shift
                    if(SHIFT_QUIRK) {
                        V[X] = V[Y];
                    }
                    
                    uint8_t flag = V[X] & 0x01;
                    V[X] >>= 1;

                    V[0xF] = flag;
                    break;
                }
                case 0x7: { //subtract X - Y
                    uint8_t flag = (V[Y] >= V[X]) ? 1 : 0;
                    V[X] = V[Y] - V[X];
                    V[0xF] = flag; 

                    break;
                }
                case 0xE: { //left shift
                    if(SHIFT_QUIRK) {
                        V[X] = V[Y];
                    }
                    
                    uint8_t flag = (V[X] & 0x80) >> 7;
                    V[X] <<= 1;

                    V[0xF] = flag;
                    break;
                }
            }

            break;
        case 0x9: //9XY0 skip if not equal (VX != VY)
            if(V[X] != V[Y]) {
                pc += 2;
            }
        case 0xA:
            I = NNN;
            break;
        case 0xB:
            if(JUMP_QUIRK) { //Implements a quirk from CHIP-48 and SUPER-CHIP
                pc = NNN + V[X];
            } else {
                pc = NNN + V[0x0];
            }
            break;
        case 0xC: //generate random number into V[X]
            V[X] = static_cast<uint8_t>(rand_byte(rng)) & NN;
            break;
        case 0xD: { //display draw function (DXYN)
            uint8_t x_coord = V[X] % 64;
            uint8_t y_coord = V[Y] % 32;

            V[0xF] = 0;

            for(uint8_t i = 0; i < N; i++) {
                uint8_t sprite_data = memory[I + i];

                uint8_t py = y_coord + i;

                if(py >= SCREEN_HEIGHT) { //sprites clip at the bottom edge rather than wrapping around
                    break; 
                }
                
                for(uint8_t col = 0; col < 8; col++) {
                    uint8_t px = x_coord + col;

                    if (px >= SCREEN_WIDTH) { //sprites clip at right edge rather than wrapping around
                        break;
                    }

                    if((sprite_data & (0x80 >> col)) != 0) {
                        if(display[py * SCREEN_WIDTH + px] != 0) {
                            V[0xF] = 1;
                        }
                        display[py * SCREEN_WIDTH + px] = !display[py * SCREEN_WIDTH + px]; //toggle pixel on or off
                    }
                }  
            }

            draw_flag = true;
            break;
        }
        case 0xE:
            switch(NN) {
                case 0x9E: {
                    uint8_t key = V[X] & 0x0F; //map to 0-15
                    if(keypad[key]) {
                        pc += 2;
                    }
                    break;
                }
                case 0xA1: {
                    uint8_t key = V[X] & 0x0F; //map to 0-15
                    if(!keypad[key]) {
                        pc += 2;
                    }
                    break;
                }
            }
            break;
        case 0xF: //All in format of 0xFX__
            switch(NN) {
                case 0x0A: //get key
                    for(uint8_t i = 0; i < keypad.size(); i++) {
                        if(keypad[i]) {
                            V[X] = i;
                            return;
                        }
                    }
                    pc -= 2; //instruction should not advance until a key is pressed
                    break;
                case 0x07: //set V[x] to delay_timer
                    V[X] = delay_timer;
                    break;
                case 0x15: //set delay timer to V[X]
                    delay_timer = V[X];
                    break;
                case 0x1E: //add V[X] to index register
                    I += V[X];
                    break;
                case 0x18: //set sound timer to V[X]
                    sound_timer = V[X];
                    break; 
                case 0x29: //font character
                    I = FONTSET_START_ADDRESS + ((V[X] & 0x0F) * 5);
                    break;
                case 0x33: { //load individual digits of a binary number into consecutive memory slots
                    uint8_t value = V[X];    

                    memory[I] = value / 100; //hundreds digit
                    memory[I+1] = (value / 10) % 10; //tens digit
                    memory[I+2] = value % 10; //ones digit
                    break;
                }
                case 0x55: //load registers 0-X into memory
                    for(uint8_t i = 0; i <= X; i++) {
                        memory[I+i] = V[i]; //store value into memory from index register
                    }

                    if(MEMORY_QUIRK) { 
                        I += X+1;
                    }
                    break;
                case 0x65: //store values from I-I+X into registers 0-X
                    for(uint8_t i = 0; i <= X; i++) {
                        V[i] = memory[I+i]; //load value from memory into register
                    }

                    if(MEMORY_QUIRK) { 
                        I += X+1;
                    }
                    break;
            }
    }

}

void Chip8::tick_timers()
{
    if(delay_timer > 0) {
        --delay_timer;
    }
    if(sound_timer > 0) {
        --sound_timer;
    }
}

void Chip8::set_key(uint8_t key, bool is_pressed)
{

}

bool Chip8::should_draw() const
{
    return draw_flag;
}

void Chip8::reset_draw_flag()
{
    draw_flag = false;
}

bool Chip8::is_beeping() const
{
    return (sound_timer > 0);
}

bool Chip8::push_stack(uint16_t addr)
{
    if(sp >= STACK_DEPTH) {
        return false;
    }

    stack[sp++] = addr;

    return true;
}

bool Chip8::pop_stack(uint16_t& out_addr)
{
    if(sp == 0) {
        return false;
    }

    out_addr = stack[--sp];

    return true;
}

