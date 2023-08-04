#include "graphics/BaseGeometry.h"
#include <gtest/gtest.h>

using embedded::Size;
using embedded::Point;
using embedded::Rect;
using embedded::Matrix2x2;

TEST(BaseGeometryTest, SizeAddition) {
    Size<int> s1{2, 3};
    Size<int> s2{4, 5};
    auto result = s1 + s2;
    EXPECT_EQ(result.width, 6);
    EXPECT_EQ(result.height, 8);
}

TEST(BaseGeometryTest, SizeSubtraction) {
    Size<int> s1{10, 8};
    Size<int> s2{5, 3};
    auto result = s1 - s2;
    EXPECT_EQ(result.width, 5);
    EXPECT_EQ(result.height, 5);
}

TEST(BaseGeometryTest, SizeMultiplication) {
    Size<int> s{3, 4};
    int scale = 2;
    auto result = s * scale;
    EXPECT_EQ(result.width, 6);
    EXPECT_EQ(result.height, 8);
}

TEST(BaseGeometryTest, SizeDivision) {
    Size<int> s{12, 10};
    int scale = 2;
    auto result = s / scale;
    EXPECT_EQ(result.width, 6);
    EXPECT_EQ(result.height, 5);
}

TEST(BaseGeometryTest, PointAddition) {
    Point<int> p{2, 3};
    Size<int> s{4, 5};
    auto result = p + s;
    EXPECT_EQ(result.x, 6);
    EXPECT_EQ(result.y, 8);
}

TEST(BaseGeometryTest, PointCompoundAddition) {
    Point<int> p{2, 3};
    Size<int> s{4, 5};
    p += s;
    EXPECT_EQ(p.x, 6);
    EXPECT_EQ(p.y, 8);
}

TEST(BaseGeometryTest, PointEquality) {
    Point<int> p1{2, 3};
    Point<int> p2{2, 3};
    Point<int> p3{4, 5};
    EXPECT_EQ(p1, p2);
    EXPECT_NE(p1, p3);
}

// Test the Rect struct
TEST(BaseGeometryTest, RectAddition) {
    Rect<int> rect{{1, 2}, {3, 4}};
    Size<int> s{5, 6};
    auto result = rect + s;
    EXPECT_EQ(result.topLeft.x, 6);
    EXPECT_EQ(result.topLeft.y, 8);
    EXPECT_EQ(result.size.width, 3);
    EXPECT_EQ(result.size.height, 4);
}

TEST(BaseGeometryTest, RectCompoundAddition) {
    Rect<int> rect{{1, 2}, {3, 4}};
    Size<int> s{5, 6};
    rect += s;
    EXPECT_EQ(rect.topLeft.x, 6);
    EXPECT_EQ(rect.topLeft.y, 8);
    EXPECT_EQ(rect.size.width, 3);
    EXPECT_EQ(rect.size.height, 4);
}

TEST(BaseGeometryTest, RectBottomLeft) {
    Rect<int> rect{{1, 2}, {3, 4}};
    auto bottomLeft = rect.bottomLeft();
    EXPECT_EQ(bottomLeft.x, 1);
    EXPECT_EQ(bottomLeft.y, 5);
}

TEST(BaseGeometryTest, RectTopRight) {
    Rect<int> rect{{1, 2}, {3, 4}};
    auto topRight = rect.topRight();
    EXPECT_EQ(topRight.x, 3);
    EXPECT_EQ(topRight.y, 2);
}

TEST(BaseGeometryTest, RectBottomRight) {
    Rect<int> rect{{1, 2}, {3, 4}};
    auto bottomRight = rect.bottomRight();
    EXPECT_EQ(bottomRight.x, 3);
    EXPECT_EQ(bottomRight.y, 5);
}

TEST(BaseGeometryTest, Matrix2x2Multiplication) {
    Matrix2x2 matrix(2, 1, -3, 0);
    Point<int> v{2, 3};
    auto result = matrix * v;
    EXPECT_EQ(result.x, 7);
    EXPECT_EQ(result.y, -6);
}
