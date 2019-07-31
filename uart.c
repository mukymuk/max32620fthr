#include "global.h"
#include "clkman_regs.h"
#include "csl\periphdriver\include\uart.h"
#include "uart.h"
#include "board.h"

static uart_t * s_uart[4];


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
    p_uart->p_mxc_uart_regs->inten &= ~MXC_F_UART_INTEN_RX_FIFO_NOT_EMPTY;
}

static void read_unlock( void * pv )
{
    uart_t * p_uart = (uart_t*)pv;
    p_uart->p_mxc_uart_regs->inten |= MXC_F_UART_INTEN_RX_FIFO_NOT_EMPTY;
}

static void write_out( const uart_t *p_uart )
{
	uint32_t data_avail;
	uint8_t tx_fifo_avail;
	uint32_t xfer_count;
    uint32_t i;
    uint8_t *p;

    __IO uint8_t * p_fifo = &p_uart->p_fifo->tx;

	do
	{
		data_avail = cbuf_read_aquire( p_uart->p_cbuf_write, (void**)&p );
		tx_fifo_avail = MXC_UART_FIFO_DEPTH - (p_uart->p_mxc_uart_regs->tx_fifo_ctrl & MXC_F_UART_TX_FIFO_CTRL_FIFO_ENTRY);
		xfer_count = tx_fifo_avail > data_avail ? data_avail : tx_fifo_avail;
		for(i=0;i<xfer_count;i++)
		{
			if( p[i] & 0x80 )
				p[i] = 0;
			*p_fifo = p[i];
		}
		if( data_avail )
			cbuf_read_release( p_uart->p_cbuf_write, xfer_count );
	}
	while( xfer_count );
}

static void write_lock( void * pv )
{
    const uart_t * p_uart = (const uart_t*)pv;
    p_uart->p_mxc_uart_regs->inten &= ~MXC_F_UART_INTEN_TX_FIFO_AE;
    write_out(p_uart);
}


static void write_unlock( void * pv )
{
    uart_t * p_uart = (uart_t*)pv;
	write_out(p_uart);
    p_uart->p_mxc_uart_regs->inten |= MXC_F_UART_INTEN_TX_FIFO_AE;
}

static void write_pend( void * pv )
{
    write_unlock(pv);
    board_sleep();
    write_lock(pv);
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
    uint32_t uart_ctrl = (MXC_F_UART_CTRL_UART_EN | MXC_F_UART_CTRL_TX_FIFO_EN | MXC_F_UART_CTRL_RX_FIFO_EN | ((MXC_UART_FIFO_DEPTH/2) << MXC_F_UART_CTRL_RTS_LEVEL_POS));

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


    cbuf_write_lock( p_cbuf_write, write_lock, write_unlock, write_pend, p_uart );
    cbuf_read_lock( p_cbuf_read, read_lock, read_unlock, p_uart );

    p_uart->p_mxc_uart_regs = MXC_UART_GET_UART( uart_ndx );
    p_uart->p_mxc_uart_regs->baud = baud;
    p_uart->p_mxc_uart_regs->inten = MXC_F_UART_INTEN_TX_FIFO_AE | MXC_F_UART_INTEN_RX_FIFO_NOT_EMPTY;

    p_uart->p_mxc_uart_regs->ctrl = uart_ctrl;

    p_uart->p_mxc_uart_regs->tx_fifo_ctrl = (MXC_UART_FIFO_DEPTH-1) << MXC_F_UART_TX_FIFO_CTRL_FIFO_AE_LVL_POS;

    p_uart->p_cbuf_read = p_cbuf_read;
    p_uart->p_cbuf_write = p_cbuf_write;

    p_uart->irq = MXC_UART_GET_IRQ( uart_ndx );
    p_uart->p_fifo = MXC_UART_GET_FIFO( uart_ndx );

    NVIC_EnableIRQ( p_uart->irq );
}

void uart_isr( uart_t * p_uart )
{
    mxc_uart_regs_t *p_regs = p_uart->p_mxc_uart_regs;

    if( p_regs->intfl & MXC_F_UART_INTFL_RX_FIFO_NOT_EMPTY )
    {
        uint8_t count = p_regs->rx_fifo_ctrl & MXC_F_UART_RX_FIFO_CTRL_FIFO_ENTRY;
        if( !cbuf_write( p_uart->p_cbuf_read, (const void*)&p_uart->p_fifo->rx, count ) )
            p_uart->p_mxc_uart_regs->inten &= !MXC_F_UART_INTEN_RX_FIFO_NOT_EMPTY;
        p_regs->intfl = MXC_F_UART_INTFL_RX_FIFO_NOT_EMPTY;
    }
    if( p_regs->intfl & MXC_F_UART_INTEN_TX_FIFO_AE )
    {
        write_out(p_uart);
        p_regs->intfl = MXC_F_UART_INTEN_TX_FIFO_AE;
    }
    NVIC_ClearPendingIRQ( p_uart->irq );
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
