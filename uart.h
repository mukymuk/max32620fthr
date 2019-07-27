#ifndef __UART_H__
#define __UART_H__

#include "clkman.h"
#include "cbuf.h"
#include "uart_regs.h"
#include "ioman.h"

#include "pmu.h"

typedef struct
{
    mxc_uart_regs_t *   p_mxc_uart_regs;
    mxc_uart_fifo_regs_t * p_fifo;
    cbuf_t *            p_cbuf_read;
    cbuf_t *            p_cbuf_write;
    uint32_t            uart_ndx;
    uint32_t            pmu_ndx;
    IRQn_Type           irq;
    pmu_channel_t *     pmu_channel_write;
    uint32_t            pmu_int_mask;
    pmu_transfer_des_t  pmu_transfer_des;
}
uart_t;

void uart_init( uart_t * p_uart, cbuf_t * p_cbuf_read, cbuf_t * p_cbuf_write,
                uint32_t uart_ndx, uint32_t config, uint32_t baud );

void uart_common_clock( clkman_scale_t scale );



#define UART_CONFIG_CTS_ENABLE           1<<0
#define UART_CONFIG_RTS_ENABLE           1<<1
#define UART_CONFIG_CTS_POLARITY         1<<2
#define UART_CONFIG_RTS_POLARITY         1<<3
#define UART_CONFIG_VDDOIH               1<<4
#define UART_CONFIG_TXRX_SWAP            1<<7
#define UART_CONFIG_CTSRTS_SWAP          1<<8

#define UART_CONFIG_BITS_SHIFT           9
#define UART_CONFIG_BITS_5               0<<UART_CONFIG_BITS_SHIFT
#define UART_CONFIG_BITS_6               1<<UART_CONFIG_BITS_SHIFT
#define UART_CONFIG_BITS_7               2<<UART_CONFIG_BITS_SHIFT
#define UART_CONFIG_BITS_8               3<<UART_CONFIG_BITS_SHIFT

#define UART_CONFIG_STOP_1               0<<11
#define UART_CONFIG_STOP_2               1<<11

#define UART_CONFIG_PARITY_SHIFT         12
#define UART_CONFIG_PARITY_NONE          0<<UART_CONFIG_PARITY_SHIFT
#define UART_CONFIG_PARITY_ODD           1<<UART_CONFIG_PARITY_SHIFT
#define UART_CONFIG_PARITY_EVEN          2<<UART_CONFIG_PARITY_SHIFT

#define UART_CONFIG_8N1                  UART_CONFIG_BITS_8|UART_CONFIG_PARITY_NONE|UART_CONFIG_STOP_1

#define UART_CONFIG_PARITY_MASK          (UART_CONFIG_PARITY_NONE|UART_CONFIG_PARITY_ODD|UART_CONFIG_PARITY_EVEN)
#define UART_CONFIG_BITS_MASK            (UART_CONFIG_BITS_5|UART_CONFIG_BITS_6|UART_CONFIG_BITS_7|UART_CONFIG_BITS_8)

// common baud rate configurations for each uart clock scale factor which give less than 0.8% or better error.
// the same scale factor is used for all uarts.

// scale is set by uart_common_clock()

#define DIV16 (0<<8)
#define DIV8  (1<<8)
#define DIV4  (2<<8)

#define UART_CONFIG_BAUD_SCALE1_921600 (13 | DIV4)
#define UART_CONFIG_BAUD_SCALE1_460800 (13 | DIV8)
#define UART_CONFIG_BAUD_SCALE1_230400 (13 | DIV16)
#define UART_CONFIG_BAUD_SCALE1_115200 (26 | DIV16)
#define UART_CONFIG_BAUD_SCALE1_57600  (52 | DIV16)
#define UART_CONFIG_BAUD_SCALE1_38400  (78 | DIV16)
#define UART_CONFIG_BAUD_SCALE1_19200  (156 | DIV16)

#define UART_CONFIG_BAUD_SCALE2_230400 (35 | DIV4)
#define UART_CONFIG_BAUD_SCALE2_115200 (69 | DIV4)
#define UART_CONFIG_BAUD_SCALE2_57600  (139 | DIV4)
#define UART_CONFIG_BAUD_SCALE2_38400  (52 | DIV16)
#define UART_CONFIG_BAUD_SCALE2_19200  (104 | DIV16)
#define UART_CONFIG_BAUD_SCALE2_9600   (08 | DIV16)

#define UART_CONFIG_BAUD_SCALE4_230400 (13 | DIV4)
#define UART_CONFIG_BAUD_SCALE4_115200 (13 | DIV8)
#define UART_CONFIG_BAUD_SCALE4_57600  (13 | DIV16)
#define UART_CONFIG_BAUD_SCALE4_38400  (39 | DIV8)
#define UART_CONFIG_BAUD_SCALE4_19200  (38 | DIV16)
#define UART_CONFIG_BAUD_SCALE4_4800   (156 | DIV16)

#define UART_CONFIG_BAUD_SCALE8_115200 (13 | DIV4)
#define UART_CONFIG_BAUD_SCALE8_57600  (13 | DIV8)
#define UART_CONFIG_BAUD_SCALE8_38400  (39 | DIV4)
#define UART_CONFIG_BAUD_SCALE8_19200  (39 | DIV8)
#define UART_CONFIG_BAUD_SCALE8_9600   (39 | DIV16)
#define UART_CONFIG_BAUD_SCALE8_4800   (156 | DIV16)
 
#endif


