#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/CalculatorService.h"
#include "mocks/MockCalculationLog.h"

using ::testing::_;
using ::testing::AllOf;
using ::testing::Field;

// UNIT: the use case in isolation. The mock stands in for the outside world
// and lets us assert *how* the core drives the outbound port.
TEST(CalculatorServiceTest, AddReturnsSum) {
    MockCalculationLog log;
    EXPECT_CALL(log, record(_)).Times(1);      // expect exactly one outbound call
    CalculatorService svc(log);
    EXPECT_DOUBLE_EQ(svc.add(2, 3), 5.0);
}

TEST(CalculatorServiceTest, RecordsCalculationThroughPort) {
    MockCalculationLog log;
    EXPECT_CALL(log, record(AllOf(
        Field(&Calculation::op,     "add"),
        Field(&Calculation::result, 5.0))));   // assert the value crossing the port
    CalculatorService svc(log);
    svc.add(2, 3);
}
