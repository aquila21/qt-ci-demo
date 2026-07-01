#pragma once
#include "ports/ICalculatorService.h"
#include "ports/ICalculationLog.h"
#include "core/Calculator.h"

// Application / use-case layer. Implements the inbound port, and reaches the
// outside world ONLY through the outbound port it is given (constructor injection
// = dependency inversion). Knows nothing about Qt or where results actually go.
class CalculatorService : public ICalculatorService {
public:
    explicit CalculatorService(ICalculationLog& log);
    double add(double a, double b) override;
private:
    Calculator      m_calc;
    ICalculationLog& m_log;
};
