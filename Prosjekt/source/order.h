/**
 * @file order.h
 * @brief Håndtering av bestillinger i heissystemet.
 */

 #ifndef ORDER_H
 #define ORDER_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include "driver/elevio.h"
 
 #define N_FLOORS 4  ///< Antall etasjer i bygget.
 #define N_BUTTONS 3 ///< Antall knappetyper i heisen.
 
 /**
  * @brief Global matrise som lagrer bestillinger.
  * 
  * orders[floor][button] = 1 indikerer at en bestilling er aktiv.
  */
 extern int orders[N_FLOORS][N_BUTTONS];
 
 /**
  * @brief Legger til en bestilling i bestillingsmatrisen.
  * @param floor Etasjen bestillingen ble gjort i.
  * @param button Typen knapp som ble trykket.
  */
 void queue_add_order(int floor, ButtonType button);
 
 /**
  * @brief Fjerner alle bestillinger for en spesifikk etasje.
  * @param floor Etasjen som skal tømmes for bestillinger.
  */
 void queue_clear_floor_orders(int floor);
 
 /**
  * @brief Sjekker om det finnes noen aktive bestillinger.
  * @return 1 hvis det finnes bestillinger, ellers 0.
  */
 int queue_has_orders();
 
 /**
  * @brief Henter neste bestilling basert på heisens nåværende etasje og retning.
  * @param current_floor Nåværende etasje.
  * @param direction Retning heisen beveger seg i.
  * @return Etasje for neste bestilling, eller -1 hvis ingen bestilling finnes.
  */
 int queue_get_next_order(int current_floor, int direction);
 
 /**
  * @brief Skanner etter nye bestillinger og legger dem til i køen.
  */
 void orderScanner();
 
 /**
  * @brief Sjekker om det finnes bestillinger i en gitt retning.
  * @param direction Retningen som skal sjekkes.
  * @param current_floor Nåværende etasje.
  * @return 1 hvis det finnes bestillinger i retningen, ellers 0.
  */
 int queue_has_orders_in_direction(int direction, int current_floor);
 
 /**
  * @brief Initialiserer bestillingssystemet ved å fjerne alle tidligere bestillinger.
  */
 void initOrders();
 
 #endif // ORDER_H