#ifndef __UART_H__
#define __UART_H__

#include "cirbuf.h"

typedef struct
{
    cirbuf_t    cirbuf;
    uint32_t    uart_ndx;
    uint32_t    pmu_ndx;
}
uart_t;

void uart_init( uart_t * p_uart, void *pv_buffer, uint32_t size, uint32_t uart_ndx, uint32_t pmu_ndx );
void uart_isr( uart_t * p_uart );

#endif
