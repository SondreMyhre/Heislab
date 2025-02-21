#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> 
#include <fsm.h>

typedef enum {
    IDLE,
    MOVING_UP,
    MOVING_DOWN,
    DOOR_OPEN,
    EMERGENCY_STOP,
    INITIALIZING,
    OBSTRUCTION
} ElevatorState;

ElevatorState currentState = INITIALIZING;
int currentFloor = -1; 
bool stopButtonPressed = false;
bool obstructionDetected = false;
int destinationFloor = -1;

int exec_scan_orders() {

    return -1;
}

void elevator_fsm() {
    switch (currentState) {
        case INITIALIZING:
            printf("Heis: Søker etter etasje");
            currentFloor = 1; 
            currentState = IDLE;
            break;

        case IDLE:
            printf("Heis: Står stille i etasje %d. Venter på bestilling.", currentFloor);
            destinationFloor = exec_scan_orders(); // Sjekker bestillinger
            
            if (destinationFloor != -1) {
                if (currentFloor < destinationFloor) {
                    currentState = MOVING_UP;
                } else if (currentFloor > destinationFloor) {
                    currentState = MOVING_DOWN;
                }
            }
            break;

        case MOVING_UP:
            printf("Heis: Kjører oppover");
            sleep(1);
            currentFloor++;
            
            if (currentFloor == destinationFloor) {
                currentState = DOOR_OPEN;
            }
            break;

        case MOVING_DOWN:
            printf("Heis: Kjører nedover");
            sleep(1);
            currentFloor--;
            
            if (currentFloor == destinationFloor) {
                currentState = DOOR_OPEN;
            }
            break;

        case DOOR_OPEN:
            printf("Heis: Åpner døren i etasje %d.", currentFloor);
            sleep(3); 
            
            if (!obstructionDetected) {
                currentState = IDLE;
            } else {
                currentState = OBSTRUCTION;
            }
            break;

        case OBSTRUCTION:
            printf("Heis: Obstruksjon oppdaget! Døren forblir åpen.");
            if (!obstructionDetected) {
                printf("Heis: Obstruksjon fjernet, lukker døren.");
                currentState = DOOR_OPEN;
            }
            break;

        case EMERGENCY_STOP:
            printf("Heis: Nødstopp aktivert! Står stille.");
            if (!stopButtonPressed) {
                currentState = INITIALIZING;
            }
            break;
    }
}

void elevator_init() {
    currentState = INITIALIZING;
    currentFloor = -1;
    stopButtonPressed = false;
    obstructionDetected = false;
    destinationFloor = -1;
}

void elevator_request_floor(int floor) {
    destinationFloor = floor;
}

void elevator_stop() {
    stopButtonPressed = true;
    currentState = EMERGENCY_STOP;
}

void elevator_clear_stop() {
    stopButtonPressed = false;
    currentState = INITIALIZING;
}

void elevator_set_obstruction(bool status) {
    obstructionDetected = status;
}

ElevatorState elevator_get_state() {
    return currentState;
}

int elevator_get_current_floor() {
    return currentFloor;
}





