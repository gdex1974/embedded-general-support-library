#pragma once

#include "MemoryView.h"
#include "BaseGeometry.h"

class FrameBufferBase
{
public:
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual embedded::BytesView getImage() = 0;
    virtual embedded::ConstBytesView getImage() const = 0;
    virtual void clear(uint32_t color) = 0;
    virtual void drawPixel(int posX, int posY) = 0;
    virtual void setColor(uint32_t color) = 0;
};
