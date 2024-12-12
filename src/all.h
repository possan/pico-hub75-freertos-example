#include <stdlib.h>

#define WIDTH 64
#define HEIGHT 32
#define PANELWIDTH 64
#define PANELHEIGHT 32
#define ROWS_OF_PANELS 1
#define SCANLINES 16

#define ADDR0_PIN 2
#define ADDR1_PIN 3
#define ADDR2_PIN 4
#define ADDR3_PIN 5
#define ADDR4_PIN 9
#define OE_PIN 6
#define CLOCK_PIN 7
#define LATCH_PIN 8
#define R1_PIN 10
#define R2_PIN 11
#define G1_PIN 12
#define G2_PIN 13
#define B1_PIN 14
#define B2_PIN 15

#define BUF_OFFSET(x,y) ((y * WIDTH + x) * 3)

extern uint32_t ntp_epoch;
extern absolute_time_t ntp_synctime;
extern void wifi_init();

extern uint8_t led_buffer[];
extern void leds_init();

extern void server_init();

extern void demo_init();
