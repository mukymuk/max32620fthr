#include "global.h"
#include "board.h"
#include "mc.h"
#include "gpio.h"
#include "mxc_sys.h"
#include "tmr.h"
#include "cbuf.h"
#include "lp.h"

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

void board_init(void)
{

    uint8_t i;
    for( i = 0; i < ARRAY_COUNT( s_gpio_cfg_led ); i++ )
    {
        SYS_IOMAN_UseVDDIOH( &s_gpio_cfg_led[i] );
        board_led( i, board_led_off );
        GPIO_Config( &s_gpio_cfg_led[i] );
    }

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
        .mode = TMR32_MODE_PWM
    };
    TMR32_Config( MXC_TMR4, &tmr32_cfg );
    TMR32_SetCount(MXC_TMR4,1);
    TMR32_EnableINT(MXC_TMR4);
 //   NVIC_EnableIRQ(TMR4_0_IRQn);

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
        TMR32_SetDuty( MXC_TMR4, period >> 1 );
        TMR32_Start( MXC_TMR4 );
    }
    else
    {
        TMR32_Stop( MXC_TMR4 );
        TMR32_SetCount(MXC_TMR4,1);
    }
}

void board_sleep( void )
{
    LP_EnterLP2();
}

void TMR4_IRQHandler( void )
{
    mc_timer_isr(0);
}


