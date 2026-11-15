#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <random>


#define MEMORY_SIZE 4096
#define START_ADDRESS 0x200

#define FONTSET_SIZE 80
#define FONTSET_START_ADDRESS 0x050

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#define REGISTER_COUNT 16

#define STACK_DEPTH 16

#define KEY_COUNT 16

namespace {
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
    }
}

class Chip8 {
public:
    Chip8();
    bool load_rom(const std::string& filepath); //read binary ROM into memory from START_ADDRESS

    void cycle(); //run one FDE cycle
    void tick_timers(); //decrement delay and sound timers at 60hz

    void set_key(uint8_t key, bool is_pressed);
    const auto& get_display();
    bool should_draw() const;
    void reset_draw_flag();
    bool is_beeping() const; //returns when sound_timer > 0
private:
    std::array<uint8_t, MEMORY_SIZE> memory; //direct-addressed 4KB RAM
    std::array<uint8_t, REGISTER_COUNT> V; //16 general purpose 8-bit registers (V0-VF) (VF is the carry/collision flag register as well)
    uint8_t I{0}; //index register, initialised to 0
    uint16_t pc{START_ADDRESS}; //program counter, initialised to START_ADDRESS
    std::array<uint16_t, 16> stack; //stack for 16-bit addresses, used to call and return from subroutines
    uint8_t delay_timer{0};
    uint8_t sound_timer{0};
    std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT> display{}; //framebuffer representing pixel state (either 0 or 1)
    std::array <bool, KEY_COUNT> keypad{}; //tracks whether each hex key is pressed or released (0x0 to 0xF)
    bool draw_flag{false}; //keeps track of whether any pixels changed during the last cycle, only redraw the screen when needed
        

};
