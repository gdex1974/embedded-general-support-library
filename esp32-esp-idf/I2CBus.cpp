#include "I2CBus.h"
#include <driver/i2c.h>

void embedded::I2CBus::resetBus() const
{
    uint8_t data = 0x06;
    i2c_master_write_to_device(bus, 0, &data, sizeof(data), 200);
}

bool embedded::I2CBus::init(int sdaPin, int sclPin, uint32_t frequency, bool pullupSda, bool pullupScl) const
{
    i2c_config_t conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = sdaPin,
            .scl_io_num = sclPin,
            .sda_pullup_en = pullupSda,
            .scl_pullup_en = pullupScl,
            .master {.clk_speed = frequency},
            .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
    };

    return i2c_param_config(bus, &conf) == ESP_OK &&
        i2c_driver_install(bus, conf.mode, 0, 0, 0) == ESP_OK;
}
