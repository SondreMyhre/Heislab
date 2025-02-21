#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> 
#include "driver/elevio.h"
#include "fsm.h"


ElevatorState currentState = INITIALIZING;
int currentFloor = -1; 
bool stopButtonPressed = false;
bool obstructionDetected = false;
int destinationFloor = 1;

int exec_scan_orders() {

    return -1;
}

void elevator_fsm() {
    switch (currentState) {
        case INITIALIZING:
            printf("Heis: Søker etter etasje");
            while(currentFloor == -1) {
                elevio_motorDirection(DIRN_DOWN);
                currentFloor = elevio_floorSensor();
                if (currentFloor != -1) {
                    elevio_motorDirection(DIRN_STOP);
                    elevio_floorIndicator(currentFloor);
                    currentState = AT_FLOOR;
                }
            }
            break;

        case AT_FLOOR:
            printf("Heis: Står stille i etasje %d. Venter på bestilling.", currentFloor);
            elevio_doorOpenLamp(1);
            sleep(3);
            elevio_doorOpenLamp(0);
            
            destinationFloor = exec_scan_orders(); // Sjekker bestillinger
            
           
            if (currentFloor < destinationFloor) {
                currentState = MOVING_UP;
            } else if (currentFloor > destinationFloor) {
                currentState = MOVING_DOWN;
            } else if (currentFloor == destinationFloor) {
                currentState = AT_FLOOR;
            }
            break;


        case MOVING_UP:
            printf("Heis: Kjører oppover");
            sleep(1);
            elevio_motorDirection(DIRN_UP);

            while (currentFloor < destinationFloor) {
                int floor = elevio_floorSensor();
                if (floor != -1) {
                    currentFloor = floor;
                    elevio_floorIndicator(currentFloor);
                }
            }
            
            elevio_motorDirection(DIRN_STOP);
            currentState = AT_FLOOR;
            break;

        case MOVING_DOWN:
            printf("Heis: Kjører nedover");
            sleep(1);
            elevio_motorDirection(DIRN_DOWN);
            
            while (currentFloor > destinationFloor) {
                int floor = elevio_floorSensor();
                if (floor != -1) {
                    currentFloor = floor;
                    elevio_floorIndicator(currentFloor);
                }
            }
            
            elevio_motorDirection(DIRN_STOP);
            currentState = AT_FLOOR;
            break;

        /*case DOOR_OPEN:
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
            break;*/

        case EMERGENCY_STOP_FLOOR:
            elevio_motorDirection(DIRN_STOP);
            elevio_doorOpenLamp(1);
            printf("Heis: Nødstopp aktivert! Står stille.");
            if (!stopButtonPressed && currentFloor != -1) {
                currentState = INITIALIZING;
            }
            break;

        case EMERGENCY_STOP_SHAFT:
            printf("Heis: Nødstopp aktivert! Står stille i shaft.");
            if (!stopButtonPressed && currentFloor == -1) {
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
    destinationFloor = 1;
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





