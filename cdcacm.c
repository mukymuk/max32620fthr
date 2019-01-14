#include "global.h"
#include "cdcacm.h"
#include "usb_regs.h"
#include "cirbuf.h"

static cirbuf_t s_p_cirbuf;

typedef struct
{
    uint8_t * write_buffer;
    uint32_t  write_size;
}
void cdcacm_init( uint8_t *p_write_buffer, uint32_t write_size, uint8_t *p_write_buffer, uint32_t read_size )
{
    SYS_USB_Enable(1);
    MXC_USB->cn = MXC_F_USB_CN_USB_EN;
    s_p_cirbuf = p_cirbuf;
    NVIC_EnableIRQ(USB_IRQn);
}

uint32_t cdcacm_write( const void *pv_data, uint32_t length)
{
    cirbuf_write( s_p_cirbuf, pv_data, length );

}

void cdcacm_write_all( const void *p_data, uint32_t length)
{
}

uint32_t cdcacm_read( void *p_data, uint32_t length )
{
}

void cdcacm_read_all( void *p_data, uint32_t length )
{
}

uint32_t cdcacm_read_term( void *p_data, uint32_t length, uint8_t *p_term, uint32_t term_count )
{
}

void cdcacm_isr(void)
{
    uint32_t dev_intfl = MXC_USB->dev_intfl;

}
