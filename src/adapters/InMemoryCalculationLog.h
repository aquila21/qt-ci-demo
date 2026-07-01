#pragma once
#include <vector>
#include "ports/ICalculationLog.h"
// Real driven adapter used by integration tests (and usable in-app).
// Captures every Calculation so a test can assert what crossed the port.
class InMemoryCalculationLog : public ICalculationLog {
public:
    void record(const Calculation& c) override { m_entries.push_back(c); }
    const std::vector<Calculation>& entries() const { return m_entries; }
private:
    std::vector<Calculation> m_entries;
};
