#include <SDL3/SDL.h>
#include <vector>
#include "platform.h"

struct Platform::Impl {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* renderFrame = nullptr;

    uint8_t tex_width;
    uint8_t tex_height; 
    std::vector<uint32_t> pixelBuffer;
};

Platform::Platform() : impl(std::make_unique<Impl>()) {}

Platform::~Platform() {
    shutdown();
}

bool Platform::init(const std::string title, int window_width, int window_height, int tex_width, int tex_height)
{
    //initialise SDL3 video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
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

void Platform::shutdown()
{
    SDL_DestroyRenderer(impl->renderer);
    SDL_DestroyWindow(impl->window);
    SDL_Quit();
}
