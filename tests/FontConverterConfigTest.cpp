#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <unistd.h>

#include "gfx-font-converter/FontConverterConfig.h"

namespace
{

// Builds a FontConverterConfig from a list of CLI arguments (without argv[0]).
//
// FontConverterConfig parses its arguments with getopt(3), which keeps its
// cursor in the process-global `optind`. Because a single test process
// constructs the config many times, we must reset that cursor before each
// construction. `optind = 1` is the portable reset: it points at the first
// option (skipping argv[0]) and works on both GNU and BSD getopt.
FontConverterConfig makeConfig(const std::vector<std::string> &args)
{
    optind = 1;

    std::vector<std::string> storage;
    storage.reserve(args.size() + 1);
    storage.push_back("font-converter-test"); // argv[0]
    for (const auto &a : args)
    {
        storage.push_back(a);
    }

    std::vector<char*> argv;
    argv.reserve(storage.size());
    for (auto &s : storage)
    {
        // getopt does not modify the argument strings in this usage (every
        // argument is an option or an option value), so the const_cast is safe.
        argv.push_back(const_cast<char*>(s.c_str()));
    }

    return FontConverterConfig(static_cast<int>(argv.size()), argv.data());
}

std::vector<std::uint16_t> codePoints(const std::vector<std::string> &args)
{
    return makeConfig(args).getCodePoints();
}

} // namespace

// ---------------------------------------------------------------------------
// Code point specification (-u)
// ---------------------------------------------------------------------------

TEST(FontConverterConfig, DefaultRangeWhenNoU)
{
    const auto cp = codePoints({ "-f", "x.ttf" });
    ASSERT_EQ(cp.size(), 0x7E - 0x20 + 1);
    EXPECT_EQ(cp.front(), 0x20);
    EXPECT_EQ(cp.back(), 0x7E);
}

TEST(FontConverterConfig, EmptyUStringFallsBackToDefault)
{
    const auto cp = codePoints({ "-f", "x.ttf", "-u", "" });
    ASSERT_EQ(cp.size(), 0x7E - 0x20 + 1);
    EXPECT_EQ(cp.front(), 0x20);
    EXPECT_EQ(cp.back(), 0x7E);
}

TEST(FontConverterConfig, SingleHexCodePoint)
{
    const auto cp = codePoints({ "-u", "0x41" });
    ASSERT_EQ(cp.size(), 1u);
    EXPECT_EQ(cp[0], 0x41);
}

TEST(FontConverterConfig, HexRange)
{
    const auto cp = codePoints({ "-u", "0x41-0x43" });
    ASSERT_EQ(cp.size(), 3u);
    EXPECT_EQ(cp[0], 0x41);
    EXPECT_EQ(cp[1], 0x42);
    EXPECT_EQ(cp[2], 0x43);
}

TEST(FontConverterConfig, DecimalInputUsesBaseZero)
{
    // Base-0 auto-detection: a plain number is decimal.
    const auto cp = codePoints({ "-u", "65" });
    ASSERT_EQ(cp.size(), 1u);
    EXPECT_EQ(cp[0], 65);
    EXPECT_EQ(cp[0], 0x41);
}

TEST(FontConverterConfig, MixedSinglesRangesAndDecimal)
{
    // 0x41, 0x43-0x44, and 66 (== 0x42) given as separate -u options.
    const auto cp = codePoints({ "-u", "0x41", "-u", "0x43-0x44", "-u", "66" });
    ASSERT_EQ(cp.size(), 4u);
    EXPECT_EQ(cp[0], 0x41);
    EXPECT_EQ(cp[1], 0x42);
    EXPECT_EQ(cp[2], 0x43);
    EXPECT_EQ(cp[3], 0x44);
}

TEST(FontConverterConfig, DuplicateCodePointsAreRemoved)
{
    // 0x42 appears both inside the range and as a separate -u option.
    const auto cp = codePoints({ "-u", "0x41-0x43", "-u", "0x42" });
    ASSERT_EQ(cp.size(), 3u);
    EXPECT_EQ(cp[0], 0x41);
    EXPECT_EQ(cp[1], 0x42);
    EXPECT_EQ(cp[2], 0x43);
}

TEST(FontConverterConfig, ReversedRangeIsNormalized)
{
    const auto cp = codePoints({ "-u", "0x43-0x41" });
    ASSERT_EQ(cp.size(), 3u);
    EXPECT_EQ(cp[0], 0x41);
    EXPECT_EQ(cp[1], 0x42);
    EXPECT_EQ(cp[2], 0x43);
}

TEST(FontConverterConfig, SurroundingWhitespaceIsTolerated)
{
    const auto cp = codePoints({ "-u", " 0x41 ", "-u", " 0x43 " });
    ASSERT_EQ(cp.size(), 2u);
    EXPECT_EQ(cp[0], 0x41);
    EXPECT_EQ(cp[1], 0x43);
}

