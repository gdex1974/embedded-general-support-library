#include "../I2CDevice.h"
#include "I2CBus.h"

#include <freertos/FreeRTOS.h>
#include <driver/i2c.h>

bool embedded::I2CDevice::sendSync(const uint8_t *data, uint16_t count) const
{
    return i2c_master_write_to_device(impl.bus, deviceAddress, data, count, timeout / portTICK_RATE_MS) == ESP_OK;
}

bool embedded::I2CDevice::receiveSync(uint8_t *data, uint16_t count) const
{
    return i2c_master_read_from_device(impl.bus, deviceAddress, data, count, timeout / portTICK_RATE_MS) == ESP_OK;
}

