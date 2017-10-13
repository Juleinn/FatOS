#include "timer.h"

#define DIVISOR 838 // gets the clock to tick at a multiple of 10 clockrate (in Hz) -> tick 1000 times per second approx.  

void timer_init()
{
	short divisor = DIVISOR;
	outb(0x36, 0x43);
	outb(divisor & 0xFF, 0x40);	// low byte
	outb(divisor >> 8, 0x40);	// high byte
}

int clk_ticks = 0;

void clock0_irq_handler()
{
	// called upon clock tick
	clk_ticks++;	
}

void timer_wait(int ms)
{
	// tick 1000 times per second approx.
	clk_ticks = 0;
	while(clk_ticks < ms);

	return; 
}