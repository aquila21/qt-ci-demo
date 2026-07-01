#pragma once
#include <string>
// A value crossing the outbound boundary (part of the shared-kernel data).
struct Calculation {
    std::string op;
    double a;
    double b;
    double result;
};
// Outbound (driven) port: something the core NEEDS from the outside.
// The core defines it; driven adapters (in-memory, stdout, file, DB) implement it.
// This is the stand-in for ISensorSource / IResultSink in the real product.
class ICalculationLog {
public:
    virtual ~ICalculationLog() = default;
    virtual void record(const Calculation& c) = 0;
};
