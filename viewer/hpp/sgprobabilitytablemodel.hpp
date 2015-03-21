#ifndef SGPROBABILITYTABLEMODEL_H
#define SGPROBABILITYTABLEMODEL_H

#include <QtWidgets>
#include <QAbstractTableModel>
#include <QTableView>
#include "sggame.hpp"
#include "sgpayofftablemodel.hpp"

//! Reimplements SGPayoffTable model to specialize to probabilities.
/*! An SGSolutionHandler object contains one of these objects for each
    possible state tomorrow. The nextState member indicates which
    state this object is associated with. 

    \ingroup viewer
*/
class SGProbabilityTableModel : public SGPayoffTableModel
{
  Q_OBJECT

private:
  //! Tomorrow's state.
  /*! This object interfaces with the probabilities of transitioning
      to nextState tomorrow. */
  int nextState;

public:
  //! Constructor
  /*! Constructor takes an additional argument, which is the nextState
      whose transition probabilities this model will interface to. */
  SGProbabilityTableModel(SGGame * _game,
			  int _state, int _nextState):
    SGPayoffTableModel(_game,_state), nextState(_nextState)
  { }

  //! Reimplements the data method
  /*! Retrieves the probability of going to nextState from state, when
      the action profile indicated by index is played. Retrieves the
      data using SGGame::getProbabilities. */
  QVariant data(const QModelIndex & index,
		int role) const Q_DECL_OVERRIDE;
    
  //! Reimplements the setData method
  /*! Sets the transition probability from state to nextState to
      value, using SGGame::setProbability. */
  bool setData(const QModelIndex & index, const QVariant & value, int role);

  //! Sets nextState
  bool setNextState(int newState)
  {
    if (newState < 0 || newState > game->getNumStates())
      return false;
    nextState = newState;
    return true;
  }
  

}; // SGProbabilityTableModel

#endif
