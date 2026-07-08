#include <gmock/gmock.h>
#include "ports/ICalculationLog.h"
class MockCalculationLog : public ICalculationLog {
public:
    MOCK_METHOD(void, record, (const Calculation& c), (override));
};