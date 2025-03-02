#include <stdio.h>
#include <stdlib.h>
#include "elevio.h"

#define N_FLOORS 4
#define N_BUTTONS 3

int orders[N_FLOORS][N_BUTTONS] = {0};

void queue_add_order(int floor, ButtonType button) {
    if (floor >= 0 && floor < N_FLOORS && button >= 0 && button < N_BUTTONS) {
        orders[floor][button] = 1;
        elevio_buttonLamp(floor, button, 1);
    }
}

void queue_clear_floor_orders(int floor) {
    for (int b = 0; b < N_BUTTONS; b++) {
        orders[floor][b] = 0;
        elevio_buttonLamp(floor, b, 0);
    }
}

int queue_has_orders() {
    for (int f = 0; f < N_FLOORS; f++) {
        for (int b = 0; b < N_BUTTONS; b++) {
            if (orders[f][b]) {
                return 1;
            }
        }
    }
    return 0;
}

int queue_get_next_order(int current_floor, int direction) {
    if (direction == DIRN_UP) {
        for (int f = current_floor + 1; f < N_FLOORS; f++) {
            for (int b = 0; b < N_BUTTONS; b++) {
                if (orders[f][b]) {
                    return f;
                }
            }
        }
    } else if (direction == DIRN_DOWN) {
        for (int f = current_floor - 1; f >= 0; f--) {
            for (int b = 0; b < N_BUTTONS; b++) {
                if (orders[f][b]) {
                    return f;
                }
            }
        }
    }
    return -1;
}

void orderScanner() {
    for (int f = 0; f < N_FLOORS; f++) {
        for (int b = 0; b < N_BUTTONS; b++) {
            if (elevio_callButton(f, b)) {
                queue_add_order(f, b);
            }
        }
    }
}


