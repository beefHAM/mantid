#include "ColorMapWidget.h"
#include "MantidColorMap.h"
#include "GraphOptions.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QMouseEvent>
#include <QApplication>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>

/**
  * Constructor.
  * @param type The scale type, e.g. "Linear" or "Log10"
  * @param parent A parent widget
  * @param minPositiveValue A minimum positive value for the Log10 scale
  */
ColorMapWidget::ColorMapWidget(int type,QWidget* parent,const double& minPositiveValue):
QFrame(parent),m_minPositiveValue(minPositiveValue),m_dragging(false)
{
  m_scaleWidget = new QwtScaleWidget(QwtScaleDraw::RightScale);
  m_scaleWidget->setColorBarEnabled(true);
  m_scaleWidget->setColorBarWidth(20);
  m_scaleWidget->setAlignment(QwtScaleDraw::RightScale);
  m_scaleWidget->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter);
  m_scaleWidget->setCursor(Qt::OpenHandCursor);

  m_minValueBox = new QLineEdit();
  m_maxValueBox = new QLineEdit();
  m_minValueBox->setMinimumWidth(40);
  m_maxValueBox->setMinimumWidth(40);
  m_minValueBox->setMaximumWidth(60);
  m_maxValueBox->setMaximumWidth(60);
  m_minValueBox->setValidator(new QDoubleValidator(m_minValueBox));
  m_maxValueBox->setValidator(new QDoubleValidator(m_maxValueBox));
  //Ensure the boxes start empty, this is important for checking if values have been set from the scripting side
  m_minValueBox->setText("");
  m_maxValueBox->setText("");
  connect(m_minValueBox,SIGNAL(editingFinished()),this,SLOT(minValueChanged()));
  connect(m_maxValueBox,SIGNAL(editingFinished()),this,SLOT(maxValueChanged()));

  QVBoxLayout* lColormapLayout = new QVBoxLayout;
  lColormapLayout->addWidget(m_maxValueBox);
  lColormapLayout->addWidget(m_scaleWidget);
  lColormapLayout->addWidget(m_minValueBox);

  m_scaleOptions = new QComboBox;
  m_scaleOptions->addItem("Log10", QVariant(GraphOptions::Log10));
  m_scaleOptions->addItem("Linear", QVariant(GraphOptions::Linear));
  m_scaleOptions->setCurrentIndex(m_scaleOptions->findData(type));
  connect(m_scaleOptions, SIGNAL(currentIndexChanged(int)), this, SLOT(scaleOptionsChanged(int)));

  QVBoxLayout* options_layout = new QVBoxLayout;
  options_layout->addStretch();
  options_layout->addWidget(m_scaleOptions);

  QHBoxLayout *colourmap_layout = new QHBoxLayout;
  colourmap_layout->addLayout(lColormapLayout);
  colourmap_layout->addLayout(options_layout);
  this->setLayout(colourmap_layout);

}

void ColorMapWidget::scaleOptionsChanged(int i)
{
  emit scaleTypeChanged(m_scaleOptions->itemData(i).toUInt());
}

/**
 *
 */
void ColorMapWidget::setupColorBarScaling(const MantidColorMap& colorMap)
{
  double minValue = m_minValueBox->displayText().toDouble();
  double maxValue = m_maxValueBox->displayText().toDouble();

  GraphOptions::ScaleType type = colorMap.getScaleType();
  if( type == GraphOptions::Linear )
  {
    QwtLinearScaleEngine linScaler;
    m_scaleWidget->setScaleDiv(linScaler.transformation(), linScaler.divideScale(minValue, maxValue,  20, 5));
    m_scaleWidget->setColorMap(QwtDoubleInterval(minValue, maxValue),colorMap);
  }
  else
 {
    QwtLog10ScaleEngine logScaler;    
    double logmin(minValue);
    if( logmin <= 0.0 )
    {
      logmin = m_minPositiveValue;
      m_minValueBox->blockSignals(true);
      setMinValue(logmin);
      m_minValueBox->blockSignals(false);
    }
    m_scaleWidget->setScaleDiv(logScaler.transformation(), logScaler.divideScale(logmin, maxValue, 20, 5));
    m_scaleWidget->setColorMap(QwtDoubleInterval(logmin, maxValue), colorMap);
  }
  m_scaleOptions->blockSignals(true);
  m_scaleOptions->setCurrentIndex(m_scaleOptions->findData(type));
  m_scaleOptions->blockSignals(false);
}

void ColorMapWidget::minValueChanged()
{
  emit minValueChanged(m_minValueBox->text().toDouble());
}

void ColorMapWidget::maxValueChanged()
{
  emit maxValueChanged(m_maxValueBox->text().toDouble());
}

void ColorMapWidget::setMinValue(double value)
{
  m_minValueBox->setText(QString::number(value));
}

void ColorMapWidget::setMaxValue(double value)
{
  m_maxValueBox->setText(QString::number(value));
}

/**
  * Set the minimum positive value for use with the Log10 scale. Values below this will
  * not be displayed on a Log10 scale.
  */
void ColorMapWidget::setMinPositiveValue(double value)
{
  m_minPositiveValue = value;
}

int ColorMapWidget::getScaleType()const
{
  return m_scaleOptions->itemData(m_scaleOptions->currentIndex()).toUInt();
}

void ColorMapWidget::setScaleType(int type)
{
  m_scaleOptions->setCurrentIndex(m_scaleOptions->findData(type));
}

void ColorMapWidget::mousePressEvent(QMouseEvent* e)
{
  QRect rect = m_scaleWidget->rect();
  if (e->x() > rect.left() && e->x() < rect.right())
  {
    m_dragging = true;
    m_y = e->y();
    m_dtype = m_y > height()/2 ? Bottom : Top;
    QApplication::setOverrideCursor(Qt::ClosedHandCursor);
  }
}

void ColorMapWidget::mouseMoveEvent(QMouseEvent* e)
{
  if (!m_dragging) return;
 
  double minValue = m_minValueBox->displayText().toDouble();
  double maxValue = m_maxValueBox->displayText().toDouble();

  if (m_dtype == Bottom)
  {
    minValue += double(e->y() - m_y)/height()*(maxValue - minValue);
    setMinValue(minValue);
  }
  else
  {
    maxValue += double(e->y() - m_y)/height()*(maxValue - minValue);
    setMaxValue(maxValue);
  }
  m_y = e->y();

  GraphOptions::ScaleType type = (GraphOptions::ScaleType)m_scaleOptions->itemData(m_scaleOptions->currentIndex()).toUInt();
  if( type == GraphOptions::Linear )
  {
    QwtLinearScaleEngine linScaler;
    m_scaleWidget->setScaleDiv(linScaler.transformation(), linScaler.divideScale(minValue, maxValue,  20, 5));
  }
  else
 {
    QwtLog10ScaleEngine logScaler;    
    double logmin(minValue);
    if( logmin <= 0.0 )
    {
      logmin = m_minPositiveValue;
    }
    m_scaleWidget->setScaleDiv(logScaler.transformation(), logScaler.divideScale(logmin, maxValue, 20, 5));
  }
}

void ColorMapWidget::mouseReleaseEvent(QMouseEvent* e)
{
  if (!m_dragging) return;
  if (m_dtype == Bottom)
  {
    minValueChanged();
  }
  else
  {
    maxValueChanged();
  }
  QApplication::restoreOverrideCursor();
  m_dragging = false;
}
