#include "graphics/Canvas.h"
#include "graphics/EmbeddedFonfs.h"
#include "PBMHelper.h"
#include "graphics/SimpleFrameBuffer.h"
#include <gtest/gtest.h>
#include <cstring>

namespace
{
constexpr int frameWidth = 280;
constexpr int frameHeight = 480;

void drawPortrait(Canvas &frameBuffer)
{
    frameBuffer.clear(false);
    auto size = frameBuffer.getSize();
    const auto& width = size.width;
    const auto& height = size.height;
    frameBuffer.drawRectangle({{0,0},size});
    frameBuffer.drawRectangle({{10,10},size - Canvas::Size{20, 20}});
    frameBuffer.drawRectangle({{20,20},size - Canvas::Size{40, 40}});
    frameBuffer.drawCircle({width/2, width/2}, (width - 60)/2);
    frameBuffer.drawCircle({width/2, width/2}, (width - 60)/2 - 55, true);
    Canvas::Point p1 = { width / 2,30};
    Canvas::Point p2 = { 45, (width - 60) - 24};
    Canvas::Point p3 = { width - 45, (width - 60) - 24};
    frameBuffer.drawLine(p1,p2);
    frameBuffer.drawLine(p2, p3);
    frameBuffer.drawLine(p3, p1);
    frameBuffer.drawRectangle({{30, width-20},{width - 60, 80}});
    frameBuffer.drawFilledRectangle({{50, width},{width - 100, 40}});
    frameBuffer.drawStringAt({60, width + 100}, "Hello World!", embedded::fonts::FreeSans15pt);
}

void drawLandscape(Canvas &frameBuffer)
{
    frameBuffer.clear(false);
    auto size = frameBuffer.getSize();
    const auto& width = size.width;
    const auto& height = size.height;
    frameBuffer.drawRectangle({{0,0},size});
    frameBuffer.drawRectangle({{10,10},size - Canvas::Size{20, 20}});
    frameBuffer.drawRectangle({{20,20},size - Canvas::Size{40, 40}});
    frameBuffer.drawCircle({height/2, height/2}, (height - 60)/2);
    frameBuffer.drawCircle({height/2, height/2}, (height - 60)/2 - 55, true);
    Canvas::Point p1 = { height / 2,30};
    Canvas::Point p2 = { 45, (height - 60) - 24};
    Canvas::Point p3 = { height - 45, (height - 60) - 24};
    frameBuffer.drawLine(p1,p2);
    frameBuffer.drawLine(p2, p3);
    frameBuffer.drawLine(p3, p1);
    frameBuffer.drawRectangle({{height - 20, 30 },{width - height - 10, 80}});
    frameBuffer.drawFilledRectangle({{height, 50},{width - height - 50, 40}});
    frameBuffer.drawLine({height, 160}, {height + 100, 160});
    frameBuffer.drawStringAt({ height, 160}, "Hello World!", embedded::fonts::FreeSans15pt);
}

void checkBuffer(Canvas& frameBuffer, std::string_view testFileName)
{
    auto fileData = testing::PBMHelper::readPBM(testFileName);
    ASSERT_TRUE(fileData);
    EXPECT_EQ(std::get<2>(*fileData).size(), frameBuffer.getImage().size());
    switch (frameBuffer.getRotation())
    {
    case Canvas::Rotation::Rotation0:
    case Canvas::Rotation::Rotation180:
        EXPECT_EQ(std::get<0>(*fileData), frameBuffer.getSize().width);
        EXPECT_EQ(std::get<1>(*fileData), frameBuffer.getSize().height);
        break;
    case Canvas::Rotation::Rotation90:
    case Canvas::Rotation::Rotation270:
        EXPECT_EQ(std::get<0>(*fileData), frameBuffer.getSize().height);
        EXPECT_EQ(std::get<1>(*fileData), frameBuffer.getSize().width);
        break;
    }
    const auto &image = frameBuffer.getImage();
    ASSERT_EQ(std::get<2>(*fileData).size(), image.size());
    EXPECT_EQ(std::memcmp(&std::get<2>(*fileData)[0], image.begin(), image.size()), 0);
}

}

