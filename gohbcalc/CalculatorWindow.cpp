#include "CalculatorWindow.h"
#include "ResultView.h"
#include "ohbException.h"
#include "ohbTransform.h"
#include <QLineEdit>
#include <set>
#include <QHBoxLayout>

const auto g_bkgColor = Qt::lightGray;
const int g_spacingBetweenEditorAndView = 10;
const char g_binaryByteSeparator = '-';

CalculatorWindow::CalculatorWindow()
  : m_editor(new QLineEdit(this))
  , m_errorLine(new QLineEdit(this))
  , m_editorFont(m_editor->font())
  , m_decimal(new ResultView(this, "Signed"))
  , m_udecimal(new ResultView(this, "Unsigned"))
  , m_hexadecimal(new ResultView(this, "Hex"))
  , m_octal(new ResultView(this, "Oct"))
  , m_chars(new ResultView(this, "Char"))
  , m_binary(new ResultView(this, "Binary"))
{
  setWindowIcon(QIcon(":calculator.png"));
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
  layout->addSpacing(g_spacingBetweenEditorAndView);
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
  layout->addStretch(-1);
  setCentralWidget(centralWidget);

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
                                         tmp.insert(tmp.begin() + separator, g_binaryByteSeparator);
                                       }
                                       return tmp;
                                     });

  m_errorLine->setDisabled(true);
  // TODO: ? change m_errorLine color to darker one (like background)
  // and write with white characters
  int lineHeight = fontMetrics().height();
  setMaximumHeight(m_editor->height() + m_errorLine->height() +
                     g_spacingBetweenEditorAndView + m_decimal->height() +
                     lineHeight * sizeof(int64_t) + m_binary->height());
  int zeroWidth = fontMetrics().width('0');
  int dashWidth = fontMetrics().width(g_binaryByteSeparator);
  setMinimumWidth(m_binary->width() + zeroWidth * sizeof(int64_t) * 8 +
                    dashWidth * (sizeof(int64_t) - 1) + 5); // 5 is epsilon

  // Disable maximize button. Doesn't work on linux
  setWindowFlags((Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint) &
                   ~Qt::WindowMaximizeButtonHint);

}

void CalculatorWindow::on_returnPressed() {
  QString errorResult;
  std::string expression = m_editor->text().toStdString();
  try
  {

    if (std::all_of(expression.begin(), expression.end(), [](char c) {return isspace(c);}))
    {
      return;
    }
    int64_t result = m_calc.eval(expression);
    emit calculated(result);
  }
  catch (const OHBException &error)
  {
    auto metrics = QFontMetrics(m_editorFont);
    int position = static_cast<int>(error.getPos());
    auto message = QString::fromLocal8Bit(error.what());
    int messageWLength = metrics.width(message);
    auto qexpression = QString::fromStdString(expression);

    int expressionWPos = metrics.width(qexpression, position);
    int spaceWLength = metrics.width(QChar::fromLatin1(' '));

    auto getStringOfWidth = [&metrics](const int width)
    {
      auto result = QString::fromLocal8Bit(" ");
      while(metrics.width(result) < width)
      {
        result.push_back(QChar::fromLatin1(' '));
      }
      //result.resize(result.size() -1);
      return result;
    };

    if (expressionWPos > messageWLength + spaceWLength)
    {
      errorResult = message;
      expressionWPos -= messageWLength;
      errorResult += getStringOfWidth(expressionWPos);
      errorResult.push_back(QChar::fromLatin1('^'));
    }
    else
    {
      errorResult =getStringOfWidth(expressionWPos);
      errorResult.push_back(QChar::fromLatin1('^'));
      errorResult.push_back(QChar::fromLatin1(' '));
      errorResult += message;
    }
  }
  m_errorLine->setText(errorResult);
}
