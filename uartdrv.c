#include "global.h"
#include "uartdrv.h"
#include "clkman_regs.h"
#include "uart.h"

static void write_update_cb( cirbuf_t *p_cirbuf )
{
    uartdrv_t *p_uart = (uartdrv_t*)p_cirbuf;
    NVIC_EnableIRQ( MXC_UART_GET_IRQ(p_uart->uartdrv_ndx) );
}

static void lock_cb( cirbuf_t *p_cirbuf )
{
    uartdrv_t *p_uart = (uartdrv_t*)p_cirbuf;
    NVIC_DisableIRQ( MXC_UART_GET_IRQ(p_uart->uartdrv_ndx) );
}

void uartdrv_common_clock( clkman_scale_t scale )
{
    MXC_CLKMAN->sys_clk_ctrl_8_uart = scale;

}
void uartdrv_init( uartdrv_t * p_uart, const uartdrv_config_t * p_uartdrv_config )
{
    p_uart->pmu_ndx = p_uartdrv_config->pmu_ndx;
    p_uart->uartdrv_ndx = p_uartdrv_config->uartdrv_ndx;
    cirbuf_init( &p_uart->cirbuf, p_uartdrv_config->buffer, p_uartdrv_config->size, write_update_cb, NULL, lock_cb );

    uint32_t * p_req = (int32_t *)(MXC_BASE_IOMAN + MXC_R_IOMAN_OFFS_UART0_REQ + (p_uart->uartdrv_ndx<<3));
    *p_req = p_uartdrv_config->ioman;

    mxc_uart_regs_t *p = MXC_UART_GET_UART(p_uart->uartdrv_ndx);
    p->ctrl = p_uartdrv_config->ctrl | (MXC_F_UART_CTRL_UART_EN | MXC_F_UART_CTRL_TX_FIFO_EN |
                  MXC_F_UART_CTRL_RX_FIFO_EN | ((MXC_UART_FIFO_DEPTH-3) <<  MXC_F_UART_CTRL_RTS_LEVEL_POS));
    p->baud = p_uartdrv_config->baud;

}

void uartdrv_isr( uartdrv_t * p_uart )
{

}