TEST(MonochromeFrameBufferTest, Line1)
{
    SimpleFrameBuffer<48, 60> simpleFrameBuffer;
    Canvas frameBuffer(simpleFrameBuffer);
    auto testFileName = "MonochromeFrameBufferTestLine1.pbm";
    frameBuffer.setRotation(Canvas::Rotation::Rotation0);
    frameBuffer.clear(false);
    frameBuffer.drawLine({0,0}, {47,59});
    frameBuffer.drawLine({47,0}, {0,59});
    checkBuffer(frameBuffer, testFileName);
    SimpleFrameBuffer<48, 60> simpleFrameBuffer2;
    Canvas frameBuffer2(simpleFrameBuffer2);
    frameBuffer2.setRotation(Canvas::Rotation::Rotation0);
    frameBuffer2.clear(false);
    frameBuffer2.drawLine({47,59}, {0,0});
    frameBuffer2.drawLine({0,59}, {47,0});
    const auto &buffer = frameBuffer.getImage();
    const auto &buffer2 = frameBuffer2.getImage();
    EXPECT_EQ(std::memcmp(buffer.begin(), buffer2.begin(), buffer.size()), 0);
    frameBuffer2.setRotation(Canvas::Rotation::Rotation180);
    frameBuffer2.clear(false);
    frameBuffer2.drawLine({47,59}, {0,0});
    frameBuffer2.drawLine({0,59}, {47,0});
    EXPECT_EQ(std::memcmp(buffer.begin(), buffer2.begin(), buffer2.size()), 0);
    frameBuffer2.setRotation(Canvas::Rotation::Rotation90);
    frameBuffer2.clear(false);
    frameBuffer2.drawLine({0,0}, {59,47});
    frameBuffer2.drawLine({0,47}, {59,0});
    EXPECT_EQ(std::memcmp(buffer.begin(), buffer2.begin(), buffer2.size()), 0);
    frameBuffer2.setRotation(Canvas::Rotation::Rotation270);
    frameBuffer2.clear(false);
    frameBuffer2.drawLine({0,0}, {59,47});
    frameBuffer2.drawLine({0,47}, {59,0});
    EXPECT_EQ(std::memcmp(buffer.begin(), buffer2.begin(), buffer2.size()), 0);
}


TEST(MonochromeFrameBufferTest, Line2)
{
    SimpleFrameBuffer<48, 60> simpleFrameBuffer;
    Canvas frameBuffer(simpleFrameBuffer);
    auto testFileName = "MonochromeFrameBufferTestLine2.pbm";
    frameBuffer.setRotation(Canvas::Rotation::Rotation0);
    frameBuffer.clear(false);
    int x1 = 1;
    int y1 = 20;
    int x2 = 46;
    int y2 = 40;
    int y1Mirror = y1 - 1;
    int y2Mirror = y2 - 1;
    frameBuffer.drawLine({x1,y1}, {x2,y2});
    frameBuffer.drawLine({x1,y2}, {x2,y1});
    checkBuffer(frameBuffer, testFileName);
    SimpleFrameBuffer<48, 60> simpleFrameBuffer2;
    Canvas frameBuffer2(simpleFrameBuffer2);
    frameBuffer2.setRotation(Canvas::Rotation::Rotation0);
    frameBuffer2.clear(false);
    frameBuffer2.drawLine({x2,y2}, {x1,y1});
    frameBuffer2.drawLine({x2,y1}, {x1,y2});
    const auto &buffer = frameBuffer.getImage();
    const auto &buffer2 = frameBuffer2.getImage();
    EXPECT_EQ(std::memcmp(buffer.begin(), buffer2.begin(), buffer.size()), 0);
    frameBuffer2.setRotation(Canvas::Rotation::Rotation180);
    frameBuffer2.clear(false);
    frameBuffer2.drawLine({x1,y1Mirror}, {x2,y2Mirror});
    frameBuffer2.drawLine({x1,y2Mirror}, {x2,y1Mirror});
    EXPECT_EQ(std::memcmp(buffer.begin(), buffer2.begin(), buffer.size()), 0);
    frameBuffer2.setRotation(Canvas::Rotation::Rotation90);
    frameBuffer2.clear(false);
    frameBuffer2.drawLine({y1,x1}, {y2,x2});
    frameBuffer2.drawLine({y2,x1}, {y1,x2});
    EXPECT_EQ(std::memcmp(buffer.begin(), buffer2.begin(), buffer.size()), 0);
    frameBuffer2.setRotation(Canvas::Rotation::Rotation270);
    frameBuffer2.clear(false);
    frameBuffer2.drawLine({y1Mirror,x1}, {y2Mirror,x2});
    frameBuffer2.drawLine({y2Mirror,x1}, {y1Mirror,x2});
    EXPECT_EQ(std::memcmp(buffer.begin(), buffer2.begin(), buffer.size()), 0);
}

