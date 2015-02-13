#include <QtWidgets>
#include "sgpayofftablemodel.hpp"

QVariant SGPayoffTableModel::data(const QModelIndex & index,
				  int role) const Q_DECL_OVERRIDE
{
  if (role == Qt::SizeHintRole)
    return QSize(1,1);
  else if (role == Qt::DisplayRole)
    {

      int action = (index.column() * game->getNumActions()[state][0]
		    + index.row() );
      
      return QVariant(QString::number(game->getPayoffs()[state][action][0])
		      +QString(", ")
		      +QString::number(game->getPayoffs()[state][action][1]));
    }
  else
    return QVariant();
} // data
    
QVariant SGPayoffTableModel::headerData(int section,
					Qt::Orientation orientation,
					int role) const Q_DECL_OVERRIDE
{
  if (role == Qt::DisplayRole)
    {
      switch (orientation)
	{
	case Qt::Horizontal:
	  return QVariant(QString("C")
			  +QString::number(section));

	case Qt::Vertical:
	  return QVariant(QString("R")
			  +QString::number(section));
	}
    }
  else
    return QVariant();
} // headerData

bool SGPayoffTableModel::setData(const QModelIndex & index,
				 const QVariant & value,
				 int role)
{
  if (role == Qt::EditRole)
    {
      int action = index.row()+index.column()*game->getNumActions()[state][0];

      QRegExp rx("[, ]");
      QStringList list = value.toString().split(rx,QString::SkipEmptyParts);

      for (int player = 0; player < std::min(list.size(),2); player++)
	game->setPayoff(state,action,player,list.at(player).toDouble());

      emit dataChanged(index,index);
      return true;
    }
  return false;
} // setData
