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
void initQueue(Queue* q) { q->front = q->rear = NULL; q->size = 0; }
void enqueue(Queue* q, const char* vehicle) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->vehicle, vehicle);
    newNode->next = NULL;
    if (q->rear == NULL) q->front = q->rear = newNode;
    else { q->rear->next = newNode; q->rear = newNode; }
    q->size++;
}
char* dequeue(Queue* q) {
    if (q->front == NULL) return NULL;
    Node* temp = q->front;
    static char vehicle[20];
    strcpy(vehicle, temp->vehicle);
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    q->size--;
    return vehicle;
}
int isEmpty(Queue* q) { return q->size == 0; }

// Main function
int main() {
    srand(time(NULL));

    // Initialize queues
    Queue AL1, AL2, AL3, BL1, BL2, BL3, CL1, CL2, CL3, DL1, DL2, DL3;
    Queue* queues[] = {&AL1,&AL2,&AL3,&BL1,&BL2,&BL3,&CL1,&CL2,&CL3,&DL1,&DL2,&DL3};
    const char* names[] = {"AL1","AL2","AL3","BL1","BL2","BL3","CL1","CL2","CL3","DL1","DL2","DL3"};
    for(int i=0;i<12;i++) initQueue(queues[i]);

    long lastPos = 0;
    char line[MAX_LINE];
    int priorityMode = 0;

    printf("Simulator running...\n");

    while(1) {
        // Read new vehicles from file
        FILE* file = fopen(VEHICLE_FILE,"r");
        if(file) {
            fseek(file,lastPos,SEEK_SET);
            while(fgets(line,sizeof(line),file)){
                char vehicle[20], road;
                sscanf(line,"%[^:]:%c", vehicle, &road);
                int lane = rand()%5; // Random lane assignment
                if(road=='A'){
                    if(lane==0) enqueue(&AL1, vehicle);
                    else if(lane<=3) enqueue(&AL2, vehicle);
                    else enqueue(&AL3, vehicle);
                } else if(road=='B'){
                    if(lane==0) enqueue(&BL1, vehicle);
                    else if(lane==1) enqueue(&BL2, vehicle);
                    else enqueue(&BL3, vehicle);
                } else if(road=='C'){
                    if(lane==0) enqueue(&CL1, vehicle);
                    else if(lane==1) enqueue(&CL2, vehicle);
                    else enqueue(&CL3, vehicle);
                } else if(road=='D'){
                    if(lane==0) enqueue(&DL1, vehicle);
                    else if(lane==1) enqueue(&DL2, vehicle);
                    else enqueue(&DL3, vehicle);
                }
            }
            lastPos = ftell(file);
            fclose(file);
        }

        // Check priority lane AL2
        if(AL2.size > 10) priorityMode = 1;
        if(AL2.size < 5) priorityMode = 0;

        printf("\n--- New Cycle ---\n");

        // Priority mode: serve AL2 until size < 5
        if(priorityMode && !isEmpty(&AL2)) {
            printf("Priority Mode: Serving AL2\n");
            while(!isEmpty(&AL2) && AL2.size >= 5) {
                char* v = dequeue(&AL2);
                printf("Dequeued from AL2: %s (Remaining %d)\n", v, AL2.size);
                sleep(1); // simulate time to pass one vehicle
            }
        }

        // Normal mode: serve other lanes fairly
        Queue* normalQueues[] = {&AL1,&AL3,&BL1,&BL2,&BL3,&CL1,&CL2,&CL3,&DL1,&DL2,&DL3};
        const char* normalNames[] = {"AL1","AL3","BL1","BL2","BL3","CL1","CL2","CL3","DL1","DL2","DL3"};

        for(int i=0;i<11;i++){ // 11 because AL2 is priority
            if(!isEmpty(normalQueues[i])){
                char* v = dequeue(normalQueues[i]);
                printf("Dequeued from %s: %s (Remaining %d)\n", normalNames[i], v, normalQueues[i]->size);
                sleep(1); // simulate passing time
            }
        }

        // AL2 normal dequeue if not in priority mode
        if(!priorityMode && !isEmpty(&AL2)){
            char* v = dequeue(&AL2);
            printf("Dequeued from AL2: %s (Remaining %d)\n", v, AL2.size);
            sleep(1);
        }
    }

    return 0;
}
