#include "Delays.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include <esp_sleep.h>

void embedded::ligntSleep(uint32_t milliseconds)
{
    uint32_t tickstart = getMillisecondTicks();
    uint32_t wait = tickstart + milliseconds;

    while ((getMillisecondTicks() < wait))
    {
        esp_sleep_enable_timer_wakeup((wait - getMillisecondTicks()) * 1000);
        if (esp_light_sleep_start() != ESP_OK)
        {
            auto current = getMillisecondTicks();
            if (wait > current)
            {
                delay(wait - current);
            }
        }
    }
}

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

void embedded::deepSleep(uint32_t milliseconds)
{
    esp_sleep_enable_timer_wakeup(milliseconds * 1000ull);
    esp_deep_sleep_start();
}

uint64_t embedded::getMicrosecondTicks()
{
    return static_cast<uint64_t>(esp_timer_get_time());
}
