#pragma once
#include "Delays.h"

#include <esp_sleep.h>

namespace embedded
{

void ligntSleep(uint32_t milliseconds)
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


void deepSleep(uint32_t milliseconds)
{
    esp_sleep_enable_timer_wakeup(milliseconds * 1000ull);
    esp_deep_sleep_start();
}

}