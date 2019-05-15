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

#include <QtWidgets>
#include "sgprobabilitytablemodel.hpp"

QVariant SGProbabilityTableModel::data(const QModelIndex & index,
				  int role) const
{
  if (role == Qt::SizeHintRole)
    return QSize(1,1);
  else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      int action = (index.column() * game->getNumActions()[state][0]
		    + index.row() );

      assert(action < game->getNumActions()[state][0]
	     *game->getNumActions()[state][1]);
      assert(nextState < game->getNumStates());
      assert(state < game->getNumStates());
      
      return QVariant(QString::number(game->getProbabilities()
				      [state][action][nextState]));
    }
  else
    return QVariant();
} // data
    
bool SGProbabilityTableModel::setData(const QModelIndex & index,
				      const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    {
      QRegExp rx("[, ]");
      QStringList list = value.toString().split(rx,QString::SkipEmptyParts);

      int action = (index.column() * game->getNumActions()[state][0]
		    + index.row() );
      
      assert(action < game->getNumActions()[state][0]
	     *game->getNumActions()[state][1]);
      assert(nextState < game->getNumStates());
      assert(state < game->getNumStates());
      
      if (list.size())
	game->setProbability(state,action,nextState,
			     list.at(0).toDouble());
      
      emit dataChanged(index,index);
      return true;
    }
  return false;
} // setData

