#include <cmath>
#include <limits>

#include <gtest/gtest.h>
#include "core/Calculator.h"

namespace {
constexpr double kFpTolerance = 1e-15;
}

// UNIT: pure arithmetic. No Qt, no ports, no display.
TEST(CalculatorTest, Add)          { EXPECT_DOUBLE_EQ(Calculator().add(2, 3),       5.0); }
TEST(CalculatorTest, Subtract)     { EXPECT_DOUBLE_EQ(Calculator().subtract(10, 4), 6.0); }
TEST(CalculatorTest, Multiply)     { EXPECT_DOUBLE_EQ(Calculator().multiply(3, 4),  12.0); }
TEST(CalculatorTest, Divide)       { EXPECT_DOUBLE_EQ(Calculator().divide(10, 2),   5.0); }
TEST(CalculatorTest, DivideByZero) { EXPECT_DOUBLE_EQ(Calculator().divide(5, 0),    0.0); }

// Negative operands
TEST(CalculatorTest, AddNegativeAndPositive) { EXPECT_DOUBLE_EQ(Calculator().add(-2, 3), 1.0); }
TEST(CalculatorTest, AddNegativeToNegative)   { EXPECT_DOUBLE_EQ(Calculator().add(-5, 2), -3.0); }
TEST(CalculatorTest, AddBothNegative)        { EXPECT_DOUBLE_EQ(Calculator().add(-2, -3), -5.0); }
TEST(CalculatorTest, SubtractToNegative)     { EXPECT_DOUBLE_EQ(Calculator().subtract(4, 10), -6.0); }
TEST(CalculatorTest, SubtractBothNegative)   { EXPECT_DOUBLE_EQ(Calculator().subtract(-5, -3), -2.0); }
TEST(CalculatorTest, MultiplyMixedSigns)     { EXPECT_DOUBLE_EQ(Calculator().multiply(-3, 4), -12.0); }
TEST(CalculatorTest, MultiplyBothNegative)   { EXPECT_DOUBLE_EQ(Calculator().multiply(-3, -4), 12.0); }
TEST(CalculatorTest, DivideNegativeDividend) { EXPECT_DOUBLE_EQ(Calculator().divide(-10, 2), -5.0); }
TEST(CalculatorTest, DivideNegativeDivisor)  { EXPECT_DOUBLE_EQ(Calculator().divide(10, -2), -5.0); }
TEST(CalculatorTest, DivideBothNegative)     { EXPECT_DOUBLE_EQ(Calculator().divide(-10, -2), 5.0); }

// Zero operands and zero results
TEST(CalculatorTest, AddZeroLeft)            { EXPECT_DOUBLE_EQ(Calculator().add(0, 5), 5.0); }
TEST(CalculatorTest, AddZeroRight)           { EXPECT_DOUBLE_EQ(Calculator().add(5, 0), 5.0); }
TEST(CalculatorTest, AddBothZero)            { EXPECT_DOUBLE_EQ(Calculator().add(0, 0), 0.0); }
TEST(CalculatorTest, SubtractToZero)         { EXPECT_DOUBLE_EQ(Calculator().subtract(5, 5), 0.0); }
TEST(CalculatorTest, MultiplyByZero)         { EXPECT_DOUBLE_EQ(Calculator().multiply(5, 0), 0.0); }
TEST(CalculatorTest, DivideZeroDividend)     { EXPECT_DOUBLE_EQ(Calculator().divide(0, 5), 0.0); }
TEST(CalculatorTest, DivideNegativeByZero)   { EXPECT_DOUBLE_EQ(Calculator().divide(-5, 0), 0.0); }
TEST(CalculatorTest, DivideByNegativeZero)   { EXPECT_DOUBLE_EQ(Calculator().divide(5, -0.0), 0.0); }

// Floating-point precision
TEST(CalculatorTest, AddFractions)           { EXPECT_NEAR(Calculator().add(0.1, 0.2), 0.3, kFpTolerance); }
TEST(CalculatorTest, SubtractFractions)      { EXPECT_NEAR(Calculator().subtract(1.0, 0.3), 0.7, kFpTolerance); }
TEST(CalculatorTest, MultiplyFractions)      { EXPECT_NEAR(Calculator().multiply(0.1, 0.3), 0.03, kFpTolerance); }
TEST(CalculatorTest, DivideNonTerminating)   { EXPECT_NEAR(Calculator().divide(1, 3), 1.0 / 3.0, kFpTolerance); }
TEST(CalculatorTest, DivideFractions)        { EXPECT_NEAR(Calculator().divide(0.1, 0.2), 0.5, kFpTolerance); }

// Divide zero-check boundary: values that must not hit the b == 0 guard
TEST(CalculatorTest, DivideBySmallestPositiveDouble) {
    const double divisor = std::nextafter(0.0, 1.0);
    const double result = Calculator().divide(1.0, divisor);
    EXPECT_NE(result, 0.0);
    EXPECT_TRUE(std::isinf(result));
    EXPECT_GT(result, 0.0);
}

TEST(CalculatorTest, DivideBySmallestNegativeDouble) {
    const double divisor = std::nextafter(0.0, -1.0);
    const double result = Calculator().divide(1.0, divisor);
    EXPECT_NE(result, 0.0);
    EXPECT_TRUE(std::isinf(result));
    EXPECT_LT(result, 0.0);
}

TEST(CalculatorTest, DivideByDenormMin) {
    const double divisor = std::numeric_limits<double>::denorm_min();
    const double result = Calculator().divide(1.0, divisor);
    EXPECT_NE(result, 0.0);
    EXPECT_TRUE(std::isinf(result) || result > 1e300);
}

TEST(CalculatorTest, DivideByVerySmallDivisor) {
    const double divisor = 1e-300;
    const double result = Calculator().divide(1.0, divisor);
    EXPECT_NE(result, 0.0);
    EXPECT_FALSE(std::isinf(result));
    EXPECT_NEAR(result, 1e300, 1e290);
}
