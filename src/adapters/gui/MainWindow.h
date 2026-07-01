#pragma once
#include <QMainWindow>

class QLineEdit;
class QPushButton;
class QLabel;
class ICalculatorService;   // driving adapter depends on the PORT, not the core

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(ICalculatorService& service, QWidget* parent = nullptr);

private slots:
    void onAddClicked();
    void onClearClicked();

private:
    QLineEdit*   m_inputA;
    QLineEdit*   m_inputB;
    QPushButton* m_addButton;
    QPushButton* m_clearButton;
    QLabel*      m_resultLabel;
    ICalculatorService& m_service;   // injected, not owned
};
