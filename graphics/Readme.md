# General Support Library - graphics

This library contains various graphics primitives, like `Point`, `Rect`, `Size`.
Also in contains a simple templated frame buffer and a canvas with GFX-compatible font support

## BaseGeometry.h

Contains `Point`, `Rect`, `Size` templated classes with basic operations.
Contains Matrix2x2 class to allow simple coordinate transformations.

## SimpleFrameBuffer.h

Contains `SimpleFrameBuffer` class, which is a continuous frame buffer with templated bits per pixel.
In provides the following operations:
* `clear` - fill the buffer with a given value
* `getSize` - return the size in pixels
* `drawPixel` - set a pixel at a given position
* `getImage` - get a view of the underlying buffer

It's used by `Canvas` class to store the image.

## Canvas.h

A templated class uses external frame buffer to draw on it.
It provides the following operations:
* `drawPixel` - set a pixel at a given position
* `drawLine` - draw a line from one point to another
* `drawRect` - draw a rectangle
* `drawCircle` - draw a circle
* 'drawCharAt' - draw a single character at a given position using GFX-compatible font
* 'drawStringAt' - draw a string at a given position using GFX-compatible font
* 'getImage' - get a view of the underlying buffer
* 'getSize' - get the size of the canvas with a given rotation
* 'getRotation' - get the current rotation
* 'setRotation' - set the current rotation
* 'clear' - clear the canvas with a given color
* 'setColor' - set the current color

## EmbeddedFont.h

Contains `EmbeddedFont` class, which is a simple wrapper around GFX-compatible font.
It provides the following operations:
* `getBitmap` - get the bitmap descriptor of a given character
* 'getTextBounds' - get the rectangle in which a given string fits.

## EmbeddedFonts(.h/.cpp)

For the fonts converted to the C++ headers used in application it declares and define a set of `EmbeddedFont` objects.
EmbeddedFonts.cpp contains an example font definition and can be used as a template for adding new fonts.
It maps the GFX font definition files to `EmbeddedFont` objects.
The font definition files are located in `GFXFonts` directory. Initially it contains the following fonts:
* `FreeSans15pt7b`

## GFXFontsAdapter.h

Contains font and glyph adapters for GFX-compatible fonts.
It can be initialized from the embedded binary data or with the arrays from the generated C++ header
