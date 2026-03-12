#pragma once

#include "FrameBufferBase.h"

template<int width, int height, uint8_t colorDepth = 1>
class SimpleFrameBuffer : public FrameBufferBase
{
public:
    explicit SimpleFrameBuffer() noexcept
    {
        static_assert(colorDepth == 1 || colorDepth == 2 || colorDepth == 4 || colorDepth == 8 || colorDepth == 16 || colorDepth == 24 || colorDepth == 32, "Invalid color depth");
        static_assert(width * (1 << (colorDepth - 1)) % 8 == 0, "Width must be byte-aligned");
        selectedColor = (1 << colorDepth) - 1;
    }
    int getWidth() const override { return effectiveWidth; }
    int getHeight() const override { return height; }
    embedded::BytesView getImage() override { return buffer; }
    embedded::ConstBytesView getImage() const override { return { std::begin(buffer), bufferSize }; }
    void clear(uint32_t color) override
    {
        for (int x = 0; x < effectiveWidth; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                drawPixel(x, y, color);
            }
        }
    }
    void drawPixel(int posX, int posY) override
    {
        drawPixel(posX, posY, selectedColor);
    }

    void setColor(uint32_t color) override
    {
        selectedColor = color & ((1 << colorDepth) - 1);
    }

    void drawPixel(int posX, int posY, uint32_t color)
    {
        auto pixelOffset = (posY * effectiveWidth + posX) * bitsPerPixel / 8;
        if constexpr (colorDepth == 1)
        {
            const auto mask = 0x80 >> (posX % 8);
            if (color != 0)
            {
                buffer[pixelOffset] |= mask;
            }
            else
            {
                buffer[pixelOffset] &= ~mask;
            }
        }
        else if constexpr (colorDepth == 2)
        {
            const auto mask = 0xC0 >> (posX % 4) * 2;
            buffer[pixelOffset] &= ~mask;
            buffer[pixelOffset] |= (color & 3) << (2 - (posX % 4) * 2);
        }
        else if constexpr (colorDepth == 4)
        {
            const auto mask = 0xF0 >> (posX % 2) * 4;
            buffer[pixelOffset] &= ~mask;
            buffer[pixelOffset] |= (color & 15) << (4 - (posX % 2) * 4);
        }
        else if constexpr (colorDepth == 8)
        {
            buffer[pixelOffset] = color;
        }
        else if constexpr (colorDepth == 16)
        {
            buffer[pixelOffset] = color >> 8;
            buffer[pixelOffset + 1] = color & 0xFF;
        }
        else if constexpr (colorDepth == 24)
        {
            buffer[pixelOffset] = color >> 16;
            buffer[pixelOffset + 1] = (color >> 8) & 0xFF;
            buffer[pixelOffset + 2] = color & 0xFF;
        }
        else if constexpr (colorDepth == 32)
        {
            buffer[pixelOffset] = color >> 24;
            buffer[pixelOffset + 1] = (color >> 16) & 0xFF;
            buffer[pixelOffset + 2] = (color >> 8) & 0xFF;
            buffer[pixelOffset + 3] = color & 0xFF;
        }
    }
private:
    static constexpr int bitsPerPixel = colorDepth;
    static constexpr int effectiveWidth = (width * bitsPerPixel / 8) * 8;
    static constexpr size_t bufferSize = effectiveWidth * height * bitsPerPixel / 8;
    std::array<uint8_t, bufferSize> buffer;
    uint32_t selectedColor;
};
