#include "chip8.h"
#include "platform.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

constexpr auto TIMER_INTERVAL = std::chrono::microseconds(16667); //60hz

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

    bool running = true;

    while(running) {
        chip8.cycle();

        if(chip8.should_draw()) { //if drawing is enabled
            auto& display = chip8.get_display(); //load contents of display into buffer
            
            platform.present(display.data());
        }
        chip8.reset_draw_flag(); //reset flag after every cycle

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    platform.shutdown();
}