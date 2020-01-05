#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "SDL.h"

#define WIDTH 1920
#define HEIGHT 1080

#define PIXELSIZE 4

#define TOTAL_FRAMES 6000

#define COLOR_INTENSITY 2000

#define GRAV_LIMIT 10.0

float metaCalc(float x, float y)
{
    return 1.0/(x * x + y * y);
}

struct Vec2 {
    float x;
    float y;
} Vec2;

typedef struct Meta {
    struct Vec2 pos;
    struct Vec2 v;  // Velocity
} Meta;

uint8_t leik(float x)
{
    if (x > 255.0)
        return 255;
    else
        return (uint8_t)x;
}

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
            
            if((fabsf(xDelta) > 15) && (fabsf(yDelta) > 15))
            {               
                gravity = 1.0 / ( sqrt((xDelta * xDelta + yDelta * yDelta))); // distance sqrt( (x2-x1)^2 + (y2-y1)^2 ) 
            }
            else
            {
                gravity = 0.0;
            }

            //if (gravity > GRAV_LIMIT)
            //    gravity = GRAV_LIMIT;

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
    meta[0].pos.y = HEIGHT/2-50;
    meta[0].v.x = 0.0;
    meta[0].v.y = 0.0;

    meta[1].pos.x = WIDTH/2+100;
    meta[1].pos.y = HEIGHT/2+100;
    meta[1].v.x = 0.0;
    meta[1].v.y = 0;

    meta[2].pos.x = WIDTH/2-100;
    meta[2].pos.y = HEIGHT/2+100;
    meta[2].v.x = 0.0;
    meta[2].v.y = 0.0;

    uint32_t iter = 0;

    uint8_t color = 0;

    //printf("hello");

    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8* wavBuffer;

    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN, &window, &renderer); //
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_LoadWAV("tune.wav", &wavSpec, &wavBuffer, &wavLength);

    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

    int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);

    //SDL_Delay(3000);

 
    //return 0;

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

                //float color = ((float)0xFF * amplitudeSum * 20000);
                amplitudeSum = amplitude1 + amplitude2 + amplitude3;
                if(amplitudeSum > 0.00025)
                {
                    //SDL_SetRenderDrawColor(renderer, 
                    //    (uint8_t)(255.0 * (amplitude1-(int)amplitude1)),
                    //    (uint8_t)(255.0 * (amplitude2-(int)amplitude2)),
                    //    (uint8_t)(255.0 * (amplitude3-(int)amplitude3)),
                    //    255);
                    
                    SDL_SetRenderDrawColor(renderer, 
                        leik(amplitude1*400000.0) ,
                        leik(amplitude2*400000.0) ,
                        leik(amplitude3*400000.0) ,
                        255);
                        
                   // SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                    SDL_RenderDrawPoint(renderer, x, y);

                }
            }
        }

        SDL_RenderPresent(renderer);

        updateForces(meta);

        if ((SDL_PollEvent(&event) && ((event.type == SDL_QUIT) || (event.key.keysym.sym == SDLK_ESCAPE)) ) || (SDL_GetTicks() > 60000) )
            break;
    }

    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;

  
}

