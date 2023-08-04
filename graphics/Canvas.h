#pragma once

#include "graphics/FrameBufferBase.h"
#include "graphics/BaseGeometry.h"
#include "MemoryView.h"

namespace embedded::fonts
{
class EmbeddedFont;
}

class Canvas
{
public:
    enum class Rotation { Rotation0, Rotation90, Rotation180, Rotation270 };

    using Point = embedded::Point<int>;
    using Rect = embedded::Rect<int>;
    using Size = embedded::Size<int>;
    using Matrix = embedded::Matrix2x2<int>;

    explicit Canvas(FrameBufferBase &frameBuffer)
    : frameBuffer(frameBuffer)
    , size({frameBuffer.getWidth(), frameBuffer.getHeight()})
    {
        rotate = Rotation::Rotation0;
    }

    void setColor(uint32_t foregroundColor)
    {
        frameBuffer.setColor(foregroundColor);
    }

    void clear(uint32_t backgroundColor)
    {
        frameBuffer.clear(backgroundColor);
    }

    Size getSize() const
    {
        switch (getRotation())
        {
            case Rotation::Rotation90:
            case Rotation::Rotation270:
                return { size.height, size.width };
            default:
                break;
        }
        return size;
    }

    Rotation getRotation() const { return rotate; }
    void setRotation(Rotation rotation) { rotate = rotation; }

    embedded::ConstBytesView getImage() const { return frameBuffer.getImage(); }

    void drawPixel(Point pos)
    {
        setIfVisible(transformCoords(pos));
    }

    int drawCharAt(Point pos, char ascii_char, const embedded::fonts::EmbeddedFont &font);
    void drawStringAt(Point topLeft, std::string_view text, const embedded::fonts::EmbeddedFont &font)
    {
        for (auto symbol: text)
        {
            topLeft.x += drawCharAt(topLeft, symbol, font);
        }
    }

    void drawLine(Point start, Point end);
    void drawRectangle(Rect rect);
    void drawFilledRectangle(Rect rect);
    void drawCircle(Point center, int radius, bool filled = false);

private:
    void drawHorizontalLine(Point startPos, int width);
    void drawVerticalLine(Point startPos, int height);
    Point transformCoords(Point pos) const;
    Rect transformCoords(Rect rect) const;
    Matrix getTransformMatrix() const;
    Size getShifts() const;

    void setIfVisible(const Point &point)
    {
        if (point.x >= 0 && point.x < size.width && point.y >= 0 && point.y < size.height)
        {
            frameBuffer.drawPixel(point.x, point.y);
        }
    }

    FrameBufferBase &frameBuffer;
    Size size;
    Rotation rotate;
};


