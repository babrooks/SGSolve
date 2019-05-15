// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2019 Benjamin A. Brooks
// 
// SGSolve free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SGSolve is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
// 
// Benjamin A. Brooks
// ben@benjaminbrooks.net
// Chicago, IL

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
  //! The number of states in the given plot.
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
