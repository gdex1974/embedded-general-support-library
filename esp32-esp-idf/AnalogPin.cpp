#include "../AnalogPin.h"
#include "GpioPinDefinition.h"

#include <esp_err.h>
#include <soc/adc_periph.h>

#include <tuple>
#include <optional>
#include <esp_adc/adc_oneshot.h>

namespace
{
constexpr std::optional<std::tuple<int, adc_channel_t>> digitalPinToAnalogChannel(uint8_t pin)
{
    if (pin < SOC_GPIO_PIN_COUNT)
    {
        for (uint8_t i = 0; i < SOC_ADC_PERIPH_NUM; i++)
        {
            for (uint8_t j = 0; j < SOC_ADC_MAX_CHANNEL_NUM; j++)
            {
                if (adc_channel_io_map[i][j] == pin)
                {
                    return std::make_tuple(i, static_cast<adc_channel_t>(j));
                }
            }
        }
    }
    return std::nullopt;
}

int singleReadADC(const adc_unit_t unit, adc_channel_t channel)
{
    int result = -1;
    adc_oneshot_unit_handle_t handle;
    adc_oneshot_unit_init_cfg_t cfg =
    {
        .unit_id = unit,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    if (adc_oneshot_new_unit(&cfg, &handle) == ESP_OK)
    {
        adc_oneshot_chan_cfg_t chanCfg =
        {
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_12,
        };
        if (adc_oneshot_config_channel(handle, channel, &chanCfg) == ESP_OK)
        {
            if (adc_oneshot_read(handle, channel, &result) != ESP_OK)
            {
                return -1;
            }
        }
    }
    return result;
}

}

int embedded::AnalogPin::singleRead() const
{
    auto mapping = digitalPinToAnalogChannel(gpioPin.pin);
    if (!mapping)
    {
        return -1;
    }
    const auto [adc, channel] = *mapping;
    int result = -1;
#if __GNUC__ < 9
    adc_power_acquire();
#endif
    switch (adc)
    {
        case 0:
            result = singleReadADC(ADC_UNIT_1, channel);
            break;
        case 1:
            result = singleReadADC(ADC_UNIT_2, channel);
            break;
        default:
            break;
    };
#if __GNUC__ < 9
    adc_power_release();
#endif
    return result;
}
