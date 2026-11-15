#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>

int main() {
    // 1. Initialize SDL3 video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    // 2. Create a window and renderer together
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("SDL3 Test Window", 800, 600, 0, &window, &renderer)) {
        std::cerr << "SDL_CreateWindowAndRenderer failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    // 3. Main loop
    bool running = true;
    SDL_Event event;

    while (running) {
        // Poll events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Clear screen with a solid color (RGBA)
        SDL_SetRenderDrawColor(renderer, 30, 41, 59, 255);
        SDL_RenderClear(renderer);

        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    // 4. Clean up and exit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Test";

    return 0;
}