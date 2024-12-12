#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "all.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "lwip/netdb.h"
#include "font_8x8.h"

#define FX_INTERVAL_MS 2000
#define TICK_INTERVAL_MS 40
#define TIME_UPDATE_INTERVAL_MS 500

int demo_mode = 0;
int run_demo = 1;
int cnt = 0;
float rot_x = 0;
float rot_y = 0;
float rot_z = 0;
uint32_t demo_nextfxtime;
uint32_t demo_nextupdatetime;
uint32_t demo_frame;

uint8_t life_frame[WIDTH * HEIGHT];
uint8_t life_tempbuf[WIDTH * HEIGHT];
uint32_t life_timer;

uint8_t demo_buf[WIDTH * HEIGHT * 3] = {
    0,
};

char timestr[40] = {
    0,
};

void life_add_random_full()
{
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        life_frame[i] ^= (rand() % 3) > 0 ? 1 : 0;
    }
}

void life_add_random_hline(int y)
{
    if (y < 0)
        return;
    if (y >= HEIGHT)
        return;
    for (int i = 0; i < WIDTH; i++)
    {
        life_frame[y * WIDTH + i] ^= (rand() % 3) > 0 ? 1 : 0;
    }
}

void life_add_random_vline(int x)
{
    if (x < 0)
        return;
    if (x >= WIDTH)
        return;
    int sub = rand() % 2;
    for (int i = 0; i < HEIGHT; i++)
    {
        int r = (rand() % 5 - 2) > 0 ? 1 : 0;
        if (sub)
        {
            life_frame[i * WIDTH + x] &= r;
        }
        else
        {
            life_frame[i * WIDTH + x] ^= r;
        }
    }
}

void life_add_floater(int x, int y, char *def)
{
    int fx = x;
    int fy = y;
    char *c = def;
    while (*c)
    {
        if (*c == 'x')
        {
            if (fx >= 0 && fy >= 0 && fx < WIDTH && fy < HEIGHT)
            {
                life_frame[fy * WIDTH + fx] = 1;
            }
            fx++;
        }
        if (*c == '.')
        {
            if (fx >= 0 && fy >= 0 && fx < WIDTH && fy < HEIGHT)
            {
                life_frame[fy * WIDTH + fx] = 0;
            }
            fx++;
        }
        if (*c == '\n')
        {
            fy++;
            fx = x;
        }
        c++;
    }
}

char *floater1 =
    "....\n"
    ".xx.\n"
    ".xx.\n"
    "....";

char *floater2 =
    "..xx\n"
    ".x..x\n"
    ".x..x\n"
    "..xx.";

char *floater3 =
    "..x.\n"
    "..x.\n"
    "..x.\n"
    "....";

char *floater4 =
    "..x.\n"
    "...x\n"
    ".xxx\n";

char *floater5 =
    "..x.\n"
    "....\n"
    ".xxx\n"
    "....";

char *floater6 =
    "x..x..\n"
    "....x\n"
    "x...x\n"
    ".xxxx";

char *floater7 =
    "..x..\n"
    ".xxx.\n"
    "xxxxx";

void life_add_random_floater(int x, int y)
{
    int r = rand() % 7;
    if (r == 0)
        life_add_floater(x, y, floater1);
    if (r == 1)
        life_add_floater(x, y, floater2);
    if (r == 2)
        life_add_floater(x, y, floater3);
    if (r == 3)
        life_add_floater(x, y, floater4);
    if (r == 4)
        life_add_floater(x, y, floater5);
    if (r == 5)
        life_add_floater(x, y, floater6);
    if (r == 6)
        life_add_floater(x, y, floater7);
}

void life_step()
{
    int o = 0;
    for (int j = 0; j < HEIGHT; j++)
    {
        for (int i = 0; i < WIDTH; i++)
        {
            int nn = 0;

            int im1 = (i + WIDTH - 1) % WIDTH;
            int ip1 = (i + 1) % WIDTH;

            int jm1 = (j + HEIGHT - 1) % HEIGHT;
            int jp1 = (j + 1) % HEIGHT;

            nn += life_frame[(jm1 * WIDTH + im1)];
            nn += life_frame[(jm1 * WIDTH + i)];
            nn += life_frame[(jm1 * WIDTH + ip1)];
            nn += life_frame[(j * WIDTH + im1)];
            nn += life_frame[(j * WIDTH + ip1)];
            nn += life_frame[(jp1 * WIDTH + im1)];
            nn += life_frame[(jp1 * WIDTH + i)];
            nn += life_frame[(jp1 * WIDTH + ip1)];

            int cp = life_frame[j * WIDTH + i];
            int op = cp;

            if (cp)
            {
                // i am live
                if (nn < 2)
                {
                    // 1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
                    op = 0;
                }
                else if (nn == 2 || nn == 3)
                {
                    // 2. Any live cell with two or three live neighbours lives on to the next generation.
                    op = 1;
                }
                else if (nn > 3)
                {
                    // 3. Any live cell with more than three live neighbours dies, as if by overpopulation.
                    op = 0;
                }
            }
            else
            {
                // i am dead
                if (nn == 3)
                {
                    // 4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
                    // resurrect.
                    op = 1;
                }
            }

            life_tempbuf[o] = op;
            o++;
        }
    }
    memcpy(life_frame, life_tempbuf, WIDTH * HEIGHT);
}