TEST(MonochromeFrameBufferTest, Line3)
{
    SimpleFrameBuffer<48, 60> simpleFrameBuffer;
    Canvas frameBuffer(simpleFrameBuffer);
    int step = 19;
    int shift = 3;
    auto testFileName = "MonochromeFrameBufferTestLine3.pbm";
    frameBuffer.setRotation(Canvas::Rotation::Rotation0);
    frameBuffer.clear(false);
    frameBuffer.drawLine({step + shift,shift}, {shift,step + shift});
    frameBuffer.drawLine({step + shift +1,shift}, {2*step + shift +1,step + shift});
    frameBuffer.drawLine({shift,step + shift +1}, {step + shift,2*step + shift +1});
    frameBuffer.drawLine({step + shift +1,2*step + shift +1}, {2*step + shift +1, step + shift +1});
    frameBuffer.drawLine({2,1}, {45,1});
    frameBuffer.drawLine({45,46}, {2, 46});
    frameBuffer.drawLine({1,2}, {1, 45});
    frameBuffer.drawLine({46,45}, {46, 2});
    checkBuffer(frameBuffer, testFileName);
}

TEST(MonochromeFrameBufferTest, Rotation0)
{
    SimpleFrameBuffer<frameWidth, frameHeight> simpleFrameBuffer;
    Canvas frameBuffer(simpleFrameBuffer);
    frameBuffer.setRotation(Canvas::Rotation::Rotation0);
    drawPortrait(frameBuffer);
    auto testFileName = "MonochromeFrameBufferTest0.pbm";
    checkBuffer(frameBuffer, testFileName);
}

TEST(MonochromeFrameBufferTest, Rotation180)
{
    SimpleFrameBuffer<frameWidth, frameHeight> simpleFrameBuffer;
    Canvas frameBuffer(simpleFrameBuffer);
    frameBuffer.setRotation(Canvas::Rotation::Rotation180);
    drawPortrait(frameBuffer);
    auto testFileName = "MonochromeFrameBufferTest180.pbm";
    checkBuffer(frameBuffer, testFileName);
}

TEST(MonochromeFrameBufferTest, Rotation90)
{
    SimpleFrameBuffer<frameWidth, frameHeight> simpleFrameBuffer;
    Canvas frameBuffer(simpleFrameBuffer);
    frameBuffer.setRotation(Canvas::Rotation::Rotation90);
    drawLandscape(frameBuffer);
    auto testFileName = "MonochromeFrameBufferTest90.pbm";
    checkBuffer(frameBuffer, testFileName);
}

TEST(MonochromeFrameBufferTest, Rotation270)
{
    SimpleFrameBuffer<frameWidth, frameHeight> simpleFrameBuffer;
    Canvas frameBuffer(simpleFrameBuffer);
    frameBuffer.setRotation(Canvas::Rotation::Rotation270);
    drawLandscape(frameBuffer);
    auto testFileName = "MonochromeFrameBufferTest270.pbm";
    checkBuffer(frameBuffer, testFileName);
}
