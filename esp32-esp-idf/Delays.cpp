#include "Delays.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include <esp_timer.h>

void embedded::delay(uint32_t milliseconds)
{
    vTaskDelay(milliseconds / portTICK_PERIOD_MS);
}

uint32_t embedded::getMillisecondTicks()
{
    return esp_timer_get_time() / 1000;
}

void embedded::delayMicroseconds(uint32_t microseconds)
{
    if (microseconds){
        auto current = static_cast<uint64_t>(esp_timer_get_time());
        uint64_t expected = current + microseconds;
        if (expected < current)
        {
            while(static_cast<uint64_t>(esp_timer_get_time()) > expected);
        }
        while(static_cast<uint64_t>(esp_timer_get_time()) < expected);
    }
}

uint64_t embedded::getMicrosecondTicks()
{
    return static_cast<uint64_t>(esp_timer_get_time());
}
