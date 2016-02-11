#include "MantidQtSliceViewer/PeakOverlayViewFactoryBase.h"
#include "MantidQtSliceViewer/PeakPalette.h"
#include <boost/make_shared.hpp>

using namespace Mantid::API;

namespace MantidQt
{
  namespace SliceViewer
  {

    PeakOverlayViewFactoryBase::PeakOverlayViewFactoryBase(QwtPlot * plot, QWidget * parent, const int plotXIndex, const int plotYIndex, const size_t workspaceNumber) : PeakOverlayViewFactory(), m_plot(plot), m_parent(parent),
        m_plotXIndex(plotXIndex), m_plotYIndex(plotYIndex)
    {
      if(!plot)
        throw std::invalid_argument("PeakOverlayViewFactoryBase plot is null");
      if(!parent)
        throw std::invalid_argument("PeakOverlayViewFactoryBase parent widget is null");

      PeakPalette<QColor> defaultPalette;
      auto peakColourEnum = defaultPalette.foregroundIndexToColour(static_cast<int>(workspaceNumber));
      auto backColourEnum = defaultPalette.backgroundIndexToColour(static_cast<int>(workspaceNumber));
      m_peakColour = QColor(peakColourEnum);
      m_backColour = QColor(backColourEnum);
    }

    std::string PeakOverlayViewFactoryBase::getPlotXLabel() const
    {
      QwtText xDim = m_plot->axisTitle(QwtPlot::xBottom);
      return xDim.text().toStdString();
    }

    std::string PeakOverlayViewFactoryBase::getPlotYLabel() const
    {
      QwtText yDim = m_plot->axisTitle(QwtPlot::yLeft);
      return yDim.text().toStdString();
    }

    PeakOverlayViewFactoryBase::~PeakOverlayViewFactoryBase()
    {
    }
  }
}
