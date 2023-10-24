#include "Canvas.h"
#include "graphics/EmbeddedFont.h"
#include <algorithm>

int Canvas::drawCharAt(Point pos, const char ascii_char, const embedded::fonts::EmbeddedFont &font)
{
    auto matrix = getTransformMatrix();
    auto shifts = getShifts();
    auto glyph = font.getBitmap(ascii_char);
    auto ptr = glyph.bitmap.begin();
    pos.y += glyph.offset.height;
    pos.x += glyph.offset.width;
    for (int j = 0; j < glyph.effectiveSize.height; ++j)
    {
        for (int i = 0; i < glyph.effectiveSize.width; ++i)
        {
            if ((*ptr) & (0x80 >> ((i + j * glyph.effectiveSize.width) % 8)))
            {
                const auto absoluteCoords = matrix * (pos + Size { i, j }) + shifts;
                setIfVisible(absoluteCoords);
            }
            if ((i + j * glyph.effectiveSize.width) % 8 == 7)
            {
                ++ptr;
            }
        }
    }
    return glyph.width;
}

void Canvas::drawLine(Point start, Point end)
{
    start = transformCoords(start);
    end = transformCoords(end);

    auto diffX = std::abs(end.x - start.x);
    auto diffY = std::abs(end.y - start.y);

    if (diffX == 0)
    {
        drawVerticalLine({ start.x, std::min(start.y, end.y) }, diffY + 1);
        return;
    }

    if (diffY == 0)
    {
        drawHorizontalLine({ std::min(start.x, end.x), start.y }, diffX + 1);
        return;
    }

    // Bresenham algorithm

    const bool moveY = diffY > diffX;
    // we want to move along the longer axis
    // so we make the aliases based on the largest difference
    auto &shortCoord = moveY ? start.x : start.y;
    auto &longCoord = moveY ? start.y : start.x;
    const auto &shortMax = moveY ? end.x : end.y;
    const auto &longMax = moveY ? end.y : end.x;
    const auto &shortDiff = moveY ? diffX : diffY;
    const auto &longDiff = moveY ? diffY : diffX;

    if (longCoord > longMax)
    {
        std::swap(start, end);
    }
    auto err = longDiff / 2;
    auto sign = shortCoord < shortMax ? 1 : -1;
    while (longCoord <= longMax)
    {
        setIfVisible(start);
        err -= shortDiff;
        if (err < 0)
        {
            shortCoord += sign;
            err += longDiff;
        }
        ++longCoord;
    }
}

void Canvas::drawHorizontalLine(Point startPos, const int line_width)
{
    for (int i = 0; i < line_width; ++i, ++startPos.x)
    {
        setIfVisible(startPos);
    }
}

void Canvas::drawVerticalLine(Point startPos, const int line_height)
{
    for (int i = 0; i < line_height; ++i, ++startPos.y)
    {
        setIfVisible(startPos);
    }
}

void Canvas::drawRectangle(Rect rect)
{
    rect = transformCoords(rect);
    drawHorizontalLine(rect.topLeft, rect.size.width);
    drawHorizontalLine(rect.bottomLeft(), rect.size.width);
    drawVerticalLine(rect.topLeft, rect.size.height);
    drawVerticalLine(rect.topRight(), rect.size.height);
}

void Canvas::drawFilledRectangle(Rect rect)
{
    rect = transformCoords(rect);
    auto topLeft = rect.topLeft;
    auto bottomRight = rect.bottomRight();
    topLeft.x = std::max(topLeft.x, 0);
    topLeft.y = std::max(topLeft.y, 0);
    bottomRight.x = std::min(bottomRight.x, size.width - 1);
    bottomRight.y = std::min(bottomRight.y, size.height - 1);

    const auto width = bottomRight.x - topLeft.x + 1;
    while (topLeft.y <= bottomRight.y)
    {
        drawHorizontalLine(topLeft, width);
        ++topLeft.y;
    }
}

void Canvas::drawCircle(Point center, int radius, bool filled)
{
    center = transformCoords(center);
    // Bresenham algorithm
    int xPos = -radius;
    int yPos = 0;
    int err = 2 - 2 * radius;

    do
    {
        setIfVisible(center + Size { -xPos, yPos });
        setIfVisible(center + Size { xPos, yPos });
        setIfVisible(center + Size { xPos, -yPos });
        setIfVisible(center + Size { -xPos, -yPos });
        if (filled)
        {
            drawHorizontalLine(center + Size { xPos, yPos }, 2 * (-xPos) + 1);
            drawHorizontalLine(center + Size { xPos, -yPos }, 2 * (-xPos) + 1);
        }
        int e2 = err;
        if (e2 <= yPos)
        {
            err += ++yPos * 2 + 1;
            if (-xPos == yPos && e2 <= xPos)
            {
                e2 = 0;
            }
        }
        if (e2 > xPos)
        {
            err += ++xPos * 2 + 1;
        }
    } while (xPos <= 0);
}

Canvas::Point Canvas::transformCoords(Point pos) const
{
    //return getTransformMatrix()*pos + getShifts();
    switch (rotate)
    {
        case Rotation::Rotation90:
            return { size.width - 1 - pos.y, pos.x };
        case Rotation::Rotation180:
            return { size.width - 1 - pos.x, size.height - 1 - pos.y };
        case Rotation::Rotation270:;
            return { pos.y, size.height - 1 - pos.x };
        default:
            return { pos.x, pos.y };
    }
}

Canvas::Rect Canvas::transformCoords(Rect pos) const
{
    switch (rotate)
    {
        case Rotation::Rotation90:
            return { transformCoords(pos.bottomLeft()), Size { pos.size.height, pos.size.width } };
        case Rotation::Rotation180:
            return { transformCoords(pos.bottomRight()), pos.size };
        case Rotation::Rotation270:
            return { transformCoords(pos.topRight()), Size { pos.size.height, pos.size.width } };
        default:
            return pos;
    }
}

Canvas::Matrix Canvas::getTransformMatrix() const
{
    switch (rotate)
    {
        case Rotation::Rotation90:
            return { 0, -1, 1, 0 };
        case Rotation::Rotation180:
            return { -1, 0, 0, -1 };
        case Rotation::Rotation270:;
            return { 0, 1, -1, 0 };
        default:
            return { 1, 0, 0, 1 };
    }
}

Canvas::Size Canvas::getShifts() const
{
    switch (rotate)
    {
        case Rotation::Rotation90:
            return { size.width - 1, 0 };
        case Rotation::Rotation180:
            return { size.width - 1, size.height - 1 };
        case Rotation::Rotation270:;
            return { 0, size.height - 1 };
        default:
            return { 0, 0 };
    }
}
