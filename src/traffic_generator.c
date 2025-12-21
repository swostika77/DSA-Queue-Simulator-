#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // For sleep()

#define FILENAME "vehicles.data"

void clearFile() {
    FILE* file = fopen(FILENAME, "w");
    if (file) fclose(file);
}

// Function to generate a random vehicle number
void generateVehicleNumber(char* buffer) {
    buffer[0] = 'A' + rand() % 26;
    buffer[1] = 'A' + rand() % 26;
    buffer[2] = '0' + rand() % 10;
    buffer[3] = 'A' + rand() % 26;
    buffer[4] = 'A' + rand() % 26;
    buffer[5] = '0' + rand() % 10;
    buffer[6] = '0' + rand() % 10;
    buffer[7] = '0' + rand() % 10;
    buffer[8] = '\0';
}

// Function to generate a random lane
char generateLane() {
    char lanes[] = {'A', 'B', 'C', 'D'};
    return lanes[rand() % 4];
}

int main() {

    srand(time(NULL)^getpid());
    clearFile(); // Initialize random seed
       FILE* file=fopen(FILENAME,"a");
       if(!file){
        perror("Error opening the file");
        return 1;
       }
    printf("Traffic Generator Started....\n");

    while (1) {
        char vehicle[9];
        generateVehicleNumber(vehicle);

        char lane=generateLane();

        // Write to file
        fprintf(file, "%s:%c\n", vehicle, lane);
        fflush(file); // Ensure data is written immediately
        printf("Generated: %s:%c\n", vehicle, lane);

        sleep(1); // Wait 1 second before generating next entry
    }
    fclose(file);

    return 0;
}