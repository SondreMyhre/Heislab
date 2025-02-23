#ifndef ELEVATOR_FSM_H
#define ELEVATOR_FSM_H

#include <stdbool.h>

typedef enum {
    AT_FLOOR,
    MOVING_UP,
    MOVING_DOWN,
    EMERGENCY_STOP_FLOOR,
    INITIALIZING,
    EMERGENCY_STOP_SHAFT


} ElevatorState;

void elevator_init();           
void elevator_fsm();             
void elevator_request_floor(int floor);  
void elevator_stop();            
void elevator_clear_stop();     
void elevator_set_obstruction(bool status); 

ElevatorState elevator_get_state();
int elevator_get_current_floor();

#endif 
