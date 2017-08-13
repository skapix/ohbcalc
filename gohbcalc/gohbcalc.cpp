#include "CalculatorWindow.h"

#include <QApplication>
#include <memory>

using namespace std;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  auto window = make_unique<CalculatorWindow>();
  window->show();
  app.exec();
}