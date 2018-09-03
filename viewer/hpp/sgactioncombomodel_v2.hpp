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

#ifndef SGACTIONCOMBOMODEL_V2_HPP
#define SGACTIONCOMBOMODEL_V2_HPP

#include "sgplotcontroller_v2.hpp"
#include <QAbstractListModel>
#include <QComboBox>

//! Model for the action select menu
/*! This class interfaces between the QComboBox for selecting the
    action and the underlying SGSolution object. The model
    automatically synchronizes the actionCombo with the set of actions
    which are available on the current iteration and in the state
    selected in the stateCombo, where the current iteration and state
    are controlled by the associated SGPlotController_V2 object. When the
    action, state, or currentIter is changed in the SGPlotController_V2,
    the changeLayout slot is triggered, which causes actionCombo to
    request new data entries to reflect the actions that are
    available. The action that generates the best test direction is
    indicated with an asterisk. 

    \ingroup viewer
*/
class SGActionComboModel_V2 : public QAbstractListModel
{
  Q_OBJECT;
private:
  //! Pointer to the associated SGPlotController_V2 object.
  SGPlotController_V2 * controller;

public:
  //! Constructor
  SGActionComboModel_V2(SGPlotController_V2 * _controller):
    controller(_controller)
  {
    connect(controller,SIGNAL(solutionChanged()),
    	    this,SLOT(changeLayout()));
    // connect(controller,SIGNAL(stateChanged()),
    // 	    this,SLOT(changeLayout()));
  }
  
  //! Reimplement rowcount
  int rowCount(const QModelIndex & parent) const
  {
    if (controller->hasSolution()
	&& controller->getState()>-1)
      return controller->getCurrentIter().getActions()[controller->getState()].size()+1;
    else
      return 1;
  } // rowCount

  //! Reimplement data
  QVariant data(const QModelIndex & index, int role) const
  {
    if (index.row()>0)
      {
	int state = controller->getState();
	int action = controller->getCurrentIter().getActions()[state][index.row()-1].getAction();
	const vector< int >& numActions = controller->getSolution()->getGame().getNumActions()[state];
	QString dataString = QString("A")
	  + QString::number(action)
	  + QString(": (R")
	  + QString::number(action%numActions[0])
	  + QString(",C")
	  + QString::number(action/numActions[0])
	  + QString(")");
     if (index.row() == controller->getCurrentStep().getActionTuple()[state]-1)
       dataString += QString("*");
	return dataString;
      }
    else if (index.row()==0)
      return QString("-");
  }

public slots:
  //! Signals to the associated actionController to change the action
  void actionChanged(int index)
  {
    controller->setActionIndex(index-1);
  }
  //! Signals to gui to change the layout
  void changeLayout()
  { emit layoutChanged(); }

}; // SGActionComboModel_V2

#endif
