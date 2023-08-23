#pragma once

#include <cstdint>

namespace embedded
{

struct I2CBus
{
    explicit I2CBus(int busNum) : bus(busNum) {}
    bool init(int sdaPin, int sclPin, uint32_t frequency, bool pullupSda = true, bool pullupScl = true) const;
    void resetBus() const;

    int bus;
};

}
