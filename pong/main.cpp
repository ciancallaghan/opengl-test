#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 500;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;

bool init();
void close();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

bool
init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Error: %s\n", SDL_GetError());
        success = false;
    } else {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            printf("Warning: Linear texture filtering not enabled!");
        }

        gWindow = SDL_CreateWindow(
                "Pong",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH,
                SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
                );
        if (gWindow == NULL) {
            printf("SDL Error: %s\n", SDL_GetError());
            success = false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL) {
                printf("SDL Error: %s\n", SDL_GetError());
                success = false;
            } else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }

    return success;
}

void
close()
{
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    SDL_Quit();
}

int
main(int argc, char* args[])
{
    if (!init()) {
        printf("Failed to initialise\n");
    } else {
        bool quit = false;

        SDL_Event e;

        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }

            SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear(gRenderer);

            SDL_Rect paddle = {
                10,
                SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2,
                PADDLE_WIDTH,
                PADDLE_HEIGHT
            };
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderFillRect(gRenderer, &paddle);

            SDL_RenderPresent(gRenderer);
        }
    }

    close();

    return 0;
}
