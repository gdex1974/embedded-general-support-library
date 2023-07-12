#include "esp32-hal.h"
#include "Delays.h"

void embedded::ligntSleep(uint32_t milliseconds)
{
    uint32_t tickstart = ::millis();
    uint32_t wait = tickstart + milliseconds;

    while ((::millis() < wait))
    {
        esp_sleep_enable_timer_wakeup((wait - millis()) * 1000);
        if (esp_light_sleep_start() != ESP_OK)
        {
            auto current = millis();
            if (wait > current)
            {
                delay(wait - current);
            }
        }
    }
}

void embedded::delay(uint32_t milliseconds)
{
    ::delay(milliseconds);
}

uint32_t embedded::getMillisecondTicks()
{
    return ::millis();
}

void embedded::delayMicroseconds(uint32_t microseconds)
{
    ::delayMicroseconds(microseconds);
}
