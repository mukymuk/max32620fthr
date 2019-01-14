#include "global.h"
#include "uart.h"
#include "csl\PeriphDriver\Include\uart.h"

static void write_update_cb( cirbuf_t *p_cirbuf )
{
    uart_t *p_uart = (uart_t*)p_cirbuf;
    NVIC_EnableIRQ( MXC_UART_GET_IRQ(p_uart->uart_ndx) );
}

static void lock_cb( cirbuf_t *p_cirbuf )
{
    uart_t *p_uart = (uart_t*)p_cirbuf;
    NVIC_DisableIRQ( MXC_UART_GET_IRQ(p_uart->uart_ndx) );
}

void uart_init( uart_t * p_uart, void *pv_buffer, uint32_t size, uint32_t uart_ndx, uint32_t pmu_ndx )
{
    p_uart->pmu_ndx = pmu_ndx;
    p_uart->uart_ndx = uart_ndx;
    cirbuf_init( &p_uart->cirbuf, pv_buffer, size, write_update_cb, NULL, lock_cb );
}

void uart_isr( uart_t * p_uart )
{

}
