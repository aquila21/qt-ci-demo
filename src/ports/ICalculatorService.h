#pragma once
class ICalculatorService {
public:
    virtual ~ICalculatorService() = default;
    virtual double add(double a, double b) = 0;
    virtual double subtract(double a, double b) = 0;
};