#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "all.h"

uint8_t led_buffer[WIDTH * HEIGHT * 3] = {
    0,
};

uint8_t output[WIDTH * HEIGHT * 3] = {
    0,
};

void ledTask(void *tmp)
{
    int cnt = 0;
    // int bri = 0;

    while (1)
    {
        memcpy(output, led_buffer, WIDTH * HEIGHT * 3);

        // bri = cnt % 2;
        int thresh = 128;
        // int oe_time = 15;

        // // if (bri == 0)
        // // {
        //     thresh = 64;
        //     oe_time = 1;
        //     // thresh = 16;
        //     // oe_time = 1;
        // }

        // if (bri == 1)
        // {
        //     thresh = 192;
        //     oe_time = 256;
        // }

        // if (bri == 1)
        // {
        //     thresh = 128;
        //     oe_time = 16;
        // }

        for (int scanline = 0; scanline < 16; scanline++)
        {
            for (int panel = 0; panel < ROWS_OF_PANELS; panel++)
            {
                int y = scanline + PANELHEIGHT * panel;
                int o1 = ((y + 0) * WIDTH) * 3;
                int o2 = ((y + 16) * WIDTH) * 3;

                for (int k = 0; k < WIDTH; k++)
                {
                    gpio_put(R1_PIN, output[o1++] > thresh);
                    gpio_put(R2_PIN, output[o2++] > thresh);

                    gpio_put(G1_PIN, output[o1++] > thresh);
                    gpio_put(G2_PIN, output[o2++] > thresh);

                    gpio_put(B1_PIN, output[o1++] > thresh);
                    gpio_put(B2_PIN, output[o2++] > thresh);

                    gpio_put(CLOCK_PIN, 1);
                    sleep_us(1);
                    gpio_put(CLOCK_PIN, 0);
                }
            }

            gpio_put(OE_PIN, 1);

            gpio_put(ADDR0_PIN, (scanline & 1));
            gpio_put(ADDR1_PIN, (scanline & 2));
            gpio_put(ADDR2_PIN, (scanline & 4));
            gpio_put(ADDR3_PIN, (scanline & 8));

            gpio_put(LATCH_PIN, 1);
            sleep_us(1);
            gpio_put(LATCH_PIN, 0);
            sleep_us(1);

            gpio_put(OE_PIN, 0);

            // if (oe_time > 0)
            // {
            //     sleep_us(oe_time);
            // }
            // else
            // {
            //     __asm volatile("nop\n");
            // }

            vTaskDelay(1); //  / portTICK_PERIOD_MS) ;//  / portTICK_PERIOD_MS); // oe_time);
            // taskYIELD();
        }

        cnt++;
    }
}

void leds_init()
{
    for (int k = 0; k < WIDTH * HEIGHT * 3; k++)
    {
        led_buffer[k] = 0;
        output[k] = 0;
    }

    // multicore_launch_core1(core1_entrypoint); // This runs outside of FreeRTOS

    gpio_init(ADDR0_PIN);
    gpio_init(ADDR1_PIN);
    gpio_init(ADDR2_PIN);
    gpio_init(ADDR3_PIN);
    gpio_init(CLOCK_PIN);
    gpio_init(LATCH_PIN);
    gpio_init(OE_PIN);
    gpio_init(R1_PIN);
    gpio_init(R2_PIN);
    gpio_init(G1_PIN);
    gpio_init(G2_PIN);
    gpio_init(B1_PIN);
    gpio_init(B2_PIN);

    gpio_set_dir(ADDR0_PIN, GPIO_OUT);
    gpio_set_dir(ADDR1_PIN, GPIO_OUT);
    gpio_set_dir(ADDR2_PIN, GPIO_OUT);
    gpio_set_dir(ADDR3_PIN, GPIO_OUT);
    gpio_set_dir(CLOCK_PIN, GPIO_OUT);
    gpio_set_dir(LATCH_PIN, GPIO_OUT);
    gpio_set_dir(OE_PIN, GPIO_OUT);
    gpio_set_dir(R1_PIN, GPIO_OUT);
    gpio_set_dir(R2_PIN, GPIO_OUT);
    gpio_set_dir(G1_PIN, GPIO_OUT);
    gpio_set_dir(G2_PIN, GPIO_OUT);
    gpio_set_dir(B1_PIN, GPIO_OUT);
    gpio_set_dir(B2_PIN, GPIO_OUT);

    TaskHandle_t task;
    xTaskCreate(ledTask, "leds", 4096, NULL, tskIDLE_PRIORITY + 10, &task);
    // vTaskSetAffinity(task, 1 << 1);
}
