#ifndef SGACTIONCOMBOMODEL_HPP
#define SGACTIONCOMBOMODEL_HPP

#include "sgplotcontroller.hpp"
#include <QAbstractListModel>
#include <QComboBox>

//! Model for the action select menu
/*! This class interfaces between the QComboBox for selecting the
    action and the underlying SGSolution object. The model
    automatically synchronizes the actionCombo with the set of actions
    which are available on the current iteration and in the state
    selected in the stateCombo, where the current iteration and state
    are controlled by the associated SGPlotController object. When the
    action, state, or currentIter is changed in the SGPlotController,
    the changeLayout slot is triggered, which causes actionCombo to
    request new data entries to reflect the actions that are
    available. The action that generates the best test direction is
    indicated with an asterisk. 

    \ingroup viewer
*/
class SGActionComboModel : public QAbstractListModel
{
  Q_OBJECT;
private:
  //! Pointer to the associated SGPlotController object.
  SGPlotController * controller;

public:
  //! Constructor
  SGActionComboModel(SGPlotController * _controller):
    controller(_controller)
  {
    connect(controller,SIGNAL(solutionChanged()),
    	    this,SLOT(changeLayout()));
    // connect(controller,SIGNAL(stateChanged()),
    // 	    this,SLOT(changeLayout()));
  }
  
  //! Reimplement rowcount
  int rowCount(const QModelIndex & parent) const
  {
    if (controller->hasSolution()
	&& controller->getState()>-1)
      return controller->getCurrentIter().getActions()[controller->getState()].size()+1;
    else
      return 1;
  } // rowCount

  //! Reimplement data
  QVariant data(const QModelIndex & index, int role) const
  {
    if (index.row()>0)
      {
	int state = controller->getState();
	int action = controller->getCurrentIter().getActions()[state][index.row()-1].getAction();
	const vector< int >& numActions = controller->getSolution()->getGame().getNumActions()[state];
	QString dataString = QString("A")
	  + QString::number(action)
	  + QString(": (R")
	  + QString::number(action%numActions[0])
	  + QString(",C")
	  + QString::number(action/numActions[0])
	  + QString(")");
	if (state == controller->getCurrentIter().getBestState()
	    && action == controller->getCurrentIter().getBestAction())
	  dataString += QString("*");
	return dataString;
      }
    else if (index.row()==0)
      return QString("-");
  }

public slots:
  //! Signals to the associated actionController to change the action
  void actionChanged(int index)
  {
    controller->setActionIndex(index-1);
  }
  //! Signals to gui to change the layout
  void changeLayout()
  { emit layoutChanged(); }

}; // SGActionComboModel

#endif
