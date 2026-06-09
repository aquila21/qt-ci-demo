#include <QtTest>
#include "Calculator.h"

class TestCalculator : public QObject {
    Q_OBJECT

private slots:
    void testAdd()      { QCOMPARE(Calculator().add(2, 3),       5.0); }
    void testSubtract() { QCOMPARE(Calculator().subtract(10, 4), 6.0); }
    void testMultiply() { QCOMPARE(Calculator().multiply(3, 4),  12.0); }
    void testDivide()   { QCOMPARE(Calculator().divide(10, 2),   5.0); }
    void testDivideByZero() { QCOMPARE(Calculator().divide(5, 0), 0.0); }
};

QTEST_MAIN(TestCalculator)
#include "test_calculator.moc"
