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

#include "sgplotsettings.hpp"

void SGPlotSettings::restoreDefault(const SGPlotSettings::SGScatterParam param)
{
  switch (param)
    {
    case PivotStyle:
      styles[SGPlotSettings::PivotStyle]
	=QCPScatterStyle(QCPScatterStyle::ssCircle,
			 QPen(Qt::blue),
			 QBrush(Qt::blue),8);
      break;
    case ExpPivotStyle:
      styles[SGPlotSettings::ExpPivotStyle]
	=QCPScatterStyle(QCPScatterStyle::ssCircle,
			 QPen(Qt::red),
			 QBrush(Qt::red),8);
      break;
    case StageStyle:
      styles[SGPlotSettings::StageStyle]
	=QCPScatterStyle(QCPScatterStyle::ssStar,
			 QPen(Qt::black),
			 QBrush(Qt::black),6);
      break;
    case BindingPayoffStyle:
      styles[SGPlotSettings::BindingPayoffStyle]
	=QCPScatterStyle(QCPScatterStyle::ssCircle,
			 QPen(Qt::magenta),
			 QBrush(Qt::magenta),6);
      break;
    case ICPayoffStyle:
      styles[SGPlotSettings::ICPayoffStyle]
	=QCPScatterStyle(QCPScatterStyle::ssSquare,
			 QPen(Qt::magenta),
			 QBrush(Qt::magenta),2);
      break;
    case PayoffStyle:
      styles[SGPlotSettings::PayoffStyle]
	=QCPScatterStyle(QCPScatterStyle::ssCircle,
			 QPen(Qt::black),
			 QBrush(Qt::black),6);
      break;
    case TupleStyle:
      styles[SGPlotSettings::TupleStyle]
	=QCPScatterStyle(QCPScatterStyle::ssSquare,
			 QPen(Qt::blue),
			 QBrush(Qt::blue),2);
      break;

    } // switch
} // restoreDefault (scatter param)

void SGPlotSettings::restoreDefault(const SGPlotSettings::SGPenParam param)
{
  // Set default values
  switch (param)
    {
    case StagePen:
      pens[SGPlotSettings::StagePen]
	=QPen(Qt::NoPen);
      break;
    case PivotPen:
      pens[SGPlotSettings::PivotPen]
	=QPen(Qt::NoPen);
      break;
    case ExpPivotPen:
      pens[SGPlotSettings::ExpPivotPen]
	=QPen(Qt::NoPen);
      break;
    case ExpPen:
      pens[SGPlotSettings::ExpPen]
	=QPen(Qt::red);
      break;
    case PrevExpPen:
      pens[SGPlotSettings::PrevExpPen]
	=QPen(Qt::red);
      pens[SGPlotSettings::PrevExpPen].setStyle(Qt::DashLine);
      break;
    case ValuePen:
      pens[SGPlotSettings::ValuePen]
	=QPen(Qt::blue);
      break;
    case ICPen:
      pens[SGPlotSettings::ICPen]
	=QPen(Qt::magenta);
      break;
    case BindingPayoffPen:
      pens[SGPlotSettings::BindingPayoffPen]
	=QPen(Qt::NoPen);
      break;
    case DirectionPen:
      pens[SGPlotSettings::DirectionPen]
	=QPen(Qt::darkGreen);

      pens[SGPlotSettings::DirectionPen].setStyle(Qt::DashLine);
      break;
    case GenLinePen:
      pens[SGPlotSettings::GenLinePen]
	=QPen(Qt::DashLine);
      break;
    } // switch
} // restore defaults
  
void SGPlotSettings::set(const SGPlotSettings::SGPlotFeature param,
	 bool tf)
{
  features[param] = tf;
  switch (param)
    {
    case StagePayoffs:
      if (tf)
	restoreDefault(SGPlotSettings::StageStyle);
      else
	disable(SGPlotSettings::StageStyle);

      break;
    case Pivot:
      if (tf)
	restoreDefault(SGPlotSettings::PivotStyle);
      else
	disable(SGPlotSettings::PivotStyle);

      break;
    case ExpPivot:
      if (tf)
	restoreDefault(SGPlotSettings::ExpPivotStyle);
      else
	disable(SGPlotSettings::ExpPivotStyle);

      break;
    case PayoffSet:
      if (tf)
	restoreDefault(SGPlotSettings::PayoffStyle);
      else
	disable(SGPlotSettings::PayoffStyle);

      break;
    case ExpPayoffSet:
      if (tf)
	restoreDefault(SGPlotSettings::ExpPen);
      else
	disable(SGPlotSettings::ExpPen);

      break;
    case PrevExpPayoffSet:
      if (tf)
	restoreDefault(SGPlotSettings::PrevExpPen);
      else
	disable(SGPlotSettings::PrevExpPen);

      break;
    case ICBoundary:
      if (tf)
	restoreDefault(SGPlotSettings::ICPen);
      else
	disable(SGPlotSettings::ICPen);

      break;
    case BindingPayoffs:
      if (tf)
	restoreDefault(SGPlotSettings::ICPayoffStyle);
      else
	disable(SGPlotSettings::ICPayoffStyle);

      break;
    case Direction:
      if (tf)
	restoreDefault(SGPlotSettings::DirectionPen);
      else
	disable(SGPlotSettings::DirectionPen);

      break;
    case GenLines:
      if (tf)
	restoreDefault(SGPlotSettings::GenLinePen);
      else
	disable(SGPlotSettings::GenLinePen);

      break;
    case GridLines:

      break;
    case ZeroLines:

      break;
    case UniformRanges:

      break;
    }
}



