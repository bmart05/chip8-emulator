#include "chip8.h"
#include "platform.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using Clock = std::chrono::steady_clock;
constexpr auto TIMER_INTERVAL = std::chrono::microseconds(16667); //60hz
constexpr auto CYCLE_INTERVAL = std::chrono::microseconds(1428); //~700hz ish

int main(int argc, char * argv[]) {
    std::string filepath;
    if(argc > 1) {
        filepath = argv[1];
    }

    Chip8 chip8; //allocate chip8 class on stack 
    Platform platform;
    if(!platform.init(filepath, 800, 400, SCREEN_WIDTH, SCREEN_HEIGHT)) {
        return 1;
    }
    
    //load rom into memory
    if(!chip8.load_rom(filepath)) {
        return 1;
    }

    auto last_time = Clock::now();
    std::chrono::microseconds timer_acc{0};
    std::chrono::microseconds cycle_acc{0};

    bool running = true;

    while(running) {
        running = platform.process_input(chip8.get_keypad());
        if (!running) break;

        auto now = Clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_time);
        last_time = now;

        if(dt > std::chrono::milliseconds(250)) {
            dt = std::chrono::milliseconds(250);
        }
        timer_acc += dt;
        cycle_acc += dt;

        while(cycle_acc >= CYCLE_INTERVAL) {
            chip8.cycle();
            cycle_acc -= CYCLE_INTERVAL;
        }

        while(timer_acc >= TIMER_INTERVAL) {
            chip8.tick_timers();
            timer_acc -= TIMER_INTERVAL;
        }

        if(chip8.should_draw()) { //if drawing is enabled
            auto& display = chip8.get_display(); //load contents of display into buffer
            
            platform.present(display.data());
            chip8.reset_draw_flag(); //reset flag after every cycle
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    platform.shutdown();
}