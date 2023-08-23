#include "SpiBus.h"

#include "GpioPinDefinition.h"

#include <driver/spi_master.h>
#include <algorithm>

bool embedded::SpiBus::transfer(const uint8_t *sendBytes, uint8_t *receiveBytes, uint32_t length)
{
    spi_transaction_t transaction{};
    transaction.user= nullptr;
    while (length > 0)
    {
        const auto effectiveLength = std::min(length , maxTransferSize);
        if (sendBytes != nullptr)
        {
            transaction.tx_buffer = sendBytes;
            transaction.length = 8 * effectiveLength;
        }
        if (receiveBytes != nullptr)
        {
            transaction.rx_buffer = receiveBytes;
            transaction.rxlength = 8 * effectiveLength;
        }
        if (spi_device_polling_transmit(spiDevice, &transaction) != ESP_OK)
        {
            return false;
        }
        length -= effectiveLength;
        if (sendBytes != nullptr)
        {
            sendBytes += effectiveLength;
        }
        if (receiveBytes != nullptr)
        {
            receiveBytes += effectiveLength;
        }
    }

    return true;
}

bool embedded::SpiBus::init(embedded::GpioPinDefinition &sck,
                            embedded::GpioPinDefinition &miso,
                            embedded::GpioPinDefinition &mosi,
                            uint32_t frequency,
                            embedded::SpiBus::DuplexMode duplexMode)
{
    maxTransferSize = LLDESC_MAX_NUM_PER_DESC;
    spi_bus_config_t buscfg={};
    buscfg.mosi_io_num = mosi.pin;
    buscfg.miso_io_num = miso.pin;
    buscfg.sclk_io_num = sck.pin;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = static_cast<int>(maxTransferSize);
    buscfg.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_NATIVE_PINS;

    if (spi_bus_initialize(static_cast<spi_host_device_t>(spiBusNumber), &buscfg, SPI_DMA_CH_AUTO) != ESP_OK)
    {
        return false;
    }

    spi_device_interface_config_t devcfg={};
    devcfg.mode=0;
    devcfg.clock_speed_hz=static_cast<int>(frequency);
    devcfg.spics_io_num=-1;
    devcfg.queue_size=7;
    if (duplexMode == DuplexMode::Half)
    {
        devcfg.flags |= SPI_DEVICE_HALFDUPLEX;
    }

    return spi_bus_add_device(static_cast<spi_host_device_t>(spiBusNumber), &devcfg, &spiDevice) == ESP_OK;
}

bool embedded::SpiBus::release()
{
    if (spiDevice != nullptr)
    {
        if (spi_bus_remove_device(spiDevice))
        {
            spiDevice = nullptr;
            return spi_bus_free(static_cast<spi_host_device_t>(spiBusNumber)) == ESP_OK;
        }
        return false;
    }
    return true;
}

embedded::SpiBus::~SpiBus()
{
    release();
}
