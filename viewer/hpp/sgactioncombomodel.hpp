#ifndef SGACTIONCOMBOMODEL_HPP
#define SGACTIONCOMBOMODEL_HPP

#include "sgplotcontroller.hpp"
#include <QAbstractListModel>
#include <QComboBox>

class SGActionComboModel : public QAbstractListModel
{
  Q_OBJECT;
private:
  SGPlotController * controller;

public:
  //! Constructor
  SGActionComboModel(SGPlotController * _controller):
    controller(_controller)
  {
    connect(controller,SIGNAL(solutionChanged()),
	    this,SLOT(solutionChanged()));
    connect(controller,SIGNAL(stateChanged()),
	    this,SLOT(changeLayout()));
  }
  
  //! Reimplement rowcount
  int rowCount(const QModelIndex & parent) const
  {
    if (controller->hasSolution()
	&& controller->getState()>-1)
      return controller->getCurrentIter().actions[controller->getState()].size()+1;
    else
      return 1;
  } // rowCount

  //! Reimplement data
  QVariant data(const QModelIndex & index, int role) const
  {
    if (index.row()>0)
      {
	int state = controller->getState();
	int action = controller->getCurrentIter().actions[state][index.row()-1].getAction();
	const vector< int >& numActions = controller->getSolution()->game.getNumActions()[state];
	QString dataString = QString("(R")
	  + QString::number(action%numActions[0])
	  + QString(",C")
	  + QString::number(action/numActions[0])
	  + QString(")");
	if (state == controller->getCurrentIter().bestState
	    && action == controller->getCurrentIter().bestAction)
	  dataString += QString("*");
	return dataString;
      }
    else if (index.row()==0)
      return QString("-");
  }

public slots:
  void actionChanged(int index)
  {
    controller->setActionIndex(index-1);
  }
  void solutionChanged()
  {
    emit layoutChanged();
  }
  void changeLayout()
  { emit layoutChanged(); }

}; // SGActionComboModel

#endif
