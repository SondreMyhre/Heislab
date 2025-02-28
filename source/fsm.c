#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> 
#include "fsm.h"
#include "elevio.h"


ElevatorState currentState = INITIALIZING;
int currentFloor = -1;
bool stopButtonPressed = false;
bool obstructionDetected = false;
int destinationFloor = 1;
int lastFloor = -1;



void elevator_fsm() {
    switch (currentState) {
        case INITIALIZING:
            printf("Heis: Søker etter etasje\n");
            elevio_motorDirection(DIRN_DOWN);
            currentFloor = elevator_get_current_floor();
            if (currentFloor != -1) {
            currentState = AT_FLOOR;
            }
            break;

        case AT_FLOOR:
            printf("Heis: Står stille i etasje %d. Venter på bestilling.\n", currentFloor);
            destinationFloor = exec_scan_orders();
            currentFloor = elevator_get_current_floor();
            if (destinationFloor != -1) {
                if (currentFloor < destinationFloor) {
                    currentState = MOVING_UP;
                } else if (currentFloor > destinationFloor) {
                    currentState = MOVING_DOWN;
                }
        }
            break;

        case MOVING_UP:
            printf("Heis: Kjører oppover\n");
            destinationFloor = exec_scan_orders();
            elevio_motorDirection(DIRN_UP);
            while (currentFloor < destinationFloor) {
                currentFloor = elevio_floorSensor();
                if (currentFloor == destinationFloor) {
                    elevio_motorDirection(DIRN_STOP);
                    currentState = AT_FLOOR;
                    break;
                }
            }
            break;

        case MOVING_DOWN:
            printf("Heis: Kjører nedover\n");
            destinationFloor = exec_scan_orders();
            elevio_motorDirection(DIRN_DOWN);
            while (currentFloor > destinationFloor) {
                currentFloor = elevio_floorSensor();
                if (currentFloor == destinationFloor) {
                    elevio_motorDirection(DIRN_STOP);
                    currentState = AT_FLOOR;
                        break;
                }
            }
            break;

        case EMERGENCY_STOP_FLOOR:
            printf("Heis: Nødstopp aktivert! Står stille.\n");
            elevio_motorDirection(DIRN_STOP);
            currentFloor = elevio_floorSensor();
            if (!stopButtonPressed && currentFloor != -1) {
                currentState = INITIALIZING;
            }
            break;

        case EMERGENCY_STOP_SHAFT:
            printf("Heis: Nødstopp aktivert! Står stille i shaft.\n");
            elevio_motorDirection(DIRN_STOP);
            currentFloor = elevio_floorSensor();
            if (!stopButtonPressed && currentFloor == -1) {
                currentState = INITIALIZING;
            }
            break;
    }
}

void elevator_init() {
    elevio_init();
    while(elevio_floorSensor() == -1) {
        elev_set_motor_direction(DIRN_DOWN);
    }
    elev_set_motor_direction(DIRN_STOP);
    currentState = INITIALIZING;
    currentFloor = elevio_floorSensor();
    stopButtonPressed = false;
    obstructionDetected = false;
}

void elevator_request_floor(int floor) {
    destinationFloor = floor;
}

void elevator_stop() {
    stopButtonPressed = true;
    currentState = EMERGENCY_STOP_FLOOR;
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
    return elevio_floorSensor();
}

void exec_set_last_floor(int floor) {
    lastFloor = floor;
}

void fsmSetState(ElevatorState newState) {
    currentState = newState;
}


