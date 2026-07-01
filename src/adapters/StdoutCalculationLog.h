#pragma once
#include <iostream>
#include "ports/ICalculationLog.h"
// Real driven adapter wired into the running application.
class StdoutCalculationLog : public ICalculationLog {
public:
    void record(const Calculation& c) override {
        std::cout << c.op << "(" << c.a << ", " << c.b << ") = " << c.result << "\n";
    }
};
