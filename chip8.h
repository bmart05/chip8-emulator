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

#define SHIFT_QUIRK 1
#define JUMP_QUIRK 0
#define MEMORY_QUIRK 0

class Chip8 {
public:
    Chip8();
    bool load_rom(const std::string& filepath); //read binary ROM into memory from START_ADDRESS

    void cycle(); //run one FDE cycle
    void tick_timers(); //decrement delay and sound timers at 60hz

    void set_key(uint8_t key, bool is_pressed);
    const auto& get_display() {
        return display;
    }
    bool should_draw() const;
    void reset_draw_flag();
    bool is_beeping() const; //returns when sound_timer > 0
    std::array<bool, KEY_COUNT>& get_keypad() { return keypad; }
private:
    std::array<uint8_t, MEMORY_SIZE> memory{}; //direct-addressed 4KB RAM
    std::array<uint8_t, REGISTER_COUNT> V{}; //16 general purpose 8-bit registers (V0-VF) (VF is the carry/collision flag register as well)
    uint16_t I{0}; //index register, initialised to 0
    uint16_t pc{START_ADDRESS}; //program counter, initialised to START_ADDRESS
    std::array<uint16_t, 16> stack; //stack for 16-bit addresses, used to call and return from subroutines
    uint8_t sp{0}; //stack pointer, points to top of stack
    bool push_stack(uint16_t addr);
    bool pop_stack(uint16_t& out_addr);
    uint8_t delay_timer{0};
    uint8_t sound_timer{0};
    std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT> display{}; //framebuffer representing pixel state (either 0 or 1)
    std::array <bool, KEY_COUNT> keypad{}; //tracks whether each hex key is pressed or released (0x0 to 0xF)
    bool draw_flag{false}; //keeps track of whether any pixels changed during the last cycle, only redraw the screen when needed
    std::mt19937 rng{std::random_device{}()};                  //ensures random numbers are seeded 
    std::uniform_int_distribution<uint16_t> rand_byte{0, 255}; //when the emulator starts rather than every cycle
};
