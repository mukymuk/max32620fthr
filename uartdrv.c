#include "global.h"
#include "uartdrv.h"
#include "clkman_regs.h"
#include "uart.h"

static void write_update_cb( cirbuf_t *p_cirbuf )
{
    uartdrv_t *p_uart = (uartdrv_t*)p_cirbuf;
    NVIC_EnableIRQ( MXC_UART_GET_IRQ(p_uart->uart_ndx) );
}

static void lock_cb( cirbuf_t *p_cirbuf )
{
    uartdrv_t *p_uart = (uartdrv_t*)p_cirbuf;
    NVIC_DisableIRQ( MXC_UART_GET_IRQ(p_uart->uart_ndx) );
}

void uartdrv_common_clock( clkman_scale_t scale )
{
    MXC_CLKMAN->sys_clk_ctrl_8_uart = scale;

}

static void inline use_vddioh( uint8_t bit )
{
    volatile uint32_t *use_vddioh_reg = &MXC_IOMAN->use_vddioh_0 + (bit>>5);
    *use_vddioh_reg |= 1 << (bit & 0x1F);
}


void uartdrv_init( uartdrv_t * p_uart, const uartdrv_config_t * p_uartdrv_config )
{
    static const uint8_t s_uart_pin[4] =
    {
        0*8, 2*8, 3*8, 5*8+3    // uart pin mappings for the max32620
    };

    p_uart->pmu_ndx = p_uartdrv_config->pmu_ndx;
    p_uart->uart_ndx = p_uartdrv_config->uart_ndx;

    cirbuf_init( &p_uart->cirbuf, p_uartdrv_config->buffer, p_uartdrv_config->size, write_update_cb, NULL, lock_cb );

    if( p_uartdrv_config->config & UARTDRV_CONFIG_VDDOIH )
    {
        uint8_t rx = s_uart_pin[ p_uart->uart_ndx ];
        use_vddioh(rx);
        use_vddioh(rx+1);   // tx
        if( p_uartdrv_config->config & UARTDRV_CONFIG_RTS_ENABLE )
        {
            if( p_uartdrv_config->config & UARTDRV_CONFIG_CTSRTS_SWAP )
                use_vddioh( rx + 3 ); // swapped rts
            else
                use_vddioh( rx + 2 ); // swapped cts
        }
        if( p_uartdrv_config->config & UARTDRV_CONFIG_CTS_ENABLE )
        {
            if( p_uartdrv_config->config & UARTDRV_CONFIG_CTSRTS_SWAP )
                use_vddioh( rx + 2 );
            else
                use_vddioh( rx + 3 );
        }
    }
    uint32_t * p_ioman_req = (int32_t *)(MXC_BASE_IOMAN + MXC_R_IOMAN_OFFS_UART0_REQ + (p_uart->uart_ndx<<3));
    uint32_t ioman_req = 0;
    uint32_t uart_ctrl = (MXC_F_UART_CTRL_UART_EN | MXC_F_UART_CTRL_TX_FIFO_EN | MXC_F_UART_CTRL_RX_FIFO_EN | ((MXC_UART_FIFO_DEPTH-3) <<  MXC_F_UART_CTRL_RTS_LEVEL_POS));
    if( p_uartdrv_config->config & UARTDRV_CONFIG_CTSRTS_SWAP )
    {
        ioman_req |= MXC_F_IOMAN_UART0_REQ_CTS_MAP | MXC_F_IOMAN_UART0_REQ_RTS_MAP;
    }
    if( p_uartdrv_config->config & UARTDRV_CONFIG_IOMAN_TXRX_SWAP )
    {
        ioman_req |= MXC_F_IOMAN_UART0_REQ_IO_MAP;
    }
    if( p_uartdrv_config->config & UARTDRV_CONFIG_CTRL_CTS_ENABLE )
    {
        ioman_req |= MXC_F_IOMAN_UART0_REQ_CTS_IO_REQ;
        uart_ctrl |= MXC_F_UART_CTRL_CTS_EN;
    }
    if( p_uartdrv_config->config & UARTDRV_CONFIG_CTRL_RTS_ENABLE )
    {
        ioman_req |= MXC_F_IOMAN_UART0_REQ_RTS_IO_REQ;
        uart_ctrl |= MXC_F_UART_CTRL_RTS_EN;
    }
    *p_ioman_req = ioman_req;

    mxc_uart_regs_t *p = MXC_UART_GET_UART(p_uart->uart_ndx);
    p->baud = p_uartdrv_config->baud;
    p->ctrl = uart_ctrl;
}

void uartdrv_isr( uartdrv_t * p_uart )
{

}
