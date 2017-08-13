#pragma once

#include "ExpressionHistory.h"
#include "ohbcalc.h"
#include <QObject>
#include <QtWidgets/QMainWindow>

class ResultView;
class QLineEdit;
class QLabel;

class CalculatorWindow : public QMainWindow {
  Q_OBJECT
public:
  CalculatorWindow();

private slots:
  void on_returnPressed();

signals:
  void calculated(int64_t result);

private:
  OHBCalc m_calc;

  QLineEdit *m_editor;
  QLineEdit *m_errorLine;

  ResultView *m_decimal;
  ResultView *m_udecimal;
  ResultView *m_hexadecimal;
  ResultView *m_octal;
  ResultView *m_chars;
  ResultView *m_binary;
};
