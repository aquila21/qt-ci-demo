#pragma once
#include <string>
struct Calculation { std::string op; double a; double b; double result; };
class ICalculationLog {
public:
    virtual ~ICalculationLog() = default;
    virtual void record(const Calculation& c) = 0;
};