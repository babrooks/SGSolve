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

#include "sgactioncombomodel.hpp"

SGActionComboModel::SGActionComboModel(SGPlotController * _controller):
  controller(_controller)
{
  connect(controller,SIGNAL(solutionChanged()),
	  this,SLOT(changeLayout()));
  connect(controller,SIGNAL(stateChanged()),
	  this,SLOT(changeLayout()));
}
  
//! Reimplement rowcount
int SGActionComboModel::rowCount(const QModelIndex & parent) const
{
  if (controller->hasSolution()
      && state>-1)
    {
      qDebug() << "In row count: " << controller->getState() << " " << controller->getAction()
	       <<  " " << controller->getCurrentIter()->getActions()[controller->getState()].size()+1
	       << endl;
      return controller->getCurrentIter()->getActions()[state].size()+1;
    }
  else
    return 1;
} // rowCount

  //! Reimplement data
QVariant SGActionComboModel::data(const QModelIndex & index, int role) const
{
  if (index.row()>0 && state > -1)
    {
      int action = controller->getCurrentIter()->getActions()[state][index.row()-1].getAction();
      qDebug() << "In data: " << controller->getState() << " " << controller->getAction() << endl;
      const vector< int >& numActions = controller->getSolution()->getGame().getNumActions()[state];
      QString dataString = QString("A")
	+ QString::number(action)
	+ QString(": (R")
	+ QString::number(action%numActions[0])
	+ QString(",C")
	+ QString::number(action/numActions[0])
	+ QString(")");
      if (index.row() == controller->getCurrentStep().getActionTuple()[state]+1)
	dataString += QString("*");
      dataString += QString("    ");
      return dataString;
    }
  else if (index.row()==0)
    return QString("-");
}

//! Signals to gui to change the layout
void SGActionComboModel::changeLayout()
{
  emit layoutAboutToBeChanged();
  state=controller->getState();
  emit layoutChanged();
}

