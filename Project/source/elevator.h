#include <stdbool.h>
#include "driver/elevio.h"

#define N_FLOORS 4

extern int direction;
extern int currentFloor;

void order_add_cabin(int floor);
void order_add_hall(int floor, MotorDirection direction);
void order_remove(int floor);
bool order_exists();
int order_next(int currentFloor, MotorDirection direction);
void order_handle();
int exec_scan_orders();
