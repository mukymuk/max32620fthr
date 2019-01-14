#include "global.h"
#include "board.h"
#include "gpio.h"
#include "mxc_sys.h"
#include "tmr.h"
#include "cirbuf.h"

static const gpio_cfg_t s_gpio_cfg_motor_step =
{
    PORT_5, PIN_0, GPIO_FUNC_TMR, GPIO_PAD_NORMAL
};
static const gpio_cfg_t s_gpio_cfg_motor_direction =
{
    PORT_5, PIN_1, GPIO_FUNC_GPIO, GPIO_PAD_NORMAL
};
static const gpio_cfg_t s_gpio_cfg_motor_enable =
{
    PORT_5, PIN_2, GPIO_FUNC_GPIO, GPIO_PAD_NORMAL
};

static const gpio_cfg_t s_gpio_cfg_sw1_boot =
{
    PORT_2, PIN_7, GPIO_FUNC_GPIO, GPIO_PAD_INPUT_PULLUP
};

static const gpio_cfg_t s_gpio_cfg_led[] =
{
    // LED's on the feather board
    { PORT_2, PIN_4, GPIO_FUNC_GPIO, GPIO_PAD_OPEN_DRAIN },
    { PORT_2, PIN_5, GPIO_FUNC_GPIO, GPIO_PAD_OPEN_DRAIN },
    { PORT_2, PIN_6, GPIO_FUNC_GPIO, GPIO_PAD_OPEN_DRAIN }
};

typedef struct
{
    cirbuf_t    cirbuf;
    uint32_t    device_id;
    uint8_t     buffer[256];
}
serial_buffer_t;

static serial_buffer_t s_uart_buffer[4];
static serial_buffer_t s_cdcacm_buffer;

void uart_update_tx(cirbuf_t * p_cirbuf)
{
    serial_buffer_t *p = (serial_buffer_t*)p_cirbuf;
    NVIC_EnableIRQ( MXC_UART_GET_IRQ(p->device_id) );
}

void uart_update_rx(cirbuf_t * p_cirbuf)
{
    serial_buffer_t *p = (serial_buffer_t*)p_cirbuf;
}

void uart_lock(cirbuf_t * p_cirbuf)
{
    serial_buffer_t *p = (serial_buffer_t*)p_cirbuf;
    NVIC_DisableIRQ( MXC_UART_GET_IRQ(p->device_id) );
}

void cdcacm_update_tx(cirbuf_t * p_cirbuf)
{
}

void cdcacm_update_rx(cirbuf_t * p_cirbuf)
{
}

void cdcacm_lock(cirbuf_t * p_cirbuf)
{
}

typedef enum
{
    board_serial_device_uart0,
    board_serial_device_uart1, 
    board_serial_device_uart2,
    board_serial_device_uart3,
    board_serial_device_usb
}
board_serial_device_t;

cirbuf_t * board_serial_device( board_serial_device_t id )
{
    switch( id )
    {
        case board_serial_device_uart0:
            return &s_uart_buffer[0].cirbuf;
        case board_serial_device_uart1:
            return &s_uart_buffer[1].cirbuf;
        case board_serial_device_uart2:
            return &s_uart_buffer[2].cirbuf;
        case board_serial_device_uart3:
            return &s_uart_buffer[3].cirbuf;
        case board_serial_device_usb:
            return &s_cdcacm_buffer.cirbuf;
    }
    while( 1 );
    return (cirbuf_t*)0;
}

void board_init(void)
{

    uint8_t i;
    for( i = 0; i < ARRAY_COUNT( s_gpio_cfg_led ); i++ )
    {
        SYS_IOMAN_UseVDDIOH( &s_gpio_cfg_led[i] );
        board_led( i, board_led_off );
        GPIO_Config( &s_gpio_cfg_led[i] );
    }
    for(i=0;i<ARRAY_COUNT(s_uart_buffer);i++)
    {
        cirbuf_init( &s_uart_buffer[i].cirbuf, s_uart_buffer[i].buffer, sizeof(s_uart_buffer[i].buffer), uart_update_tx, uart_update_rx, uart_lock );
        s_uart_buffer[i].device_id = i;
    }
    cirbuf_init( &s_cdcacm_buffer.cirbuf, &s_cdcacm_buffer.buffer, sizeof(s_cdcacm_buffer.buffer), cdcacm_update_tx, cdcacm_update_rx, cdcacm_lock );

    SYS_IOMAN_UseVDDIOH( &s_gpio_cfg_motor_step );
    SYS_IOMAN_UseVDDIOH( &s_gpio_cfg_motor_direction );
    SYS_IOMAN_UseVDDIOH( &s_gpio_cfg_motor_enable );

    GPIO_OutClr(&s_gpio_cfg_motor_enable);
    GPIO_OutClr(&s_gpio_cfg_motor_direction);

    GPIO_Config( &s_gpio_cfg_motor_direction );
    GPIO_Config( &s_gpio_cfg_motor_enable );
    GPIO_Config( &s_gpio_cfg_sw1_boot );

    TMR_Init( MXC_TMR4, TMR_PRESCALE_DIV_2_0, &s_gpio_cfg_motor_step );
    static const tmr32_cfg_t tmr32_cfg =
    {
        .mode = TMR32_MODE_CONTINUOUS,
    };
    TMR32_Config( MXC_TMR4, &tmr32_cfg );

}

void board_motor_enable( uint8_t ndx, bool enable )
{
    if( enable )
        GPIO_OutSet(&s_gpio_cfg_motor_enable);
    else
        GPIO_OutClr(&s_gpio_cfg_motor_enable);
}


void board_led( uint8_t ndx, board_led_t state )
{
    if( ndx < ARRAY_COUNT( s_gpio_cfg_led ) )
    {
        if( state == board_led_toggle )
            GPIO_OutToggle( &s_gpio_cfg_led[ndx] );
        else
            GPIO_OutPut( &s_gpio_cfg_led[ndx], state == board_led_off ? ~0 : 0 );
    }
}

void board_snd( uint8_t ndx, int32_t freq_hz )
{
    if( freq_hz >= 0 )
    {
        GPIO_OutSet(&s_gpio_cfg_motor_direction);
    }
    else
    {
        GPIO_OutClr(&s_gpio_cfg_motor_direction);
        freq_hz = -freq_hz;
    }
    if( freq_hz )
    {
        uint32_t period = SystemCoreClock / (freq_hz << 1);
        TMR32_SetCompare( MXC_TMR4, period );
        TMR32_Start( MXC_TMR4 );
    }
    else
        TMR32_Stop( MXC_TMR4 );

}