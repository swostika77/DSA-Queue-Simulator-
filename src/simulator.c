#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W 800
#define H 800
#define ROAD_W 180
#define LANE_W (ROAD_W/3)
#define VEH_W 30
#define VEH_H 20
#define SPEED 2
#define SPAWN_INTERVAL 10 // frames between checking file
#define TIME_PER_VEHICLE 100 // ms per vehicle
#define MIN_GREEN_TIME 300
#define PRIORITY_COUNT 10  
#define MAX_VEHICLES 500

typedef enum { RED, GREEN } Light;

typedef struct {
    float x, y;
    float dx, dy;
    char road;
    int lane;   
    int turned;
    int active;
} Vehicle;

typedef struct {
    Vehicle q[MAX_VEHICLES];
    int front, rear, count;
} VehicleQueue;

VehicleQueue VQ;

Light lightA = GREEN, lightB = RED, lightC = RED, lightD = RED;
int lightTimer = 0;
int GREEN_TIME = 300;
char currentRoad = 'A';

#define STOP_A (H/2 - ROAD_W/2 - 5)
#define STOP_B (H/2 + ROAD_W/2 + 5)
#define STOP_C (W/2 - ROAD_W/2 - 5)
#define STOP_D (W/2 + ROAD_W/2 + 5)

void initQueue() {
    VQ.front = 0;
    VQ.rear = -1;
    VQ.count = 0;
}

void enqueue(Vehicle v) {
    if (VQ.count >= MAX_VEHICLES) return;
    VQ.rear = (VQ.rear + 1) % MAX_VEHICLES;
    VQ.q[VQ.rear] = v;
    VQ.q[VQ.rear].active = 1;
    VQ.count++;
}

void addVehicle(char r, int lane) {
    Vehicle v = {0};
    v.road = r;
    v.lane = lane;
    v.turned = 0;

    int cx = W/2, cy = H/2;
    if(lane==0){ // left-turn lane
        if(r=='A'){ v.x=cx-ROAD_W/2+2*LANE_W+(LANE_W-VEH_W)/2; v.y=-VEH_H; v.dx=0; v.dy=SPEED; }
        if(r=='B'){ v.x=cx-ROAD_W/2+(LANE_W-VEH_W)/2; v.y=H; v.dx=0; v.dy=-SPEED; }
        if(r=='C'){ v.x=-VEH_W; v.y=cy-ROAD_W/2+(LANE_W-VEH_H)/2; v.dx=SPEED; v.dy=0; }
        if(r=='D'){ v.x=W; v.y=cy-ROAD_W/2+2*LANE_W+(LANE_W-VEH_H)/2; v.dx=-SPEED; v.dy=0; }
    } else if(lane==1){ // middle lane (straight)
        if(r=='A'){ v.x=cx-ROAD_W/2+LANE_W+(LANE_W-VEH_W)/2; v.y=-VEH_H; v.dx=0; v.dy=SPEED; }
        if(r=='B'){ v.x=cx-ROAD_W/2+LANE_W+(LANE_W-VEH_W)/2; v.y=H; v.dx=0; v.dy=-SPEED; }
        if(r=='C'){ v.x=-VEH_W; v.y=cy-ROAD_W/2+LANE_W+(LANE_W-VEH_H)/2; v.dx=SPEED; v.dy=0; }
        if(r=='D'){ v.x=W; v.y=cy-ROAD_W/2+LANE_W+(LANE_W-VEH_H)/2; v.dx=-SPEED; v.dy=0; }
    } else { 
        if(r=='A'){ v.x=cx-ROAD_W/2+LANE_W*2+(LANE_W-VEH_W)/2; v.y=-VEH_H; v.dx=0; v.dy=0; }
        if(r=='B'){ v.x=cx-ROAD_W/2+LANE_W*2+(LANE_W-VEH_W)/2; v.y=H; v.dx=0; v.dy=0; }
        if(r=='C'){ v.x=-VEH_W; v.y=cy-ROAD_W/2+LANE_W*2+(LANE_W-VEH_H)/2; v.dx=0; v.dy=0; }
        if(r=='D'){ v.x=W; v.y=cy-ROAD_W/2+LANE_W*2+(LANE_W-VEH_H)/2; v.dx=0; v.dy=0; }
    }

    enqueue(v);
}

