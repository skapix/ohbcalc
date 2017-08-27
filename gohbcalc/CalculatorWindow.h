#pragma once

#include "../cohbcalc/ExpressionHistory.h"
#include "ohbcalc.h"
#include <QObject>
#include <QtWidgets/QMainWindow>

class ResultView;
class QComboBox;
class QLabel;

class CalculatorWindow : public QMainWindow {
  Q_OBJECT
public:
  CalculatorWindow();

private slots:
  void on_enterPressed(const QString &data);

signals:
  void calculated(int64_t result);

private:
  OHBCalc m_calc;

  QComboBox *m_editor;
  QLabel *m_errorLine;
  const QFont& m_editorFont;

  ResultView *m_decimal;
  ResultView *m_udecimal;
  ResultView *m_hexadecimal;
  ResultView *m_octal;
  ResultView *m_chars;
  ResultView *m_binary;
};
