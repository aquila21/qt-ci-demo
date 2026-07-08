#include <gtest/gtest.h>
#include "core/CalculatorService.h"
#include "adapters/InMemoryCalculationLog.h"

// INTEGRATION: two REAL components collaborating across the outbound port.
TEST(ServiceLogIntegration, ComputesAndPersistsAcrossPort) {
    InMemoryCalculationLog log;      // real driven adapter
    CalculatorService      svc(log); // real core wired to it

    svc.add(10, 5);
    svc.add(1, 1);

    ASSERT_EQ(log.entries().size(), 2u);
    EXPECT_DOUBLE_EQ(log.entries().at(0).result, 15.0);
    EXPECT_DOUBLE_EQ(log.entries().at(1).result, 2.0);
}

TEST(ServiceLogIntegration, SubtractLogsOperationThroughPort) {
    InMemoryCalculationLog log;
    CalculatorService      svc(log);

    const double result = svc.subtract(10, 4);

    ASSERT_EQ(log.entries().size(), 1u);
    const Calculation& entry = log.entries().front();
    EXPECT_EQ(entry.op, "subtract");
    EXPECT_DOUBLE_EQ(entry.a, 10.0);
    EXPECT_DOUBLE_EQ(entry.b, 4.0);
    EXPECT_DOUBLE_EQ(entry.result, 6.0);
    EXPECT_DOUBLE_EQ(result, 6.0);
}