TEST(FontConverterConfig, CommaIsNoLongerSupported)
{
    // The old comma-separated form was removed. A stray comma is rejected
    // outright instead of silently parsing only the first code point.
    EXPECT_THROW(makeConfig({ "-u", "0x41,0x42" }), std::runtime_error);
}

TEST(FontConverterConfig, MultipleUFlagsAreMerged)
{
    // Repeated -u options (even overlapping ranges) merge into one sorted set.
    const auto cp = codePoints({ "-u", "0x41-0x43", "-u", "0x42-0x44" });
    ASSERT_EQ(cp.size(), 4u);
    EXPECT_EQ(cp[0], 0x41);
    EXPECT_EQ(cp[1], 0x42);
    EXPECT_EQ(cp[2], 0x43);
    EXPECT_EQ(cp[3], 0x44);
}

TEST(FontConverterConfig, NonContiguousRangesArePreserved)
{
    const auto cp = codePoints({ "-u", "0x20-0x22", "-u", "0x40-0x41" });
    ASSERT_EQ(cp.size(), 5u);
    EXPECT_EQ(cp[0], 0x20);
    EXPECT_EQ(cp[1], 0x21);
    EXPECT_EQ(cp[2], 0x22);
    EXPECT_EQ(cp[3], 0x40);
    EXPECT_EQ(cp[4], 0x41);
}

TEST(FontConverterConfig, RangeEndingAtBmpMaxTerminates)
{
    // Exercises the loop bound at 0xFFFF (a naive uint16_t counter would wrap
    // and loop forever).
    const auto cp = codePoints({ "-u", "0xFFFE-0xFFFF" });
    ASSERT_EQ(cp.size(), 2u);
    EXPECT_EQ(cp[0], 0xFFFE);
    EXPECT_EQ(cp[1], 0xFFFF);
}

TEST(FontConverterConfig, BmpMaxBoundaryIsAccepted)
{
    const auto cp = codePoints({ "-u", "0xFFFF" });
    ASSERT_EQ(cp.size(), 1u);
    EXPECT_EQ(cp[0], 0xFFFF);
}

TEST(FontConverterConfig, OutsideBmpThrows)
{
    EXPECT_THROW(makeConfig({ "-u", "0x10000" }), std::runtime_error);
}

// ---------------------------------------------------------------------------
// Other options and defaults
// ---------------------------------------------------------------------------

TEST(FontConverterConfig, FontFileName)
{
    const auto config = makeConfig({ "-f", "myfont.ttf" });
    EXPECT_EQ(config.getFontFileName(), "myfont.ttf");
}

TEST(FontConverterConfig, OutputDirectory)
{
    const auto config = makeConfig({ "-o", "/tmp/out" });
    EXPECT_EQ(config.getOutputDirectory(), "/tmp/out");
}

TEST(FontConverterConfig, Dpi)
{
    EXPECT_EQ(makeConfig({ "-d", "96" }).getDPI(), 96);
    EXPECT_EQ(makeConfig({}).getDPI(), 141); // default
}

TEST(FontConverterConfig, FontSize)
{
    EXPECT_EQ(makeConfig({ "-s", "24" }).getFontSize(), 24);
    EXPECT_EQ(makeConfig({}).getFontSize(), 12); // default
}

TEST(FontConverterConfig, Format)
{
    EXPECT_EQ(makeConfig({ "-b" }).getFormat(), FontConverterConfig::Format::Binary);
    EXPECT_EQ(makeConfig({}).getFormat(), FontConverterConfig::Format::Header); // default
}

TEST(FontConverterConfig, PrintHelp)
{
    EXPECT_TRUE(makeConfig({ "-h" }).getPrintHelp());
    EXPECT_FALSE(makeConfig({}).getPrintHelp()); // default
}

TEST(FontConverterConfig, CombinedOptions)
{
    const auto config = makeConfig({ "-f", "f.ttf", "-o", "/tmp/o", "-d", "72",
                                     "-s", "18", "-b", "-u", "0x30-0x39" });
    EXPECT_EQ(config.getFontFileName(), "f.ttf");
    EXPECT_EQ(config.getOutputDirectory(), "/tmp/o");
    EXPECT_EQ(config.getDPI(), 72);
    EXPECT_EQ(config.getFontSize(), 18);
    EXPECT_EQ(config.getFormat(), FontConverterConfig::Format::Binary);
    EXPECT_FALSE(config.getPrintHelp());
    ASSERT_EQ(config.getCodePoints().size(), 10u);
    EXPECT_EQ(config.getCodePoints().front(), 0x30);
    EXPECT_EQ(config.getCodePoints().back(), 0x39);
}
