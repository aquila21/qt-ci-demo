#include "core/CalculatorService.h"
CalculatorService::CalculatorService(ICalculationLog& log) : m_log(log) {}
double CalculatorService::add(double a, double b) {
    const double r = m_calc.add(a, b);
    m_log.record({"add", a, b, r});
    return r;
}