#include "CalculatorWindow.h"
#include "ResultView.h"
#include "ohbException.h"
#include "ohbTransform.h"
#include <QLineEdit>
#include <set>
#include <QHBoxLayout>

const auto g_bkgColor = Qt::lightGray;

CalculatorWindow::CalculatorWindow()
  : m_editor(new QLineEdit(this))
  , m_errorLine(new QLineEdit(this))
  , m_decimal(new ResultView(this, "Signed"))
  , m_udecimal(new ResultView(this, "Unsigned"))
  , m_hexadecimal(new ResultView(this, "Hex"))
  , m_octal(new ResultView(this, "Oct"))
  , m_chars(new ResultView(this, "Char"))
  , m_binary(new ResultView(this, "Binary"))
{
  QWidget *centralWidget = new QWidget;
  auto layout = new QVBoxLayout(centralWidget);
  layout->setSpacing(0);
  layout->setContentsMargins(0,0,0,0);
  QPalette pal = palette();
  pal.setColor(QPalette::Background, g_bkgColor);
  centralWidget->setAutoFillBackground(true);
  centralWidget->setPalette(pal);
  layout->addWidget(m_editor);
  layout->addWidget(m_errorLine);
  layout->addSpacing(10);
  auto decimalLayout = new QHBoxLayout;
  decimalLayout->addWidget(m_decimal);
  decimalLayout->addWidget(m_udecimal);
  layout->addLayout(decimalLayout);
  auto ohcLayout = new QHBoxLayout;
  ohcLayout->addWidget(m_octal);
  ohcLayout->addWidget(m_hexadecimal);
  ohcLayout->addWidget(m_chars);
  layout->addLayout(ohcLayout);
  layout->addWidget(m_binary);
  setCentralWidget(centralWidget);

  // TODO: add QScrollArea and stop window resizing; set default size
  // TODO: stop resizing widget heights
  // TODO: set calculator icon sth like (0i/0h)
  // TODO: add history
  connect(m_editor, &QLineEdit::returnPressed, this, &CalculatorWindow::on_returnPressed);
  m_decimal->setRepresentationFunction([](int64_t result)
                                       {return std::to_string(result);});
  m_udecimal->setRepresentationFunction([](int64_t result)
                                        {return std::to_string(static_cast<uint64_t>(result));});
  m_hexadecimal->setRepresentationFunction([](int64_t result)
                                           {return toHex(result, false);});
  m_octal->setRepresentationFunction([](int64_t result)
                                           {return toOctal(result, false);});
  m_chars->setRepresentationFunction([](int64_t result)
                                     {
                                       return '"' + toChars(result, true) + '"';
                                     });
  m_binary->setRepresentationFunction([](int64_t result)
                                     {
                                       std::string tmp = toBinary(result, true);
                                       for (size_t separator = 8; separator < tmp.size(); separator += 8 + 1)
                                       {
                                         tmp.insert(tmp.begin() + separator, '-');
                                       }
                                       return tmp;
                                     });

  m_errorLine->setDisabled(true);
}

void CalculatorWindow::on_returnPressed() {
  try
  {
    std::string expression = m_editor->text().toStdString();
    if (std::all_of(expression.begin(), expression.end(), [](char c) {return isspace(c);}))
    {
      return;
    }
    int64_t result = m_calc.eval(expression);
    emit calculated(result);
  }
  catch (const OHBException &error)
  {
    // TODO: set position sign more accurately
    int position = static_cast<int>(error.getPos());
    auto message = QString::fromLocal8Bit(error.what());
    QString result;
    if (position > message.size() + 1)
    {
      result = message;
      position -= message.size();
      result += QString(position, QChar::fromLatin1(' '));
      result.push_back(QChar::fromLatin1('^'));
    }
    else
    {
      result = QString(position, QChar::fromLatin1(' '));
      result.push_back(QChar::fromLatin1('^'));
      result.push_back(QChar::fromLatin1(' '));
      result += message;
    }
    m_errorLine->setText(result);
  }
}
