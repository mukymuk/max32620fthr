#ifndef __UART_H__
#define __UART_H__

#include "clkman.h"
#include "cirbuf.h"

typedef struct
{
    cirbuf_t    cirbuf;
    uint32_t    uartdrv_ndx;
    uint32_t    pmu_ndx;
}
uartdrv_t;

typedef struct
{
    uint32_t    uartdrv_ndx;
    uint32_t    pmu_ndx;
    void *      buffer;
    uint32_t    size;
    uint32_t    ctrl;   // UARTn_CTRL register
    uint32_t    baud;   // UARTn_BAUD register
    uint32_t    ioman;  // IOMAN_UARTn_REQ register
}
uartdrv_config_t;
void uartdrv_init( uartdrv_t * p_uart, const uartdrv_config_t * p_uartdrv_config );
void uartdrv_isr( uartdrv_t * p_uart );
void uartdrv_common_clock( clkman_scale_t scale );

#define UARTDRV_CONFIG_CTRL_DEFAULT    MXC_S_UART_CTRL_DATA_SIZE_8_BITS | MXC_S_UART_CTRL_PARITY_DISABLE
#define UARTDRV_CONFIG_CTRL_HANDSHAKE  MXC_F_UART_CTRL_RTS_EN | MXC_F_UART_CTRL_CTS_EN

// common baud rate configurations for each uart clock scale factor which give 0.16% or better error.
// the same scale factor is used for all uarts.

// scale is set by uartdrv_common_clock()

#define UARTDRV_SCALE1_BAUD_460800 13
#define UARTDRV_SCALE1_BAUD_230400 26
#define UARTDRV_SCALE1_BAUD_115200 39
#define UARTDRV_SCALE1_BAUD_57600  78
#define UARTDRV_SCALE1_BAUD_38400  156

#define UARTDRV_SCALE2_BAUD_230400 13
#define UARTDRV_SCALE2_BAUD_115200 26
#define UARTDRV_SCALE2_BAUD_57600  39
#define UARTDRV_SCALE2_BAUD_38400  78
#define UARTDRV_SCALE2_BAUD_19200  156

#define UARTDRV_SCALE4_BAUD_115200 13
#define UARTDRV_SCALE4_BAUD_57600  26
#define UARTDRV_SCALE4_BAUD_38400  39
#define UARTDRV_SCALE4_BAUD_19200  78
#define UARTDRV_SCALE4_BAUD_9600   156

#define UARTDRV_SCALE8_BAUD_57600  13
#define UARTDRV_SCALE8_BAUD_19200  39
#define UARTDRV_SCALE8_BAUD_9600   78
#define UARTDRV_SCALE8_BAUD_4800   156

#define UARTDRV_SCALE16_BAUD_9600   39
#define UARTDRV_SCALE16_BAUD_4800   78
#define UARTDRV_SCALE16_BAUD_2400   156

#define UARTDRV_SCALE32_BAUD_4800   39
#define UARTDRV_SCALE32_BAUD_2400   78
#define UARTDRV_SCALE32_BAUD_1200   156

#define UARTDRV_SCALE64_BAUD_2400   39
#define UARTDRV_SCALE64_BAUD_1200   78

#define UARTDRV_SCALE128_BAUD_1200  39
#define UARTDRV_SCALE128_BAUD_300   156


#endif


