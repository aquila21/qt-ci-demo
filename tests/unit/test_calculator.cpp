#include <gtest/gtest.h>
#include "core/Calculator.h"

// UNIT: pure arithmetic. No Qt, no ports, no display.
TEST(CalculatorTest, Add)          { EXPECT_DOUBLE_EQ(Calculator().add(2, 3),       5.0); }
TEST(CalculatorTest, Subtract)     { EXPECT_DOUBLE_EQ(Calculator().subtract(10, 4), 6.0); }
TEST(CalculatorTest, Multiply)     { EXPECT_DOUBLE_EQ(Calculator().multiply(3, 4),  12.0); }
TEST(CalculatorTest, Divide)       { EXPECT_DOUBLE_EQ(Calculator().divide(10, 2),   5.0); }
TEST(CalculatorTest, DivideByZero) { EXPECT_DOUBLE_EQ(Calculator().divide(5, 0),    0.0); }
