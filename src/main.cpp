#include <QApplication>
#include "adapters/gui/MainWindow.h"
#include "adapters/StdoutCalculationLog.h"
#include "core/CalculatorService.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    StdoutCalculationLog log;
    CalculatorService    service(log);
    MainWindow            window(service);
    
    window.show();
    return app.exec();
}