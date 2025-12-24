#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define W 800
#define H 800
#define ROAD_W 180
#define LANE_W (ROAD_W/3)

#define VEH_W 30
#define VEH_H 20
#define SPEED 2
#define SPAWN_INTERVAL 10 // frames between checking file
#define TIME_PER_VEHICLE 100 // ms per vehicle
#define MIN_GREEN_TIME 300    // minimum green time for visibility

typedef enum { RED, GREEN } Light;

typedef struct Vehicle {
    float x, y;
    float dx, dy;
    char road;       // original road
    int lane;        // 0 = left-turn, 1 = middle-straight
    int turned;      // has passed intersection
    struct Vehicle* next;
} Vehicle;

Vehicle* head = NULL;

/* Traffic lights */
Light lightA = GREEN, lightB = RED, lightC = RED, lightD = RED;
int lightTimer = 0;
int GREEN_TIME = 300;
char currentRoad = 'A';

#define STOP_A (H/2 - ROAD_W/2 - 5)
#define STOP_B (H/2 + ROAD_W/2 + 5)
#define STOP_C (W/2 - ROAD_W/2 - 5)
#define STOP_D (W/2 + ROAD_W/2 + 5)

/* Add vehicle to simulation */
void addVehicle(char r, int lane) {
    Vehicle* v = malloc(sizeof(Vehicle));
    v->road = r;
    v->lane = lane;
    v->turned = 0;

    if(lane == 0){ // left-turn lane
        if(r=='A'){ v->x=W/2-ROAD_W/2+2*LANE_W+(LANE_W-VEH_W)/2; v->y=-VEH_H; v->dx=0; v->dy=SPEED;}
        if(r=='B'){ v->x=W/2-ROAD_W/2+(LANE_W-VEH_W)/2; v->y=H; v->dx=0; v->dy=-SPEED;}
        if(r=='C'){ v->x=-VEH_W; v->y=H/2-ROAD_W/2+(LANE_W-VEH_H)/2; v->dx=SPEED; v->dy=0;}
        if(r=='D'){ v->x=W; v->y=H/2-ROAD_W/2+2*LANE_W+(LANE_W-VEH_H)/2; v->dx=-SPEED; v->dy=0;}
    }
    else if(lane == 1){ // middle lane straight
        if(r=='A'){ v->x=W/2-ROAD_W/2+LANE_W+(LANE_W-VEH_W)/2; v->y=-VEH_H; v->dx=0; v->dy=SPEED;}
        if(r=='B'){ v->x=W/2-ROAD_W/2+LANE_W+(LANE_W-VEH_W)/2; v->y=H; v->dx=0; v->dy=-SPEED;}
        if(r=='C'){ v->x=-VEH_W; v->y=H/2-ROAD_W/2+LANE_W+(LANE_W-VEH_H)/2; v->dx=SPEED; v->dy=0;}
        if(r=='D'){ v->x=W; v->y=H/2-ROAD_W/2+LANE_W+(LANE_W-VEH_H)/2; v->dx=-SPEED; v->dy=0;}
    }

    v->next = head;
    head = v;
}

/* Count vehicles in a specific road/lane type before intersection */
int countVehicles(char road, int laneType){
    Vehicle* v = head;
    int count = 0;
    while(v){
        if(v->road == road && v->lane == laneType && !v->turned)
            count++;
        v = v->next;
    }
    return count;
}

/* Update traffic lights with dynamic green time */
void updateLights() {
    lightTimer++;
    if(lightTimer < GREEN_TIME) return;

    // Compute number of vehicles in current road's normal lanes
    int total = countVehicles(currentRoad,0) + countVehicles(currentRoad,1);
    int n = 2; // left + middle lanes
    int V = (n>0) ? total/n : 1;

// Green time calculation based on vehicles
    GREEN_TIME = V * TIME_PER_VEHICLE;
    if(GREEN_TIME < MIN_GREEN_TIME) GREEN_TIME = MIN_GREEN_TIME; // minimum green

    // Reset timer and switch lights
    lightTimer = 0;
    lightA=lightB=lightC=lightD=RED;
    if(currentRoad=='A'){ currentRoad='B'; lightB=GREEN; }
    else if(currentRoad=='B'){ currentRoad='C'; lightC=GREEN; }
    else if(currentRoad=='C'){ currentRoad='D'; lightD=GREEN; }
    else { currentRoad='A'; lightA=GREEN; }
}

