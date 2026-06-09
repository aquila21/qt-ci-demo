#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include "Calculator.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onAddClicked();
    void onClearClicked();

private:
    QLineEdit   *m_inputA;
    QLineEdit   *m_inputB;
    QPushButton *m_addButton;
    QPushButton *m_clearButton;
    QLabel      *m_resultLabel;
    Calculator   m_calculator;
};
