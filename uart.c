#include "global.h"
#include "clkman_regs.h"
#include "csl\periphdriver\include\uart.h"
#include "uart.h"
#include "pmu.h"

static uart_t * s_uart[4];

static void write_update_cb( cbuf_t * p_cbuf )
{
    uart_t * p_uart = (uart_t*)p_cbuf;
    NVIC_EnableIRQ( p_uart->irq );
}

static void lock_cb( cbuf_t * p_cbuf )
{
    uart_t * p_uart = (uart_t*)p_cbuf;
    NVIC_DisableIRQ( p_uart->irq );
}

void uart_common_clock( clkman_scale_t scale )
{
    MXC_CLKMAN->sys_clk_ctrl_8_uart = scale;

}

static void inline use_vddioh( uint8_t bit )
{
    volatile uint32_t * use_vddioh_reg = &MXC_IOMAN->use_vddioh_0 + (bit >> 5);
    *use_vddioh_reg |= 1 << (bit & 0x1F);
}

static void read_lock( void * pv )
{
    const uart_t * p_uart = (const uart_t*)pv;
    NVIC_DisableIRQ( p_uart->irq );
}

static void write_lock( void * pv )
{
    const uart_t * p_uart = (const uart_t*)pv;
    pmu_lock( p_uart->pmu_channel_write );
}

static void write_out( uart_t * p_uart )
{
    void * p;
    uint32_t size = cbuf_read_aquire( p_uart->p_cbuf_write, &p );
    if( size )
    {
        p_uart->pmu_transfer_des.read_address = (uint32_t)p;
        p_uart->pmu_transfer_des.tx_length = size;
        pmu_execute( p_uart->pmu_channel_write, (const void*)&p_uart->pmu_transfer_des, 0, 0, p_uart->pmu_transfer_des.burst_size );
    }
}

static void write_unlock( void * pv )
{
    uart_t * p_uart = (uart_t*)pv;
    if( !p_uart->pmu_transfer_des.tx_length )
    {
        write_out( p_uart );
    }
    pmu_unlock( p_uart->pmu_channel_write );
}

static void read_unlock( void * pv )
{
    uart_t * p_uart = (uart_t*)pv;
    NVIC_EnableIRQ( p_uart->irq );
}

static void pmu_isr( void * pv )
{
    uart_t * p_uart = (uart_t*)pv;
    cbuf_read_release( p_uart->p_cbuf_write, p_uart->pmu_transfer_des.tx_length );
    p_uart->pmu_transfer_des.tx_length = 0;
    write_out( p_uart );
}

