#ifndef SGPROBABILITYTABLEMODEL_H
#define SGPROBABILITYTABLEMODEL_H

#include <QtWidgets>
#include <QMainWindow>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"
#include "sgtablemodel.hpp"

// One SGProbabilityTableModel for each state. Control which one is
// displayed by changing the model that is plugged into the table
// view.
class SGProbabilityTableModel : public SGTableModel
{
  Q_OBJECT

public:
  SGProbabilityTableModel(SGGame * _game,
		     int _state):
    SGTableModel(_game,_state)
  { }
  
  int rowCount(const QModelIndex & parent) const Q_DECL_OVERRIDE
  { return (game->getNumActions()[state][0]
	    * game->getNumActions()[state][1]); }
  int columnCount(const QModelIndex & parent) const Q_DECL_OVERRIDE
  { return game->getNumStates(); }

  QVariant data(const QModelIndex & index,
		int role) const Q_DECL_OVERRIDE;
    
  QVariant headerData(int section,
		      Qt::Orientation orientation,
		      int role) const Q_DECL_OVERRIDE;

  bool setData(const QModelIndex & index, const QVariant & value, int role);


}; // SGProbabilityTableModel

#endif