int countVehicles(char road, int lane) {
    if(lane == 2) return 0; 
    int c=0;
    for(int i=0;i<VQ.count;i++){
        int idx=(VQ.front+i)%MAX_VEHICLES;
        Vehicle* v=&VQ.q[idx];
        if(v->active && v->road==road && v->lane==lane && !v->turned)
            c++;
    }
    return c;
}

void updateLights() {
    lightTimer++;

    int middleA = countVehicles('A',1);
    if(middleA > PRIORITY_COUNT && currentRoad != 'A'){ 
        lightTimer = 0;
        GREEN_TIME = middleA * TIME_PER_VEHICLE;
        if(GREEN_TIME < MIN_GREEN_TIME) GREEN_TIME = MIN_GREEN_TIME;
        lightA = GREEN; lightB=lightC=lightD=RED;
        currentRoad='A';
        return;
    }

    if(lightTimer < GREEN_TIME) return;

    int lane0 = countVehicles(currentRoad, 0);
    int lane1 = countVehicles(currentRoad, 1); 
    int sum = lane0 + lane1;

    int normal_lanes = 2; 
    GREEN_TIME = (sum / normal_lanes) * TIME_PER_VEHICLE;
    if(GREEN_TIME < MIN_GREEN_TIME) GREEN_TIME = MIN_GREEN_TIME;

    lightTimer = 0;
    lightA=lightB=lightC=lightD=RED;
    if(currentRoad=='A'){ currentRoad='B'; lightB=GREEN; }
    else if(currentRoad=='B'){ currentRoad='C'; lightC=GREEN; }
    else if(currentRoad=='C'){ currentRoad='D'; lightD=GREEN; }
    else { currentRoad='A'; lightA=GREEN; }
}

void moveVehicles() {
    for(int i=0;i<VQ.count;i++){
        int idx=(VQ.front+i)%MAX_VEHICLES;
        Vehicle* v=&VQ.q[idx];
        if(!v->active) continue;

        if(v->lane == 2) continue;

        int stop=0;
        if(!v->turned){
            if(v->road=='A' && lightA==RED && v->y+VEH_H>=STOP_A) stop=1;
            if(v->road=='B' && lightB==RED && v->y<=STOP_B) stop=1;
            if(v->road=='C' && lightC==RED && v->x+VEH_W>=STOP_C) stop=1;
            if(v->road=='D' && lightD==RED && v->x<=STOP_D) stop=1;
        }

        if(!stop){ v->x += v->dx; v->y += v->dy; }

        // Left-turn
        if(!v->turned && v->lane==0){
            int cx=W/2, cy=H/2;
            if(v->road=='A' && v->y>=cy-ROAD_W/2){ 
                v->dx=SPEED; v->dy=0; 
                v->y = cy-ROAD_W/2 + (LANE_W-VEH_H)/2; 
                v->turned=1; 
            }
            if(v->road=='B' && v->y<=cy+ROAD_W/2){ 
                v->dx=-SPEED; v->dy=0; 
                v->y = cy+ROAD_W/2 - LANE_W + (LANE_W-VEH_H)/2; 
                v->turned=1; 
            }
            if(v->road=='C' && v->x>=cx-ROAD_W/2){ 
                v->dx=0; v->dy=-SPEED; 
                v->x = cx-ROAD_W/2 + (LANE_W-VEH_W)/2; 
                v->turned=1; 
            }
            if(v->road=='D' && v->x<=cx+ROAD_W/2){ 
                v->dx=0; v->dy=SPEED; 
                v->x = cx+ROAD_W/2 - LANE_W + (LANE_W-VEH_W)/2; 
                v->turned=1; 
            }
        }

        // Middle lane straight
        if(!v->turned && v->lane==1){
            int cx=W/2, cy=H/2;
            if(v->road=='A' && v->y>=cy-ROAD_W/2) v->turned=1;
            if(v->road=='B' && v->y<=cy+ROAD_W/2) v->turned=1;
            if(v->road=='C' && v->x>=cx-ROAD_W/2) v->turned=1;
            if(v->road=='D' && v->x<=cx+ROAD_W/2) v->turned=1;
        }

        if(v->x>W || v->x<-VEH_W || v->y>H || v->y<-VEH_H) v->active=0;
    }
}

