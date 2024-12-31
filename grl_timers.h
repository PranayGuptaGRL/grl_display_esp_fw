
#ifndef GRL_TIMERS_H
#define GRL_TIMERS_H

// Code Here
#include "Includes.h"

extern TimerHandle_t timer_1;

void Timer0_ISR(void *z );
size_t grl_uart_write(uint8_t *atx_buf, size_t abuf_size);
extern uint8_t polling_buf[255];

#endif // GRL_TIMERS_H


