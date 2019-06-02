#ifndef __CONSTS_H__
#define __CONSTS_H__

#define LED_MATRIX_PORT PORTB

//ADDRESSES FOR MAX7219
#define NO_OP      0x0000
#define DIG_0      0x0100
#define DIG_1      0x0200
#define DIG_2      0x0300
#define DIG_3      0x0400
#define DIG_4      0x0500
#define DIG_5      0x0600
#define DIG_6      0x0700
#define DIG_7      0x0800

#define DECODE_MODE_REG_ADDR 0x0900
#define INTENSITY_REG_ADDR   0x0A00
#define SCAN_LIMIT_REG_ADDR  0x0B00
#define SHUTDOWN_REG_ADDR    0x0C00
#define DISPLAY_TST_REG_ADDR 0x0F00

//DECODE MODES
#define NO_DECODE  0x00
#define ALL_DIGITS 0xFF
//INTENSITIES
#define INTENSITY_LOW  0x00
#define INTENSITY_MID  0x07
#define INTENSITY_HIGH 0x0F

//SCAN LIMITS
#define SCAN_LOW_NIB 0x03
#define SCAN_BYTE    0x07

//SHUTDOWN MODES
#define SHUTDOWN_MODE        0x00
#define SHUTDOWN_NORMAL_MODE 0x01

//DISPLAY TEST MODES
#define DISP_NORMAL_MODE 0x00
#define DISP_TEST_MODE   0x01

#define MAX_CLK ((~PINA >> 3) & 0X01)
#define MAX_CLK_HIGH 0x01
#define MAX_CLK_LOW  0x00
#define MAX_CLK_BIT 3

#define LOAD  0x02
#define CLEAR 0x00

#define BOTTOM_MATRIX 3

#endif
