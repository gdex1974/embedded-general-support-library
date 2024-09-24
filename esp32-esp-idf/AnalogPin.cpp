#include "../AnalogPin.h"
#include "GpioPinDefinition.h"

#include <esp_err.h>
#include <soc/adc_periph.h>
#include <driver/adc.h>

#include <tuple>

namespace
{
std::tuple<int8_t, int8_t> digitalPinToAnalogChannel(uint8_t pin)
{
    if (pin < SOC_GPIO_PIN_COUNT)
    {
        for (uint8_t i = 0; i < SOC_ADC_PERIPH_NUM; i++)
        {
            for (uint8_t j = 0; j < SOC_ADC_MAX_CHANNEL_NUM; j++)
            {
                if (adc_channel_io_map[i][j] == pin)
                {
                    return { i, j };
                }
            }
        }
    }
    return {-1, -1 };
}

int singleReadADC1(int8_t channel)
{
    int result = -1;
    adc1_config_width(ADC_WIDTH_BIT_12);
    if (adc1_config_channel_atten((adc1_channel_t)channel, ADC_ATTEN_DB_12) == ESP_OK)
    {
        result = adc1_get_raw((adc1_channel_t)channel);
    }
    return result;
}

int singleReadADC2(int8_t channel)
{
    int result = -1;
    if (adc2_config_channel_atten((adc2_channel_t)channel, ADC_ATTEN_DB_12) == ESP_OK)
    {
        adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &result);
    }
    return result;
}
}

int embedded::AnalogPin::singleRead() const
{
    auto [adc, channel] = digitalPinToAnalogChannel(gpioPin.pin);
    int result = -1;
#if __GNUC__ < 9
    adc_power_acquire();
#endif
    switch (adc)
    {
        case 0:
            result = singleReadADC1(channel);
            break;
        case 1:
            result = singleReadADC2(channel);
            break;
        default:
            break;
    };
#if __GNUC__ < 9
    adc_power_release();
#endif
    return result;
}
