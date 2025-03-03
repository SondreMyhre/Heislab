#include <stdio.h>
#include <unistd.h>
#include "driver/elevio.h"
#include "fsm.h"
#include "order.h"

ElevatorState currentState = INITIALIZING;
int currentFloor = -1; 
int stopButtonPressed = 0;
int obstructionDetected = 0;
int destinationFloor = -1;
int direction = DIRN_STOP;

void elevator_fsm() {
    orderScanner();
    
    switch (currentState) {
        case INITIALIZING:
            printf("Heis: Søker etter etasje\n");
            while(currentFloor == -1) {
                elevio_motorDirection(DIRN_DOWN);
                currentFloor = elevio_floorSensor();    
                if (currentFloor != -1) {
                    elevio_motorDirection(DIRN_STOP);
                    elevio_floorIndicator(currentFloor);
                    printf("Heis: Står i etasje %d\n", currentFloor);
                    direction = DIRN_STOP;
                    currentState = IDLE;
                }
            }
            break;

        case IDLE:
            direction = DIRN_STOP;
            elevio_motorDirection(direction);
            orderScanner();
            elevio_floorIndicator(currentFloor);
            if (queue_has_orders()) {
                destinationFloor = queue_get_next_order(currentFloor, direction);
                if (destinationFloor == -1) {
                    elevio_motorDirection(DIRN_STOP);
                    currentState = IDLE;
                    break;
                } else if (destinationFloor > currentFloor) {
                    currentState = MOVING_UP;
                } else if (destinationFloor < currentFloor) {
                    currentState = MOVING_DOWN;
                } else {
                    currentState = AT_DESTINATION;
                }
            }
            break;

        case AT_DESTINATION:
        orderScanner();
        elevio_floorIndicator(currentFloor);
        elevio_motorDirection(DIRN_STOP); 
        elevio_doorOpenLamp(1);
        sleep(3);
        elevio_doorOpenLamp(0);
        
        queue_clear_floor_orders(currentFloor);
    
        if (queue_has_orders_in_direction(direction, currentFloor)) {
            destinationFloor = queue_get_next_order(currentFloor, direction);
            currentState = (direction == DIRN_UP) ? MOVING_UP : MOVING_DOWN;
        } 
        else if (queue_has_orders()) { 
            direction = (direction == DIRN_UP) ? DIRN_DOWN : DIRN_UP;
            destinationFloor = queue_get_next_order(currentFloor, direction);
            currentState = (direction == DIRN_UP) ? MOVING_UP : MOVING_DOWN;
        } 
        else {
            currentState = IDLE;
        }
        break;
        
        case MOVING_UP:
        direction = DIRN_UP;
        elevio_motorDirection(DIRN_UP);
        
        while (1) {
            orderScanner();
            int floor = elevio_floorSensor();
            
            if (floor != -1) {
                currentFloor = floor;
                elevio_floorIndicator(currentFloor);
    
                if (orders[currentFloor][BUTTON_HALL_UP] || orders[currentFloor][BUTTON_CAB] || 
                    (currentFloor == destinationFloor)) {
                    currentState = AT_DESTINATION;
                    break;
                }
            }
            usleep(10000);
        }
        break;

        case MOVING_DOWN:
        direction = DIRN_DOWN;
        elevio_motorDirection(DIRN_DOWN);
        
        while (1) {
            orderScanner();
            int floor = elevio_floorSensor();
            
            if (floor != -1) {
                currentFloor = floor;
                elevio_floorIndicator(currentFloor);
    
                if (orders[currentFloor][BUTTON_HALL_DOWN] || orders[currentFloor][BUTTON_CAB] || 
                    (currentFloor == destinationFloor)) {
                    currentState = AT_DESTINATION;
                    break;
                }
            }
            usleep(10000);
        }
        break;
/*       case AT_DESTINATION:
        orderScanner();
        elevio_floorIndicator(currentFloor);
        elevio_doorOpenLamp(1);
        sleep(3);
        elevio_doorOpenLamp(0);
        queue_clear_floor_orders(currentFloor);
        if (queue_has_orders()) {
            destinationFloor = queue_get_next_order(currentFloor, direction);
            if (destinationFloor == -1) {
                elevio_motorDirection(DIRN_STOP);
                currentState = IDLE;
                break;
            } else if (destinationFloor > currentFloor) {
                currentState = MOVING_UP;
            } else if (destinationFloor < currentFloor) {
                currentState = MOVING_DOWN;
            }
        } else {
            currentState = IDLE;
        }
        break;

        case MOVING_UP:
        direction = DIRN_UP;
        elevio_motorDirection(DIRN_UP);

        while (1) {
        orderScanner(); // Oppdater bestillinger underveis
        int floor = elevio_floorSensor();

        if (floor != -1) {
            currentFloor = floor;
            elevio_floorIndicator(currentFloor);

            // Sjekk om vi skal stoppe her
            if (orders[currentFloor][BUTTON_HALL_UP] || orders[currentFloor][BUTTON_CAB] || 
                (currentFloor == destinationFloor)) {

                elevio_motorDirection(DIRN_STOP);
                queue_clear_floor_orders(currentFloor); // Fjern bestillingen fra køen
                usleep(500000); // Kort pause for å simulere stopp
                elevio_doorOpenLamp(1);
                sleep(3);
                elevio_doorOpenLamp(0);

                // Sjekk om det er flere bestillinger i samme retning
                if (!queue_has_orders_in_direction(DIRN_UP, currentFloor)) {
                    if (queue_has_orders_in_direction(DIRN_DOWN, currentFloor)) {
                        currentState = MOVING_DOWN; // Snu om det er bestillinger nedover
                    } else {
                        currentState = IDLE; // Gå til idle hvis ingen flere bestillinger
                    }
                    break;
                }

                destinationFloor = queue_get_next_order(currentFloor, direction);
                elevio_motorDirection(DIRN_UP);
            }
        }
        usleep(10000);
        }
        break; 

        case MOVING_DOWN:
        direction = DIRN_DOWN;
        elevio_motorDirection(DIRN_DOWN);
        
        while (1) {
        orderScanner(); // Oppdater bestillinger underveis
        int floor = elevio_floorSensor();
            
        if (floor != -1) {
            currentFloor = floor;
            elevio_floorIndicator(currentFloor);

            // Sjekk om vi skal stoppe her
            if (orders[currentFloor][BUTTON_HALL_DOWN] || orders[currentFloor][BUTTON_CAB] || 
                (currentFloor == destinationFloor)) {

                elevio_motorDirection(DIRN_STOP);
                queue_clear_floor_orders(currentFloor); // Fjern bestillingen fra køen
                usleep(500000); // Kort pause for å simulere stopp
                elevio_doorOpenLamp(1);
                sleep(3);
                elevio_doorOpenLamp(0);

                // Sjekk om det er flere bestillinger i samme retning
                if (!queue_has_orders_in_direction(DIRN_DOWN, currentFloor)) {
                    if (queue_has_orders_in_direction(DIRN_UP, currentFloor)) {
                        currentState = MOVING_UP; // Snu om det er bestillinger oppover
                    } else {
                        currentState = IDLE; // Gå til idle hvis ingen flere bestillinger
                    }
                    break;
                }

                destinationFloor = queue_get_next_order(currentFloor, direction);
                elevio_motorDirection(DIRN_DOWN);
            }
        }
        usleep(10000);
        }
        break;
*/
        case EMERGENCY_STOP_FLOOR:
            elevio_motorDirection(DIRN_STOP);
            if (!stopButtonPressed && currentFloor != -1) {
                currentState = INITIALIZING;
            }
            break;

        case EMERGENCY_STOP_SHAFT:
            elevio_motorDirection(DIRN_STOP);
            if (!stopButtonPressed && currentFloor == -1) {
                currentState = INITIALIZING;
            }
            break;
    }
}

void elevator_init() {
    elevio_init();
    currentState = INITIALIZING;
    currentFloor = -1;
    stopButtonPressed = 0;
    obstructionDetected = 0;
    destinationFloor = -1;
}

void elevator_request_floor(int floor) {
    queue_add_order(floor, BUTTON_CAB);
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
