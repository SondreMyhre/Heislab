/** @file elevator.c
 *  @brief Hovedprogram for heissystemet.
 *
 *  Initialiserer heisen og starter hovedsløyfen for FSM (Finite State Machine).
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <signal.h>
 #include <time.h>
 #include "driver/elevio.h"
 #include "driver/con_load.h"
 #include "fsm.h"
 #include "order.h"
 
 /** @brief Hovedfunksjon som starter heisen og kjører FSM.
  *  @return 0 ved normal avslutning.
  */
 int main() {
     printf("Heis: Starter heisen\n");
     elevator_init();
     while (1) {
         elevator_fsm();
     }
     return 0;
 }