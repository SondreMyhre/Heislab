/** @file fsm.c
 *  @brief Implementasjon av finite state machine for heisen.
 */

#include <stdio.h>
#include <unistd.h>
#include "driver/elevio.h"
#include "fsm.h"
#include "order.h"

/** @brief Nåværende tilstand for heisen. */
ElevatorState currentState = INITIALIZING;
/** @brief Nåværende etasje. */
int currentFloor = -1;
/** @brief Indikerer om stoppknappen er trykket. */
int stopButtonPressed = 0;
/** @brief Indikerer om en obstruksjon er oppdaget. */
int obstructionDetected = 0;
/** @brief Måletasje for heisen. */
int destinationFloor = -1;
/** @brief Nåværende retning. */
int direction = DIRN_STOP;

/** @brief Håndterer logikken for heisens finite state machine. */
void elevator_fsm() {
    orderScanner();
    
    switch (currentState) {
        case INITIALIZING:
            printf("Heis: Søker etter etasje\n");
            while(1) {
                elevio_motorDirection(DIRN_DOWN);
                currentFloor = elevio_floorSensor();    
                if (currentFloor != -1) {
                    elevio_motorDirection(DIRN_STOP);
                    elevio_floorIndicator(currentFloor);
                    printf("Heis: Står i etasje %d\n", currentFloor);
                    direction = DIRN_STOP;
                    currentState = IDLE;
                    break;
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

        if (elevio_stopButton()) {
            currentState = STOP;
            break;
        }
        elevio_doorOpenLamp(1);
        queue_clear_floor_orders(currentFloor);

        for (int i = 0; i < 30; i++) { 
            orderScanner();
            usleep(100000);
            if (elevio_stopButton()) {
                currentState = STOP;
                break;
            }
        }
        if (currentState == STOP) break;
        while (elevio_obstruction()) {
            orderScanner();
            usleep(100000);
            if (elevio_stopButton()) { 
                currentState = STOP;
                break;
            }
        }
        if (currentState == STOP) break;
        sleepScan(1000);
        elevio_doorOpenLamp(0);
        

    
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

            if (elevio_stopButton()) {
                currentState = STOP;
                break;
            }
            
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

            if (elevio_stopButton()) {
                currentState = STOP;
                break;
            }
            
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

        case STOP:
        elevio_motorDirection(DIRN_STOP); 
        elevio_stopLamp(1); 
        for (int f = 0; f < N_FLOORS; f++) {
            queue_clear_floor_orders(f);
        }
        if (elevio_floorSensor() != -1) {
            elevio_doorOpenLamp(1);
        }
        while (elevio_stopButton()) {
            usleep(100000); 
        }
        elevio_stopLamp(0); 
        while (elevio_obstruction()) {
            usleep(100000);
        }
        sleep(3);
        elevio_doorOpenLamp(0); 

        if (queue_has_orders()) {
            destinationFloor = queue_get_next_order(currentFloor, direction);
            currentState = (destinationFloor > currentFloor) ? MOVING_UP : MOVING_DOWN;
        } else {
            currentState = INITIALIZING;
        }
        break;
    }
}

/**
 * @brief Initialiserer heissystemet.
 * 
 * Setter opp maskinvare, tømmer ordrelisten og setter initialtilstander.
 */
 void elevator_init() {
    elevio_init();
    initOrders();
    elevio_doorOpenLamp(0);
    currentState = INITIALIZING;
    currentFloor = -1;
    stopButtonPressed = 0;
    obstructionDetected = 0;
    destinationFloor = -1;
}

/**
 * @brief Legger til en heisbestilling for en etasje.
 * 
 * @param floor Etasjen det legges inn en bestilling for.
 */
void elevator_request_floor(int floor) {
    queue_add_order(floor, BUTTON_CAB);
}

/**
 * @brief Nullstiller stoppknappen og setter heisen i initialiseringsmodus.
 */
void elevator_clear_stop() {
    stopButtonPressed = 0;
    currentState = INITIALIZING;
}

/**
 * @brief Setter obstruksjonsstatusen for heisen.
 * 
 * @param status 1 hvis obstruksjon er detektert, ellers 0.
 */
void elevator_set_obstruction(int status) {
    obstructionDetected = status;
}

/**
 * @brief Henter heisens nåværende tilstand.
 * 
 * @return Gjeldende tilstand av typen ElevatorState.
 */
ElevatorState elevator_get_state() {
    return currentState;
}

/**
 * @brief Henter heisens nåværende etasje.
 * 
 * @return Nåværende etasje (0-basert), eller -1 hvis ukjent.
 */
int elevator_get_current_floor() {
    return currentFloor;
}

/**
 * @brief Sover i angitt tid samtidig som ordre overvåkes.
 * 
 * @param milliseconds Antall millisekunder å sove.
 */
void sleepScan(int milliseconds) {
    for (int i = 0; i < milliseconds; i++) {
        orderScanner();
        usleep(1000);
    }
}

/**
 * @brief Rydder terminalskjermen.
 */
void clearScreen() {
    printf("\033[H\033[J");
    usleep(1000);
}