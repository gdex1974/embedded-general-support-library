#pragma once

#include <cstdint>
namespace embedded
{

struct GpioPinDefinition;

class GpioDigitalPin
{
public:
    enum class Direction : uint8_t { Input, Output };
    enum class PullMode : uint8_t { Up, Down, None };

    explicit GpioDigitalPin(GpioPinDefinition &pin) : gpioPin(pin) {}

    bool init(Direction type = Direction::Output, PullMode pullMode = PullMode::None) const;
    void set() const;
    void reset() const;
    bool check() const;

    void set(bool level) const
    {
        if (level)
            set();
        else
            reset();
    }

private:
    GpioPinDefinition &gpioPin;
};


class ChipSelector
{
public:
    explicit ChipSelector(const GpioDigitalPin &p) : pin(p)
    {
        pin.reset();
    }

    ~ChipSelector()
    {
        pin.set();
    }

private:
    const GpioDigitalPin &pin;
};

}
