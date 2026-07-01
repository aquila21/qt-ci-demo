#include "adapters/gui/MainWindow.h"
#include "ports/ICalculatorService.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(ICalculatorService& service, QWidget* parent)
    : QMainWindow(parent), m_service(service) {
    setWindowTitle("Qt CI Demo — Calculator");
    setMinimumSize(300, 200);

    auto* central = new QWidget(this);
    auto* layout  = new QVBoxLayout(central);

    auto* inputRow = new QHBoxLayout();
    m_inputA = new QLineEdit(this);
    m_inputA->setObjectName("inputA");            // named for testability
    m_inputA->setPlaceholderText("Value A");
    m_inputB = new QLineEdit(this);
    m_inputB->setObjectName("inputB");
    m_inputB->setPlaceholderText("Value B");
    inputRow->addWidget(m_inputA);
    inputRow->addWidget(m_inputB);

    auto* buttonRow = new QHBoxLayout();
    m_addButton   = new QPushButton("Add", this);
    m_addButton->setObjectName("addButton");
    m_clearButton = new QPushButton("Clear", this);
    m_clearButton->setObjectName("clearButton");
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_clearButton);

    m_resultLabel = new QLabel("Result: —", this);
    m_resultLabel->setObjectName("resultLabel");
    m_resultLabel->setAlignment(Qt::AlignCenter);

    layout->addLayout(inputRow);
    layout->addLayout(buttonRow);
    layout->addWidget(m_resultLabel);
    setCentralWidget(central);

    connect(m_addButton,   &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);
}

// The slot is now THIN: parse -> delegate to the port -> render. No business logic.
void MainWindow::onAddClicked() {
    bool okA, okB;
    double a = m_inputA->text().toDouble(&okA);
    double b = m_inputB->text().toDouble(&okB);
    if (okA && okB) {
        double result = m_service.add(a, b);
        m_resultLabel->setText(QString("Result: %1").arg(result));
    } else {
        m_resultLabel->setText("Result: invalid input");
    }
}

void MainWindow::onClearClicked() {
    m_inputA->clear();
    m_inputB->clear();
    m_resultLabel->setText("Result: —");
}
