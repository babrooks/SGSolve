#include "sgcustomplot.hpp"

SGCustomPlot::SGCustomPlot(int _state, bool _isDetailPlot) : 
  QCustomPlot(), 
  state(_state), 
  isDetailPlot(_isDetailPlot)
{
  path = QString("./");

  QSizePolicy qsp(QSizePolicy::Preferred,
		  QSizePolicy::Minimum);
  qsp.setHeightForWidth(true);

  setSizePolicy(qsp);

  QFont font = xAxis->labelFont();
  font.setPointSize(10);
  xAxis->setTickLabelFont(font);
  yAxis->setTickLabelFont(font);

  title = new QCPPlotTitle(this,"");
  font = title->font();
  font.setPointSize(12);
  title->setFont(font);
  
  plotLayout()->insertRow(0);
  plotLayout()->addElement(0,0,title);

  setContextMenuPolicy(Qt::CustomContextMenu);

  inspectPointAction = new QAction(tr("&Inspect point"),this);
  savePDFAction = new QAction(tr("Save &PDF"), this);
  savePNGAction = new QAction(tr("Save P&NG"), this);

  connect(savePDFAction,SIGNAL(triggered()),
	  this,SLOT(savePDF()));
  connect(savePNGAction,SIGNAL(triggered()),
	  this,SLOT(savePNG()));
  connect(inspectPointAction,SIGNAL(triggered()),
	  this,SLOT(pointInspected()));

  connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,SLOT(ShowContextMenu(const QPoint &)));
} //! Constructor

void SGCustomPlot::setRanges(const QCPRange & xrange,
			     const QCPRange & yrange)
{
  nominalXRange = xrange;
  nominalYRange = yrange;

  adjustRanges();
}

void SGCustomPlot::equalizeAxesScales()
{
  double xDist = nominalXRange.upper - nominalXRange.lower;
  double yDist = nominalYRange.upper - nominalYRange.lower;

  double dist = max(xDist,yDist);
  
  double xAvg = (nominalXRange.upper + nominalXRange.lower)/2.0;
  double yAvg = (nominalYRange.upper + nominalYRange.lower)/2.0;
  nominalXRange = QCPRange(xAvg - dist/2.0, xAvg + dist/2.0);
  nominalYRange = QCPRange(yAvg - dist/2.0, yAvg + dist/2.0);

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
