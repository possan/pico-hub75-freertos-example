#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "all.h"

static char freertosstats[2000] = {
    0,
};

uint32_t wifi_connected_time = 0;

void debugTask(void *tmp)
{
    while (true)
    {
        vTaskDelay(10000);

        vTaskGetRunTimeStatistics((char *)&freertosstats, 2000);
        portYIELD();

        printf("FreeRTOS Stats:\n%s\n", freertosstats);
    }
}

void vLaunch( void) {
    vTaskStartScheduler();
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    printf("Sign started.\n");

    leds_init();

    TaskHandle_t task;
    xTaskCreate(debugTask, "debug", 2048, NULL, tskIDLE_PRIORITY + 1, &task);
    // vTaskCoreAffinitySet(task, 1 << 0);

    //
    // draw some test graphics on panel
    //

    for (int k = 0; k < WIDTH; k++)
    {
        int b = 255 - ((k * 255) / WIDTH);

        led_buffer[BUF_OFFSET(k, 0) + 0] = b;
        led_buffer[BUF_OFFSET(k, 1) + 0] = b;
        led_buffer[BUF_OFFSET(k, 2) + 0] = b;

        led_buffer[BUF_OFFSET(k, 3) + 1] = b;
        led_buffer[BUF_OFFSET(k, 4) + 1] = b;
        led_buffer[BUF_OFFSET(k, 5) + 1] = b;

        led_buffer[BUF_OFFSET(k, 6) + 2] = b;
        led_buffer[BUF_OFFSET(k, 7) + 2] = b;
        led_buffer[BUF_OFFSET(k, 8) + 2] = b;

        led_buffer[BUF_OFFSET(k, 9) + 0] = b;
        led_buffer[BUF_OFFSET(k, 9) + 1] = b;
        led_buffer[BUF_OFFSET(k, 9) + 2] = b;

        led_buffer[BUF_OFFSET(k, 10) + 0] = b;
        led_buffer[BUF_OFFSET(k, 10) + 1] = b;
        led_buffer[BUF_OFFSET(k, 10) + 2] = b;

        led_buffer[BUF_OFFSET(k, 11) + 0] = b;
        led_buffer[BUF_OFFSET(k, 11) + 1] = b;
        led_buffer[BUF_OFFSET(k, 11) + 2] = b;
    }

    demo_init();
    wifi_init();

    const char *rtos_name;
#if ( configNUMBER_OF_CORES > 1 )
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if ( configNUMBER_OF_CORES == 2 )
    printf("Starting %s on both cores:\n", rtos_name);
    vLaunch();
#elif ( RUN_FREERTOS_ON_CORE == 1 )
    printf("Starting %s on core 1:\n", rtos_name);
    multicore_launch_core1(vLaunch);
    while (true);
#else
    printf("Starting %s on core 0:\n", rtos_name);
    vLaunch();
#endif
    return 0;
}
