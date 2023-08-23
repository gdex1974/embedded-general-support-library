#include "../GpioDigitalPin.h"

#include "GpioPinDefinition.h"

#include <driver/gpio.h>

namespace embedded
{

void GpioDigitalPin::set() const
{
    gpio_set_level((gpio_num_t)gpioPin.pin, 1);
}

void GpioDigitalPin::reset() const
{
    gpio_set_level((gpio_num_t)gpioPin.pin, 0);
}

bool GpioDigitalPin::check() const
{
    return gpio_get_level((gpio_num_t)gpioPin.pin) != 0;
}

bool GpioDigitalPin::init(GpioDigitalPin::Direction type, PullMode pullMode) const
{
    if (!GPIO_IS_VALID_GPIO(gpioPin.pin)) {
        return false;
    }

    gpio_config_t conf = {
            .pin_bit_mask = (1ULL<<gpioPin.pin),
            .mode = (type == Direction::Input ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT),
            .pull_up_en = pullMode == PullMode::Up ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
            .pull_down_en = pullMode == PullMode::Down ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
    };

    return gpio_config(&conf) == ESP_OK;
}

}