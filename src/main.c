#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vehicle.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define VEHICLE_COUNT 30
#define VEHICLE_SPEED 2

#define ROAD_WIDTH 120
#define LANE_COUNT 3
#define LANE_SIZE (ROAD_WIDTH / LANE_COUNT)

/* ================= ROAD DRAWING ================= */

void drawRoads(SDL_Renderer* renderer) {
    SDL_Rect road;

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);

    // Vertical road
    road.x = (WINDOW_WIDTH / 2) - (ROAD_WIDTH / 2);
    road.y = 0;
    road.w = ROAD_WIDTH;
    road.h = WINDOW_HEIGHT;
    SDL_RenderFillRect(renderer, &road);

    // Horizontal road
    road.x = 0;
    road.y = (WINDOW_HEIGHT / 2) - (ROAD_WIDTH / 2);
    road.w = WINDOW_WIDTH;
    road.h = ROAD_WIDTH;
    SDL_RenderFillRect(renderer, &road);

    // Lane separators
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 1; i < LANE_COUNT; i++) {
        // Vertical lane lines
        SDL_RenderDrawLine(
            renderer,
            (WINDOW_WIDTH / 2) - (ROAD_WIDTH / 2) + i * LANE_SIZE,
            0,
            (WINDOW_WIDTH / 2) - (ROAD_WIDTH / 2) + i * LANE_SIZE,
            WINDOW_HEIGHT
        );

        // Horizontal lane lines
        SDL_RenderDrawLine(
            renderer,
            0,
            (WINDOW_HEIGHT / 2) - (ROAD_WIDTH / 2) + i * LANE_SIZE,
            WINDOW_WIDTH,
            (WINDOW_HEIGHT / 2) - (ROAD_WIDTH / 2) + i * LANE_SIZE
        );
    }
}

/* ================= VEHICLES ================= */

void drawVehicle(SDL_Renderer* renderer, Vehicle* v) {
    SDL_Rect rect = { v->x, v->y, v->w, v->h };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void updateVehicle(Vehicle* v) {
    switch (v->dir) {
        case 0: v->x += VEHICLE_SPEED; break;
        case 1: v->y += VEHICLE_SPEED; break;
        case 2: v->x -= VEHICLE_SPEED; break;
        case 3: v->y -= VEHICLE_SPEED; break;
    }

    // wrap around
    if (v->x > WINDOW_WIDTH) v->x = -v->w;
    if (v->x < -v->w) v->x = WINDOW_WIDTH;
    if (v->y > WINDOW_HEIGHT) v->y = -v->h;
    if (v->y < -v->h) v->y = WINDOW_HEIGHT;
}

/* ================= MAIN ================= */

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Traffic Simulator - 3 Lanes",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    srand(time(NULL));

    Vehicle vehicles[VEHICLE_COUNT];

    for (int i = 0; i < VEHICLE_COUNT; i++) {
        int dir = rand() % 4;
        int lane = rand() % LANE_COUNT;

        vehicles[i].dir = dir;

        if (dir == 0 || dir == 2) { // horizontal
            vehicles[i].y =
                (WINDOW_HEIGHT / 2) - (ROAD_WIDTH / 2) +
                lane * LANE_SIZE + (LANE_SIZE / 2) - 5;

            vehicles[i].x = rand() % WINDOW_WIDTH;
            vehicles[i].w = 22;
            vehicles[i].h = 10;
        } else { // vertical
            vehicles[i].x =
                (WINDOW_WIDTH / 2) - (ROAD_WIDTH / 2) +
                lane * LANE_SIZE + (LANE_SIZE / 2) - 5;

            vehicles[i].y = rand() % WINDOW_HEIGHT;
            vehicles[i].w = 10;
            vehicles[i].h = 22;
        }
    }

    SDL_Event e;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;
        }

        SDL_SetRenderDrawColor(renderer, 0, 120, 0, 255);
        SDL_RenderClear(renderer);

        drawRoads(renderer);

        for (int i = 0; i < VEHICLE_COUNT; i++) {
            updateVehicle(&vehicles[i]);
            drawVehicle(renderer, &vehicles[i]);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
