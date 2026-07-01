#pragma once
// Domain primitive: pure arithmetic. No Qt, no I/O.
class Calculator {
public:
    double add(double a, double b);
    double subtract(double a, double b);
    double multiply(double a, double b);
    double divide(double a, double b);  // returns 0 on division by zero
};