// Move vehicles
void moveVehicles() {
    Vehicle* v=head;
    Vehicle* prev=NULL;

    while(v){
        int stop=0;

        // Stop at red light ONLY if vehicle has not passed intersection
        if(!v->turned){
            if(v->road=='A' && lightA==RED && v->y+VEH_H>=STOP_A) stop=1;
            if(v->road=='B' && lightB==RED && v->y<=STOP_B) stop=1;
            if(v->road=='C' && lightC==RED && v->x+VEH_W>=STOP_C) stop=1;
            if(v->road=='D' && lightD==RED && v->x<=STOP_D) stop=1;
        }

        if(!stop){
            v->x += v->dx;
            v->y += v->dy;
        }

        // Left-turn vehicles: turn at intersection
        if(!v->turned && v->lane==0){
            if(v->road=='A' && v->y>=H/2-ROAD_W/2){ v->dx=SPEED; v->dy=0; v->y=H/2-ROAD_W/2+LANE_W/2; v->turned=1;}
            if(v->road=='B' && v->y<=H/2+ROAD_W/2){ v->dx=-SPEED; v->dy=0; v->y=H/2-ROAD_W/2+2*LANE_W+LANE_W/2; v->turned=1;}
            if(v->road=='C' && v->x>=W/2-ROAD_W/2){ v->dx=0; v->dy=-SPEED; v->x=W/2-ROAD_W/2+LANE_W/2; v->turned=1;}
            if(v->road=='D' && v->x<=W/2+ROAD_W/2){ v->dx=0; v->dy=SPEED; v->x=W/2-ROAD_W/2+2*LANE_W+LANE_W/2; v->turned=1;}
        }

        // Middle-lane vehicles: mark as "passed intersection" once reached center
        if(!v->turned && v->lane==1){
            if(v->road=='A' && v->y>=H/2-ROAD_W/2) v->turned=1;
            if(v->road=='B' && v->y<=H/2+ROAD_W/2) v->turned=1;
            if(v->road=='C' && v->x>=W/2-ROAD_W/2) v->turned=1;
            if(v->road=='D' && v->x<=W/2+ROAD_W/2) v->turned=1;
        }

        // Remove vehicles that leave screen
        int remove=0;
        if(v->x>W||v->x<-VEH_W||v->y>H||v->y<-VEH_H) remove=1;
        if(remove){
            Vehicle* temp=v;
            if(prev) prev->next=v->next;
            else head=v->next;
            v=v->next;
            free(temp);
        } else { prev=v; v=v->next; }
    }
}

/* Draw functions */
void drawRoads(SDL_Renderer* r){
    SDL_SetRenderDrawColor(r,100,100,100,255);
    SDL_Rect v={W/2-ROAD_W/2,0,ROAD_W,H};
    SDL_Rect h={0,H/2-ROAD_W/2,W,ROAD_W};
    SDL_RenderFillRect(r,&v);
    SDL_RenderFillRect(r,&h);

    SDL_SetRenderDrawColor(r,255,255,255,255);
    for(int i=1;i<3;i++){
        SDL_RenderDrawLine(r,W/2-ROAD_W/2+i*LANE_W,0,W/2-ROAD_W/2+i*LANE_W,H);
        SDL_RenderDrawLine(r,0,H/2-ROAD_W/2+i*LANE_W,W,H/2-ROAD_W/2+i*LANE_W);
    }
}

void drawVehicles(SDL_Renderer* r){
    SDL_SetRenderDrawColor(r,255,255,255,255);
    Vehicle* v=head;
    while(v){
        SDL_Rect rect={v->x,v->y,VEH_W,VEH_H};
        SDL_RenderFillRect(r,&rect);
        v=v->next;
    }
}

void drawLights(SDL_Renderer* r){
    SDL_Color c;
    int offset = 30; // move lights toward junction

    // Road A
    c = (lightA==GREEN) ? (SDL_Color){0,255,0,255} : (SDL_Color){255,0,0,255};
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
    SDL_Rect la = { W/2-10, STOP_A + offset, 20, 20 };
    SDL_RenderFillRect(r, &la);

    // Road B
    c = (lightB==GREEN) ? (SDL_Color){0,255,0,255} : (SDL_Color){255,0,0,255};
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
    SDL_Rect lb = { W/2-10, STOP_B - offset, 20, 20 };
    SDL_RenderFillRect(r, &lb);

    // Road C
    c = (lightC==GREEN) ? (SDL_Color){0,255,0,255} : (SDL_Color){255,0,0,255};
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
    SDL_Rect lc = { STOP_C + offset, H/2-10, 20, 20 };
    SDL_RenderFillRect(r, &lc);

    // Road D
    c = (lightD==GREEN) ? (SDL_Color){0,255,0,255} : (SDL_Color){255,0,0,255};
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
    SDL_Rect ld = { STOP_D - offset, H/2-10, 20, 20 };
    SDL_RenderFillRect(r, &ld);
}

/* Read vehicles from file and spawn them */
int spawnVehiclesFromFile() {
    FILE* f=fopen("vehicles.data","r");
    if(!f) return 0;
    char line[50];
    int spawned=0;
    while(fgets(line,sizeof(line),f)){
        if(strlen(line)<2) continue;
        char road=line[strlen(line)-2]; // A/B/C/D
        int lane = rand()%2; // 0=left, 1=middle
        addVehicle(road, lane);
        spawned++;
    }
    fclose(f);
    // Clear file
    f=fopen("vehicles.data","w"); if(f) fclose(f);
    return spawned;
}

/* MAIN */
int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* w=SDL_CreateWindow("Traffic Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W,H, SDL_WINDOW_SHOWN);
    SDL_Renderer* r=SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED);

    int run=1, frame=0;
    SDL_Event e;

    while(run){
        while(SDL_PollEvent(&e)) if(e.type==SDL_QUIT) run=0;

        frame++;
        if(frame%SPAWN_INTERVAL==0) spawnVehiclesFromFile();

        updateLights();
        moveVehicles();

        SDL_SetRenderDrawColor(r,50,50,50,255);
        SDL_RenderClear(r);

        drawRoads(r);
        drawLights(r);
        drawVehicles(r);

        SDL_RenderPresent(r);
        SDL_Delay(16);

        // Stop if generator stopped and no vehicles remain
        if(head==NULL){
            FILE* f=fopen("vehicles.data","r");
            if(f){
                fseek(f,0,SEEK_END);
                if(ftell(f)==0){ // empty file
                    fclose(f);
                    break;
                }
                fclose(f);
            }
        }
    }

    SDL_Quit();
    return 0;
}
