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

