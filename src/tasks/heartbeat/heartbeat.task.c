#include "inttypes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "include/status.h"
#include "heartbeat.task.h"

#define LED_GPIO 18

static void blinkTimes(int count)
{
    int time = (count > 1 ? 200 : 50) / portTICK_PERIOD_MS;
    for (int i = 0; i < count; i++)
    {
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(time);
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(time);
    }
}

void vTaskHeartbeat(void *pvParameter)
{
    gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    while(1) {
        uint32_t state = xEventGroupGetBits(xAppStateEventGroup);
        if (state & statusCONNECTED_TO_SERVER) {
            blinkTimes(1);
        } else {
            gpio_set_level(LED_GPIO, 1);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
