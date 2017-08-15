#include <QtWidgets/QHBoxLayout>
#include "ResultView.h"
#include "CalculatorWindow.h"

const auto g_styleSheet = "QLabel {"
  "border-style: solid;"
  "border-width: 1px;"
  "border-color: black; "
  "}";

const int g_nameWidth = 75;

ResultView::ResultView(CalculatorWindow *parent, const QString &labelName)
  : QWidget(parent)
  , m_value(new QLabel(this))
{
  auto name = new QLabel(labelName, this);

  name->setAlignment(Qt::AlignCenter);
  name->setStyleSheet(g_styleSheet);
  name->setFixedWidth(g_nameWidth);
  m_value->setTextInteractionFlags(Qt::TextSelectableByMouse);
  m_value->setStyleSheet(g_styleSheet);
  m_value->setAlignment(Qt::AlignCenter);


  auto layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(name);
  layout->addWidget(m_value);

  Q_ASSERT(parent);

  connect(parent, &CalculatorWindow::calculated, this, &ResultView::on_calculated);
}

void ResultView::setRepresentationFunction(std::function<std::string(int64_t)> repr)
{
  m_representation = std::move(repr);
}

void ResultView::on_calculated(int64_t result)
{
  if (m_representation)
  {
    std::string view = m_representation(result);
    m_value->setText(QString::fromStdString(view));
  }
}
