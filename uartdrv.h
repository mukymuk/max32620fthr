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
uartdrv_t;

typedef struct
{
    uint32_t    uart_ndx;
    uint32_t    pmu_ndx;
    void *      buffer;
    uint32_t    size;
    uint32_t    config;
    uint32_t    baud;
}
uartdrv_config_t;
void uartdrv_init( uartdrv_t * p_uart, const uartdrv_config_t * p_uartdrv_config );
void uartdrv_isr( uartdrv_t * p_uart );
void uartdrv_common_clock( clkman_scale_t scale );

#define UARTDRV_CONFIG_IOMAN_TXRX_SWAP      MXC_F_IOMAN_UART0_REQ_IO_MAP    
#define UARTDRV_CONFIG_IOMAN_CTSRTS_SWAP    MXC_F_IOMAN_UART0_REQ_CTS_MAP | MXC_F_IOMAN_UART0_REQ_RTS_MAP

#define UARTDRV_CONFIG_IOMAN_CTS_ENABLE     MXC_F_IOMAN_UART0_REQ_CTS_IO_REQ
#define UARTDRV_CONFIG_IOMAN_RTS_ENABLE     MXC_F_IOMAN_UART0_REQ_RTS_IO_REQ
#define UARTDRV_CONFIG_IOMAN_TXRX_ENABLE    MXC_F_IOMAN_UART0_REQ_IO_MAP

#define UARTDRV_CONFIG_CTRL_ENABLE          MXC_F_UART_CTRL_UART_EN | MXC_F_UART_CTRL_RX_FIFO_EN | MXC_F_UART_CTRL_TX_FIFO_EN
#define UARTDRV_CONFIG_CTRL_8N1             MXC_S_UART_CTRL_DATA_SIZE_8_BITS | MXC_S_UART_CTRL_PARITY_DISABLE
#define UARTDRV_CONFIG_CTRL_CTS_ENABLE      MXC_F_UART_CTRL_CTS_EN 
#define UARTDRV_CONFIG_CTRL_RTS_ENABLE      MXC_F_UART_CTRL_RTS_EN 
#define UARTDRV_CONFIG_CTRL_CTS_POL_NEG     MXC_F_UART_CTRL_CTS_POLARITY
#define UARTDRV_CONFIG_CTRL_RTS_POL_NEG     MXC_F_UART_CTRL_CTS_POLARITY

#define UARTDRV_CONFIG_CTS_ENABLE           1<<0
#define UARTDRV_CONFIG_RTS_ENABLE           1<<1
#define UARTDRV_CONFIG_CTS_POLARITY         1<<2
#define UARTDRV_CONFIG_RTS_POLARITY         1<<3
#define UARTDRV_CONFIG_VDDOIH               1<<4
#define UARTDRV_CONFIG_RXTX_SWAP            1<<7
#define UARTDRV_CONFIG_CTSRTS_SWAP          1<<8
#define UARTDRV_CONFIG_BITS_5               0<<9
#define UARTDRV_CONFIG_BITS_6               1<<9
#define UARTDRV_CONFIG_BITS_7               2<<9
#define UARTDRV_CONFIG_BITS_8               3<<9
#define UARTDRV_CONFIG_STOP_1               0<<11
#define UARTDRV_CONFIG_STOP_2               1<<11           
#define UARTDRV_CONFIG_PARITY_NONE          0<<12
#define UARTDRV_CONFIG_PARITY_ODD           1<<12          
#define UARTDRV_CONFIG_PARITY_EVEN          2<<12

#define UARTDRV_CONFIG_PARITY_MASK          (UARTDRV_CONFIG_PARITY_NONE|UARTDRV_CONFIG_PARITY_ODD|UARTDRV_CONFIG_PARITY_EVEN)
#define UARTDRV_CONFIG_BITS_MASK            (UARTDRV_CONFIG_BITS_5|UARTDRV_CONFIG_BITS_6|UARTDRV_CONFIG_BITS_7|UARTDRV_CONFIG_BITS_8)

// common baud rate configurations for each uart clock scale factor which give 0.16% or better error.
// the same scale factor is used for all uarts.

// scale is set by uartdrv_common_clock()

#define UARTDRV_CONFIG_BAUD_SCALE1_460800 13
#define UARTDRV_CONFIG_BAUD_SCALE1_230400 26
#define UARTDRV_CONFIG_BAUD_SCALE1_115200 39
#define UARTDRV_CONFIG_BAUD_SCALE1_57600  78
#define UARTDRV_CONFIG_BAUD_SCALE1_38400  156

#define UARTDRV_CONFIG_BAUD_SCALE2_230400 13
#define UARTDRV_CONFIG_BAUD_SCALE2_115200 26
#define UARTDRV_CONFIG_BAUD_SCALE2_57600  39
#define UARTDRV_CONFIG_BAUD_SCALE2_38400  78
#define UARTDRV_CONFIG_BAUD_SCALE2_19200  156

#define UARTDRV_CONFIG_BAUD_SCALE4_115200 13
#define UARTDRV_CONFIG_BAUD_SCALE4_57600  26
#define UARTDRV_CONFIG_BAUD_SCALE4_38400  39
#define UARTDRV_CONFIG_BAUD_SCALE4_19200  78
#define UARTDRV_CONFIG_BAUD_SCALE4_9600   156

#define UARTDRV_CONFIG_BAUD_SCALE8_57600  13
#define UARTDRV_CONFIG_BAUD_SCALE8_19200  39
#define UARTDRV_CONFIG_BAUD_SCALE8_9600   78
#define UARTDRV_CONFIG_BAUD_SCALE8_4800   156

#define UARTDRV_CONFIG_BAUD_SCALE16_9600   39
#define UARTDRV_CONFIG_BAUD_SCALE16_4800   78
#define UARTDRV_CONFIG_BAUD_SCALE16_2400   156

#define UARTDRV_CONFIG_BAUD_SCALE32_4800   39
#define UARTDRV_CONFIG_BAUD_SCALE32_2400   78
#define UARTDRV_CONFIG_BAUD_SCALE32_1200   156

#define UARTDRV_CONFIG_BAUD_SCALE64_2400   39
#define UARTDRV_CONFIG_BAUD_SCALE64_1200   78

#define UARTDRV_CONFIG_BAUD_SCALE128_1200  39
#define UARTDRV_CONFIG_BAUD_SCALE128_300   156


#endif


