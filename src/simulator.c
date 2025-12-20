#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define VEHICLE_FILE "vehicles.data"
#define MAX_LINE 100

// Queue node structure
typedef struct Node {
    char vehicle[20];
    struct Node* next;
} Node;

// Queue structure
typedef struct Queue {
    Node* front;
    Node* rear;
    int size;
} Queue;

// Queue functions
void initQueue(Queue* q) {
    q->front = q->rear = NULL;
    q->size = 0;
}

void enqueue(Queue* q, const char* vehicle) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->vehicle, vehicle);
    newNode->next = NULL;

    if (q->rear == NULL)
        q->front = q->rear = newNode;
    else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->size++;
}

char* dequeue(Queue* q) {
    static char v[20];
    if (!q->front) return NULL;

    Node* temp = q->front;
    strcpy(v, temp->vehicle);

    q->front = temp->next;
    if (!q->front) q->rear = NULL;

    free(temp);
    q->size--;
    return v;
}

int isEmpty(Queue* q) {
    return q->size == 0;
}

typedef enum { RED, GREEN } LightState; 

void nextGreenRoad(char* road) {
    if (*road == 'A') *road = 'B';
    else if (*road == 'B') *road = 'C';
    else if (*road == 'C') *road = 'D';
    else *road = 'A';
}


int main() {
    srand(time(NULL));

    // Initialize queues
    Queue AL1, AL2, AL3, BL1, BL2, BL3, CL1, CL2, CL3, DL1, DL2, DL3;
    initQueue(&AL1); initQueue(&AL2); initQueue(&AL3);
    initQueue(&BL1); initQueue(&BL2); initQueue(&BL3);
    initQueue(&CL1); initQueue(&CL2); initQueue(&CL3);
    initQueue(&DL1); initQueue(&DL2); initQueue(&DL3);

    long lastPos = 0;
    char line[MAX_LINE];
    int priorityMode = 0;
      char currentGreenRoad = 'A';  

    printf("Simulator running...\n");

    while (1) {

        /* -------- READ VEHICLES FROM FILE -------- */
        FILE* file = fopen(VEHICLE_FILE, "r");
        if (file) {
            fseek(file, lastPos, SEEK_SET);

            while (fgets(line, sizeof(line), file)) {
                char vehicle[20], road;
                sscanf(line, "%[^:]:%c", vehicle, &road);

                int lane;
                if (road == 'A') {
                    int r = rand() % 100;
                    if (r < 70) lane = 1;       // AL2 priority lane
                    else if (r < 85) lane = 0; // AL1
                    else lane = 2;              // AL3
                } else {
                    lane = rand() % 3;
                }

                if (road == 'A') {
                    if (lane == 0) enqueue(&AL1, vehicle);
                    else if (lane == 1) enqueue(&AL2, vehicle);
                    else enqueue(&AL3, vehicle);
                } else if (road == 'B') {
                    if (lane == 0) enqueue(&BL1, vehicle);
                    else if (lane == 1) enqueue(&BL2, vehicle);
                    else enqueue(&BL3, vehicle);
                } else if (road == 'C') {
                    if (lane == 0) enqueue(&CL1, vehicle);
                    else if (lane == 1) enqueue(&CL2, vehicle);
                    else enqueue(&CL3, vehicle);
                } else if (road == 'D') {
                    if (lane == 0) enqueue(&DL1, vehicle);
                    else if (lane == 1) enqueue(&DL2, vehicle);
                    else enqueue(&DL3, vehicle);
                }
            }

            lastPos = ftell(file);
            fclose(file);
        }

        /* -------- PRIORITY CHECK -------- */
        if (AL2.size > 10) priorityMode = 1;
        else if (AL2.size < 5) priorityMode = 0;

        printf("\n--- New Cycle ---\n");
        printf("AL2 size = %d | Priority = %s\n",
               AL2.size, priorityMode ? "ON" : "OFF");

        /* -------- PRIORITY MODE -------- */
        if (priorityMode) {
               currentGreenRoad = 'A';
            printf("Priority Mode ACTIVE: Serving ONLY AL2\n");
            while (!isEmpty(&AL2) && AL2.size >= 5) {
                printf("Dequeued from AL2: %s (Remaining %d)\n",
                       dequeue(&AL2), AL2.size);
                sleep(1);
            }
            continue; // skip normal lanes
        }
              printf("Current GREEN Road: %c\n", currentGreenRoad);

        Queue* currentQueues[3];
        const char* currentNames[3];

        if (currentGreenRoad == 'A') { currentQueues[0] = &AL1; currentQueues[1] = &AL2; currentQueues[2] = &AL3; currentNames[0]="AL1"; currentNames[1]="AL2"; currentNames[2]="AL3";}
        else if (currentGreenRoad == 'B') { currentQueues[0] = &BL1; currentQueues[1] = &BL2; currentQueues[2] = &BL3; currentNames[0]="BL1"; currentNames[1]="BL2"; currentNames[2]="BL3";}
        else if (currentGreenRoad == 'C') { currentQueues[0] = &CL1; currentQueues[1] = &CL2; currentQueues[2] = &CL3; currentNames[0]="CL1"; currentNames[1]="CL2"; currentNames[2]="CL3";}
        else if (currentGreenRoad == 'D') { currentQueues[0] = &DL1; currentQueues[1] = &DL2; currentQueues[2] = &DL3; currentNames[0]="DL1"; currentNames[1]="DL2"; currentNames[2]="DL3";}

        // Serve 1 vehicle per lane
        for (int i = 0; i < 3; i++) {
            if (!isEmpty(currentQueues[i])) {
                printf("Dequeued from %s: %s (Remaining %d)\n", currentNames[i], dequeue(currentQueues[i]), currentQueues[i]->size);
                sleep(1);
            }
        }

        // Rotate to next GREEN road
        nextGreenRoad(&currentGreenRoad);

        sleep(1);  // prevent CPU hogging
    }

    return 0;
}