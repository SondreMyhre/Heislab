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
    elevator_init();
    while(1){
        elevator_fsm();
    }

    return 0;
}