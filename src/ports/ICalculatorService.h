#pragma once
// Inbound (driving) port: the use case the core exposes to the outside world.
// Driving adapters (GUI, CLI, tests) depend on THIS, never on the concrete core.
class ICalculatorService {
public:
    virtual ~ICalculatorService() = default;
    virtual double add(double a, double b) = 0;
};
