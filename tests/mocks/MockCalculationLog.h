#pragma once
#include <gmock/gmock.h>
#include "ports/ICalculationLog.h"

// GoogleMock mock of the outbound port. Replaces the hand-written FakeLog:
// lets tests set expectations on how the core uses the port.
class MockCalculationLog : public ICalculationLog {
public:
    MOCK_METHOD(void, record, (const Calculation& c), (override));
};
