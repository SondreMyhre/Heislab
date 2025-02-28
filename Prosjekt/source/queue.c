#include "queue.h"
#include "driver/elevio.h" 

void initOrderMatrix(OrderMatrix *M) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        for (int j = 0; j < NUM_FLOORS; j++) {
            M->orders[i][j] = 0;
        }
    }
}

void setOrderStatus(OrderMatrix *M, int button, int floor, int value) {
    if(floor < 0 || floor >= NUM_FLOORS || button < 0 || button >= NUM_BUTTONS){
        return;
    }
    elevio_buttonLamp(floor, button, value);
    M->orders[button][floor] = value;
}

int getOrderStatus(OrderMatrix *M, int button, int floor) {
    if(floor < 0 || floor >= NUM_FLOORS || button < 0 || button >= NUM_BUTTONS){
        return 0;
    }
    return M->orders[button][floor];
}

void clearOrderMatrix(OrderMatrix *M, int floor) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        setOrderStatus(M, i, floor, 0);
    }
}

int getNextDestination(OrderMatrix *M, int currentFloor, int direction) {
    int nextDestination = -1;
    if(direction == DIRN_STOP) { //NO DIRECTION
        for (int i = currentFloor; i < NUM_FLOORS; i++) {
            if (getOrderStatus(M, CABIN, i)) {
                if(nextDestination == -1 || i < nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_UP, i)) {
                if(nextDestination == -1 || i < nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_DOWN, i)) {
                if(nextDestination == -1 || i < nextDestination){
                    nextDestination = i;
                }
            }
        } if(nextDestination != -1 && nextDestination != currentFloor){
            return nextDestination;
        }
        for (int i = currentFloor; i >= 0; i--) {
            if (getOrderStatus(M, CABIN, i)) {
                if(nextDestination == -1 || i > nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_DOWN, i)) {
                if(nextDestination == -1 || i > nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_UP, i)) {
                if(nextDestination == -1 || i > nextDestination){
                    nextDestination = i;
                }
            }
        } if(nextDestination != -1 && nextDestination != currentFloor){
            return nextDestination;
        }
        return nextDestination;
        
    } else if (direction == DIRN_UP) { //DIRECTION UP
        for (int i = currentFloor; i < NUM_FLOORS; i++) {
            if (getOrderStatus(M, CABIN, i)) {
                if(nextDestination == -1 || i < nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_UP, i)) {
                if(nextDestination == -1 || i < nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_DOWN, i)) {
                if(nextDestination == -1 || i < nextDestination){
                    nextDestination = i;
                }
            }
        } if(nextDestination != -1 && nextDestination != currentFloor){
            return nextDestination;
        }

        for (int i = currentFloor; i >= 0; i--) {
            if (getOrderStatus(M, CABIN, i)) {
                if(nextDestination == -1 || i > nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_DOWN, i)) {
                if(nextDestination == -1 || i > nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_UP, i)) {
                if(nextDestination == -1 || i > nextDestination){
                    nextDestination = i;
                }
            }
        } if(nextDestination != -1 && nextDestination != currentFloor){
            return nextDestination;
        }
        return nextDestination;

    } else if (direction == DIRN_DOWN) { //DIRECTION DOWN
        for (int i = currentFloor; i >= 0; i--) {
            if (getOrderStatus(M, CABIN, i)) {
                if(nextDestination == -1 || i > nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_DOWN, i)) {
                if(nextDestination == -1 || i > nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_UP, i)) {
                if(nextDestination == -1 || i > nextDestination){
                    nextDestination = i;
                }
            }
        } if(nextDestination != -1 && nextDestination != currentFloor){
            return nextDestination;
        }

        for (int i = currentFloor; i < NUM_FLOORS; i++) {
            if (getOrderStatus(M, CABIN, i)) {
                if(nextDestination == -1 || i < nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_UP, i)) {
                if(nextDestination == -1 || i < nextDestination){
                    nextDestination = i;
                }
            }
            if (getOrderStatus(M, HALL_DOWN, i)) {
                if(nextDestination == -1 || i < nextDestination){
                    nextDestination = i;
                }
            }
        } if(nextDestination != -1 && nextDestination != currentFloor){
            return nextDestination;
        }
        
    }
    if(nextDestination != currentFloor){
        return nextDestination;
    } else {
        return -1;
    }
}