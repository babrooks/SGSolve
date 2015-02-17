#ifndef SGTABLEMODEL_H
#define SGTABLEMODEL_H

#include <QtWidgets>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"

// Class that contains features common to SGPayoffTableModel and
// SGProbabilityTableModel.
class SGTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  SGTableModel(SGGame * _game,
	       int _state):
    game(_game), state(_state)
  { }
  
  Qt::ItemFlags flags(const QModelIndex & index) const 
  { return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable; }

  int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE
  { return game->getNumActions()[state][0]; }
  int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE
  { return game->getNumActions()[state][1]; }

  void emitLayoutChanged()
  { emit layoutChanged(); }
  bool setState(int newState)
  {
    if (newState < 0 || newState > game->getNumStates())
      return false;
    state = newState;
    return true;
  }
  
protected:
  int state;
  bool isConnceted;
  SGGame * game;
  
};


#endif
