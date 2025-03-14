/**
 * @file order.c
 * @brief Håndtering av heisens ordrekø.
 *
 * Denne filen inneholder funksjoner for å håndtere bestillinger (ordre) i heissystemet,
 * inkludert lagring, sletting og henting av ordre basert på etasje og retning.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include "driver/elevio.h"
 #include "order.h"
 
 #define N_FLOORS 4   ///< Antall etasjer i bygningen
 #define N_BUTTONS 3  ///< Antall knapper per etasje
 
 /**
  * @brief Array som lagrer ordrestatus for hver etasje og knapp.
  */
 int orders[N_FLOORS][N_BUTTONS] = {0};
 
 /**
  * @brief Initialiserer ordresystemet ved å fjerne alle ordrer.
  */
 void initOrders() {
     for (int f = 0; f < N_FLOORS; f++) {
         queue_clear_floor_orders(f);
     }   
 }
 
 /**
  * @brief Legger til en ordre i køen.
  *
  * @param floor Etasjen hvor bestillingen gjøres.
  * @param button Knappetypen som ble trykket (opp, ned eller kabin).
  */
 void queue_add_order(int floor, ButtonType button) {
     if (floor >= 0 && floor < N_FLOORS && button >= 0 && button < N_BUTTONS) {
         orders[floor][button] = 1;
         elevio_buttonLamp(floor, button, 1);
     }
 }
 
 /**
  * @brief Fjerner alle ordrer for en spesifikk etasje.
  *
  * @param floor Etasjen som skal tømmes for ordre.
  */
 void queue_clear_floor_orders(int floor) {
     for (int b = 0; b < N_BUTTONS; b++) {
         orders[floor][b] = 0;
         elevio_buttonLamp(floor, b, 0);
     }
 }
 
 /**
  * @brief Sjekker om det finnes noen aktive ordrer.
  *
  * @return 1 hvis det er ordrer, ellers 0.
  */
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
 
 /**
  * @brief Sjekker om det er ordrer i en spesifikk retning.
  *
  * @param direction Retningen (opp eller ned) som skal sjekkes.
  * @param current_floor Nåværende etasje.
  * @return 1 hvis det finnes ordrer i angitt retning, ellers 0.
  */
 int queue_has_orders_in_direction(int direction, int current_floor) {
     if (direction == DIRN_UP) {
         for (int f = current_floor + 1; f < N_FLOORS; f++) {
             if (orders[f][BUTTON_HALL_UP] || orders[f][BUTTON_CAB] || orders[3][BUTTON_HALL_DOWN]) {
                 return 1; 
             }
         }
     } else if (direction == DIRN_DOWN) {
         for (int f = current_floor - 1; f >= 0; f--) {
             if (orders[f][BUTTON_HALL_DOWN] || orders[f][BUTTON_CAB] || orders[0][BUTTON_HALL_UP]) {
                 return 1; 
             }
         }
     }
     return 0;
 }
 
 /**
  * @brief Henter neste bestilte etasje basert på nåværende etasje og retning.
  *
  * @param current_floor Nåværende etasje.
  * @param direction Retningen heisen beveger seg i.
  * @return Etasjenummeret til neste ordre, eller -1 hvis ingen ordre finnes.
  */
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
     } else if (direction == DIRN_STOP) {
         for (int f = 0; f < N_FLOORS; f++) {
             for (int b = 0; b < N_BUTTONS; b++) {
                 if (orders[f][b]) {
                     return f;
                 }
             }
         }   
     }
     return -1;
 }
 
 /**
  * @brief Skanner heispanelene for nye bestillinger og legger dem til i køen.
  */
 void orderScanner() {
     for (int f = 0; f < N_FLOORS; f++) {
         for (int b = 0; b < N_BUTTONS; b++) {
             if (elevio_callButton(f, b)) {
                 queue_add_order(f, b);
             }
         }
     }
 }
 