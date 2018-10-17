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
private:
  std::vector<QCPScatterStyle> styles;
  std::vector<QPen> pens;
  std::vector<bool> features;
public:
  enum SGScatterParam
    {
      PivotStyle,
      ExpPivotStyle,
      StageStyle,
      BindingPayoffStyle,
      ICPayoffStyle,
      PayoffStyle,
      TupleStyle,
      NumScatterParams
    };

  enum SGPenParam
    {
      StagePen,
      PivotPen,
      ExpPivotPen,
      ExpPen,
      PrevExpPen,
      ValuePen,
      ICPen,
      BindingPayoffPen,
      DirectionPen,
      GenLinePen,
      NumPenParams
    };

  enum SGPlotFeature
    {
      StagePayoffs,
      Pivot,
      ExpPivot,
      PayoffSet,
      ExpPayoffSet,
      PrevExpPayoffSet,
      ICBoundary,
      BindingPayoffs,
      Direction,
      GenLines,
      GridLines,
      ZeroLines,
      UniformRanges,
      NumPlotFeatures
    };

  //! Constructor
  SGPlotSettings():
    styles(SGPlotSettings::NumScatterParams),
    pens(SGPlotSettings::NumPenParams),
    features(SGPlotSettings::NumPlotFeatures,true)
  {
    restoreDefaults();
  }

  void restoreDefaults()
  {
    // Set default values
    for (int param = 0; param < SGPlotSettings::NumScatterParams; param++)
      restoreDefault(static_cast<SGPlotSettings::SGScatterParam>(param));
    for (int param = 0; param < SGPlotSettings::NumPenParams; param++)
      restoreDefault(static_cast<SGPlotSettings::SGPenParam>(param));
  } // restore defaults

  void restoreDefault(const SGPlotSettings::SGScatterParam param);

  void restoreDefault(const SGPlotSettings::SGPenParam param);
  
  const QCPScatterStyle & get(const SGPlotSettings::SGScatterParam param) const
  { return styles[param]; }

  const QPen & get(const SGPlotSettings::SGPenParam param) const
  { return pens[param]; }

  bool get(const SGPlotSettings::SGPlotFeature param) const
  { return features[param]; }
  
  void set(const SGPlotSettings::SGScatterParam param,
	   const QCPScatterStyle & newStyle)
  { styles[param] = newStyle; }

  void set(const SGPlotSettings::SGPenParam param,
	   const QPen & newPen)
  { pens[param] = newPen; }

  void set(const SGPlotSettings::SGPlotFeature param, bool tf);

  void disable(const SGPlotSettings::SGScatterParam param)
  { styles[param].setPen(Qt::NoPen); }

  void disable(const SGPlotSettings::SGPenParam param)
  { pens[param].setStyle(Qt::NoPen); }

  
}; // SGPlotSettings




#endif
