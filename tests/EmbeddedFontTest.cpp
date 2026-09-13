#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "graphics/EmbeddedFont.h"
#include "graphics/GFXFontsAdapter.h"
#include "graphics/Utf8Decoder.h"

using embedded::Utf8Decoder;
namespace fonts = embedded::fonts;

namespace
{
std::vector<std::uint16_t> decode(std::string_view text)
{
    std::vector<std::uint16_t> result;
    Utf8Decoder decoder(text);
    while (auto codePoint = decoder.next())
    {
        result.push_back(*codePoint);
    }
    return result;
}

void expectDecode(const std::string &text, std::initializer_list<std::uint16_t> expected)
{
    const auto result = decode(text);
    ASSERT_EQ(result.size(), expected.size());
    std::size_t i = 0;
    for (const auto e : expected)
    {
        EXPECT_EQ(result[i++], e);
    }
}

// A tiny two-run test font:
//   run A: 0x0020..0x0022 -> glyph indices 0..2
//   run B: 0x0420..0x0421 -> glyph indices 3..4
const std::uint8_t kBitmaps[] = {
    0x01, // glyph0 (0x20)
    0x02, 0x04, // glyph1 (0x21)
    0x08, 0x10, 0x20, // glyph2 (0x22)
    0x40, // glyph3 (0x420)
    0x80, 0x00 // glyph4 (0x421)
};

const fonts::GlyphDescriptor kGlyphs[] = {
    {0, {1, 1}, 7, {0, 0}},
    {1, {2, 8}, 10, {1, 0}},
    {3, {3, 8}, 11, {2, 0}},
    {6, {1, 1}, 12, {3, 0}},
    {7, {2, 8}, 13, {4, 0}},
};

const fonts::CodePointBlock kCodePointBlocks[] = {
    {0x0020, 0x0022, 0},
    {0x0420, 0x0421, 3},
};

const fonts::FontDescriptor kFont = {
    {kBitmaps, sizeof(kBitmaps) / sizeof(kBitmaps[0])},
    {kGlyphs, 5},
    {kCodePointBlocks, 2}
};
} // namespace

TEST(Utf8Decoder, Ascii)
{
    expectDecode("Hello", {0x48, 0x65, 0x6C, 0x6C, 0x6F});
}

TEST(Utf8Decoder, TwoByte)
{
    expectDecode("\xC3\xA9", {0xE9}); // 'é' U+00E9
}

TEST(Utf8Decoder, ThreeByte)
{
    expectDecode("\xD0\xA1", {0x421}); // Cyrillic 'С' U+0421
}

TEST(Utf8Decoder, ValidThreeByte)
{
    expectDecode("\xE4\xB8\xAD", {0x4E2D}); // CJK '中' U+4E2D
}

TEST(Utf8Decoder, OverlongThreeByteIsSkipped)
{
    expectDecode("\xE0\x80\x80", {}); // overlong U+0000
}

TEST(Utf8Decoder, SurrogateThreeByteIsSkipped)
{
    expectDecode("\xED\xA0\x80", {}); // U+D800 surrogate
}

TEST(Utf8Decoder, FourByteIsSkipped)
{
    expectDecode("\xF0\x9F\x98\x80", {}); // U+1F600, out of BMP
}

TEST(Utf8Decoder, FourByteUpperRangeIsSkipped)
{
    expectDecode("\xF4\x8F\x80\x80", {}); // U+10F000, out of BMP
}

TEST(Utf8Decoder, StrayContinuationByte)
{
    expectDecode("A" "\x80" "B", {0x41, 0x42});
}

TEST(Utf8Decoder, TruncatedSequence)
{
    expectDecode("\xC3", {});
}

TEST(Utf8Decoder, DanglingLeadKeepsFollowingBytes)
{
    // 0xC3 is a 2-byte lead but the next byte is 'A', not a continuation:
    // the lead is skipped and 'A'/'B' are still decoded.
    expectDecode("\xC3" "A" "B", {0x41, 0x42});
}

TEST(Utf8Decoder, ThreeByteLeadWithBadSecondByte)
{
    // 0xE0 0x41 0x80: 0x41 is not a continuation, so the 0xE0 lead is skipped
    // and 'A' is still decoded (the stray 0x80 is dropped).
    expectDecode("\xE0" "A" "\x80", {0x41});
}

TEST(Utf8Decoder, ThreeByteLeadWithBadThirdByte)
{
    // 0xE0 0x9F 0x41: 0x9F is a valid continuation but 0x41 is not, so the
    // sequence is skipped and 'A' is still decoded.
    expectDecode("\xE0" "\x9F" "A", {0x41});
}

TEST(Utf8Decoder, FourByteLeadKeepsFollowingByte)
{
    // 0xF0 0x9F 0x98 0x41: a non-BMP 4-byte lead whose fourth byte is 'A';
    // the sequence is skipped and 'A' is still decoded.
    expectDecode("\xF0" "\x9F" "\x98" "A", {0x41});
}

TEST(Utf8Decoder, Empty)
{
    expectDecode("", {});
}

TEST(EmbeddedFont, LookupInRun)
{
    fonts::EmbeddedFont font{kFont};
    const auto glyph = font.getBitmap(0x0021);
    EXPECT_EQ(glyph.bitmap.size(), 2u);
    EXPECT_EQ(glyph.bitmap[0], 0x02);
    EXPECT_EQ(glyph.bitmap[1], 0x04);
    EXPECT_EQ(glyph.effectiveSize.width, 2);
    EXPECT_EQ(glyph.effectiveSize.height, 8);
    EXPECT_EQ(glyph.width, 10);
}

TEST(EmbeddedFont, LookupInSecondRun)
{
    fonts::EmbeddedFont font{kFont};
    const auto glyph = font.getBitmap(0x0420);
    EXPECT_EQ(glyph.bitmap.size(), 1u);
    EXPECT_EQ(glyph.bitmap[0], 0x40);
    EXPECT_EQ(glyph.width, 12);
}

TEST(EmbeddedFont, LastGlyphBitmapEndsAtBufferEnd)
{
    fonts::EmbeddedFont font{kFont};
    const auto glyph = font.getBitmap(0x0421);
    EXPECT_EQ(glyph.bitmap.size(), 2u);
    EXPECT_EQ(glyph.bitmap[0], 0x80);
    EXPECT_EQ(glyph.bitmap[1], 0x00);
}

TEST(EmbeddedFont, GapFallsBackToFirstGlyph)
{
    fonts::EmbeddedFont font{kFont};
    const auto glyph = font.getBitmap(0x0100); // between the two runs
    EXPECT_EQ(glyph.bitmap.size(), 1u);
    EXPECT_EQ(glyph.bitmap[0], 0x01);
    EXPECT_EQ(glyph.effectiveSize.width, 1);
}

TEST(EmbeddedFont, BelowAllCodePointBlocksFallsBackToFirstGlyph)
{
    fonts::EmbeddedFont font{kFont};
    const auto glyph = font.getBitmap(0x0005);
    EXPECT_EQ(glyph.bitmap.size(), 1u);
    EXPECT_EQ(glyph.bitmap[0], 0x01);
}

TEST(EmbeddedFont, TextBounds)
{
    fonts::EmbeddedFont font{kFont};
    const auto bounds = font.getTextBounds(" !");
    EXPECT_EQ(bounds.size.width, 3);
    EXPECT_EQ(bounds.size.height, 8);
    EXPECT_EQ(bounds.topLeft.x, 0);
    EXPECT_EQ(bounds.topLeft.y, 0);
}
