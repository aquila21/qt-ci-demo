#include <QtTest>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "adapters/gui/MainWindow.h"
#include "ports/ICalculatorService.h"

// A stub inbound adapter: drives the GUI without the real core behind it.
class StubService : public ICalculatorService {
public:
    double lastA = 0, lastB = 0;
    double toReturn = 0;
    double add(double a, double b) override { lastA = a; lastB = b; return toReturn; }
    double subtract(double a, double b) override { (void)a; (void)b; return 0; }
};

// INTEGRATION: the GUI driving adapter wired to the inbound port, run HEADLESS
// (QT_QPA_PLATFORM=offscreen). Proves GUI<->logic glue without a real display.
class TestGuiService : public QObject {
    Q_OBJECT
private slots:
    void addButtonDelegatesToPortAndRenders() {
        StubService stub;
        stub.toReturn = 7.0;
        MainWindow w(stub);

        auto* inA    = w.findChild<QLineEdit*>("inputA");
        auto* inB    = w.findChild<QLineEdit*>("inputB");
        auto* addBtn = w.findChild<QPushButton*>("addButton");
        auto* result = w.findChild<QLabel*>("resultLabel");
        QVERIFY(inA && inB && addBtn && result);

        inA->setText("2");
        inB->setText("3");
        QTest::mouseClick(addBtn, Qt::LeftButton);

        QCOMPARE(stub.lastA, 2.0);           // GUI forwarded the inputs
        QCOMPARE(stub.lastB, 3.0);
        QVERIFY(result->text().contains("7")); // GUI rendered the port's result
    }
};

QTEST_MAIN(TestGuiService)
#include "test_gui_service.moc"
