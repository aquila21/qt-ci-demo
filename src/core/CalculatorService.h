#pragma once
#include "ports/ICalculatorService.h"
#include "ports/ICalculationLog.h"
#include "core/Calculator.h"

class CalculatorService : public ICalculatorService {
public:
    explicit CalculatorService(ICalculationLog& log);
    double add(double a, double b) override;
    double subtract(double a, double b) override;
private:
    Calculator       m_calc;
    ICalculationLog& m_log;
};