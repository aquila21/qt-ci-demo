// InMemoryCalculationLog.h — real adapter used by integration tests
class InMemoryCalculationLog : public ICalculationLog {
public:
    void record(const Calculation& c) override { m_entries.push_back(c); }
    const std::vector<Calculation>& entries() const { return m_entries; }
private:
    std::vector<Calculation> m_entries;
};