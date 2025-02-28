#pragma once

#define NUM_BUTTONS 3   // 0: Hall Up, 1: Hall Down, 2: Cabin
#define NUM_FLOORS 4
#define HALL_UP 0
#define HALL_DOWN 1
#define CABIN 2

typedef struct{
    int orders[NUM_BUTTONS][NUM_FLOORS];
} OrderMatrix;



void initOrderMatrix(OrderMatrix* M);
void setOrderStatus(OrderMatrix* M, int button, int floor, int value);
int getOrderStatus(OrderMatrix* M, int button, int floor);
void clearOrderMatrix(OrderMatrix* M, int floor);
int getNextDestination(OrderMatrix* M, int currentFloor, int previousFloor);