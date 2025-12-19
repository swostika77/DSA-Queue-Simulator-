#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // For sleep()

#define FILENAME "vehicles.data"

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
    // First, create/clear the file
    FILE* file = fopen(FILENAME, "w");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
    fclose(file);

    // Open in append mode
    file = fopen(FILENAME, "a");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    srand(time(NULL)); // Initialize random seed

    while (1) {
        char vehicle[9];
        generateVehicleNumber(vehicle);

        char lane;
        int r = rand() % 10;
        if (r < 5) lane = 'A';      // 50% chance AL2 (priority lane)
        else lane = generateLane(); 

        // Write to file
        fprintf(file, "%s:%c\n", vehicle, lane);
        fflush(file); // Ensure data is written immediately

        // Print to console
        printf("Generated: %s:%c\n", vehicle, lane);

        sleep(1); // Wait 1 second before generating next entry
    }

    fclose(file);
    return 0;
}
