#include "elevator.h"
#include "fsm.h"
#include "door.h"
#include "elevio.h"
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define N_FLOORS 4

static bool cabin_orders[N_FLOORS] = {false};
static bool hall_orders[N_FLOORS][2] = {false};

void order_add_cabin(int floor) {
    if (floor >= 0 && floor < N_FLOORS) {
        cabin_orders[floor] = true;
    }
}

void order_add_hall(int floor, MotorDirection direction) {
    if (floor >= 0 && floor < N_FLOORS && (direction == 0 || direction == 1)) {
        hall_orders[floor][direction] = true;
    }
}

void order_remove(int floor) {
    if (floor >= 0 && floor < N_FLOORS) {
        cabin_orders[floor] = false;
        hall_orders[floor][0] = false;
        hall_orders[floor][1] = false;
    }
}

bool order_exists() {
    for (int f = 0; f < N_FLOORS; f++) {
        if (cabin_orders[f] || hall_orders[f][0] || hall_orders[f][1]) {
            return true;
        }
    }
    return false;
}

int order_next(int currentFloor, MotorDirection direction) {
    if (direction == 1) {
        for (int f = currentFloor + 1; f < N_FLOORS; f++) {
            if (cabin_orders[f] || hall_orders[f][1]) {
                return f;
            }
        }
    } else if (direction == 0) {
        for (int f = currentFloor - 1; f >= 0; f--) {
            if (cabin_orders[f] || hall_orders[f][0]) {
                return f;
            }
        }
    }
    
    for (int f = 0; f < N_FLOORS; f++) {
        if (cabin_orders[f] || hall_orders[f][0] || hall_orders[f][1]) {
            return f;
        }
    }
    return -1;
}

void order_handle() {
    if (!order_exists()) {
        fsm_set_state(AT_FLOOR);
        return;
    }
    
    int next = order_next(currentFloor, direction);
    if (next == -1) {
        return;
    }
    
    if (next > currentFloor) {
        fsmSetState(MOVING_UP);
    } else if (next < currentFloor) {
        fsmSetState(MOVING_DOWN);
    } else {
        fsmSetState(AT_FLOOR);
        order_remove(currentFloor);
    }
}

int checkOrderStatus() {
    for (int f = 0; f < N_FLOORS; f++) {
            for (int b = 0; b < N_BUTTONS; b++) {
                if (elevio_callButton(f, b)) {
                    return f;
                }
            }
        }
        return -1;
    }
