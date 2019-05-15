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

#ifndef SGPAYOFFTABLEMODEL_H
#define SGPAYOFFTABLEMODEL_H

#include <QtWidgets>
#include <QMainWindow>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"
#include "sgtablemodel.hpp"

//! Derived class for payoff table models
/*! This class handles the interface between SGTableView and the
    payoff matrices in SGGameHandler::game. Adds functionality for
    header data (using the row player action/ column player action
    conventions. Also reimplements the data() method to display an
    ordered pair of the two players' payoffs. 
    
    \ingroup viewer
*/
class SGPayoffTableModel : public SGTableModel
{
  Q_OBJECT

public:
  //! Constructor
  SGPayoffTableModel(SGGame * _game,
		     int _state):
    SGTableModel(_game,_state)
  {}
  
  //! Returns formatted data
  /*! Returns an ordered pair of the two players' payoffs for the
      action profile specified in index. Reads this data from the game
      object using SGGame::getPayoffs. */
  QVariant data(const QModelIndex & index,
		int role) const Q_DECL_OVERRIDE;

  //! Returns formatted header data
  /*! Returns header data formatted to indicate the appropriate row/
      column action. */
  QVariant headerData(int section,
		      Qt::Orientation orientation,
		      int role) const Q_DECL_OVERRIDE;

  //! Sets data
  /*! Parses the data in value to set a new pair of payoffs. Sets the
      data using SGGame::setData. */
  bool setData(const QModelIndex & index, const QVariant & value, int role);
  
}; // SGPayoffTableModel

#endif
