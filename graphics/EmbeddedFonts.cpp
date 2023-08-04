#include "GFXFontsAdapter.h"
#include "EmbeddedFont.h"

#ifndef PROGMEM
#define PROGMEM
#endif

namespace
{

using GFXfont = embedded::fonts::FontDescriptor;
using GFXglyph = embedded::fonts::GlyphDescriptor;

#include "GFXFonts/FreeSans15pt7b.h"
// Add more fonts here
}

namespace embedded::fonts
{
    EmbeddedFont FreeSans15pt {FreeSans15pt7b};
    // Add more fonts here
}