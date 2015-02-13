#include <QtWidgets>
#include "sgprobabilitytablemodel.hpp"

QVariant SGProbabilityTableModel::data(const QModelIndex & index,
				  int role) const Q_DECL_OVERRIDE
{
  if (role == Qt::SizeHintRole)
    return QSize(1,1);
  else if (role == Qt::DisplayRole)
    {
      return QVariant(QString::number(game->getProbabilities()
				      [state][index.row()][index.column()]));
    }
  else
    return QVariant();
} // data
    
QVariant SGProbabilityTableModel::headerData(int section,
					Qt::Orientation orientation,
					int role) const Q_DECL_OVERRIDE
{
  if (role == Qt::DisplayRole)
    {
      switch (orientation)
	{
	case Qt::Horizontal:
	  return QVariant(QString("S")
			  +QString::number(section));

	case Qt::Vertical:
	  return QVariant(QString("(R")
			  +QString::number(section
					   %game->getNumActions()[state][0])
			  +QString(",C")
			  +QString::number(section
					   /game->getNumActions()[state][0])
			  +QString(")"));
	}
    }
  else
    return QVariant();
} // headerData

bool SGProbabilityTableModel::setData(const QModelIndex & index,
				      const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    {
      QRegExp rx("[, ]");
      QStringList list = value.toString().split(rx,QString::SkipEmptyParts);

      if (list.size())
	game->setProbability(state,index.row(),index.column(),
			     list.at(0).toDouble());
      
      emit dataChanged(index,index);
      return true;
    }
  return false;
} // setData

