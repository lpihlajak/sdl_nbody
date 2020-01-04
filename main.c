#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "SDL.h"

#define WIDTH 1280
#define HEIGHT 720

#define PIXELSIZE 4

#define TOTAL_FRAMES 2400

#define COLOR_INTENSITY 2000

#define GRAV_LIMIT 0.5

float metaCalc(float x, float y)
{
    return ((float)1.0 / (x * x + y * y));
}

float gravCalc(float x, float y)
{
    return 1.0/(x * x + y * y);
}
// objects

struct Vec2 {
    float x;
    float y;
} Vec2;

typedef struct Meta {
    struct Vec2 pos;
    struct Vec2 v;  // Velocity
} Meta;

void updateForces(struct Meta meta[3])
{
    uint32_t iter2 = 0;
    uint32_t iter3 = 0;

    int32_t xDelta;
    int32_t yDelta;

    while (iter2 < 3)
    {
        iter3 = 0;
        while (iter3 < 3)
        {
            if (iter2 == iter3)
            {
                iter3++;
                continue;
            }

            float xDelta = (meta[iter3].pos.x - meta[iter2].pos.x);
            float yDelta = (meta[iter3].pos.y - meta[iter2].pos.y);
           
            float i; // direction vector for x
            float j; // direction vector for y

            float gravity;
            
            if((fabsf(xDelta) > 10) && (fabsf(yDelta) > 10))
            {
                gravity = gravCalc((float)xDelta, (float)yDelta)*100.0; // *1000 makes gravity 1 when overlapping
            }
            else
            {
                gravity = 0.0;
            }

            if (gravity > GRAV_LIMIT)
                gravity = GRAV_LIMIT;

            if (xDelta > 0)
                i = 1.0;
            else
                i = -1.0;

            if (yDelta > 0)
                j = 1.0;
            else
                j = -1.0;

            meta[iter2].pos.x += meta[iter2].v.x + i * gravity/2.0; // calculate position
            meta[iter2].v.x += i * gravity; // calc speed
         
            meta[iter2].pos.y += meta[iter2].v.y + j * gravity/2.0; // calculate position
            meta[iter2].v.y += j * gravity; // calc speed
            
            

            //printf("\nX:%fY:%f v.x: %f, v.y: %f, i: %f, j: %f, xDelta: %f yDelta: % Gravity: %f", meta[iter2].pos.x, meta[iter2].pos.y, meta[iter2].v.x, meta[iter2].v.y, i, j, xDelta, yDelta, gravity);

            iter3++;
        }

        if ((meta[iter2].pos.y < 0) || (meta[iter2].pos.y > HEIGHT))
        {
            meta[iter2].v.y *= -0.5;
            meta[iter2].pos.y += meta[iter2].v.y*2.0;
        }
        if ((meta[iter2].pos.x < 0) || (meta[iter2].pos.x > WIDTH))
        {
            meta[iter2].v.x *= -0.5;
            meta[iter2].pos.x += meta[iter2].v.x*2.0;
        }
       
        iter2++;
    }
    
}

int main()
{
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;


    float amplitude1;
    float amplitude2;
    float amplitude3;
    float amplitudeSum;

    uint32_t x;
    uint32_t y;

    struct Meta meta[3];

    meta[0].pos.x = WIDTH/2;
    meta[0].pos.y = HEIGHT/2-100;
    meta[0].v.x = 0.0;
    meta[0].v.y = 0.0;

    meta[1].pos.x = WIDTH/2+60;
    meta[1].pos.y = HEIGHT/2;
    meta[1].v.x = 0.0;
    meta[1].v.y = 0;

    meta[2].pos.x = WIDTH/2;
    meta[2].pos.y = HEIGHT/2+80;
    meta[2].v.x = 0.0;
    meta[2].v.y = 0.0;

    uint32_t iter = 0;

    uint8_t color = 0;

    //printf("hello");

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    //clear(buffer);

    

    int32_t counter = 0;

    while (1) {
        counter++;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        for (y = 0; y < HEIGHT; y++)
        {
            for (x = 0; x < WIDTH; x++)
            {
              

                amplitude1 = metaCalc(x - meta[0].pos.x, y - meta[0].pos.y);
                amplitude2 = metaCalc(x - meta[1].pos.x, y - meta[1].pos.y);
                amplitude3 = metaCalc(x - meta[2].pos.x, y - meta[2].pos.y);

                amplitudeSum = amplitude1 + amplitude2 + amplitude3;
                //float color = ((float)0xFF * amplitudeSum * 20000);
               
                if(amplitudeSum > 0.005)
                {
                    //SDL_SetRenderDrawColor(renderer, (uint8_t)((float)color * amplitude1 * COLOR_INTENSITY),
                    //    (uint8_t)((float)color * amplitude2 * COLOR_INTENSITY),
                    //    (uint8_t)((float)color * amplitude3 * COLOR_INTENSITY),
                    //    255);
                    
                    SDL_SetRenderDrawColor(renderer, 
                        0xFF,
                        0xFF,
                        0xFF,
                        255);
                        
                   // SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderDrawPoint(renderer, x, y);

                }
            }
        }

        SDL_RenderPresent(renderer);

        updateForces(meta);

        if (SDL_PollEvent(&event) && ((event.type == SDL_QUIT) || (event.key.keysym.sym == SDLK_ESCAPE)) )
            break;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;

  
}

