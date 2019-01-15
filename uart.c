#include "global.h"
#include "uart.h"
#include "csl\PeriphDriver\Include\uart.h"
#include "clkman_regs.h"

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

void uart_common_clock( clkman_scale_t scale )
{
    MXC_CLKMAN->sys_clk_ctrl_8_uart = scale;

}
void uart_init( uart_t * p_uart, const uart_config_t * p_uart_config )
{
    p_uart->pmu_ndx = p_uart_config->pmu_ndx;
    p_uart->uart_ndx = p_uart_config->uart_ndx;
    cirbuf_init( &p_uart->cirbuf, p_uart_config->buffer, p_uart_config->size, write_update_cb, NULL, lock_cb );

    uint32_t * p_req = (int32_t *)(MXC_BASE_IOMAN + MXC_R_IOMAN_OFFS_UART0_REQ + (p_uart->uart_ndx<<3));
    *p_req = p_uart_config->ioman;

    mxc_uart_regs_t *p = MXC_UART_GET_UART(p_uart->uart_ndx);
    p->ctrl = p_uart_config->ctrl | (MXC_F_UART_CTRL_UART_EN | MXC_F_UART_CTRL_TX_FIFO_EN |
                  MXC_F_UART_CTRL_RX_FIFO_EN | ((MXC_UART_FIFO_DEPTH-3) <<  MXC_F_UART_CTRL_RTS_LEVEL_POS));
    p->baud = p_uart_config->baud;
   
/*
    uart->ctrl = 0;
    uart->ctrl = (MXC_F_UART_CTRL_UART_EN | MXC_F_UART_CTRL_TX_FIFO_EN |
                  MXC_F_UART_CTRL_RX_FIFO_EN | 
                  (UART_RXFIFO_USABLE <<  MXC_F_UART_CTRL_RTS_LEVEL_POS));

    // Configure data size, stop bit, parity, cts, and rts
    uart->ctrl |= ((cfg->size << MXC_F_UART_CTRL_DATA_SIZE_POS) |
                   (cfg->extra_stop << MXC_F_UART_CTRL_EXTRA_STOP_POS) |
                   (cfg->parity << MXC_F_UART_CTRL_PARITY_POS) |
                   (cfg->cts << MXC_F_UART_CTRL_CTS_EN_POS) |
                   (cfg->rts << MXC_F_UART_CTRL_RTS_EN_POS));
*/
}

void uart_isr( uart_t * p_uart )
{

}
