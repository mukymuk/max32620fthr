#ifndef __BOARD_H__
#define __BOARD_H__

#include ".\uart.h"

#define BOARD_UART_DBG  1

void board_init(void);

typedef enum _board_led_t
{
    board_led_off, board_led_on, board_led_toggle
}
board_led_t;

void board_led( uint8_t ndx, board_led_t state );
void board_snd( uint8_t ndx, int32_t freq_hz );
void board_motor_enable( uint8_t ndx, bool enable );
void board_sleep( void );
void board_reset( void );

#endif

