#pragma once
class ICalculatorService {
public:
    virtual ~ICalculatorService() = default;
    virtual double add(double a, double b) = 0;
};