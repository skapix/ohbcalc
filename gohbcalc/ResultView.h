#pragma once

#include <QLabel>
#include <functional>

class CalculatorWindow;

class ResultView : public QWidget
{
  Q_OBJECT

public:
  ResultView(CalculatorWindow *parent, const QString &labelName);
  void setRepresentationFunction(std::function<std::string(int64_t)> repr);
private slots:
  void on_calculated(int64_t);
private:
  std::function<std::string(int64_t)> m_representation;
  QLabel *m_value;

};