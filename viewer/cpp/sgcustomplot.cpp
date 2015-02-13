#include "sgcustomplot.hpp"

void SGCustomPlot::setRanges(const QCPRange & xrange,
			     const QCPRange & yrange)
{
  nominalXRange = xrange;
  nominalYRange = yrange;

  adjustRanges();
}

void SGCustomPlot::adjustRanges()
{
  int w = width();
  int h = height();

  realXRange = nominalXRange;
  realYRange = nominalYRange;

  if (w>h)
    {
      // expand xrange
      double diff = nominalXRange.upper - nominalXRange.lower;
      double alpha = ( (1.0*w)/h - 1.0)/2.0;
      realXRange.upper += diff*alpha;
      realXRange.lower -= diff*alpha;
    }
  else if (w<h)
    {
      // expand xrange
      double diff = nominalYRange.upper - nominalYRange.lower;
      double alpha = ( (1.0*h)/w - 1.0)/2.0;
      realYRange.upper += diff*alpha;
      realYRange.lower -= diff*alpha;
    }

  xAxis->setRange(realXRange);
  yAxis->setRange(realYRange);
}
