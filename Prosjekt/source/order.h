#ifndef QUEUE_HANDLER_H
#define QUEUE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include "elevio.h"

#define N_FLOORS 4
#define N_BUTTONS 3

extern int orders[N_FLOORS][N_BUTTONS];

void queue_add_order(int floor, ButtonType button);
void queue_clear_floor_orders(int floor);
int queue_has_orders();
int queue_get_next_order(int current_floor, int direction);
void orderScanner();
int queue_has_orders_in_direction(int direction, int current_floor);

#endif 
