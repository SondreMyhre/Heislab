#pragma once


typedef enum {
    AT_DESTINATION,
    IDLE,
    MOVING_UP,
    MOVING_DOWN,
    INITIALIZING,
    STOP


} ElevatorState;

void elevator_init();           
void elevator_fsm();             
void elevator_request_floor(int floor);            
void elevator_clear_stop();     
void elevator_set_obstruction(int status); 
void sleepScan(int milliseconds);
void clearScreen();

ElevatorState elevator_get_state();
int elevator_get_current_floor();