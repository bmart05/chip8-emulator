#include <SDL3/SDL.h>
#include <vector>
#include <array>
#include "platform.h"

struct Platform::Impl {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* renderFrame = nullptr;

    uint8_t tex_width;
    uint8_t tex_height; 
    std::vector<uint32_t> pixelBuffer;
};

namespace {
int map_scancode_to_chip8(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_X: return 0x0;
        case SDL_SCANCODE_1: return 0x1;
        case SDL_SCANCODE_2: return 0x2;
        case SDL_SCANCODE_3: return 0x3;
        case SDL_SCANCODE_Q: return 0x4;
        case SDL_SCANCODE_W: return 0x5;
        case SDL_SCANCODE_E: return 0x6;
        case SDL_SCANCODE_A: return 0x7;
        case SDL_SCANCODE_S: return 0x8;
        case SDL_SCANCODE_D: return 0x9;
        case SDL_SCANCODE_Z: return 0xA;
        case SDL_SCANCODE_C: return 0xB;
        case SDL_SCANCODE_4: return 0xC;
        case SDL_SCANCODE_R: return 0xD;
        case SDL_SCANCODE_F: return 0xE;
        case SDL_SCANCODE_V: return 0xF;
        default:             return -1;
    }
}
} // namespace

Platform::Platform() : impl(std::make_unique<Impl>()) {}

Platform::~Platform() {
    shutdown();
}

bool Platform::init(const std::string title, int window_width, int window_height, int tex_width, int tex_height)
{
    //initialise SDL3 video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return false;
    }

    //create window and renderer objects
    
    if (!SDL_CreateWindowAndRenderer(title.c_str(), window_width, window_height, 0, &impl->window, &impl->renderer)) {
        std::cerr << "SDL_CreateWindowAndRenderer failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return false;
    }

    impl->tex_width = tex_width;
    impl->tex_height = tex_height;
    impl->pixelBuffer.resize(tex_width * tex_height, SECONDARY_COLOUR);

    impl->renderFrame = SDL_CreateTexture(impl->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, tex_width, tex_height);
    SDL_SetTextureScaleMode(impl->renderFrame, SDL_SCALEMODE_NEAREST);

    std::array<uint8_t, 64 * 32> blank{};
    present(blank.data());
    return true;
}

void Platform::present(const uint8_t *pixels)
{
    const size_t totalPixels = impl->pixelBuffer.size();
    for(size_t i = 0; i < totalPixels; ++i) {
        impl->pixelBuffer[i] = pixels[i] ? PRIMARY_COLOUR : SECONDARY_COLOUR;
    }

    const int pitch = impl->tex_width * sizeof(uint32_t);

    SDL_UpdateTexture(impl->renderFrame, nullptr, impl->pixelBuffer.data(), pitch);
    SDL_RenderClear(impl->renderer);
    SDL_RenderTexture(impl->renderer, impl-> renderFrame, nullptr, nullptr);
    SDL_RenderPresent(impl->renderer);
}

bool Platform::process_input(std::array<bool, 16> &keys)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_EVENT_QUIT:
                return false;
            case SDL_EVENT_KEY_DOWN: {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    return false;
                }
                int key = map_scancode_to_chip8(event.key.scancode);
                if(key != -1) {
                    keys[key] = true;
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                int key = map_scancode_to_chip8(event.key.scancode);
                if(key != -1) {
                    keys[key] = false;
                }
                break;
            }
        }
    }
    return true;
}

void Platform::shutdown()
{
    SDL_DestroyRenderer(impl->renderer);
    SDL_DestroyWindow(impl->window);
    SDL_DestroyTexture(impl->renderFrame);
    SDL_Quit();
}
