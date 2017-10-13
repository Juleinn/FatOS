/* File timer.h 
 * Anton CLAES 
 * handles the timer interrupts */

#ifndef TIMER_H_
#define TIMER_H_

#include "print.h"

void timer_init();

void clock0_irq_handler();

void timer_wait(int ms);

#endif