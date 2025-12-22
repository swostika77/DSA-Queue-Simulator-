// vehicle.h
#ifndef VEHICLE_H
#define VEHICLE_H

typedef struct {
    char id[20];   // vehicle name/number
    char road;     // 'A','B','C','D'
    int lane;      // 0,1,2
    int x, y;      // position for SDL
    int dir;       // 0=right,1=down,2=left,3=up
    int active;    // 1=vehicle waiting/active, 0=passed
    int w ,h;
} Vehicle;

#endif
//gcc src/main.c -o bin/simulator.exe -I./SDL2/include -I./include -L./SDL2/lib -lmingw32 -lSDL2main -lSDL2 -mwindows
