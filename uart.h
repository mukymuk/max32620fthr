#ifndef __UART_H__
#define __UART_H__

#include "clkman.h"
#include "cirbuf.h"

typedef struct
{
    cirbuf_t    cirbuf;
    uint32_t    uart_ndx;
    uint32_t    pmu_ndx;
}
uart_t;

typedef struct
{
    uint32_t    uart_ndx;
    uint32_t    pmu_ndx;
    void *      buffer;
    uint32_t    size;
    uint32_t    ctrl;   // UARTn_CTRL register
    uint32_t    baud;   // UARTn_BAUD register
    uint32_t    ioman;  // IOMAN_UARTn_REQ register
}
uart_config_t;
void uart_init( uart_t * p_uart, const uart_config_t * p_uart_config );
void uart_isr( uart_t * p_uart );
void uart_common_clock( clkman_scale_t scale );

#define UART_CONFIG_CTRL_DEFAULT    MXC_S_UART_CTRL_DATA_SIZE_8_BITS | MXC_S_UART_CTRL_PARITY_DISABLE
#define UART_CONFIG_CTRL_HANDSHAKE  MXC_F_UART_CTRL_RTS_EN | MXC_F_UART_CTRL_CTS_EN

#endif