void uart_init( uart_t * p_uart, cbuf_t * p_cbuf_read, cbuf_t * p_cbuf_write,
                uint32_t uart_ndx, uint32_t config, uint32_t baud )
{
    static const uint8_t s_uart_pin[4] =
    {
        0 * 8, 2 * 8, 3 * 8, 5 * 8 + 3    // uart pin mappings for the max32620
    };

    s_uart[uart_ndx] = p_uart;

    p_uart->uart_ndx = uart_ndx;
    if( config & UART_CONFIG_VDDOIH )
    {
        uint8_t rx = s_uart_pin[p_uart->uart_ndx];
        use_vddioh( rx );
        use_vddioh( rx + 1 );   // tx
        if( config & UART_CONFIG_RTS_ENABLE )
        {
            if( config & UART_CONFIG_CTSRTS_SWAP )
                use_vddioh( rx + 3 ); // swapped rts
            else
                use_vddioh( rx + 2 ); // swapped cts
        }
        if( config & UART_CONFIG_CTS_ENABLE )
        {
            if( config & UART_CONFIG_CTSRTS_SWAP )
                use_vddioh( rx + 2 );
            else
                use_vddioh( rx + 3 );
        }
    }
    uint32_t * p_ioman_req = (int32_t*)(MXC_BASE_IOMAN + MXC_R_IOMAN_OFFS_UART0_REQ + (uart_ndx << 3));
    uint32_t ioman_req = MXC_F_IOMAN_UART0_REQ_IO_REQ;
    uint32_t uart_ctrl = (MXC_F_UART_CTRL_UART_EN | MXC_F_UART_CTRL_TX_FIFO_EN | MXC_F_UART_CTRL_RX_FIFO_EN | ((MXC_UART_FIFO_DEPTH - 3) << MXC_F_UART_CTRL_RTS_LEVEL_POS));

    if( config & UART_CONFIG_TXRX_SWAP )
    {
        ioman_req |= MXC_F_IOMAN_UART0_REQ_IO_MAP;
    }
    if( config & UART_CONFIG_CTSRTS_SWAP )
    {
        ioman_req |= MXC_F_IOMAN_UART0_REQ_CTS_MAP | MXC_F_IOMAN_UART0_REQ_RTS_MAP;
    }
    if( config & UART_CONFIG_CTS_ENABLE )
    {
        ioman_req |= MXC_F_IOMAN_UART0_REQ_CTS_IO_REQ;
        uart_ctrl |= MXC_F_UART_CTRL_CTS_EN;
    }
    if( config & UART_CONFIG_RTS_ENABLE )
    {
        ioman_req |= MXC_F_IOMAN_UART0_REQ_RTS_IO_REQ;
        uart_ctrl |= MXC_F_UART_CTRL_RTS_EN;
    }
    *p_ioman_req = ioman_req;

    uart_ctrl |= ((config & UART_CONFIG_BITS_MASK) >> UART_CONFIG_BITS_SHIFT) << MXC_F_UART_CTRL_DATA_SIZE_POS;
    uart_ctrl |= ((config & UART_CONFIG_PARITY_MASK) >> UART_CONFIG_PARITY_SHIFT) << MXC_F_UART_CTRL_PARITY_POS;


    cbuf_write_lock( p_cbuf_write, write_lock, write_unlock, p_uart );
    cbuf_read_lock( p_cbuf_read, read_lock, read_unlock, p_uart );

    p_uart->p_mxc_uart_regs = MXC_UART_GET_UART( uart_ndx );
    p_uart->p_mxc_uart_regs->baud = baud;
    p_uart->p_mxc_uart_regs->inten = MXC_F_UART_INTEN_TX_FIFO_AE | MXC_F_UART_INTEN_RX_FIFO_NOT_EMPTY;

    p_uart->p_mxc_uart_regs->ctrl = uart_ctrl;

    p_uart->p_cbuf_read = p_cbuf_read;
    p_uart->p_cbuf_write = p_cbuf_write;

    p_uart->irq = MXC_UART_GET_IRQ( uart_ndx );
    p_uart->p_fifo = MXC_UART_GET_FIFO( uart_ndx );
    p_uart->pmu_channel_write = pmu_alloc( pmu_isr, p_uart );

    p_uart->pmu_transfer_des.burst_size = MXC_UART_FIFO_DEPTH >> 1;
    p_uart->pmu_transfer_des.interrupt = PMU_INTERRUPT;
    p_uart->pmu_transfer_des.op_code = PMU_TRANSFER_OP;
    p_uart->pmu_transfer_des.read_inc = PMU_TX_READ_INC;
    p_uart->pmu_transfer_des.read_size = PMU_TX_READ_8_BIT;
    p_uart->pmu_transfer_des.stop = PMU_STOP;
    p_uart->pmu_transfer_des.write_size = PMU_TX_WRITE_8_BIT;
    p_uart->pmu_transfer_des.write_inc = PMU_TX_WRITE_NO_INC;
    p_uart->pmu_transfer_des.int_mask = PMU_WAIT_IRQ_MASK1_SEL0_UART0_TX_FIFO_AE << (uart_ndx << 1);
    p_uart->pmu_transfer_des.write_address = (uint32_t)p_uart->p_fifo->tx_32;

    NVIC_EnableIRQ( p_uart->irq );
}

void uart_isr( uart_t * p_uart )
{
    mxc_uart_regs_t *p_regs = p_uart->p_mxc_uart_regs;

    if( p_regs->intfl & MXC_F_UART_INTFL_RX_FIFO_NOT_EMPTY )
    {
        uint8_t count = p_regs->rx_fifo_ctrl & MXC_F_UART_RX_FIFO_CTRL_FIFO_ENTRY;
        cbuf_write( p_uart->p_cbuf_read, (const void*)&p_uart->p_fifo->rx, count );
        p_regs->intfl = MXC_F_UART_INTFL_RX_FIFO_NOT_EMPTY;
    }
    NVIC_ClearPendingIRQ(p_uart->irq);
}

void UART0_IRQHandler( void )
{
    uart_isr( s_uart[0] );
}

void UART1_IRQHandler( void )
{
    uart_isr( s_uart[1] );
}

void UART2_IRQHandler( void )
{
    uart_isr( s_uart[2] );
}

void UART3_IRQHandler( void )
{
    uart_isr( s_uart[3] );
}
