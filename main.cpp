#include "chip8.h"
#include <iostream>
#include <string>

int main(int argc, char * argv[]) {
    std::string filepath;
    if(argc > 1) {
        filepath = argv[1];
    }

    Chip8 chip8; //allocate chip8 class on stack 
    
    //load rom into memory
    if(!chip8.load_rom(filepath)) {
        return 1;
    }


    for(uint8_t i = 0; i < 50; i++) { //run a fixed number of cycles to finish the program
        chip8.cycle();

        if(chip8.should_draw()) { //if drawing is enabled
            auto& display = chip8.get_display(); //load contents of display into buffer
            
            //print out contents to terminal
            for(uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
                for(uint8_t x = 0; x < SCREEN_WIDTH; x++) {
                    uint8_t pxl = display[y * SCREEN_WIDTH + x];
                    
                    std::cout << (pxl ? "██" : "  ");
                }
                std::cout << '\n';
            } 
        }
        chip8.reset_draw_flag(); //reset flag after every cycle
    }
}