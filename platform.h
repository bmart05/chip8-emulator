#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <iostream>

class Platform {
public:
    static constexpr uint32_t PRIMARY_COLOUR = 0xFFFFFFFF;
    static constexpr uint32_t SECONDARY_COLOUR = 0x00000000;

    Platform();
    ~Platform();

    Platform(const Platform&) = delete;
    Platform& operator=(const Platform&) = delete;

    bool init(const std::string title, int window_width, int window_height, int tex_width, int tex_height);
    void present(const uint8_t* pixels); //pitch is the number of bytes in one horizontal row of pixels in memory
    bool process_input(std::array<bool, 16>& keys);
    void shutdown();
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};