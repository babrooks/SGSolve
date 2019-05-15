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

#ifndef SGSTATECOMBOMODEL_V2_HPP
#define SGSTATECOMBOMODEL_V2_HPP

#include "sgplotcontroller_v2.hpp"
#include <QAbstractListModel>
#include <QComboBox>

//! Model for stateCombo
/*! Model that interfaces between the state QComboBox and the
    underlying SGSolution object. This model automatically
    synchronizes the number and labels of the states with the
    SGSolution, and it indicates with an asterisk which state
    generated the best test direction for the current iteration. When
    a new solution is loaded, or when the current iteration is changed
    in the SGPlotController_V2, the changeLayout slot is triggered so
    that the stateCombo is updated to reflect the true number of
    states and the identity of the best state.

    \ingroup viewer
*/
class SGStateComboModel_V2 : public QAbstractListModel
{
  Q_OBJECT;
private:
  //! The associated SGPlotController_V2 object
  SGPlotController_V2 * controller;

public:
  //! Constructor
  SGStateComboModel_V2(SGPlotController_V2 * _controller):
    controller(_controller)
  {
    connect(controller,SIGNAL(solutionChanged()),
    	    this,SLOT(changeLayout()));
  }

  //! Reimplement rowcount
  int rowCount(const QModelIndex & parent) const
  {
    if (controller->hasSolution())
      return controller->getCurrentIter()->getActions().size()+1;
    else
      return 1;
  } // rowCount

  //! Reimplement data
  QVariant data(const QModelIndex & index, int role) const
  {
    if (index.row()>0)
      {
	QString dataString = QString("S")+QString::number(index.row()-1);
	// if ( (index.row()-1) == controller->getCurrentIter()->getBestState())
	//   dataString += QString("*");
	return dataString;
      }
    else if (index.row()==0)
      return QString("-");
  }

public slots:
  //! Tells the controller to change the state when the index of the
  //! combo box is updated.
  void changeState(int index)
  { controller->setState(index-1); }

  //! Signals to the GUI to change the layout.
  void changeLayout()
  { 
    emit layoutChanged(); 
  }

}; // SGStateComboModel_V2

#endif