void draw_char(int16_t x, int16_t y, const char ch)
{
    for (uint8_t l = 0; l < 8; l++)
    {
        uint8_t b = font8x8_basic[ch][l];
        for (uint8_t c = 0; c < 8; c++)
        {
            int16_t x2 = x + c;
            int16_t y2 = y + l;
            uint32_t o = (y2 * WIDTH + x2) * 3;

            if (b & (1 << c))
            {
                demo_buf[o++] = 255;
                demo_buf[o++] = 255;
                demo_buf[o++] = 255;
            }
        }
    }
}

void draw_string(int16_t x, int16_t y, const char *text)
{
    int16_t x2 = x;
    int16_t y2 = y;
    char *ch = text;
    while (*ch)
    {
        if (*ch == '\n')
        {
            x2 = x;
            y2 += 8;
        }
        else
        {
            draw_char(x2, y2, *ch);
            x2 += 8;
        }
        ch++;
    }
}

void life_step_and_render()
{
    uint32_t T = time_us_32();

    int add_fx = 0;
    if (T > demo_nextfxtime)
    {
        demo_nextfxtime = T + FX_INTERVAL_MS * 1000;
        add_fx = 1;
    }

    if (T > demo_nextupdatetime)
    {
        demo_nextupdatetime = T + TIME_UPDATE_INTERVAL_MS * 1000;

        if (ntp_epoch)
        {
            absolute_time_t delta = get_absolute_time() - ntp_synctime;
            absolute_time_t realtime = ntp_epoch + (delta / 1000000);
            struct tm *utc = gmtime(&realtime);
            sprintf(timestr, "%02d:%02d:%02d",
                    /* utc->tm_mday, utc->tm_mon + 1, utc->tm_year + 1900, */
                    utc->tm_hour, utc->tm_min, utc->tm_sec);
        }
        else
        {
            sprintf(timestr, "...");
        }
    }

    memset(&demo_buf, 0, 3 * WIDTH * HEIGHT);
    life_step();

    if (add_fx > 0)
    {
        for (int k = 0; k < 2; k++)
        {
            int x = (rand() % (WIDTH + 3)) - 2;
            int y = (rand() % (HEIGHT + 3)) - 2;
            life_add_random_floater(x, y);
        }
    }

    int x = (demo_frame * 1) % WIDTH;
    life_add_random_vline(x);

    int o1 = 0;
    int o3 = 0;
    for (int j = 0; j < HEIGHT; j++)
    {
        for (int i = 0; i < WIDTH; i++)
        {
            int b = life_frame[o1++] * 254;
            demo_buf[o3++] = b;
            demo_buf[o3++] = 0;
            demo_buf[o3++] = 0;
        }
    }

    char buf[20];
    draw_string(1, 1, "Sign?");

    sprintf(buf, "f=%d", demo_frame);
    draw_string(1, 10, buf);

    draw_string(1, 19, timestr);

    memcpy(led_buffer, demo_buf, WIDTH * HEIGHT * 3);

    demo_frame++;
}

void demoTask(void *tmp)
{
    // wait a bit before taking over screen
    sleep_ms(3000);

    while (true)
    {
        life_step_and_render();
        vTaskDelay(32);
    }
}

void demo_init()
{
    memset(&life_frame, 0, WIDTH * HEIGHT);
    memset(&life_tempbuf, 0, WIDTH * HEIGHT);
    memset(&demo_buf, 0, 3 * WIDTH * HEIGHT);

    uint32_t T = time_us_32();

    demo_nextfxtime = T + 2000 * 1000;
    demo_nextupdatetime = 0;
    demo_frame = 0;

    xTaskCreate(demoTask, "demo", 2048, NULL, tskIDLE_PRIORITY + 3, NULL);
}