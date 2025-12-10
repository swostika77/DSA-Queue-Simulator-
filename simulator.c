#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
} Queue;

// Function to initialize a queue
void initQueue(Queue* q) {
    q->front = q->rear = NULL;
}

// Function to enqueue a vehicle
void enqueue(Queue* q, const char* vehicle) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->vehicle, vehicle);
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// Function to dequeue a vehicle
char* dequeue(Queue* q) {
    if (q->front == NULL) return NULL;
    Node* temp = q->front;
    static char vehicle[20];
    strcpy(vehicle, temp->vehicle);
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    return vehicle;
}

// Function to check if queue is empty
int isEmpty(Queue* q) {
    return q->front == NULL;
}

int main() {
    Queue laneA, laneB, laneC, laneD;
    initQueue(&laneA);
    initQueue(&laneB);
    initQueue(&laneC);
    initQueue(&laneD);

    long lastPos = 0;
    char line[MAX_LINE];

    printf("Simulator running...\n");

    while (1) {
        // Open the vehicle file
        FILE* file = fopen(VEHICLE_FILE, "r");
        if (!file) {
            perror("Error opening file");
            sleep(1);
            continue;
        }

        fseek(file, lastPos, SEEK_SET);

        // Read new lines and enqueue to appropriate lane
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0; // Remove newline
            char vehicle[20], lane;
            if (sscanf(line, "%[^:]:%c", vehicle, &lane) == 2) {
                switch(lane) {
                    case 'A': enqueue(&laneA, vehicle); break;
                    case 'B': enqueue(&laneB, vehicle); break;
                    case 'C': enqueue(&laneC, vehicle); break;
                    case 'D': enqueue(&laneD, vehicle); break;
                }
            }
        }

        lastPos = ftell(file); // Update last read position
        fclose(file);

        // Dequeue one vehicle per lane
        char* v;
        if ((v = dequeue(&laneA)) != NULL) printf("Dequeued from lane A: %s\n", v);
        if ((v = dequeue(&laneB)) != NULL) printf("Dequeued from lane B: %s\n", v);
        if ((v = dequeue(&laneC)) != NULL) printf("Dequeued from lane C: %s\n", v);
        if ((v = dequeue(&laneD)) != NULL) printf("Dequeued from lane D: %s\n", v);

        sleep(1); // Wait 1 second before next dequeue
    }

    return 0;
}
