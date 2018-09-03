// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2016 Benjamin A. Brooks
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

#ifndef SGPLOTSETTINGS_HPP
#define SGPLOTSETTINGS_HPP

#include "qcustomplot.h"

class SGPlotSettings
{
public:
  //! Pivot scatter style
  QCPScatterStyle pivotStyle;
  //! Expected pivot scatter style
  QCPScatterStyle expPivotStyle;
  //! Stage payoff scatter style
  QCPScatterStyle stageStyle;
  //! Binding continuation value scatter style
  QCPScatterStyle bindingPayoffStyle;
  //! Binding IC payoff scatter style
  QCPScatterStyle ICPayoffStyle;
  //! Payoff style
  QCPScatterStyle payoffStyle;
  //! Tuple style
  QCPScatterStyle tupleStyle;
  //! Stage payoffs pen
  QPen stagePen;
  //! Pivot pen
  QPen pivotPen;
  //! Expected pivot pen
  QPen expPivotPen;
  //! Expected payoff pen
  QPen expPen;
  //! Previous expected payoff pen
  QPen prevExpPen;
  //! Value set pen
  QPen valuePen;
  //! Incentive constraint pen
  QPen ICPen;
  //! Binding payoffs pen
  QPen bindingPayoffPen;
  //! Direction pen
  QPen directionPen;
  //! Generation line
  QPen genLinePen;

  //! Constructor
  SGPlotSettings():
    pivotStyle(QCPScatterStyle::ssCircle,
	       QPen(Qt::blue),
	       QBrush(Qt::blue),8),
    expPivotStyle(QCPScatterStyle::ssCircle,
		  QPen(Qt::red),
		  QBrush(Qt::red),8),
    stageStyle(QCPScatterStyle::ssStar,
	       QPen(Qt::black),
	       QBrush(Qt::black),6),
    bindingPayoffStyle(QCPScatterStyle::ssCircle,
               QPen(Qt::magenta),
               QBrush(Qt::magenta),6),
    ICPayoffStyle(QCPScatterStyle::ssSquare,
               QPen(Qt::magenta),
               QBrush(Qt::magenta),2),
    payoffStyle(QCPScatterStyle::ssCircle,
		QPen(Qt::black),
		QBrush(Qt::black),6),
    tupleStyle(QCPScatterStyle::ssSquare,
	       QPen(Qt::blue),
	       QBrush(Qt::blue),2),
    stagePen(Qt::NoPen),
    pivotPen(Qt::NoPen),
    expPivotPen(Qt::NoPen),
    expPen(Qt::red),
    prevExpPen(Qt::red),
    valuePen(Qt::blue),
    ICPen(Qt::magenta),
    bindingPayoffPen(Qt::NoPen),
    directionPen (Qt::darkGreen),
    genLinePen(Qt::DashLine)
  {
      directionPen.setStyle(Qt::DashLine);
      prevExpPen.setStyle(Qt::DashLine);
  }

}; // SGPlotSettings

#endif
