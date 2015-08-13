#ifndef SGSIMULATIONPLOT_HPP
#define SGSIMULATIONPLOT_HPP

#include "qcustomplot.h"
#include "sgpoint.hpp"

//! A customized version of QCustomPlot.
/*! This class inherits from QCustomPlot and mainly reimplements the
    minimumSizeHint to reflect the number of states.

  \ingroup viewer
*/
class SGSimulationPlot : public QCustomPlot
{
  Q_OBJECT;
private:
  int numStates;

public:
  //! Constructor
  SGSimulationPlot(int _numStates): numStates(_numStates) {}

protected:
  //! Reimplement resizeEvent
  /*! Calls adjust ranges before calling QCustomPlot::resizeEvent. */
  void resizeEvent(QResizeEvent * event)
  {
    QCustomPlot::resizeEvent(event);
  }

private slots:

private:

  //! Custom minimum size.
  virtual QSize minimumSizeHint() const
  { return QSize(100,200*(numStates+2)); } 

};

#endif