void drawRoads(SDL_Renderer* r){
    int cx=W/2, cy=H/2;
    
    SDL_SetRenderDrawColor(r,100,100,100,255);
    SDL_Rect v={ cx - ROAD_W/2, 0, ROAD_W, H };
    SDL_Rect h={ 0, cy - ROAD_W/2, W, ROAD_W };
    SDL_RenderFillRect(r,&v);
    SDL_RenderFillRect(r,&h);

    SDL_SetRenderDrawColor(r,255,255,255,255);
    int dashLen = 15;   
    int gapLen  = 10;   

    for(int i=1;i<3;i++){
        int x = cx-ROAD_W/2 + i*LANE_W;
        int y = cy-ROAD_W/2 + i*LANE_W;

        for(int j=0;j<H;j+=dashLen+gapLen){
            SDL_RenderDrawLine(r, x, j, x, j+dashLen);
        }

        for(int j=0;j<W;j+=dashLen+gapLen){
            SDL_RenderDrawLine(r, j, cy-ROAD_W/2 + i*LANE_W, j+dashLen, cy-ROAD_W/2 + i*LANE_W);
        }
    }
}

void drawVehicles(SDL_Renderer* r){
    for(int i=0;i<VQ.count;i++){
        int idx=(VQ.front+i)%MAX_VEHICLES;
        Vehicle* v=&VQ.q[idx];
        if(!v->active) continue;

        if(v->lane==0) SDL_SetRenderDrawColor(r,255,165,0,255);  
        else if(v->lane==1) SDL_SetRenderDrawColor(r,0,255,255,255); 
        else SDL_SetRenderDrawColor(r,200,200,200,255); 

        SDL_Rect rect={v->x,v->y,VEH_W,VEH_H};
        SDL_RenderFillRect(r,&rect);
    }
}

void drawLights(SDL_Renderer* r){
    int cx=W/2, cy=H/2;
    int offset=3;
    int size=12;

    SDL_Color c;
    c = (lightA==GREEN)?(SDL_Color){0,255,0,255}:(SDL_Color){255,0,0,255};
    SDL_SetRenderDrawColor(r,c.r,c.g,c.b,255);
    SDL_Rect la={cx-size/2, cy-ROAD_W/2-offset, size, size};
    SDL_RenderFillRect(r,&la);

    c = (lightB==GREEN)?(SDL_Color){0,255,0,255}:(SDL_Color){255,0,0,255};
    SDL_SetRenderDrawColor(r,c.r,c.g,c.b,255);
    SDL_Rect lb={cx-size/2, cy+ROAD_W/2+offset-size, size, size};
    SDL_RenderFillRect(r,&lb);

    c = (lightC==GREEN)?(SDL_Color){0,255,0,255}:(SDL_Color){255,0,0,255};
    SDL_SetRenderDrawColor(r,c.r,c.g,c.b,255);
    SDL_Rect lc={cx-ROAD_W/2-offset, cy-size/2, size, size};
    SDL_RenderFillRect(r,&lc);

    c = (lightD==GREEN)?(SDL_Color){0,255,0,255}:(SDL_Color){255,0,0,255};
    SDL_SetRenderDrawColor(r,c.r,c.g,c.b,255);
    SDL_Rect ld={cx+ROAD_W/2+offset-size, cy-size/2, size, size};
    SDL_RenderFillRect(r,&ld);
}

void spawnVehiclesFromFile(){
    FILE* f=fopen("vehicles.data","r");
    if(!f) return;
    char line[50];
    while(fgets(line,sizeof(line),f)){
        if(strlen(line)<2) continue;
        char r=line[strlen(line)-2]; // A/B/C/D
        addVehicle(r,rand()%3); 
    }
    fclose(f);
    f=fopen("vehicles.data","w"); if(f) fclose(f);
}

int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* w=SDL_CreateWindow("Queue Traffic Simulator",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,W,H,SDL_WINDOW_SHOWN);
    SDL_Renderer* r=SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED);

    initQueue();

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
    }

    SDL_Quit();
    return 0;
}
