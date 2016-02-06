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

#ifndef SGTABLEMODEL_H
#define SGTABLEMODEL_H

#include <QtWidgets>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"

//! Table models for SGViewer
/*! Specializes QAbstractTableModel for the SGViewer program.  Sets
    flags, reimplements the rowCount and columnCount methods to return
    the numbers of actions. Also contains members to indicate which
    state the model is currently associated with.
  
  \ingroup viewer
*/
class SGTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  //! Constructor
  SGTableModel(SGGame * _game,
	       int _state):
    game(_game), state(_state)
  { }
  
  //! Returns flags.
  /*! Returns flags that indicate the model is enabled, can be edited,
      and can be selected. */
  Qt::ItemFlags flags(const QModelIndex & index) const 
  { return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable; }

  //! Returns the number of row player actions.
  int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE
  { return game->getNumActions()[state][0]; }
  //! Returns the number of column player actions.
  int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE
  { return game->getNumActions()[state][1]; }

  //! Emits layoutChanged signal.
  void emitLayoutChanged()
  { emit layoutChanged(); }

  //! Sets the state to newState.
  bool setState(int newState)
  {
    if (newState < 0 || newState > game->getNumStates())
      return false;
    state = newState;
    return true;
  }
  
protected:
  //! The state that the model is associated with
  int state;
  //! Pointer to the associated game.
  SGGame * game;
  
};


#endif
