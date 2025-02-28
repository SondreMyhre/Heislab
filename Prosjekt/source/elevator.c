#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include "driver/con_load.h"
#include "driver/elevio.h"
#include "fsm.h"
#include "queue.h"



int main(){
    printf("Heis: Starter heisen\n");
    elevio_init();
    printf("Heis: Initialiserer heisen\n");
    elevator_init();
    printf("Heis: elevator_init ferdig\n");
    while(1){
        elevator_fsm();
    }

    return 0;
}