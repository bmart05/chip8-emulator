#include "chip8.h"
#include <fstream>
#include <iostream>

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
    uint8_t first = (opcode & 0xF000) >> 12; //defines instruction type
    uint8_t X = (opcode & 0x0F00) >> 8; //used to look up one of the 16 registers
    uint8_t Y = (opcode & 0x00F0) >> 4; //used to look up 
    uint8_t N = (opcode & 0x000F); //4-bit number
    uint8_t NN = (opcode & 0x00FF); //8-bit immediate number
    uint16_t NNN = (opcode & 0x0FFF); //12-bit immediate memory address


}

void Chip8::tick_timers()
{
}

void Chip8::set_key(uint8_t key, bool is_pressed)
{
}

const auto &Chip8::get_display()
{
    // TODO: insert return statement here
}

bool Chip8::should_draw() const
{
    return false;
}

void Chip8::reset_draw_flag()
{
    draw_flag = false;
}

bool Chip8::is_beeping() const
{
    return (sound_timer > 0);
}
