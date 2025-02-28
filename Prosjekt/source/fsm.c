
#include <stdio.h>
#include <unistd.h>
#include "driver/elevio.h"
#include "fsm.h"
#include "queue.h"


ElevatorState currentState = INITIALIZING;
int currentFloor = -1; 
int stopButtonPressed = 0;
int obstructionDetected = 0;
int destinationFloor = -1;
OrderMatrix orderMatrix;

void orderScanner() {
    for (int f = 0; f < N_FLOORS; f++) {
        for (int b = 0; b < N_BUTTONS; b++) {
            if (elevio_callButton(f, b)) {
                setOrderStatus(&orderMatrix, b, f, 1);
            }
        }
    }
}

void elevator_fsm() {
    orderScanner();
    /*
    if (elevio_stopButton()) {
        stopButtonPressed = 0;
        if (currentFloor != -1) {
            currentState = EMERGENCY_STOP_FLOOR;
        } else {
            currentState = EMERGENCY_STOP_SHAFT;
        }
    }
    */
    switch (currentState) {
        case INITIALIZING:
            printf("Heis: Søker etter etasje");
            while(currentFloor == -1) {
                elevio_motorDirection(DIRN_DOWN);
                currentFloor = elevio_floorSensor();    
                if (currentFloor != -1) {
                    elevio_motorDirection(DIRN_STOP);
                    elevio_floorIndicator(currentFloor);
                    printf("Heis: Står i etasje %d", currentFloor);
                    currentState = IDLE;
                }
            }
            break;

        case IDLE:
            orderScanner();
            elevio_floorIndicator(currentFloor);
            //clearScreen();
            //printf("Heis: Står stille i etasje %d. Venter på bestilling.", currentFloor);
            orderScanner();
            destinationFloor = getNextDestination(&orderMatrix, currentFloor, destinationFloor);
            if (destinationFloor < 0) {
                break;
            } else if (currentFloor < destinationFloor) {
                currentState = MOVING_UP;
            } else if (currentFloor > destinationFloor) {
                currentState = MOVING_DOWN;
            } else if (currentFloor == destinationFloor) {
                currentState = AT_DESTINATION;
            }
            break;

        case AT_DESTINATION:
            orderScanner();
            clearOrderMatrix(&orderMatrix, currentFloor);
            elevio_floorIndicator(currentFloor);
            //clearScreen();
            //printf("Heis: ankommet %d. etasje", currentFloor);
            //elevio_doorOpenLamp(1);
            //sleepScan(3000); // 3 sekunder (3000 ms)
            //elevio_doorOpenLamp(0);
            destinationFloor = getNextDestination(&orderMatrix, currentFloor, destinationFloor);     
            if (destinationFloor < 0) {
                currentState = IDLE;
            } else if (currentFloor < destinationFloor) {
                currentState = MOVING_UP;
            } else if (currentFloor > destinationFloor) {
                currentState = MOVING_DOWN;
            } else if (currentFloor == destinationFloor) {
                currentState = AT_DESTINATION;
            }
            break;


        case MOVING_UP:
            orderScanner();
            clearScreen();
            //printf("Heis: Kjører oppover");
            elevio_motorDirection(DIRN_UP);
            int floor = elevio_floorSensor();
            while (currentFloor < destinationFloor) {
                int floor = elevio_floorSensor();
                if (floor != -1) {
                    currentFloor = floor;
                    elevio_floorIndicator(currentFloor);
                }
                orderScanner();
                destinationFloor = getNextDestination(&orderMatrix, currentFloor, destinationFloor);
            }
            
            elevio_motorDirection(DIRN_STOP);
            currentState = AT_DESTINATION;
            break;

        case MOVING_DOWN:
            orderScanner();
            clearScreen();
            //printf("Heis: Kjører nedover");
            elevio_motorDirection(DIRN_DOWN);
            while (currentFloor > destinationFloor) {
                orderScanner();
                destinationFloor = getNextDestination(&orderMatrix, currentFloor, destinationFloor);
                int floor = elevio_floorSensor();
                if (floor != -1) {
                    currentFloor = floor;
                    elevio_floorIndicator(currentFloor);
                }
            }
            
            elevio_motorDirection(DIRN_STOP);
            currentState = AT_DESTINATION;
            break;

        case EMERGENCY_STOP_FLOOR:
            elevio_motorDirection(DIRN_STOP);
            //elevio_doorOpenLamp(1);
            clearScreen();
            //printf("Heis: Nødstopp aktivert! Står stille.");
            if (!stopButtonPressed && currentFloor != -1) {
                currentState = INITIALIZING;
            }
            break;

        case EMERGENCY_STOP_SHAFT:
            //clearScreen();
            //printf("Heis: Nødstopp aktivert! Står stille i shaft.");
            if (!stopButtonPressed && currentFloor == -1) {
                currentState = INITIALIZING;
            }
            break;
    }
}

void elevator_init() {
    currentState = INITIALIZING;
    currentFloor = -1;
    stopButtonPressed = 0;
    obstructionDetected = 0;
    destinationFloor = -1;
    initOrderMatrix(&orderMatrix);
}

void elevator_request_floor(int floor) {
    destinationFloor = floor;
}

void elevator_clear_stop() {
    stopButtonPressed = 0;
    currentState = INITIALIZING;
}

void elevator_set_obstruction(int status) {
    obstructionDetected = status;
}

ElevatorState elevator_get_state() {
    return currentState;
}

int elevator_get_current_floor() {
    return currentFloor;
}

void sleepScan(int milliseconds) {
    for (int i = 0; i < milliseconds; i++) {
        orderScanner();
        usleep(1000);
    }
}

void clearScreen() {
    printf("\033[H\033[J");
    usleep(1000);
}