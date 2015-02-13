#ifndef SGPROBABILITYTABLEMODEL_H
#define SGPROBABILITYTABLEMODEL_H

#include <QtWidgets>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"
#include "sgpayofftablemodel.hpp"

// One SGProbabilityTableModel for each state. Control which one is
// displayed by changing the model that is plugged into the table
// view.
class SGProbabilityTableModel : public SGPayoffTableModel
{
  Q_OBJECT

private:
  int nextState;

public:
  SGProbabilityTableModel(SGGame * _game,
			  int _state, int _nextState):
    SGPayoffTableModel(_game,_state), nextState(_nextState)
  { }

  QVariant data(const QModelIndex & index,
		int role) const Q_DECL_OVERRIDE;
    
  bool setData(const QModelIndex & index, const QVariant & value, int role);

  bool setNextState(int newState)
  {
    if (newState < 0 || newState > game->getNumStates())
      return false;
    nextState = newState;
    return true;
  }
  

}; // SGProbabilityTableModel

#endif
