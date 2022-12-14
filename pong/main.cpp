/* 
http://lazyfoo.net/tutorials/SDL/04_key_presses/index.php
http://lazyfoo.net/tutorials/SDL/08_geometry_rendering/index.php
https://wiki.libsdl.org/SDL2/SDL_GetKeyboardState
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <stdio.h>

const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 500;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;

bool init();
void close();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

class Paddle
{
    public:
        float x;
        float y;
        float width;
        float height;
        float middle_y;
        float vel = 0.5;

        Paddle(float x_, float y_, float width_, float height_) {
            x = x_;
            y = y_;
            width = width_;
            height = height_;
            middle_y = y + height / 2;
        }

        void draw(SDL_Renderer* renderer) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_FRect paddle = {x, y, width, height};
            SDL_RenderFillRectF(renderer, &paddle);
        }

        void move(int up) {
            if (up == 1) {
                y -= vel;
            } else {
                y += vel;
            }
        }
};

class Ball
{
    public:
        float x;
        float y;
        float original_x;
        float original_y;
        float radius;
        float middle_y;
        float max_vel = 0.4;
        float x_vel = max_vel;
        float y_vel = 0;

    Ball(float x_, float y_, float radius_) {
        x = x_;
        y = y_;
        original_x = x_;
        original_y = y_;
        radius = radius_;
        middle_y = y - radius / 2;
    }
    
    void draw(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_FRect ball = {x, y, radius, radius};
        SDL_RenderFillRectF(renderer, &ball);
    }

    void move() {
        x += x_vel;
        y += y_vel;
    }
};

void
draw(SDL_Renderer* renderer, Paddle paddles[2], Ball ball)
{
    // Black Window
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);

    // Paddles
    for (int i = 0; i < 2; i++) {
        paddles[i].draw(renderer);
    }

    // Middle Line
    for (int i = 10; i < SCREEN_HEIGHT; i += (SCREEN_HEIGHT / 20)) {
        if (i % 2 == 1) {
            continue;
        } else {
            SDL_Rect middle_box = {(SCREEN_WIDTH / 2) - 5, i, 10, SCREEN_HEIGHT / 20};
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderFillRect(renderer, &middle_box);
        }
    }

    ball.draw(renderer);

    SDL_RenderPresent(renderer);
}

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
        Paddle left_paddle(
                10,
                SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2,
                PADDLE_WIDTH,
                PADDLE_HEIGHT);
        Paddle right_paddle(
                SCREEN_WIDTH - PADDLE_WIDTH - 10,
                SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2,
                PADDLE_WIDTH,
                PADDLE_HEIGHT
                );
        Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 20);

        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                    }
                }

            }
            const Uint8 *keystates = SDL_GetKeyboardState(NULL);
            if (keystates[SDL_SCANCODE_UP] && right_paddle.y - right_paddle.vel >= 0) {
                right_paddle.move(1);
            }
            if (keystates[SDL_SCANCODE_DOWN] && right_paddle.y + right_paddle.vel + right_paddle.height <= SCREEN_HEIGHT) {
                right_paddle.move(0);
            }
            if (keystates[SDL_SCANCODE_W] && left_paddle.y - left_paddle.vel >= 0) {
                left_paddle.move(1);
            }
            if (keystates[SDL_SCANCODE_S] && left_paddle.y + left_paddle.vel + left_paddle.height <= SCREEN_HEIGHT) {
                left_paddle.move(0);
            }

            
            if (ball.y + ball.radius >= SCREEN_HEIGHT) {
                ball.y_vel *= -1;
            }
            if (ball.y < 0) {
                ball.y_vel *= -1;
            }

            if (ball.y >= left_paddle.y && ball.y <= left_paddle.y + left_paddle.height) {
                if (ball.x <= left_paddle.x + left_paddle.width) {
                    ball.x_vel *= -1;

                    if (ball.y < left_paddle.y + (left_paddle.height / 2)) {
                    ball.y_vel += 0.1;
                    } else if (ball.y > left_paddle.y + (left_paddle.height / 2)) {
                        ball.y_vel -= 0.1;
                    } else {
                        ball.y_vel = 0;
                    }

                    ball.y_vel *= -1;
                }
            } 
            if (ball.y >= right_paddle.y && ball.y <= right_paddle.y + right_paddle.height){
                if (ball.x + ball.radius >= right_paddle.x) {
                    ball.x_vel *= -1;

                    if (ball.y < right_paddle.y + (right_paddle.height / 2)) {
                    ball.y_vel += 0.1;
                    } else if (ball.y > right_paddle.y + (right_paddle.height / 2)) {
                        ball.y_vel -= 0.1;
                    } else {
                        ball.y_vel = 0;
                    }

                    ball.y_vel *= -1;
                }
            }

            // if (ball.x_vel < 0) {
            //     if (ball.y >= left_paddle.y && ball.y <= left_paddle.y + left_paddle.height) {
            //         if (ball.x - ball.radius <= left_paddle.x + left_paddle.width) {
            //             ball.x_vel *= -1;
            //
            //             int middle_y = left_paddle.y + left_paddle.height / 2;
            //             int diff_in_y = middle_y - ball.y;
            //             int reduction_factor = (left_paddle.height / 2) / ball.max_vel;
            //             int y_vel = diff_in_y / reduction_factor;
            //             ball.y_vel += -1 * y_vel;
            //         }
            //     }
            // } else {
            //     if (ball.y >= right_paddle.y && ball.y <= right_paddle.y + right_paddle.height) {
            //         if (ball.x + ball.radius >= right_paddle.x) {
            //             ball.x_vel *= -1;
            //
            //             int middle_y = right_paddle.y + right_paddle.height / 2;
            //             int diff_in_y = middle_y - ball.y;
            //             int reduction_factor = (right_paddle.height / 2) / ball.max_vel;
            //             int y_vel = diff_in_y / reduction_factor;
            //             ball.y_vel = -1 * y_vel;
            //         }
            //     }
            // }

            if (ball.x < 0 || ball.x > SCREEN_WIDTH) {
                ball.x = ball.original_x;
                ball.x_vel *= -1;
                ball.y_vel = 0;
            }

            Paddle paddles[2] = {left_paddle, right_paddle};
            draw(gRenderer, paddles, ball);
            ball.move();
        }
    }

    close();

    return 0;
}
