#ifndef __BOARD_H__
#define __BOARD_H__

#include "uartdrv.h"

void board_init(void);

typedef enum _board_led_t
{
    board_led_off, board_led_on, board_led_toggle
}
board_led_t;

void board_led( uint8_t ndx, board_led_t state );
void board_snd( uint8_t ndx, int32_t freq_hz );
void board_motor_enable( uint8_t ndx, bool enable );

#define BOARD_UART0_P0_PORT0    0
#define BOARD_UART1_P2_DEBUG    1
#define BOARD_UART2_P3_HEADER   2
#define BO0RD_UART3_P5_HEADER   3

#endif

